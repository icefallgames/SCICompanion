#include "stdafx.h"
#include "AutoDetectVariableNames.h"
#include "ScriptOMAll.h"
#include "format.h"

using namespace sci;
using namespace std;
using namespace fmt;

bool _IsUndeterminedFunctionScope(const std::string &suggestion)
{
    return (0 == suggestion.compare(0, 4, "temp")) || ((0 == suggestion.compare(0, 5, "param")) && (suggestion != "paramTotal"));
}

bool _IsUndetermined(const std::string &suggestion)
{
    return (0 == suggestion.compare(0, 6, "global")) ||
        (0 == suggestion.compare(0, 5, "local")) ||
        _IsUndeterminedFunctionScope(suggestion);
}

struct TwoWayMap
{
    unordered_map<string, string> originalToRename;
    unordered_map<string, string> renameToOriginal;
};

class RenameContext
{
public:
    RenameContext() : _dirty(false) {}

    void SetRenamed(FunctionBase *functionContext, const string &original, const string &suggestion)
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

    bool _dirty;
    TwoWayMap localMap;
    unordered_map<FunctionBase*, TwoWayMap> functionMaps;
};

class TrackVariablesNames : public ISyntaxNodeVisitor
{
public:
    TrackVariablesNames(RenameContext &renameContext) : _renameContext(renameContext) {}

    void Visit(const Script &script) override {}
    void Visit(const ClassDefinition &classDef) override {}
    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}
    void Visit(const PropertyValue &prop) override
    {
        if ((prop.GetType() == ValueType::Token) && _IsUndetermined(prop.GetStringValue()))
        {
            _renameContext.SetRenamed(_function, prop.GetStringValue(), _suggestion);
        }
    }
    void Visit(const ComplexPropertyValue &prop) override
    {
        if ((prop.GetType() == ValueType::Token) && _IsUndetermined(prop.GetStringValue()))
        {
            _renameContext.SetRenamed(_function, prop.GetStringValue(), _suggestion);
        }
    }
    void Visit(const Define &define) override {}
    void Visit(const ClassProperty &classProp) override {}
    void Visit(const SingleStatement &statement) override
    {
        statement.GetSyntaxNode()->Accept(*this);
    }
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
            _renameContext.SetRenamed(_function, lValue.GetName(), _suggestion);
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

    void SetSuggestion(const string &suggestion)
    {
        _suggestion = suggestion;
    }
    void SetFunctionContext(FunctionBase *function)
    {
        _function = function;
    }

private:
    FunctionBase *_function;
    string _suggestion;
    RenameContext &_renameContext;
};

class SuggestVariableNames : public ISyntaxNodeVisitor
{
private:
    string _PrependThe(const string &original)
    {
        return _Combine("the", original);
    }

    string _Combine(const string &first, const string &second)
    {
        string value = first;
        value += toupper(second[0]);
        value += second.substr(1, string::npos);
        return value;
    }

public:
    void Visit(const Script &script) override {}
    void Visit(const ClassDefinition &classDef) override {}
    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}
    void Visit(const PropertyValue &prop) override
    {
        if ((prop.GetType() == ValueType::Token) && !_IsUndetermined(prop.GetStringValue()))
        {
            ReturnSuggestion(_PrependThe(prop.GetStringValue()));
        }
    }
    void Visit(const ComplexPropertyValue &prop) override
    {
        if ((prop.GetType() == ValueType::Token) && !_IsUndetermined(prop.GetStringValue()))
        {
            ReturnSuggestion(_PrependThe(prop.GetStringValue()));
        }
    }
    void Visit(const Define &define) override {}
    void Visit(const ClassProperty &classProp) override {}
    void Visit(const SingleStatement &statement) override
    {
        statement.GetSyntaxNode()->Accept(*this);
    }
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
            ReturnSuggestion(_PrependThe(lValue.GetName()));
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
                ReturnSuggestion(_Combine(sendCall.GetObject(), param->GetSelectorName()));
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

    void Clear() { Suggestion.clear(); }
    string Suggestion;

private:
    void ReturnSuggestion(const std::string &suggestion)
    {
        Suggestion = suggestion;
    }
};

// This class is responsible for identifying areas where we can deduce variable names, getting the suggested
// variable name, and then tracking that data.
class DetectVariableNames : public IExploreNodeContext, public IExploreNode
{
public:
    DetectVariableNames(RenameContext &renameContext) : trackVarNames(renameContext) {  }

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
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

                case NodeTypeAssignment:
                {
                    // Let's start simple. An assignment is something that can transfer var names.
                    Assignment *assignment = SafeSyntaxNode<Assignment>(&node);
                    suggestVarNames.Clear();
                    // Let's visit both.
                    assignment->GetStatement1()->Accept(suggestVarNames);
                    assignment->_variable->Accept(suggestVarNames);

                    // Now let's push down
                    if (!suggestVarNames.Suggestion.empty())
                    {
                        trackVarNames.SetSuggestion(suggestVarNames.Suggestion);
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
            }
        }
    }

private:
    SuggestVariableNames suggestVarNames;
    TrackVariablesNames trackVarNames;
};

class ApplyVariableNames : public IExploreNodeContext, public IExploreNode
{
public:
    ApplyVariableNames(RenameContext &renameContext) : _renameContext(renameContext) {  }

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeFunction:
                {
                    _functionContext = static_cast<FunctionBase*>(&node);
                }
                break;

                case NodeTypeValue:
                case NodeTypeComplexValue:
                {
                    PropertyValueBase &propValue = static_cast<PropertyValueBase&>(node);
                    if (propValue.GetType() == ValueType::Token)
                    {
                        propValue.SetValue(_renameContext.GetRenamed(_functionContext, propValue.GetStringValue()), ValueType::Token);
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


void AutoDetectVariableNames(Script &script)
{
    RenameContext renameContext;

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
        script.Traverse(&detectVarNames, detectVarNames);
        ApplyVariableNames applyVarNames(renameContext);
        script.Traverse(&applyVarNames, applyVarNames);
    } while (renameContext.IsDirty());
}