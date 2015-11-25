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
#include "ScriptOMAll.h"
#include "OutputCodeHelper.h"
#include "OutputSourceCodeBase.h"
#include "PMachine.h"
#include "StringUtil.h"
#include "CompiledScript.h"
#include "AppState.h"

// Still a WIP.

/*
    Suggested syntax:
    
    classes: same as SCIStudio 

    define:
        (define name const-exp) ; can be declared in methods too.

    Procedure call:
        (ProcName param0 param1 param2)

    send call:
        (object selector: param0 pararm1, selector2: param0 param1)

    prop retrieval:
        (object prop?)

    procedure/method definition:
        (method (methodName param0 param1 &tmp temp0 temp1 temp2)
            statement0
            statement1
            statement2
        )

        ; procedures can be defined inside a class too... (e.g. see Timer.sc)
        (procedure (procName param0 param2)
            statement0
            statement1
            statement2
        )

    if/else:
        (if expression statement0 statement1 statement2 else statement0 statement1 statement2)

    cond:
        ; else is actually optional
        (cond (expression0 statement0 statement1) (expression1 statement0 statement1) (else statement90 statement1))

    repeat:
        (repeat statement0 statement1 statement2)

    while:
        (while expression statement1 statement2 statement3)

    breakif:
        (breakif expression)

    break:
        (break optionalNumberOfLevels)

    return:
        (return optionalexpression)

    for:
        // Unsure about parens here:
        (for (pre0 pre1 pre2) expression (post0 post1 post2) statement0 statement1 statement2)

    operators:
        same as c++

    switch:
        (switch expression (value statement0 statement2) (value2 statement0 statement1) (else statement0 statement1))

        switchto:
        (switch expression (statement0 statement2) (statement0 statement1) (statement0 statement1))

    arrays:
        [variableName index]

    array declaration (e.g. in parameter list):
        [variableName const-exp]

    property declaration:
        (properties
            propName const-exp
            // or possibly (both are used):
            propName: const-exp
        )

    number literals:
        $0440   ; hex
        440     ; dec
        %010111 ; binary
        `c      ; character
        `@C     ; alt-c
        `^C     ; ctrl-c
        `+      ; plus sign
        `#4     ; not sure

    assignment, etc:
        (assignmentOp lValue rValue)

    expression
        // pretty much all operators operate on n values
        (operator value0 value1 value2 value3)

    local variables:
        (local
            var1
            var2 = [2 4 19 23 2 4]
            [buffer 100]
            [buffer 20] = [1 2 3 4 5 6 7 8]
        )

    exports
        (public
            instancename0 optionalExportNumber
            procname0 optionalExportNumber
            procname2 optionalExportNumber
        )

    rest:
        &rest   ; no parameters... means after the last named parameter
        &rest p ; this is ok too. It can be any parameter.

    enum:
        ; exactly like defines, they can get converted into them.
        (enum optionalNumber ; starts at 0 if not specified
            NAME1
            NAME2
            NAME3
            anotherOptionalNumber
            NAME4
            NAME6
            NAME7
        )

    selector literals:
        #name
        ;sometimes, the following. Seems completely optional. We should error out on it.
        #name:

    Other:
        operators must be separated by whitespace from their arguments, e.g.
            (++i) is not allowed.

    Calling a selector given by a variable. This is a bit weird, parses like a procedure, will need to be resolved later? Or it doesn't matter.
        (procedure (Eval obj sel)
            (return (obj sel &rest))
        )
*/

using namespace sci;
using namespace std;

class SimpleDefineLookup : public ILookupDefine
{
public:
    bool LookupDefine(const string &str, uint16_t &wValue) override
    {
        if (str == "TRUE")
        {
            wValue = 1;
            return true;
        }
        else if (str == "FALSE")
        {
            wValue = 0;
            return true;
        }
        else
        {
            return false;
        }
    }
};

SimpleDefineLookup simpleDefineLookup;

