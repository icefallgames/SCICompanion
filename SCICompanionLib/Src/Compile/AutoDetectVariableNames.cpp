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

bool _IsUndeterminedFunctionScope(const std::string &suggestion)
{
    return (0 == suggestion.compare(0, 4, "temp")) || ((0 == suggestion.compare(0, 5, "param")) && (suggestion != "paramTotal"));
}

bool _IsUndeterminedGlobalScope(const std::string &suggestion)
{
    return (0 == suggestion.compare(0, 6, "global"));
}

bool _IsUndetermined(const std::string &suggestion)
{
    return _IsUndeterminedGlobalScope(suggestion) ||
        (0 == suggestion.compare(0, 5, "local")) ||
        _IsUndeterminedFunctionScope(suggestion);
}

struct TwoWayMap
{
    unordered_map<string, string> originalToRename;
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
    RenameContext(Script &script, const IDecompilerConfig *config, CSCOFile *mainSCO, CSCOFile *scriptSCO) : _dirty(false), _mainDirty(false), _mainSCO(mainSCO)
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

        _mainDirty = false;
    }

    bool IsMainDirty() { return _mainDirty; }

    void SetRenamed(FunctionBase *functionContext, const string &original, const string &suggestion, bool pushToMain = true)
    {
        TwoWayMap &twoWayMap = GetMap(functionContext, original);
        // If this guy has already been renamed, use what we have already
        auto it = twoWayMap.originalToRename.find(original);
        if (it != twoWayMap.originalToRename.end())
        {
            return;
        }
        else
        {
            // Otherwise, use the suggestion. Unless the suggestion has already been used for something else.
            int suffix = 2;
            string finalSuggestion = suggestion;
            while (twoWayMap.renameToOriginal.find(finalSuggestion) != twoWayMap.renameToOriginal.end())
            {
                // Someone has already suggested this. That's fine if it matches the original.
                if (twoWayMap.renameToOriginal[finalSuggestion] == original)
                {
                    return;
                }
                // Otherwise, keep trying variations until we have one that isn't found.
                finalSuggestion = format("{0}_{1}", suggestion, suffix);
                suffix++;
            }

            // First time this has been suggested.
            _dirty = true;
            twoWayMap.originalToRename[original] = finalSuggestion;
            twoWayMap.renameToOriginal[finalSuggestion] = original;

            if (pushToMain && _IsUndeterminedGlobalScope(original))
            {
                _PushToMain(original, suggestion);
            }
        }
    }

    string GetRenamed(FunctionBase *functionContext, const string &original)
    {
        TwoWayMap &twoWayMap = GetMap(functionContext, original);
        auto it = twoWayMap.originalToRename.find(original);
        if (it != twoWayMap.originalToRename.end())
        {
            return it->second;
        }
        return original;
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
            _mainDirty = true;
        }
    }

    bool _dirty;
    bool _mainDirty;
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
    void Visit(const CaseStatement &caseStatement) override {}
    void Visit(const SwitchStatement &switchStatement) override {}
    void Visit(const Assignment &assignment) override {}
    void Visit(const BinaryOp &binaryOp) override {}
    void Visit(const UnaryOp &unaryOp) override {}
    void Visit(const CppIfStatement &ifStatement) override {}
    void Visit(const Asm &asmSection) override {}
    void Visit(const AsmBlock &asmBlock) override {}
    void Visit(const ExportEntry &exportEntry) override {}
    void Visit(const CondClauseStatement &condClause) override {}
    void Visit(const CondStatement &cond) override {}
    void Visit(const RepeatStatement &repeat) override {}
    void Visit(const BreakIfStatement &breakIf) override {}

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
    void Visit(const CaseStatement &caseStatement) override {}
    void Visit(const SwitchStatement &switchStatement) override {}
    void Visit(const Assignment &assignment) override {}
    void Visit(const BinaryOp &binaryOp) override {}
    void Visit(const UnaryOp &unaryOp) override {}
    void Visit(const CppIfStatement &ifStatement) override {}
    void Visit(const Asm &asmSection) override {}
    void Visit(const AsmBlock &asmBlock) override {}
    void Visit(const ExportEntry &exportEntry) override {}
    void Visit(const CondClauseStatement &condClause) override {}
    void Visit(const CondStatement &cond) override {}
    void Visit(const RepeatStatement &repeat) override {}
    void Visit(const BreakIfStatement &breakIf) override {}

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


void AutoDetectVariableNames(Script &script, const IDecompilerConfig *config, CSCOFile *mainSCO, CSCOFile *scriptSCO, bool &mainDirty)
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

    mainDirty = renameContext.IsMainDirty();
}