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
#include "AutoDetectVariableNames.h"
#include "ScriptOMAll.h"
#include "format.h"
#include "DisassembleHelper.h"
#include "DecompilerConfig.h"

using namespace sci;
using namespace std;
using namespace fmt;

bool _IsUndeterminedHelper(const std::string &suggestion, const std::string &varType)
{
    return (0 == suggestion.compare(0, varType.length(), varType)) && isdigit(suggestion[varType.length()]);
}

bool _IsUndeterminedFunctionScope(const std::string &suggestion)
{
    return _IsUndeterminedHelper(suggestion, "temp") || _IsUndeterminedHelper(suggestion, "param");
}

bool _IsUndeterminedGlobalScope(const std::string &suggestion)
{
    return _IsUndeterminedHelper(suggestion, "global");
}

bool _IsUndetermined(const std::string &suggestion)
{
    return _IsUndeterminedGlobalScope(suggestion) ||
        _IsUndeterminedHelper(suggestion, "local") ||
        _IsUndeterminedFunctionScope(suggestion);
}

class TwoWayMap
{
public:
    void Set(const std::string &original, const std::string &finalSuggestion)
    {
        originalToRename[original] = finalSuggestion;
        renameToOriginal[finalSuggestion] = original;
    }
    std::string Get(const std::string &original)
    {
        auto it = originalToRename.find(original);
        if (it != originalToRename.end())
        {
            return it->second;
        }
        return original;
    }

    unordered_map<string, string> originalToRename;
    unordered_map<string, string> renameToOriginal;
};

class TwoWayMapHelper
{
public:
    bool IsAlreadyRenamed(const std::string &original) const
    {
        auto it = originals.find(original);
        return (it != originals.end());
    }
    bool IsAlreadyUsed(const std::string &finalSuggestion) const
    {
        return renameToOriginal.find(finalSuggestion) != renameToOriginal.end();
    }
    bool IsOriginalTheSame(const std::string &finalSuggestion, const std::string &original) const
    {
        return renameToOriginal.find(finalSuggestion)->second == original;
    }

    unordered_set<string> originals;
    unordered_map<string, string> renameToOriginal;
};

struct Suggestion
{
    Suggestion(const Suggestion &suggestion) = default;
    Suggestion &operator=(const Suggestion &suggestion) = default;
    Suggestion() {}
    Suggestion(const string &one) : PartOne(one) {}
    Suggestion(const string &one, const string &two) : PartOne(one), PartTwo(two) {}

    void clear() { PartOne.clear(); PartTwo.clear(); }
    bool empty() { return PartOne.empty() && PartTwo.empty(); }

    string PartOne;
    string PartTwo;
};