// Attemps to remove any explicit &rest parameters, and possibly a function parameter.
class TransformCleanRests : public IExploreNode
{
public:
    TransformCleanRests(sci::Script &script) { script.Traverse(*this); }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (node.GetNodeType() == NodeType::NodeTypeFunction)
        {
            if (state == ExploreNodeState::Pre)
            {
                _functionSig = (static_cast<FunctionBase*>(&node))->GetSignaturesNC()[0].get();
                _rests.clear();
                _explicitVarUsage.clear();
            }
            else
            {
                // Is the last function parameter only used by rest?
                if (!_functionSig->GetParams().empty())
                {
                    string lastParamName = _functionSig->GetParams().back()->GetName();
                    if (_explicitVarUsage.find(lastParamName) == _explicitVarUsage.end())
                    {
                        // Last parameter is never used, other than possibly in rests. Remove it:
                        _functionSig->GetParams().pop_back();

                        // And find any &rest statements that used it, and remove it:
                        for (auto &rest : _rests)
                        {
                            if (rest->GetName() == lastParamName)
                            {
                                rest->SetName("");
                            }
                        }
                    }
                }

                _functionSig = nullptr;
            }
        }
        else
        {
            if (state == ExploreNodeState::Pre)
            {
                switch (node.GetNodeType())
                {
                    // Keep track of all rests.
                    case NodeType::NodeTypeRest:
                        {
                            _rests.push_back(SafeSyntaxNode<RestStatement>(&node));
                        }
                        break;

                    case NodeType::NodeTypeValue:
                    case NodeType::NodeTypeComplexValue:
                        PropertyValueBase *value = static_cast<PropertyValueBase*>(&node);
                        if (value->GetType() == ValueType::Token)
                        {
                            _explicitVarUsage.insert(value->GetStringValue());
                        }
                        break;
                }
            }
        }
    }

private:
    FunctionSignature *_functionSig;
    vector<RestStatement*> _rests;
    std::set<string> _explicitVarUsage;
};

class TransformDeterminePropSelectors : public IExploreNode
{
public:
    TransformDeterminePropSelectors(sci::Script &script)
    {
        // We need to determine what is a property and what is a method
        if (_lookups.Load(appState->GetResourceMap().Helper()))
        {
            for (auto &script : _lookups.GetGlobalClassTable().GetAllScripts())
            {
                for (auto &object : script->GetObjects())
                {
                    for (uint16_t propSelector : object->GetProperties())
                    {
                        _propSelectors.insert(propSelector);
                    }
                    for (uint16_t methodSelector : object->GetMethods())
                    {
                        _methodSelectors.insert(methodSelector);
                    }
                }
            }
        }

        script.Traverse(*this);
    }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            SendParam *sendParam = SafeSyntaxNode<SendParam>(&node);
            if (sendParam)
            {
                uint16_t selector;
                if (_lookups.GetSelectorTable().ReverseLookup(sendParam->GetName(), selector))
                {
                    sendParam->SetIsMethod((_propSelectors.find(selector) == _propSelectors.end()) ||
                        (_methodSelectors.find(selector) != _methodSelectors.end()));
                }
            }
        }
    }
private:
    std::set<uint16_t> _propSelectors;
    std::set<uint16_t> _methodSelectors;
    GlobalCompiledScriptLookups _lookups;
};

vector<pair<string, string>> binOpConvert =
{
    { "<>", "!=" },
    { "&&", "and" },
    { "||", "or" },
};

vector<pair<string, string>> unaryOpConvert =
{
    { "bnot", "~" },
    { "neg", "-" },
};

// Others can be coalesced too (e.g. the less-than/greater-than), but we'd need
// to re-arrange code.
std::set<string> coalesceBinaryOps =
{
    "&&",
    "||",
    "|",
    "&",
    "+",
    "*",
};

string GetOperatorName(const sci::NamedNode &namedNode, const vector<pair<string, string>> &conversions)
{
    for (auto &conversion : conversions)
    {
        if (namedNode.GetName() == conversion.first)
        {
            return conversion.second;
        }
    }
    return namedNode.GetName();
}

void ConvertToSCISyntaxHelper(Script &script)
{
    TransformDeterminePropSelectors txDeterminePropSelectors(script);

    // Transform comments
    for (auto &comment : script.GetComments())
    {
        string text = comment->GetSanitizedText();

        int minTabCount;
        text = Unindent<'\t'>(text, &minTabCount);

        // If the first line of a comment isn't indented, we should indent the rest either.
        int innerCommentTab = 0;
        if (comment->GetPosition().Column() < minTabCount)
        {
            innerCommentTab = minTabCount - comment->GetPosition().Column();
            minTabCount = comment->GetPosition().Column();
        }

        vector<string> lines = Lineify(text);
        string newComment;
        bool first = true;
        for (string &line : lines)
        {
            if (!first)
            {
                newComment += "\n";
            }
            // "indent" each line.
            std::fill_n(std::back_inserter(newComment), minTabCount, '\t');
            newComment += ";";
            std::fill_n(std::back_inserter(newComment), innerCommentTab, '\t');
            if (!line.empty() && line[0] != ' ')
            {
                newComment += " ";
            }
            newComment += line;
            first = false;
        }
        comment->SetName(newComment);
    }

    TransformCleanRests txCleanRests(script);
}

