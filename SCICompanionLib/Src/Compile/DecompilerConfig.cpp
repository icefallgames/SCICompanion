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

using namespace std;
using namespace sci;
using namespace cpptoml;

class DummyLog : public ICompileLog
{
    void ReportResult(const CompileResult &result) override {} 
};

unique_ptr<Script> GetDefinesScript(const GameFolderHelper &helper)
{
    DummyLog log;
    ScriptId scriptId(helper.GetIncludeFolder() + "\\sci.sh");
    unique_ptr<Script> script = make_unique<Script>(scriptId);
    assert(scriptId.Language() == LangSyntaxSCIStudio);
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        if (!g_Parser.Parse(*script, stream, PreProcessorDefinesFromSCIVersion(helper.Version), &log))
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
        unique_ptr<Script> definesScript = GetDefinesScript(helper);

        string decompilerIniPath = helper.GetSrcFolder() + "\\Decompiler.ini";
        try
        {
            _table = make_unique<table>(parse_file(decompilerIniPath));
            if (_table->contains("methodParameterNames"))
            {
                _methodParameterNames = _table->get_table("methodParameterNames");
            }
            _CacheBitfieldProps();
            _CacheEnums(*definesScript);
            _CacheMethodCallParamTypes();
            _CacheKernelCallParamTypes();
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

private:

    void _ResolveParamStatement(sci::StatementsNode &statementNode, const vector<string> &types) const
    {
        size_t index = 0;
        for (auto &param : statementNode.GetStatements())
        {
            PropertyValue *value = SafeSyntaxNode<PropertyValue>(param->GetSyntaxNode());
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

    void _CacheEnums(Script &script)
    {
        map<string, uint16_t> defines;
        for (auto &define : script.GetDefines())
        {
            defines[define->GetName()] = define->GetValue();
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

    

    typedef std::unordered_map<uint16_t, string> enumList_t;
    std::unordered_map<string, enumList_t> _enumLists;
    std::unordered_map<string, vector<string>> _methodCallParamTypes;
    std::unordered_map<string, vector<string>> _kernelCallParamTypes;

    std::unordered_set<string> _bitfieldProperties;

    std::unique_ptr<cpptoml::table> _table;
    std::shared_ptr<cpptoml::table> _methodParameterNames;

    const SelectorTable &_selectorTable;
};

std::unique_ptr<IDecompilerConfig> CreateDecompilerConfig(const GameFolderHelper &helper, const SelectorTable &selectorTable)
{
    return make_unique<DecompilerConfig>(helper, selectorTable);
}