class RenameContext
{
public:
    RenameContext(Script &script, const IDecompilerConfig *config, CSCOFile *mainSCO, CSCOFile *scriptSCO) : _dirty(false), _mainSCO(mainSCO)
    {
        // Populate things from these SCOs. And store the main SCO in case we make mods
        CSCOFile *globalVarSCO = (mainSCO != nullptr) ? mainSCO : ((scriptSCO && scriptSCO->GetScriptNumber() == 0) ? scriptSCO : nullptr);
        if (globalVarSCO)
        {
            // Pluck out the global variables. They are ordered by index
            int index = 0;
            for (CSCOLocalVariable &globalVar : globalVarSCO->GetVariables())
            {
                string stdGlobalName = _GetGlobalVariableName(index);
                if (globalVar.GetName() != stdGlobalName)
                {
                    // It must have been given a name, so use it.
                    SetRenamed(nullptr, stdGlobalName, globalVar.GetName(), false);
                }
                index++;
            }
        }

        if (scriptSCO && (scriptSCO != globalVarSCO))
        {
            auto &decompiledScriptVarIt = script.GetScriptVariables().begin();
            // Use these as locals. The freshly decompiled script will still have things like local1 and local26.
            // We assume that the number and order of variables in the SCO is the same as in the decompiled script.
            int index = 0;
            for (CSCOLocalVariable &localVar : scriptSCO->GetVariables())
            {
                if (!localVar.GetName().empty()) // Empty ones are for padding arrays.
                {
                    if (decompiledScriptVarIt != script.GetScriptVariables().end())
                    {
                        string stdDecompiledName = (*decompiledScriptVarIt)->GetName();
                        if (localVar.GetName() != stdDecompiledName)
                        {
                            // It must have been given a name, so use it.
                            SetRenamed(nullptr, stdDecompiledName, localVar.GetName(), false);
                        }
                        index += (*decompiledScriptVarIt)->GetSize();
                        ++decompiledScriptVarIt;
                    }
                }
            }
        }

        // Next, pull method parameter names from Decompiler.ini
        if (config)
        {
            for (const auto &classDef : script.GetClasses())
            {
                for (const auto &method : classDef->GetMethods())
                {
                    int index = 1;  // Params start at 1
                    for (string &name : config->GetParameterNamesFor(classDef.get(), method->GetName()))
                    {
                        SetRenamed(method.get(), _GetParamVariableName(index), name, false);
                        index++;
                    }
                }
            }
        }

        _mainRenamesInfo.clear();
    }

    vector<pair<string, string>> IsMainDirty() { return _mainRenamesInfo; }

    void SetRenamed(FunctionBase *functionContext, const string &original, const string &suggestion, bool pushToMain = true)
    {
        TwoWayMap &twoWayMap = GetMap(functionContext, original);
        const TwoWayMapHelper twoWayMapHelper = GetMapHelper(functionContext, original);
        if (twoWayMapHelper.IsAlreadyRenamed(original))
        {
            return;
        }
        else
        {
            // Otherwise, use the suggestion. Unless the suggestion has already been used for something else.
            int suffix = 2;
            string finalSuggestion = suggestion;
            while (twoWayMapHelper.IsAlreadyUsed(finalSuggestion))
            {
                // Someone has already suggested this. That's fine if it matches the original.
                if (twoWayMapHelper.IsOriginalTheSame(finalSuggestion, original))
                {
                    return;
                }
                // Otherwise, keep trying variations until we have one that isn't found.
                finalSuggestion = format("{0}_{1}", suggestion, suffix);
                suffix++;
            }

            // First time this has been suggested.
            _dirty = true;
            twoWayMap.Set(original, finalSuggestion);

            if (pushToMain && _IsUndeterminedGlobalScope(original))
            {
                _PushToMain(original, finalSuggestion);
            }
        }
    }

    string GetRenamed(FunctionBase *functionContext, const string &original)
    {
        TwoWayMap &twoWayMap = GetMap(functionContext, original);
        return twoWayMap.Get(original);
    }

    void ClearDirty() { _dirty = false;  }
    bool IsDirty() { return _dirty; }

private:
    TwoWayMap &GetMap(FunctionBase *functionContext, const string &original)
    {
        if (_IsUndeterminedFunctionScope(original))
        {
            assert(functionContext && "Function scope variable not in a function");
            return functionMaps[functionContext];
        }
        else
        {
            return localMap;
        }
    }

    const TwoWayMapHelper GetMapHelper(FunctionBase *functionContext, const string &original)
    {
        TwoWayMapHelper helper;
        if (_IsUndeterminedFunctionScope(original))
        {
            assert(functionContext && "Function scope variable not in a function");
            // A temp var needs to be checked against local and the current map
            for (auto &oToNPair : functionMaps[functionContext].originalToRename)
            {
                helper.originals.insert(oToNPair.first);
                helper.renameToOriginal[oToNPair.second] = oToNPair.first;
            }
            for (auto &oToNPair : localMap.originalToRename)
            {
                helper.originals.insert(oToNPair.first);
                helper.renameToOriginal[oToNPair.second] = oToNPair.first;
            }
        }
        else
        {
            // A local var. Needs to be checked against all maps
            for (auto &functionMap : functionMaps)
            {
                for (auto &oToNPair : functionMap.second.originalToRename)
                {
                    helper.originals.insert(oToNPair.first);
                    helper.renameToOriginal[oToNPair.second] = oToNPair.first;
                }
            }
            // Including the local map of course...
            for (auto &oToNPair : localMap.originalToRename)
            {
                helper.originals.insert(oToNPair.first);
                helper.renameToOriginal[oToNPair.second] = oToNPair.first;
            }
        }
        return helper;
    }