class OutputSCISourceCode : public OutputSourceCodeBase
{
public:
    OutputSCISourceCode(SourceCodeWriter &out) : OutputSourceCodeBase(out) {}

    void Visit(const Script &script) override
    {
        out.OutputInitialComment();

        ScriptId scriptId = script.GetScriptId();
        if (!script.GetScriptNumberDefine().empty())
        {
            out.out << "(script# " << script.GetScriptNumberDefine() << ")" << out.NewLineString();
        }
        else
        {
            if (scriptId.GetResourceNumber() == InvalidResourceNumber)
            {
                // This is a legit (e.g. for outputting header files)
            }
            else
            {
                out.out << "(script# " << scriptId.GetResourceNumber() << ")" << out.NewLineString();
            }
        }

        for (const auto &include : script.GetIncludes())
        {
            out.out << "(include " << include << ")" << out.NewLineString();
        }
        for (const auto &uses : script.GetUses())
        {
            out.out << "(use " << uses << ")" << out.NewLineString();
        }

        ForwardOptionalSection("public", script.GetExports());
        if (!script.GetExports().empty())
        {
            out.out << "(public" << out.NewLineString();
            {
                DebugIndent indent(out);
                for (const auto &exports : script.GetExports())
                {
                    exports->Accept(*this);
                }
            }
            out.out << ")" << out.NewLineString();
        }

        Forward(script.GetSynonyms());
        out.EnsureNewLine();

        Forward(script.GetDefines());
        out.EnsureNewLine();

        ForwardOptionalSection("local", script.GetScriptVariables());

        ForwardOptionalSection("string", script.GetScriptStringsDeclarations());

        Forward(script.GetProcedures());
        out.EnsureNewLine();
        Forward(script.GetClasses());
    }

