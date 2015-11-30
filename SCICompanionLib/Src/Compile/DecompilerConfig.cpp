/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#include "stdafx.h"
#include "cpptoml.h"
#include "ScriptOMAll.h"
#include "GameFolderHelper.h"
#include "DecompilerCore.h"
#include "DecompilerConfig.h"
#include "Vocab99x.h"
#include "CrystalScriptStream.h"
#include "SyntaxParser.h"
#include "CompileInterfaces.h"
#include <regex>
#include <format.h>

using namespace std;
using namespace sci;
using namespace cpptoml;

class DummyLog : public ICompileLog
{
    void ReportResult(const CompileResult &result) override {} 
};

unique_ptr<Script> GetDefinesScript(const GameFolderHelper &helper, const std::string &name)
{
    DummyLog log;
    ScriptId scriptId(helper.GetIncludeFolder() + "\\" + name);
    unique_ptr<Script> script = make_unique<Script>(scriptId);
    assert(scriptId.Language() == LangSyntaxStudio);
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        if (!SyntaxParser_Parse(*script, stream, PreProcessorDefinesFromSCIVersion(helper.Version), &log))
        {
            assert(false);
        }
        buffer.FreeAll();
    }
    return script;
}

class DecompilerConfig : public IDecompilerConfig
{
public:
    DecompilerConfig(const GameFolderHelper &helper, const SelectorTable &selectorTable) : _selectorTable(selectorTable)
    {
        unique_ptr<Script> definesScript = GetDefinesScript(helper, "sci.sh");
        unique_ptr<Script> keysScript = GetDefinesScript(helper, "keys.sh");

        string decompilerIniPath = helper.GetSrcFolder() + "\\Decompiler.ini";
        try
        {
            _table = make_unique<table>(parse_file(decompilerIniPath));
            if (_table->contains("methodParameterNames"))
            {
                _methodParameterNames = _table->get_table("methodParameterNames");
            }
            _CacheBitfieldProps();
            _CacheEnums({ definesScript.get(), keysScript.get() });
            _CacheMethodCallParamTypes();
            _CacheKernelCallParamTypes();
            _CacheSwitchValueTypes();
        }
        catch (parse_exception &e)
        {
            error = e.what();
        }
    }

    std::vector<std::string> GetParameterNamesFor(sci::ClassDefinition *classDef, const std::string &methodName) const
    {
        vector<string> paramsOut;
        if (_methodParameterNames && _methodParameterNames->contains(methodName))
        {
            auto params = _methodParameterNames->get(methodName);
            if (params->is_array())
            {
                for (const auto &paramName : params->as_array()->get())
                {
                    auto name = paramName->as<std::string>();
                    if (name)
                    {
                        paramsOut.push_back(name->get());
                    }
                }
            }
            else if (params->is_value()) // Just a single guy
            {
                auto name = params->as<string>();
                if (name)
                {
                    paramsOut.push_back(name->get());
                }
            }
        }

        return paramsOut;
    }

    void ResolveMethodCallParameterTypes(sci::SendParam &sendParam) const
    {
        auto it = _methodCallParamTypes.find(sendParam.GetName());
        if (it != _methodCallParamTypes.end())
        {
            _ResolveParamStatement(sendParam, it->second);
        }
    }

    void ResolveProcedureCallParameterTypes(sci::ProcedureCall &procCall) const
    {
        auto it = _kernelCallParamTypes.find(procCall.GetName());
        if (it != _kernelCallParamTypes.end())
        {
            _ResolveParamStatement(procCall, it->second);
        }
    }

    bool IsBitfieldProperty(const std::string &propertyName) const
    {
        return _bitfieldProperties.find(propertyName) != _bitfieldProperties.end();
    }

    void ResolveSwitchStatementValues(sci::SwitchStatement &switchStatement) const
    {
        vector<SyntaxNode*> destinations;
        for (auto &caseStatement : switchStatement._cases)
        {
            if (!caseStatement->IsDefault())
            {
                destinations.push_back(caseStatement->GetStatement1());
            }
        }
        _ResolveValuesHelper(switchStatement.GetStatement1(), destinations);
    }

    void ResolveBinaryOpValues(sci::BinaryOp &binaryOp) const
    {
        _ResolveValuesHelper(binaryOp.GetStatement1(), { binaryOp.GetStatement2() });
        _ResolveValuesHelper(binaryOp.GetStatement2(), { binaryOp.GetStatement1() });
    }

private:

    void _ResolveValuesHelper(SyntaxNode *source, const vector<SyntaxNode*> &destinations) const
    {
        if (source)
        {
            auto itType = _switchValueTypes.end();
            PropertyValue *pv = SafeSyntaxNode<PropertyValue>(source);
            if ((pv != nullptr) && (pv->GetType() == ValueType::Token))
            {
                itType = _switchValueTypes.find(pv->GetStringValue());
            }
            LValue *lValue = SafeSyntaxNode<LValue>(source);
            if (lValue)
            {
                itType = _switchValueTypes.find(lValue->GetName());
            }
            SendCall *sendCall = SafeSyntaxNode<SendCall>(source);
            if (sendCall && (sendCall->GetParams().size() == 1) && !sendCall->GetObjectA().empty())
            {
                auto &sendParam = sendCall->GetParams()[0];
                if (sendParam->GetSelectorParams().size() == 0) // It may be a property access
                {
                    string lookupValue = fmt::format("{0}.{1}", sendCall->GetObjectA(), sendParam->GetSelectorName());
                    itType = _switchValueTypes.find(lookupValue);
                }
            }

            if (itType != _switchValueTypes.end())
            {
                const enumList_t &enumList = _enumLists.at(itType->second); // Guaranteed to exist.

                // Now go through the case statements, and see if we can resolve values
                for (auto &destination : destinations)
                {
                    PropertyValue *pvCase = SafeSyntaxNode<PropertyValue>(destination);
                    if ((pvCase != nullptr) && (pvCase->GetType() == ValueType::Number))
                    {
                        auto itValue = enumList.find(pvCase->GetNumberValue());
                        if (itValue != enumList.end())
                        {
                            pvCase->SetValue(itValue->second, ValueType::Token);
                        }
                    }
                }
            }
        }
    }