    void _PushToMain(const string &original, const string &suggestion)
    {
        if (_mainSCO)
        {
            string numberString = original.substr(lstrlenA("global"), string::npos);
            int number = stoi(numberString);
            assert(number < (int)_mainSCO->GetVariables().size());
            if (number < (int)_mainSCO->GetVariables().size())
            {
                _mainSCO->GetVariables()[number].SetName(suggestion);
            }
            _mainRenamesInfo.emplace_back(original, suggestion);
        }
    }

    bool _dirty;
    std::vector<std::pair<std::string, std::string>> _mainRenamesInfo;
    TwoWayMap localMap;
    CSCOFile *_mainSCO;
    unordered_map<FunctionBase*, TwoWayMap> functionMaps;
};

void AppendUpper(string &value, const string &append)
{
    if (value.empty())
    {
        value += append;
    }
    else
    {
        value += toupper(append[0]);
        if (!append.empty())
        {
            value += append.substr(1, string::npos);
        }
        else
        {
            value += "ERROR_VAR_SUGGESTION";
        }
    }
}

string ResolveSuggestion(const Suggestion &suggestion, const string &original, const ClassDefinition *classDef)
{
    // TODO: do different things for global vs local. And if "self"  then classdef comes into play?
    string baseValue;
    bool isGlobal = _IsUndeterminedGlobalScope(original);
    if (isGlobal)
    {
        AppendUpper(baseValue, "g");
    }

    string partOne = suggestion.PartOne;
    if (partOne == "self" && classDef)
    {
        partOne = classDef->IsInstance() ? classDef->GetSpecies() : classDef->GetName();
    }

    if (suggestion.PartTwo.empty())
    {
        if (!isGlobal)
        {
            AppendUpper(baseValue, "the");
        }
        AppendUpper(baseValue, partOne);
    }
    else
    {
        AppendUpper(baseValue, partOne);
        AppendUpper(baseValue, suggestion.PartTwo);
    }

    // We should always start vars with lower case
    baseValue[0] = tolower(baseValue[0]);

    // We can't randomly have vars with '-' in them. So replace with '_'
    for (size_t i = 0; i < baseValue.size(); i++)
    {
        if (baseValue[i] == '-')
        {
            baseValue[i] = '_';
        }
    }
    return baseValue;
}

class TrackVariablesNames : public ISyntaxNodeVisitor
{
public:
    TrackVariablesNames(RenameContext &renameContext) : _renameContext(renameContext) {}

    string ResolveSuggestion(const Suggestion &suggestion, const string &original)
    {
        return ::ResolveSuggestion(suggestion, original, _classDef);
    }