    void Visit(const ClassDefinition &classDef) override
    {
        out.SyncComments(classDef);

        if (classDef.IsInstance())
        {
            out.out << "(instance ";
            if (classDef.IsPublic())
            {
                out.out << "public ";
            }
        }
        else
        {
            out.out << "(class ";
        }
        out.out << CleanToken(classDef.GetName());
        if (!classDef.GetSuperClass().empty())
        {
            out.out << " of " << CleanToken(classDef.GetSuperClass());
        }
        out.out << out.NewLineString();

        {
            DebugIndent indent(out);
            {
                DebugLine line(out);
                if (classDef.GetProperties().empty())
                {
                    out.out << "(properties)";
                }
                else
                {
                    out.out << "(properties" << out.NewLineString();
                    {
                        DebugIndent indent2(out);
                        Forward(classDef.GetProperties());
                    }
                    Indent(out);
                    out.out << ")";
                }
            }

            // Body (methods)
            Forward(classDef.GetMethods());
        }
        out.out << ")" << out.NewLineString();
    }

    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}

    void _VisitPropertyValue(const PropertyValueBase &prop)
    {
        out.SyncComments(prop);
        DebugLine line(out);
        switch (prop.GetType())
        {
        case ValueType::String:
            out.out << "\"" << UnescapeString(prop.GetStringValue()) << "\"";
            break;
        case ValueType::Said:
            out.out << "'" << prop.GetStringValue() << "'";
            break;
        case ValueType::Number:
            _OutputNumber(out.out, prop.GetNumberValue(), prop._fHex, prop._fNegate);
            break;
        case ValueType::Token:
            if (prop.GetStringValue() == "paramTotal")
            {
                out.out << "argc";
            }
            else
            {
                // Surround in braces if there are spaces in the string.
                out.out << CleanToken(prop.GetStringValue());
            }
            break;
        case ValueType::Selector:
            out.out << "#" << prop.GetStringValue();
            break;
        case ValueType::Pointer:
            out.out << "@" << prop.GetStringValue();
            break;
        }
    }

    void Visit(const PropertyValue &prop) override
    {
        _VisitPropertyValue(prop);
    }

    void Visit(const ComplexPropertyValue &prop) override
    {
        if (prop.GetIndexer())
        {
            out.out << "[";
        }
        _VisitPropertyValue(prop);
        if (prop.GetIndexer())
        {
            out.out << " ";
            Inline inln(out, true);
            prop.GetIndexer()->Accept(*this);
            out.out << "]";
        }
    }

    void Visit(const Define &define) override
    {
        out.SyncComments(define);
        bool fHex = IsFlagSet(define.GetFlags(), IntegerFlags::Hex);
        bool fNegate = IsFlagSet(define.GetFlags(), IntegerFlags::Negative);
        out.out << "(define " << define.GetLabel() << " ";
        _OutputNumber(out.out, define.GetValue(), fHex, fNegate);
        out.out << ")" << out.NewLineString();
    }

    void Visit(const ClassProperty &classProp) override
    {
        out.SyncComments(classProp);
        DebugLine line(out, classProp);
        out.out << CleanToken(classProp.GetName()) << " ";
        Inline inln(out, true);
        classProp.GetStatement1()->Accept(*this);
    }

    void Visit(const VariableDecl &varDecl) override
    {
        out.SyncComments(varDecl);
        DebugLine line(out);
        _OutputVariableAndSize(*this, out, varDecl.GetDataType(), varDecl.GetName(), varDecl.GetSize(), varDecl.GetStatements());
    }

    void _VisitFunctionBase(const FunctionBase &function)
    {
        out.SyncComments(function);

        if (function.GetSignatures().empty())
        {
            out.out << "CORRUPT FUNCTION " << function.GetName() << out.NewLineString();
        }
        else
        {
            out.out << "(" << function.GetName();
            const FunctionParameterVector &params = function.GetSignatures()[0]->GetParams();

            for (auto &functionParam : params)
            {
                out.out << " " << functionParam->GetName();
            }
            if (!function.GetVariables().empty())
            {
                Inline inln(out, true);
                out.out <<  " &tmp ";
                for (auto &variable : function.GetVariables())
                {
                    // REVIEW: We don't handle variable initialization here. For now, assert we don't have this
                    assert(variable->GetInitializers().empty());
                }
                Forward(function.GetVariables(), " ");
            }
            out.out << ")";
            out.EnsureNewLine();

            // Body of function
            {
                DebugIndent indent(out);
                Forward(function.GetCodeSegments());
            }

            DebugLine line(out);
            out.out << ")";
            out.EnsureNewLine();
        }
    }

    void Visit(const MethodDefinition &function) override
    {
        out.SyncComments(function);
        NewLine(out);
        out.out << "(method ";
        _VisitFunctionBase(function);
    }

    void Visit(const ProcedureDefinition &function) override
    {
        out.SyncComments(function);
        NewLine(out);
        out.out << "(procedure ";
        _VisitFunctionBase(function);
    }

    void Visit(const Synonym &syn) override {}

    void Visit(const CodeBlock &block) override
    {
        out.SyncComments(block);
        if (block.GetList().size() > 1)
        {
            // Don't add this, messes up if statements.
            // DebugLine debugLine(out);
            Inline inln(out, false);
            Forward(block.GetList());
        }
        else
        {
            Forward(block.GetList());
        }
    }

    void Visit(const ConditionalExpression &conditional) override
    {
        out.SyncComments(conditional);
        DebugLine debugLine(out);
        Inline inln(out, true);
        Forward(conditional.GetStatements());
    }

    void Visit(const Comment &comment) override
    {
        // Handled elsewhere.
    }

    void Visit(const SendParam &sendParam) override
    {
        // Handled in SendCall
    }

    void Visit(const LValue &lValue) override
    {
        out.SyncComments(lValue);
        if (lValue.HasIndexer())
        {
            out.out << "[";
        }
        out.out << lValue.GetName();
        if (lValue.HasIndexer())
        {
            out.out << " ";
            Inline inln(out, true);
            lValue.GetIndexer()->Accept(*this);
            out.out << "]";
        }
    }

    void Visit(const RestStatement &rest) override
    {
        out.SyncComments(rest);
        DebugLine line(out);
        out.out << "&rest";
        if (!rest.GetName().empty())
        {
            out.out << " " << rest.GetName();
        }
    }

    void Visit(const Cast &cast) override {}


    void _VisitSendParam(SourceCodeWriter &out, const SendParam &sendParam, bool addComma)
    {
        out.SyncComments(sendParam);
        DebugLine debugLine(out);
        out.out << CleanToken(sendParam.GetSelectorName()) << ":";
        DetectIfWentNonInline detect(out);
        if (!sendParam.GetSelectorParams().empty())
        {
            out.out << " ";
            // Space-separated values
            Inline inln(out, true);
            DebugIndent indent(out);    // In case we have inline false in here:
            Forward(sendParam.GetSelectorParams(), " ");
            if (addComma)
            {
                out.out << ",";
            }
        }
        if (detect.WentInline)
        {
            out.EnsureNewLine();
            Indent(out);
        }
    }

    void Visit(const SendCall &sendCall) override
    {
        out.SyncComments(sendCall);
        DebugLine debugLine(out);
        out.out << "(";
        // This sucks because of all the different forms we have.
        if (!sendCall.GetTargetName().empty())
        {
            out.out << CleanToken(sendCall.GetTargetName()); 
        }
        else if (sendCall.GetStatement1())
        {
            sendCall.GetStatement1()->Accept(*this);
        }
        else
        {
            if (sendCall._object3)
            {
                sendCall._object3->Accept(*this);
            }
            else
            {
                out.out << "WARNING_UNKNOWN_OBJECT";
            }
        }

        out.out << " ";

        size_t count = sendCall.GetParams().size() + (sendCall._rest ? 1 : 0);
        // Now the params.
        // If there is more than one, then indent, because we'll do one on each line.
        if (count > 1)
        {
            Inline goInline(out, false);
            {
                DebugIndent indent(out);
                out.out << out.NewLineString();	// newline to start it out

                for (size_t i = 0; i < sendCall.GetParams().size(); i++)
                {
                    _VisitSendParam(out, *sendCall.GetParams()[i], (i < (sendCall.GetParams().size() - 1)));
                }
                if (sendCall._rest)
                {
                    sendCall._rest->Accept(*this);
                }
            }
            DebugLine line(out);
            out.out << ")";
        }
        else
        {
            Inline goInline(out, true);
            // We want ? instead of : when IsMethod is not true.
            if ((sendCall.GetParams().size() == 1) && !sendCall.GetParams()[0]->IsMethod() && sendCall.GetParams()[0]->GetSelectorParams().empty())
            {
                // Property retrieval.
                out.out << sendCall.GetParams()[0]->GetName() << "?";
            }
            else
            {
                for (auto &sendParam : sendCall.GetParams())
                {
                    _VisitSendParam(out, *sendParam, false);
                }
            }
            out.out << ")";
        }
    }

    void Visit(const ProcedureCall &procCall) override
    {
        out.SyncComments(procCall);
        DebugLine line(out);
        out.out << "(" << procCall.GetName();
        if (!procCall.GetStatements().empty())
        {
            out.out << " ";
        }
        DetectIfWentNonInline detect(out);
        {
            Inline inln(out, true);
            DebugIndent indent(out);    // In case we have inline false in here:
            Forward(procCall.GetStatements());
        }
        if (detect.WentInline)
        {
            out.EnsureNewLine();
            Indent(out);
        }
        out.out << ")";
    }

    void Visit(const ReturnStatement &ret) override
    {
        out.SyncComments(ret);

        DebugLine line(out);
        out.out << "(return";
        DetectIfWentNonInline detect(out);
        {
            Inline inln(out, true);
            DebugIndent indent(out);    // In case we have inline false in here:
            if (ret.GetStatement1())
            {
                out.out << " ";
                ret.GetStatement1()->Accept(*this);
            }
        }
        if (detect.WentInline)
        {
            out.EnsureNewLine();
            Indent(out);
        }
        out.out << ")";
    }

    void Visit(const ForLoop &forLoop) override
    {
        out.SyncComments(forLoop);
        // CodeBlocks are just used in for loops for now I think. And perhaps random enclosures ().
        {
            DebugLine line(out);
            out.out << "(for (";
            Inline inln(out, true);
            forLoop.GetInitializer()->Accept(*this);
            out.out << ") ";
            // Condition is not contained in ()
            forLoop.GetCondition()->Accept(*this);
            out.out << " (";
            forLoop._looper->Accept(*this);
            out.out << ")";
        }
        // Now the code
        {
            DebugIndent indent(out);
            Forward(forLoop.GetStatements());
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    bool _IsConditionalExpressionTRUE(const ConditionalExpression &condExp)
    {
        uint16_t result;
        return condExp.Evaluate(simpleDefineLookup, result) && (result == 1);
    }

    // Returns the inside of the negation
    const SyntaxNode *_IsConditionalExpressionLogicallyNegated(const ConditionalExpression &condExp)
    {
        const SyntaxNode *inner = nullptr;
        if (condExp.GetStatements().size() == 1)
        {
            const UnaryOp *unary = SafeSyntaxNode<UnaryOp>(condExp.GetStatements()[0].get());
            if (unary && (unary->GetName() == "not"))
            {
                inner = unary->GetStatement1();
            }
        }
        return inner;
    }

    void Visit(const WhileLoop &whileLoop) override
    {
        out.SyncComments(whileLoop);
        {
            DebugLine line(out);
            if (_IsConditionalExpressionTRUE(*whileLoop.GetCondition()))
            {
                out.out << "(repeat";
            }
            else
            {
                out.out << "(while ";
                Inline inln(out, true);
                whileLoop.GetCondition()->Accept(*this);
            }
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(whileLoop.GetStatements());
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const DoLoop &doLoop) override
    {
        // No do-loops in SCI. So the following:
        // (do
        //     code
        // ) while (blah)
        //
        // will get transformed into
        //
        // (repeat
        //     code
        //     (breakif (not blah))
        // )

        out.SyncComments(doLoop);
        {
            DebugLine line(out);
            out.out << "(repeat";
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(doLoop.GetStatements());

            // Now the condition (assuming it's not always TRUE).
            if (!_IsConditionalExpressionTRUE(*doLoop.GetCondition()))
            {
                out.EnsureNewLine();
                {
                    DebugLine ifLine(out);
                    out.out << "(breakif ";
                    Inline inlineCondition(out, true); // But the condition is inline

                    // We need to negate this condition. It's likely already negated though. So we can:
                    //  - Check to see if the condition is a negation, and just visit the inner.
                    //  - Otherwise, write a (not ...) 
                    const SyntaxNode *inner = _IsConditionalExpressionLogicallyNegated(*doLoop.GetCondition());
                    if (inner)
                    {
                        inner->Accept(*this);
                    }
                    else
                    { 
                        // Negate
                        out.out << "(not ";
                        doLoop.GetCondition()->Accept(*this);
                        out.out << ")";
                    }

                    DebugLine closeLine(out);
                    out.out << ")";
                }
            }
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const BreakStatement &breakStatement) override
    {
        out.SyncComments(breakStatement);
        DebugLine line(out);
        out.out << "(break)";
    }

    void Visit(const CaseStatement &caseStatement) override
    {
        out.SyncComments(caseStatement);
        {
            DebugLine line(out);
            if (caseStatement.IsDefault())
            {
                out.out << "(else ";
            }
            else
            {
                out.out << "(";
                Inline inln(out, true);
                caseStatement.GetStatement1()->Accept(*this);
            }
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(caseStatement.GetCodeSegments());
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const SwitchStatement &switchStatement) override
    {
        out.SyncComments(switchStatement);
        {
            DebugLine line(out);
            out.out << "(switch ";
            Inline inln(out, true);
            switchStatement.GetStatement1()->Accept(*this);
        }
        // Now the cases, indented.
        {
            DebugIndent indent(out);
            Forward(switchStatement._cases);
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const Assignment &assignment) override
    {
        DebugLine line(out);
        Inline inln(out, true);
        out.out << "(" << assignment.GetAssignmentOp() << " ";
        assignment._variable->Accept(*this);
        out.out << " ";
        DetectIfWentNonInline detect(out);
        {
            DebugIndent indent(out);    // In case we have inline false in here:
            assignment.GetStatement1()->Accept(*this);
        }
        if (detect.WentInline)
        {
            out.EnsureNewLine();
            Indent(out);
        }

        out.out << ")";
    }

    void _CoalesceBinaryOps(const string &operatorName, const SyntaxNode *operand, vector<const SyntaxNode*> &terms)
    {
        const BinaryOp *subOp = SafeSyntaxNode<BinaryOp>(operand);
        if (subOp && (subOp->GetName() == operatorName))
        {
            // Go deeper
            _CoalesceBinaryOps(operatorName, subOp->GetStatement1(), terms);
            _CoalesceBinaryOps(operatorName, subOp->GetStatement2(), terms);
        }
        else
        {
            terms.push_back(operand);
        }
    }

    void Visit(const BinaryOp &binaryOp) override
    {
        out.SyncComments(binaryOp);
        DebugLine line(out);
        Inline inln(out, true);

        vector<const SyntaxNode*> terms;
        bool canCoalesce = coalesceBinaryOps.find(binaryOp.GetName()) != coalesceBinaryOps.end();
        if (canCoalesce)
        {
            _CoalesceBinaryOps(binaryOp.GetName(), binaryOp.GetStatement1(), terms);
            _CoalesceBinaryOps(binaryOp.GetName(), binaryOp.GetStatement2(), terms);
        }
        else
        {
            terms.push_back(binaryOp.GetStatement1());
            terms.push_back(binaryOp.GetStatement2());
        }

        // TODO: Maybe more than two and we should do non-inline?
        string name = GetOperatorName(binaryOp, binOpConvert);
        out.out << "(" + name << " ";
        Forward(terms, " ");
        out.out << ")";
    }

    void Visit(const UnaryOp &unaryOp) override
    {
        out.SyncComments(unaryOp);
        DebugLine line(out);
        Inline inln(out, true);
        out.out << "(" << GetOperatorName(unaryOp, unaryOpConvert);
        out.out << " ";
        unaryOp.GetStatement1()->Accept(*this);
        out.out << ")";
    }

    const IfStatement *_GetSingleIfStatementInElseBlock(const IfStatement &ifStatement)
    {
        const CodeBlock *elseBlock = SafeSyntaxNode<CodeBlock>(ifStatement.GetStatement2());
        if (elseBlock &&
            (elseBlock->GetStatements().size() == 1))
        {
            return SafeSyntaxNode<IfStatement>(elseBlock->GetStatements()[0].get());
        }
        return nullptr;
    }

    const BreakStatement *_GetSingleBreakStatementInThenBlock(const IfStatement &ifStatement)
    {
        const CodeBlock *thenBlock = SafeSyntaxNode<CodeBlock>(ifStatement.GetStatement1());
        const CodeBlock *elseBlock = SafeSyntaxNode<CodeBlock>(ifStatement.GetStatement2());
        if ((!elseBlock || elseBlock->GetStatements().empty()) &&
            thenBlock &&
            (thenBlock->GetStatements().size() == 1))
        {
            return SafeSyntaxNode<BreakStatement>(thenBlock->GetStatements()[0].get());
        }
        return nullptr;
    }

    void Visit(const IfStatement &ifStatement) override
    {
        Inline inln(out, false);	// Line by line now, overall
        {
            // Detect the cond pattern
            const IfStatement *singleIf = _GetSingleIfStatementInElseBlock(ifStatement);
            if (singleIf)
            {
                // If with an else with a single if inside it. That's good enough to create a cond clause.
                // Let's collect cond clauses.
                vector<pair<const SyntaxNode*, const SyntaxNode*>> conditionAndCodes;
                vector<const SyntaxNode*> code;
                const SyntaxNode *finalElse = nullptr;
                conditionAndCodes.push_back({ ifStatement.GetCondition().get(), ifStatement.GetStatement1() });
                while (singleIf)
                {
                    conditionAndCodes.push_back({ singleIf->GetCondition().get(), singleIf->GetStatement1() });
                    const IfStatement *nextSingleIf = _GetSingleIfStatementInElseBlock(*singleIf);
                    if (!nextSingleIf)
                    {
                        // That's the end of it. Stick in any else clause. Cond does not require an else clause
                        finalElse = singleIf->GetStatement2();
                    }
                    singleIf = nextSingleIf;
                }

                // Now we're ready to output a cond statement
                out.EnsureNewLine();
                {
                    DebugLine condLine(out);
                    out.out << "(cond ";

                    {
                        DebugIndent indentClauses(out);
                        for (auto &conditionAndCode : conditionAndCodes)
                        {
                            out.EnsureNewLine();
                            DebugLine expLine(out);
                            out.out << "(";
                            {
                                Inline inlineCondition(out, true); // But the condition is inline
                                conditionAndCode.first->Accept(*this);
                            }
                            {
                                // Code is indented
                                Inline inlineCondition(out, false);
                                DebugIndent indentCode(out);
                                out.EnsureNewLine();
                                //DebugLine codeLine(out);
                                conditionAndCode.second->Accept(*this);
                            }
                            DebugLine closeLine(out);
                            out.out << ")";
                        }

                        if (finalElse)
                        {
                            out.EnsureNewLine();
                            DebugLine expLine(out);
                            out.out << "(else";
                            {
                                // Code is indented
                                Inline inlineCondition(out, false);
                                DebugIndent indent(out);
                                out.EnsureNewLine();
                                //DebugLine codeLine(out);
                                finalElse->Accept(*this);
                            }
                            DebugLine closeLine(out);
                            out.out << ")";
                        }
                        out.EnsureNewLine();
                    }
                    DebugLine closeLine(out);
                    out.out << ")";
                }
            }
            else if (_GetSingleBreakStatementInThenBlock(ifStatement))
            {
                // This is:
                //      (if (...) break)
                //
                // We change it to
                //      (breakif (...))
                out.EnsureNewLine();
                {
                    DebugLine ifLine(out);
                    out.out << "(breakif ";
                    Inline inlineCondition(out, true); // But the condition is inline
                    ifStatement.GetCondition()->Accept(*this);
                    DebugLine closeLine(out);
                    out.out << ")";
                }
            }
            else
            {
                // Keep the if format.
                out.EnsureNewLine();
                {
                    DebugLine ifLine(out);
                    out.out << "(if ";
                    Inline inlineCondition(out, true); // But the condition is inline
                    ifStatement.GetCondition()->Accept(*this);
                    out.out << "";
                }
                {
                    DebugIndent indent(out);
                    Inline inlineCondition(out, false);
                    ifStatement.GetStatement1()->Accept(*this);
                }
                {
                    DebugLine closeLine(out);
                    out.out << (ifStatement.HasElse() ? "else" : ")");
                }
                if (ifStatement.HasElse())
                {
                    {
                        DebugIndent indent(out);
                        Inline inlineCondition(out, false);
                        ifStatement.GetStatement2()->Accept(*this);
                    }
                    DebugLine closeLine(out);
                    out.out << ")";
                }
            }
        }
    }


    void Visit(const Asm &asmStatement) override
    {
        // Let's use a more custom text formatting.
        out.EnsureNewLine();

        Inline inlineAsm(out, true);

        int labelSize = 0;
        if (!asmStatement.GetLabel().empty())
        {
            out.out << asmStatement.GetLabel() << ":";
            labelSize = (int)asmStatement.GetLabel().size() + 1;
        }

        // Move forward to the current indent level.
        string spaces;
        spaces.append(max(0, out.iIndent - labelSize), ' ');
        out.out << spaces;

        // output the instruction
        out.out << asmStatement.GetName();

        // Move forward to a common column
        spaces.clear();
        spaces.append(max(0, 8 - asmStatement.GetName().size()), ' ');
        out.out << spaces;

        Forward(asmStatement.GetStatements(), ", ");
    }

    void Visit(const AsmBlock &asmSection) override
    {
        out.disallowedTokens = &GetOpcodeSet();

        {
            DebugLine asmLine(out);
            out.out << "(asm";
            {
                DebugIndent indent(out);
                Forward(asmSection.GetStatements());
            }
        }
        {
            out.EnsureNewLine();
            DebugLine asmLine(out);
            out.out << ")";
        }

        // Can't have asmblock inside another, so it's ok to not have a stack here, and just force values.
        out.disallowedTokens = nullptr;
    }

    void Visit(const ExportEntry &exportEntry) override
    {
        DebugLine exportLine(out);
        out.out << CleanToken(exportEntry.Name) << " " << exportEntry.Slot;
    }

    void Visit(const CondStatement &cond) override
    {
        // TODO
    }
    void Visit(const CondClauseStatement &cond) override
    {
        // TODO
    }
    void Visit(const BreakIfStatement &cond) override
    {
        // TODO
    }
    void Visit(const RepeatStatement &cond) override
    {
        // TODO
    }
};

void OutputSourceCode_SCI(const sci::Script &script, sci::SourceCodeWriter &out)
{
    OutputSCISourceCode output(out);
    output.Visit(script);
}

void OutputSourceCode_SCI(const sci::ClassDefinition &classDef, sci::SourceCodeWriter &out)
{
    OutputSCISourceCode output(out);
    output.Visit(classDef);
}

void OutputSourceCode_SCI(const sci::MethodDefinition &method, sci::SourceCodeWriter &out)
{
    OutputSCISourceCode output(out);
    output.Visit(method);
}

void OutputSourceCode_SCI(const sci::ClassProperty &classProp, sci::SourceCodeWriter &out)
{
    OutputSCISourceCode output(out);
    output.Visit(classProp);
}