    void _ResolveParamStatement(sci::StatementsNode &statementNode, const vector<string> &types) const
    {
        size_t index = 0;
        for (auto &param : statementNode.GetStatements())
        {
            PropertyValue *value = SafeSyntaxNode<PropertyValue>(param.get());
            if (value && (value->GetType() == ValueType::Number) && (index < types.size()))
            {
                const string &type = types[index];
                if (type == "selector")
                {
                    // This is special. Let's look up the selector
                    string selectorName = _selectorTable.Lookup(value->GetNumberValue());
                    if (!selectorName.empty())
                    {
                        value->SetValue(selectorName, ValueType::Selector);
                    }
                }
                else
                {
                    auto itEnum = _enumLists.find(type);
                    if (itEnum != _enumLists.end())
                    {
                        const enumList_t &enumList = itEnum->second;
                        auto itValue = enumList.find(value->GetNumberValue());
                        if (itValue != enumList.end())
                        {
                            value->SetValue(itValue->second, ValueType::Token);
                        }
                    }
                }
            }
            index++;
        }
    }

    void _CacheBitfieldProps()
    {
        if (_table->contains_qualified("bitfieldProperties.bitfieldProperties"))
        {
            auto bitfieldProps = _table->get_array_qualified("bitfieldProperties.bitfieldProperties");
            for (auto prop : bitfieldProps->get())
            {
                _bitfieldProperties.insert(prop->as<string>()->get());
            }
        }
    }

    void _CacheEnums(vector<Script*> scripts)
    {
        map<string, uint16_t> defines;
        for (auto &script : scripts)
        {
            for (auto &define : script->GetDefines())
            {
                defines[define->GetName()] = define->GetValue();
            }
        }

        if (_table->contains_qualified("enums"))
        {
            for (auto keyValuePairs : *_table->get("enums")->as_table())
            {
                string enumCategory = keyValuePairs.first;
                enumList_t enumList;
                for (auto &valueDescriptor : keyValuePairs.second->as_array()->get())
                {
                    const string &valueDescriptorString = valueDescriptor->as<string>()->get();
                    // Match this to what's in the defines.
                    if (valueDescriptorString.find('*') == string::npos)
                    {
                        auto it = defines.find(valueDescriptorString);
                        if (it != defines.end())
                        {
                            enumList[it->second] = it->first;
                        }
                    }
                    else
                    {
                        // Slower, wildcard match
                        regex pattern(valueDescriptorString, regex::icase);
                        for (auto &definePair : defines)
                        {
                            if (regex_match(definePair.first, pattern))
                            {
                                enumList[definePair.second] = definePair.first;
                            }
                        }
                    }
                }

                _enumLists[enumCategory] = enumList;
            }
        }
    }

    void _CacheCallParamTypes(const string &keyName, std::unordered_map<string, vector<string>> &storage)
    {
        if (_table->contains_qualified(keyName))
        {
            for (auto keyValuePairs : *_table->get(keyName)->as_table())
            {
                vector<string> types;
                for (auto type : keyValuePairs.second->as_array()->get())
                {
                    types.push_back(type->as<string>()->get());
                }
                storage[keyValuePairs.first] = types;
            }
        }
    }

    void _CacheMethodCallParamTypes()
    {
        _CacheCallParamTypes("methodParameterTypes", _methodCallParamTypes);
    }

    void _CacheKernelCallParamTypes()
    {
        _CacheCallParamTypes("kernelParameterTypes", _kernelCallParamTypes);
    }

    void _CacheSwitchValueTypes()
    {
        if (_table->contains_qualified("commonSwitchValueTypes"))
        {
            for (auto keyValuePairs : *_table->get("commonSwitchValueTypes")->as_table())
            {
                vector<string> types;
                auto theValue = keyValuePairs.second->as<string>();
                if (_enumLists.find(theValue->get()) != _enumLists.end())
                {
                    _switchValueTypes[keyValuePairs.first] = theValue->get();
                }
                else
                {
                    // TODO: Warn about invalid enum value?
                }
            }
        }
    }

    typedef std::unordered_map<uint16_t, string> enumList_t;
    std::unordered_map<string, enumList_t> _enumLists;
    std::unordered_map<string, vector<string>> _methodCallParamTypes;
    std::unordered_map<string, vector<string>> _kernelCallParamTypes;
    std::unordered_map<string, string> _switchValueTypes;

    std::unordered_set<string> _bitfieldProperties;

    std::unique_ptr<cpptoml::table> _table;
    std::shared_ptr<cpptoml::table> _methodParameterNames;

    const SelectorTable &_selectorTable;
};

std::unique_ptr<IDecompilerConfig> CreateDecompilerConfig(const GameFolderHelper &helper, const SelectorTable &selectorTable)
{
    return make_unique<DecompilerConfig>(helper, selectorTable);
}