    void Visit(const Script &script) override {}
    void Visit(const ClassDefinition &classDef) override {}
    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}
    void Visit(const PropertyValue &prop) override
    {
        if (((prop.GetType() == ValueType::Token) || (prop.GetType() == ValueType::Pointer)) && _IsUndetermined(prop.GetStringValue()))
        {
            _renameContext.SetRenamed(_function, prop.GetStringValue(), ResolveSuggestion(_suggestion, prop.GetStringValue()));
        }
    }
    void Visit(const ComplexPropertyValue &prop) override
    {
        if (((prop.GetType() == ValueType::Token) || (prop.GetType() == ValueType::Pointer)) && _IsUndetermined(prop.GetStringValue()))
        {
            _renameContext.SetRenamed(_function, prop.GetStringValue(), ResolveSuggestion(_suggestion, prop.GetStringValue()));
        }
    }
    void Visit(const Define &define) override {}
    void Visit(const ClassProperty &classProp) override {}
    void Visit(const VariableDecl &varDecl) override {}
    void Visit(const MethodDefinition &function) override {}
    void Visit(const ProcedureDefinition &function) override {}
    void Visit(const Synonym &syn) override {}
    void Visit(const CodeBlock &block) override {}
    void Visit(const ConditionalExpression &conditinoal) override {}
    void Visit(const Comment &comment) override {}
    void Visit(const SendParam &sendParam) override {}
    void Visit(const LValue &lValue) override
    {
        if (_IsUndetermined(lValue.GetName()))
        {
            _renameContext.SetRenamed(_function, lValue.GetName(), ResolveSuggestion(_suggestion, lValue.GetName()));
        }
    }
    void Visit(const RestStatement &rest) override {}
    void Visit(const Cast &cast) override {}
    void Visit(const SendCall &sendCall) override
    {
        // Figure this out later.
    }
    void Visit(const ProcedureCall &procCall) override {}
    void Visit(const ReturnStatement &ret) override {}
    void Visit(const ForLoop &forLoop) override {}
    void Visit(const WhileLoop &whileLoop) override {}
    void Visit(const DoLoop &doLoop) override {}
    void Visit(const BreakStatement &breakStatement) override {}
    void Visit(const ContinueStatement &breakStatement) override {}
    void Visit(const CaseStatement &caseStatement) override {}
    void Visit(const SwitchStatement &switchStatement) override {}
    void Visit(const CondStatement &switchStatement) override{}
    void Visit(const Assignment &assignment) override {}
    void Visit(const BinaryOp &binaryOp) override {}
    void Visit(const UnaryOp &unaryOp) override {}
    void Visit(const IfStatement &ifStatement) override {}
    void Visit(const Asm &asmSection) override {}
    void Visit(const AsmBlock &asmBlock) override {}
    void Visit(const ExportEntry &exportEntry) override {}
    void Visit(const WeakSyntaxNode &weakNode) override {}
    void Visit(const NaryOp &weakNode) override {}
    void Visit(const ClassDefDeclaration &classDef) override {}
    void Visit(const SelectorDeclaration &selectorDef) override {}
    void Visit(const GlobalDeclaration &globalDecl) override {}
    void Visit(const ExternDeclaration &externDecl) override {}

    void Enter(const SyntaxNode &node) override {}
    void Leave(const SyntaxNode &node) override {}

    void SetSuggestion(const Suggestion &suggestion)
    {
        _suggestion = suggestion;
    }
    void SetFunctionContext(FunctionBase *function)
    {
        _function = function;
    }
    void SetClassContext(ClassDefinition *classDef)
    {
        _classDef = classDef;
    }

private:
    FunctionBase *_function;
    ClassDefinition *_classDef;
    Suggestion _suggestion;
    RenameContext &_renameContext;
};

class SuggestVariableNames : public ISyntaxNodeVisitor
{
public:
    void Visit(const Script &script) override {}
    void Visit(const ClassDefinition &classDef) override {}
    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}
    void Visit(const PropertyValue &prop) override
    {
        if ((prop.GetType() == ValueType::Token) && !_IsUndetermined(prop.GetStringValue()))
        {
            ReturnSuggestion(Suggestion(prop.GetStringValue()));
        }
    }
    void Visit(const ComplexPropertyValue &prop) override
    {
        if ((prop.GetType() == ValueType::Token) && !_IsUndetermined(prop.GetStringValue()))
        {
            ReturnSuggestion(Suggestion(prop.GetStringValue()));
        }
    }
    void Visit(const Define &define) override {}
    void Visit(const ClassProperty &classProp) override {}
    void Visit(const VariableDecl &varDecl) override {}
    void Visit(const MethodDefinition &function) override {}
    void Visit(const ProcedureDefinition &function) override {}
    void Visit(const Synonym &syn) override {}
    void Visit(const CodeBlock &block) override {}
    void Visit(const ConditionalExpression &conditinoal) override {}
    void Visit(const Comment &comment) override {}
    void Visit(const SendParam &sendParam) override {}
    void Visit(const LValue &lValue) override
    {
        if (!_IsUndetermined(lValue.GetName()))
        {
            ReturnSuggestion(Suggestion(lValue.GetName()));
        }
    }
    void Visit(const RestStatement &rest) override {}
    void Visit(const Cast &cast) override {}
    void Visit(const SendCall &sendCall) override
    {
        // Make a suggestion if we have just one send selector with zero params.
        if ((sendCall.GetParams().size() == 1) && !sendCall.GetObject().empty() && !_IsUndetermined(sendCall.GetObject()))
        {
            auto &param = sendCall.GetParams()[0];
            if (param->GetSelectorParams().size() == 0)
            {
                if (param->GetSelectorName() != "new")
                {
                    ReturnSuggestion(Suggestion(sendCall.GetObject(), param->GetSelectorName()));
                }
                else
                {
                    // We'd rather have "newEvent" than "EventNew"
                    ReturnSuggestion(Suggestion(param->GetSelectorName(), sendCall.GetObject()));
                }
            }
        }
    }
    void Visit(const ProcedureCall &procCall) override {}
    void Visit(const ReturnStatement &ret) override {}
    void Visit(const ForLoop &forLoop) override {}
    void Visit(const WhileLoop &whileLoop) override {}
    void Visit(const DoLoop &doLoop) override {}
    void Visit(const BreakStatement &breakStatement) override {}
    void Visit(const ContinueStatement &breakStatement) override {}
    void Visit(const CaseStatement &caseStatement) override {}
    void Visit(const SwitchStatement &switchStatement) override {}
    void Visit(const CondStatement &switchStatement) override {}
    void Visit(const Assignment &assignment) override {}
    void Visit(const BinaryOp &binaryOp) override {}
    void Visit(const UnaryOp &unaryOp) override {}
    void Visit(const IfStatement &ifStatement) override {}
    void Visit(const Asm &asmSection) override {}
    void Visit(const AsmBlock &asmBlock) override {}
    void Visit(const ExportEntry &exportEntry) override {}
    void Visit(const WeakSyntaxNode &weakNode) override {}
    void Visit(const NaryOp &weakNode) override {}
    void Visit(const ClassDefDeclaration &classDef) override {}
    void Visit(const SelectorDeclaration &selectorDef) override {}
    void Visit(const GlobalDeclaration &globalDecl) override {}
    void Visit(const ExternDeclaration &externDecl) override {}

    void Enter(const SyntaxNode &node) override {}
    void Leave(const SyntaxNode &node) override {}

    void Clear() { CurrentSuggestion.clear(); }
    Suggestion CurrentSuggestion;

private:
    void ReturnSuggestion(const Suggestion &suggestion)
    {
        CurrentSuggestion = suggestion;
    }
};

// This class is responsible for identifying areas where we can deduce variable names, getting the suggested
// variable name, and then tracking that data.
class DetectVariableNames : public IExploreNode
{
public:
    DetectVariableNames(RenameContext &renameContext) : trackVarNames(renameContext) {  }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeFunction:
                {
                    trackVarNames.SetFunctionContext(static_cast<FunctionBase*>(&node));
                }
                    break;

                case NodeTypeClassDefinition:
                {
                    trackVarNames.SetClassContext(static_cast<ClassDefinition*>(&node));
                }
                break;

                case NodeTypeAssignment:
                {
                    // Let's start simple. An assignment is something that can transfer var names.
                    Assignment *assignment = SafeSyntaxNode<Assignment>(&node);
                    suggestVarNames.Clear();
                    // Let's visit both.
                    assignment->GetStatement1()->Accept(suggestVarNames);
                    assignment->_variable->Accept(suggestVarNames);

                    // Now let's push down
                    if (!suggestVarNames.CurrentSuggestion.empty())
                    {
                        trackVarNames.SetSuggestion(suggestVarNames.CurrentSuggestion);
                        assignment->GetStatement1()->Accept(trackVarNames);
                        assignment->_variable->Accept(trackVarNames);
                    }
                }
                    break;
            }
        }
        else if (state == ExploreNodeState::Post)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeFunction:
                {
                    trackVarNames.SetFunctionContext(nullptr);
                }
                break;

                case NodeTypeClassDefinition:
                {
                    trackVarNames.SetClassContext(nullptr);
                }
                break;

            }
        }
    }

private:
    SuggestVariableNames suggestVarNames;
    TrackVariablesNames trackVarNames;
};

class ApplyVariableNames : public IExploreNode
{
public:
    ApplyVariableNames(RenameContext &renameContext) : _renameContext(renameContext) {  }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeRest:
                {
                    RestStatement &rest = static_cast<RestStatement&>(node);
                    rest.SetName(_renameContext.GetRenamed(_functionContext, rest.GetName()));
                }
                break;

                case NodeTypeFunction:
                {
                    _functionContext = static_cast<FunctionBase*>(&node);
                }
                break;

                case NodeTypeValue:
                case NodeTypeComplexValue:
                {
                    PropertyValueBase &propValue = static_cast<PropertyValueBase&>(node);
                    if ((propValue.GetType() == ValueType::Token) || (propValue.GetType() == ValueType::Pointer))
                    {
                        propValue.SetValue(_renameContext.GetRenamed(_functionContext, propValue.GetStringValue()), propValue.GetType());
                    }
                }
                break;

                case NodeTypeLValue:
                {
                    LValue &lValue = static_cast<LValue&>(node);
                    lValue.SetName(_renameContext.GetRenamed(_functionContext, lValue.GetName()));
                }
                break;

                case NodeTypeFunctionParameter:
                {
                    FunctionParameter &functionParam = static_cast<FunctionParameter&>(node);
                    functionParam.SetName(_renameContext.GetRenamed(_functionContext, functionParam.GetName()));
                }
                break;

                case NodeTypeVariableDeclaration:
                {
                    VariableDecl &varDecl = static_cast<VariableDecl&>(node);
                    varDecl.SetName(_renameContext.GetRenamed(_functionContext, varDecl.GetName()));
                }
                break;
            }
        }
        else if (state == ExploreNodeState::Post)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeFunction:
                {
                    _functionContext = nullptr;
                }
                break;
            }
        }
    }


private:
    FunctionBase *_functionContext;
    RenameContext &_renameContext;
};


void AutoDetectVariableNames(Script &script, const IDecompilerConfig *config, CSCOFile *mainSCO, CSCOFile *scriptSCO, vector<pair<string, string>> &mainDirtyRenames)
{
    RenameContext renameContext(script, config, mainSCO, scriptSCO);

    // Iteratively figure out variable names. Each cycle can propagate
    // variable names one more step.
    // e.g.
    // (= temp0 client)
    // (= temp1 (send temp0:x(5)))
    //****
    // (= theClient client)
    // (= temp1 (send theClient:x(5)))
    //****
    // (= theClient client)
    // (= theClientX (send theClient:x(5)))
    do
    {
        renameContext.ClearDirty();
        DetectVariableNames detectVarNames(renameContext);
        script.Traverse(detectVarNames);
        ApplyVariableNames applyVarNames(renameContext);
        script.Traverse(applyVarNames);
    } while (renameContext.IsDirty());

    mainDirtyRenames = renameContext.IsMainDirty();
}