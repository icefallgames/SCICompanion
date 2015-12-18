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
#include "SourceCodeFormatter.h"
#include "PMachine.h"
#include "StringUtil.h"
#include "CompiledScript.h"
#include "AppState.h"
#include "OperatorTables.h"
#include <numeric>

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
        ; else is optional
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
        `#4     ; Function key

    assignment, etc:
        (assignmentOp lValue rValue)

    expression
        // pretty much all operators operate on n values
        (operator value0 value1 value2 value3)

    local variables:
        (local
            var1
            var2 = 4
            var3 = [2 4 19 23 2 4]
            [buffer 100]
            [buffer 20] = [1 2 3 4 5 6 7 8]
        )

    exports
        (public
            instancename0 exportNumber
            procname0 exportNumber
            procname2 exportNumber
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

// In Studio syntax, we need to wrap some class names and tokens in {} to identify them as tokens.
// For instance b-moveCnt, or -info-. However, in the original syntax, I don't think this is needed.
// For now, however, we'll keep track of the places that need this by having this dummy function.
// Actually, we can use it to convert paramTotal to argc.
// Oh... in the SCI0 template game there is an object with a space in its name. Let's replace though with _
std::string CleanTokenSCI(const std::string &src)
{
    if (src == "paramTotal")
    {
        return "argc";
    }
    std::string output;
    std::transform(src.begin(), src.end(), std::back_inserter(output), [](char ch)
    {
        if (std::isspace(ch))
        {
            ch = '_';
        }
        return ch;
    }
    );
    return output;
}

std::string GetPropertyText(const PropertyValueBase &prop)
{
    std::stringstream mw;
    switch (prop.GetType())
    {
        case ValueType::String:
            mw << "{" << EscapeSpaces(EscapeBraceString(prop.GetStringValue())) << "}";
            break;
        case ValueType::ResourceString:
            mw << "\"" << EscapeSpaces(EscapeQuotedString(prop.GetStringValue())) << "\"";
            break;
        case ValueType::Said:
            mw << "'" << prop.GetStringValue() << "'";
            break;
        case ValueType::Number:
            _OutputNumber(mw, prop.GetNumberValue(), prop._fHex, prop._fNegate);
            break;
        case ValueType::Token:
            mw << CleanTokenSCI(prop.GetStringValue());
            break;
        case ValueType::Selector:
            mw << "#" << prop.GetStringValue();
            break;
        case ValueType::Pointer:
            mw << "@" << CleanTokenSCI(prop.GetStringValue());
            break;
    }
    return mw.str();
}


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

// If we need to transform a class name to remove spaces, we need to set its original name in the props listd.
class TransformCleanClassName : public IExploreNode
{
public:
    TransformCleanClassName(sci::Script &script) { script.Traverse(*this); }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if ((state == ExploreNodeState::Pre) && (node.GetNodeType() == NodeType::NodeTypeClassDefinition))
        {
            ClassDefinition *classDef = SafeSyntaxNode<ClassDefinition>(&node);
            if (CleanTokenSCI(classDef->GetName()) != classDef->GetName())
            {
                bool nameAlreadySpecified = false;
                for (auto &prop : classDef->GetProperties())
                {
                    if (prop->GetName() == "name")
                    {
                        nameAlreadySpecified = true;
                        break;
                    }
                }
                if (!nameAlreadySpecified)
                {
                    classDef->GetPropertiesNC().push_back(std::make_unique<ClassProperty>("name", PropertyValue(classDef->GetName(), ValueType::ResourceString)));
                }
            }

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
                    {
                        PropertyValueBase *value = static_cast<PropertyValueBase*>(&node);
                        if ((value->GetType() == ValueType::Token) || (value->GetType() == ValueType::Pointer))
                        {
                            _explicitVarUsage.insert(value->GetStringValue());
                        }
                        break;
                    }
                    case NodeType::NodeTypeLValue:
                    {
                        _explicitVarUsage.insert(static_cast<LValue*>(&node)->GetName());
                        break;
                    }
                    case NodeType::NodeTypeSendCall:
                    {
                        SendCall *send = static_cast<SendCall*>(&node);
                        if (!send->GetName().empty())
                        {
                            _explicitVarUsage.insert(send->GetName());
                        }
                        break;
                    }
                    case NodeType::NodeTypeSendParam:
                    {
                        // Technically, a send selector can be a variable.
                        SendParam *sendParam = static_cast<SendParam*>(&node);
                        _explicitVarUsage.insert(sendParam->GetName());
                    }
                    // Are there any other cases where variable names appear?
                }
            }
        }
    }

private:
    FunctionSignature *_functionSig;
    vector<RestStatement*> _rests;
    std::set<string> _explicitVarUsage;
};

vector<pair<string, string>> unaryOpConvert =
{
    { "bnot", "~" },
    { "neg", "-" },
};

// Others can be coalesced too (e.g. the less-than/greater-than), but we'd need
// to re-arrange code.
std::set<BinaryOperator> coalesceBinaryOps =
{
    BinaryOperator::LogicalAnd,
    BinaryOperator::LogicalOr,
    BinaryOperator::BinaryOr,
    BinaryOperator::BinaryAnd,
    BinaryOperator::Add,
    BinaryOperator::Multiply,
};


class TransformDeterminePropSelectors : public IExploreNode
{
public:
    TransformDeterminePropSelectors(sci::Script &script, GlobalCompiledScriptLookups *lookups) : _lookups(lookups)
    {
        // We need to determine what is a property and what is a method
        if (!_lookups && _lookupsOwned.Load(appState->GetResourceMap().Helper()))
        {
            _lookups = &_lookupsOwned;
        }
        if (_lookups)
        {
            for (auto &script : _lookups->GetGlobalClassTable().GetAllScripts())
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

            script.Traverse(*this);
        }
    }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            SendParam *sendParam = SafeSyntaxNode<SendParam>(&node);
            if (sendParam)
            {
                uint16_t selector;
                if (_lookups->GetSelectorTable().ReverseLookup(sendParam->GetName(), selector))
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
    GlobalCompiledScriptLookups *_lookups;
    GlobalCompiledScriptLookups _lookupsOwned;
};

std::string _GetCommentText(const Comment &comment)
{
    string text = comment.GetSanitizedText();

    int minTabCount;
    text = Unindent<'\t'>(text, &minTabCount);

    // If the first line of a comment isn't indented, we should indent the rest either.
    int innerCommentTab = 0;
    if (comment.GetPosition().Column() < minTabCount)
    {
        innerCommentTab = minTabCount - comment.GetPosition().Column();
        minTabCount = comment.GetPosition().Column();
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
    return newComment;
}

void ConvertToSCISyntaxHelper(Script &script, GlobalCompiledScriptLookups *lookups)
{
    TransformDeterminePropSelectors txPropSelectors(script, lookups);

    // Transform comments
    for (auto &comment : script.GetComments())
    {
        std::string newText = _GetCommentText(*comment);
        comment->SetName(newText);
    }

    TransformCleanRests txCleanRests(script);

    TransformCleanClassName txCleanClassName(script);

    // Remove some keywords that might be present as variable names in other syntaxes.
    vector<pair<string, string>> fromToMapping =
    {
        { "cond", "case" },
        { "continue", "continue_" },
        { "repeat", "repeat_" },
    };
    CleanVariableNames(script, fromToMapping);
}

// Configure output
const size_t MaxColumnWidth = 60;
const size_t MaxSendParamWidth = 75;
const size_t MaxConditionWidth = 50;
const size_t MaxCaseWidth = 35;

// Macros yuck! But they get the job done.
#define GO_INLINE auto line = _GoInline()
#define GO_MULTILINE auto line = _GoMultiline()
#define INDENT_BLOCK DebugIndent indent(out)
#define MAYBE_INDENT_BLOCK DebugIndent indent(out, out.fInline ? 0 : -1)
#define SET_MULTILINEMODE(x) Inline line(out, !(x));


class SCISourceCodeFormatter : public SourceCodeFormatter
{
public:
    SCISourceCodeFormatter(SourceCodeWriter &out) : SourceCodeFormatter(out), _isCalculateSizePass(false), _skipNextSpace(false) {}

    template<typename _T>
    void DoTheThing(_T &node)
    {
        // Switch out a temporary stringstream to use while we calculate size.
        std::stringstream ssTemp;
        std::swap(out.out, ssTemp);

        _currentPosition = LineCol();
        _skipNextSpace = false;
        _isCalculateSizePass = true;
        // Configure our output so it write the least amount possible.
        int oldIndentAmount = out.indentAmount;
        out.indentAmount = 0; // Should now just be newline or space between things.
        auto tempComments = move(out.pComments);    // Remove comments for the time being.
        Visit(node);

        // Now try again with the real deal.
        // This is very hacky, we should create a temporary SourceCodeWriter...
        _currentPosition = LineCol();
        _skipNextSpace = false;
        _isCalculateSizePass = false;
        out.lastNewLineLength = 0;
        // Restore the original stream.
        std::swap(out.out, ssTemp);
        out.indentAmount = oldIndentAmount;
        out.pComments = move(tempComments);
        Visit(node);
    }

private:

    LineCol _currentPosition;
    bool _skipNextSpace;

    // Avoids the extra space here:
    // (cond
    //      ( (IsObject foo) ...
    //
    // (switch
    //      ( 1
    //
    void _SkipNextSpace() { _skipNextSpace = true; }

    bool _ShouldBeMultiline(const SyntaxNode *node, size_t maxInlineWidth = MaxColumnWidth)
    {
        return _nodeSize[node] > maxInlineWidth;
    }

    _Check_return_
    Inline _GoMultiline()
    {
        return Inline(out, false);
    }

    _Check_return_
    Inline _GoInline()
    {
        return Inline(out, true);
    }
    void _IndentAccept(const SyntaxNode &node)
    {
        DebugIndent indent(out);
        node.Accept(*this);
    }
    void _MaybeIndentAccept(const SyntaxNode &node)
    {
        DebugIndent indent(out, out.fInline ? 0 : -1);
        node.Accept(*this);
    }
    template<typename _TCollection>
    void _AcceptChildren(const _TCollection &things)
    {
        for (auto &thing : things)
        {
            thing->Accept(*this);
        }
    }
    template<typename _TCollection>
    void _IndentAcceptChildren(const _TCollection &things)
    {
        DebugIndent indent(out);
        for (auto &thing : things)
        {
            thing->Accept(*this);
        }
    }
    template<typename _TCollection>
    void _MaybeIndentAcceptChildren(const _TCollection &things)
    {
        DebugIndent indent(out, out.fInline ? 0 : -1);
        for (auto &thing : things)
        {
            thing->Accept(*this);
        }
    }
    template<typename _TCollection>
    void _NoIndentAcceptChildren(const _TCollection &things)
    {
        for (auto &thing : things)
        {
            thing->Accept(*this);
        }
    }

    void _AboutToGoToNextLine()
    {
        if (!_isCalculateSizePass)
        {
            // Here we need to decide if we should output a comment.
            const Comment *pComment;
            LineCol nextLine(_currentPosition.Line() + 1, 0);
            while (out.pComments && (pComment = out.pComments->GetCurrentComment()))
            {
                if (pComment->GetPosition() <= _currentPosition)
                {
                    switch (pComment->CommentType)
                    {
                        case CommentType::Positioned:
                        {
                            // Append it to this line. But first we need to figure out
                            // our "visual" position
                            int numberOfSpaces = 1;
                            numberOfSpaces = max(numberOfSpaces, pComment->GetColumnNumber() - out.VisualCharsSinceLastNewLine(4));
                            std::string spaces;
                            spaces.append(numberOfSpaces, ' ');
                            out.out << spaces;
                            out.out << pComment->GetName();
                            break;
                        }
                        case CommentType::Indented:
                            // Put it on a new line, indented.
                            out.NewLine();
                            Indent(out);
                            out.out << pComment->GetName();
                            break;

                        case CommentType::LeftJustified:
                            // Put it on a new line, un-indented.
                            out.NewLine();
                            out.out << pComment->GetName();
                            break;

                        default:
                            assert(false);
                    }
                    out.pComments->MoveToNextComment();
                }
                else
                {
                    break;
                }
            }
        }
    }

    void _MaybeNewLineNoIndent()
    {
        if (out.fInline)
        {
            if (!_skipNextSpace)
            {
                out.out << " ";
            }
        }
        else
        {
            _AboutToGoToNextLine();
            out.NewLine();
        }
        _skipNextSpace = false;
    }

    void _FakeGoToNextLine()
    {
        // _currentPosition is used for determining where to output comments,
        // and is generally set when we enter/leave ScriptOM nodes. However, sometimes
        // we output text that isn't part of any node. This is a way to pretend that we've
        // moved to a new line, so that any comment on the previous line is output.
        _currentPosition = LineCol(_currentPosition.Line() + 1, 0);
    }

    void _MaybeNewLineIndent()
    {
        if (out.fInline)
        {
            if (!_skipNextSpace)
            {
                out.out << " ";
            }
        }
        else
        {
            _AboutToGoToNextLine();
            out.NewLine();
            Indent(out);
        }
        _skipNextSpace = false;
    }
    void _MaybeNewLineIndentNoSpace()
    {
        if (!out.fInline)
        {
            _AboutToGoToNextLine();
            out.NewLine();
            Indent(out);
        }
    }

    void _OutputVariableAndSizeSCI(sci::SourceCodeWriter &out, const std::string &type, const std::string &name, WORD wSize, const sci::SyntaxNodeVector &initValues, const uint16_t *optionalSlot = nullptr)
    {
        if (wSize > 1)
        {
            out.out << "[";
        }
        out.out << name;
        if (wSize > 1)
        {
            out.out << " " << wSize << "]"; // array
        }

        if (optionalSlot)
        {
            out.out << " " << *optionalSlot;
        }

        if (!initValues.empty())
        {
            GO_INLINE;
            if (wSize > 1)
            {
                out.out << " = [";
                bool first = true;
                for (auto &initValue : initValues)
                {
                    if (first)
                    {
                        _SkipNextSpace();
                    }
                    _MaybeIndentAccept(*initValue);
                    first = false;
                }
                out.out << "]";
            }
            else
            {
                out.out << " = ";
                _MaybeIndentAccept(*initValues[0]);
            }
        }
    }


public:

    template<typename _T>
    void ForwardOptionalSection(const char *sectionName, _T &things)
    {
        if (!things.empty())
        {
            _MaybeNewLineIndent();
            out.out << "(" << sectionName;
            _IndentAcceptChildren(things);
            _MaybeNewLineIndent();
            out.out << ")";
        }
    }

    void Visit(const Script &script) override
    {
        // Place the language marker if it isn't there already.
        std::string firstComment;
        if (out.pComments)
        {
            firstComment = out.pComments->GetFirstComment();
        }
        if (firstComment.find(SCILanguageMarker) == std::string::npos)
        {
            out.out << ";;; " << SCILanguageMarker << " 1.0 - (do not remove this comment)" << out.pszNewLine;
        }

        out.OutputInitialComment();

        ScriptId scriptId = script.GetScriptId();
        if (!script.GetScriptNumberDefine().empty())
        {
            out.out << "(script# " << script.GetScriptNumberDefine() << ")";
            _MaybeNewLineIndent();
        }
        else
        {
            if (scriptId.GetResourceNumber() == InvalidResourceNumber)
            {
                // This is a legit (e.g. for outputting header files)
            }
            else
            {
                out.out << "(script# " << scriptId.GetResourceNumber() << ")";
                _MaybeNewLineIndent();
            }
        }

        for (const auto &include : script.GetIncludes())
        {
            out.out << "(include " << include << ")";
            _MaybeNewLineIndent();
        }
        for (const auto &uses : script.GetUses())
        {
            out.out << "(use " << uses << ")";
            _MaybeNewLineIndent();
        }

        ForwardOptionalSection("public", script.GetExports());
        ForwardOptionalSection("synonyms", script.GetSynonyms());

        _AcceptChildren(script.GetDefines());

        _MaybeNewLineIndent();

        ForwardOptionalSection("local", script.GetScriptVariables());

        //
        ForwardOptionalSection("extern", script.Externs);
        ForwardOptionalSection("selectors", script.Selectors);
        ForwardOptionalSection("global", script.Globals);
        _AcceptChildren(script.ClassDefs);

        // These are not supported. They aren't used by the SCI0 or SCI1.1 template games,
        // and only a few fan-made games declare them (but don't use them).
        if (!script.GetScriptStringsDeclarations().empty())
        {
            _MaybeNewLineIndent();
            out.out << "; WARNING: This script contained a string section, which is not supported. It may not compile.";
        }

        // In order to properly sync up with comments in scripts that were converted from Studio syntax,
        // we need to sort procedures and classes.
        std::vector<SyntaxNode*> procsAndClasses;
        for (auto &proc : script.GetProcedures())
        {
            if (proc->GetClass().empty())
            {
                procsAndClasses.push_back(proc.get());
            }
            // else Procs that "belong" to a class are handled in the class...
        }
        for (auto &theClass : script.GetClasses())
        {
            procsAndClasses.push_back(theClass.get());
        }
        // Now sort them
        std::sort(procsAndClasses.begin(), procsAndClasses.end(),
            [](SyntaxNode *one, SyntaxNode *two) { return one->GetPosition() < two->GetPosition(); }
            );
        // And spit them out.
        for (auto procOrClass : procsAndClasses)
        {
            procOrClass->Accept(*this);
        }
    }

    void Visit(const ClassDefinition &classDef) override
    {
        _MaybeNewLineIndent();

        {
            GO_INLINE;
            if (classDef.IsInstance())
            {
                out.out << "(instance ";
            }
            else
            {
                out.out << "(class ";
            }
            out.out << CleanTokenSCI(classDef.GetName());
            if (!classDef.GetSuperClass().empty())
            {
                out.out << " of " << CleanTokenSCI(classDef.GetSuperClass());
            }
        }

        GO_MULTILINE;
        {
            INDENT_BLOCK;
            _MaybeNewLineIndent();
            if (classDef.GetProperties().empty())
            {
                out.out << "(properties)";
            }
            else
            {
                out.out << "(properties";
                _IndentAcceptChildren(classDef.GetProperties());
                _FakeGoToNextLine();
                _MaybeNewLineIndent();
                out.out << ")";
            }

            // Any class procedures (rare, but used by Timer.sc for instance)
            // In the OM, these are stored at the top level, but we want to print them
            // out as if they were contained in the class.
            const Script *script = classDef.GetOwnerScript();
            if (script)
            {
                // Find any procedures that are *of* this class
                for (auto &proc : script->GetProcedures())
                {
                    if (proc->GetClass() == classDef.GetName())
                    {
                        _MaybeNewLineIndent();
                        proc->Accept(*this);
                    }
                }
            }

            // Body (methods)
            for (auto &method : classDef.GetMethods())
            {
                _MaybeNewLineIndent();
                method->Accept(*this);
            }
        }

        _MaybeNewLineIndent();
        out.out << ")";

        _MaybeNewLineIndent();
    }

    void Visit(const FunctionParameter &param) override
    {
        _MaybeNewLineIndent();
        out.out << param.GetName();
    }

    void Visit(const FunctionSignature &sig) override {}

    void _VisitPropertyValue(const PropertyValueBase &prop)
    {
        _MaybeNewLineIndent();
        out.out << GetPropertyText(prop);
    }

    void Visit(const PropertyValue &prop) override
    {
        _VisitPropertyValue(prop);
    }

    void Visit(const ComplexPropertyValue &prop) override
    {
        if (prop.GetIndexer())
        {
            _MaybeNewLineIndent();
            if (prop.GetType() == ValueType::Pointer)
            {
                out.out << "@";
            }
            out.out << "[";
            assert((prop.GetType() == ValueType::Pointer) || (prop.GetType() == ValueType::Token));
            out.out << prop.GetStringValue();
            prop.GetIndexer()->Accept(*this);
            out.out << "]";
        }
        else
        {
            _VisitPropertyValue(prop);
        }
    }

    void Visit(const Define &define) override
    {
        _MaybeNewLineIndent();
        bool fHex = IsFlagSet(define.GetFlags(), IntegerFlags::Hex);
        bool fNegate = IsFlagSet(define.GetFlags(), IntegerFlags::Negative);
        out.out << "(define " << define.GetLabel() << " ";
        _OutputNumber(out.out, define.GetValue(), fHex, fNegate);
        out.out << ")";
    }

    void Visit(const ClassProperty &classProp) override
    {
        _MaybeNewLineIndent();
        // Always inline
        GO_INLINE;
        out.out << CleanTokenSCI(classProp.GetName());
        classProp.GetStatement1()->Accept(*this);
    }

    void Visit(const VariableDecl &varDecl) override
    {
        _MaybeNewLineIndent();
        _OutputVariableAndSizeSCI(out, varDecl.GetDataType(), varDecl.GetName(), varDecl.GetSize(), varDecl.GetStatements());
    }

    const size_t FunctionParamMultilineThreshold = 40;

    void _VisitFunctionBase(const FunctionBase &function)
    {
        if (function.GetSignatures().empty())
        {
            out.out << "CORRUPT FUNCTION " << function.GetName();
            _MaybeNewLineIndent();
        }
        else
        {
            // Function signatures can go multiline if there are a lot of parameters.
            auto &signature = function.GetSignatures()[0];
            size_t totalSize = _nodeSize[signature.get()];
            for (auto &varDecl : function.GetVariables())
            {
                totalSize += _nodeSize[varDecl.get()];
            }
            std::map<std::string, SyntaxNode*> varNameToInitializer;
            bool signatureMultiline = totalSize > FunctionParamMultilineThreshold;
            {
                SET_MULTILINEMODE(signatureMultiline);
                out.out << "(" << function.GetName();
                _IndentAcceptChildren(signature->GetParams());
                if (!function.GetVariables().empty())
                {
                    _MaybeNewLineIndent();
                    out.out << "&tmp";
                    // We don't want initializers to be spit out here, so don't just blindly forward to GetVariables()
                }

                {
                    SyntaxNodeVector emptyInitializer;
                    INDENT_BLOCK;
                    for (auto &variable : function.GetVariables())
                    {
                        assert(variable->GetInitializers().size() <= 1); // Max 1 initializer value permitted in temp vars (per our parsing).
                        if (!variable->GetInitializers().empty())
                        {
                            varNameToInitializer[variable->GetName()] = variable->GetInitializers()[0].get();
                        }
                        _MaybeNewLineIndent();
                        _OutputVariableAndSizeSCI(out, variable->GetDataType(), variable->GetName(), variable->GetSize(), emptyInitializer);
                    }
                }
                _MaybeNewLineIndentNoSpace();
                out.out << ")";
            }


            // Body of function
            {
                INDENT_BLOCK;
                // Deal with any initializers
                for (auto &initializer : varNameToInitializer)
                {
                    _MaybeNewLineIndent();
                    GO_INLINE;
                    out.out << "(= " << initializer.first << " ";
                    initializer.second->Accept(*this);
                    out.out << ")";
                }

                // We already indented above.
                _NoIndentAcceptChildren(function.GetCodeSegments());
            }
            _MaybeNewLineIndent();
            out.out << ")";
        }
    }

    void Visit(const MethodDefinition &function) override
    {
        _MaybeNewLineIndent();
        out.out << "(method ";
        _VisitFunctionBase(function);
    }

    void Visit(const ProcedureDefinition &function) override
    {
        _MaybeNewLineIndent();
        out.out << "(procedure ";
        _VisitFunctionBase(function);
        _MaybeNewLineIndent();
    }

    void Visit(const Synonym &syn) override
    {
        _MaybeNewLineIndent();
        out.out << "(" << syn.MainWord;
        for (auto &synonym : syn.Synonyms)
        {
            out.out << " " << synonym;
        }
        out.out << ")";
    }

    void Visit(const CodeBlock &block) override
    {
        // The parent should have set is multiline (e.g. if statements, etc...)
        // SET_MULTILINEMODE(_ShouldBeMultiline(&block));
        // No indent, because the parent will already have indented (e.g. if statements, etc...)
        _NoIndentAcceptChildren(block.GetList());
    }

    void Visit(const ConditionalExpression &conditional) override
    {
        // This happens in the caller. ConditionalExpression just pushes through.
        // _MaybeNewLineIndent();
        SET_MULTILINEMODE(_ShouldBeMultiline(&conditional, MaxConditionWidth));
        _NoIndentAcceptChildren(conditional.GetStatements());
    }

    void Visit(const Comment &comment) override
    {
        // Generally handled elsewhere, but when constructing scripts "manually" we do put comments directly in code.
        if (comment.CommentType != CommentType::None)
        {
            _MaybeNewLineIndent();
            out.out << _GetCommentText(comment);
            if (out.fInline)
            {
                GO_MULTILINE;
                _MaybeNewLineIndent();
            }
        }
    }

    void Visit(const SendParam &sendParam) override
    {
        _MaybeNewLineIndent();

        out.out << CleanTokenSCI(sendParam.GetSelectorName()) << ":";

        if (!sendParam.GetSelectorParams().empty())
        {
            bool isMultiline = _ShouldBeMultiline(&sendParam, MaxSendParamWidth);
            SET_MULTILINEMODE(isMultiline);
            _MaybeIndentAcceptChildren(sendParam.GetSelectorParams());
        }
    }

    void Visit(const LValue &lValue) override
    {
        // Going on a limb and saying this is always inline
        _MaybeNewLineIndent();

        GO_INLINE;
        if (lValue.HasIndexer())
        {
            out.out << "[";
        }
        out.out << lValue.GetName();
        if (lValue.HasIndexer())
        {
            lValue.GetIndexer()->Accept(*this);
            out.out << "]";
        }
    }

    void Visit(const RestStatement &rest) override
    {
        _MaybeNewLineIndent();
        GO_INLINE;
        out.out << "&rest";
        if (!rest.GetName().empty())
        {
            out.out << " " << rest.GetName();
        }
    }

    void Visit(const Cast &cast) override {}

    void Visit(const SendCall &sendCall) override
    {
        _MaybeNewLineIndent();
        out.out << "(";

        {
            // For now we'll go inline. Large send target expressions could look bad though.
            GO_INLINE;
            MAYBE_INDENT_BLOCK;
            // This sucks because of all the different forms we have.
            if (!sendCall.GetTargetName().empty())
            {
                _MaybeNewLineIndentNoSpace();
                out.out << CleanTokenSCI(sendCall.GetTargetName());
            }
            else if (sendCall.GetStatement1())
            {
                _SkipNextSpace();
                _MaybeIndentAccept(*sendCall.GetStatement1());
            }
            else
            {
                if (sendCall._object3)
                {
                    _SkipNextSpace();
                    _MaybeIndentAccept(*sendCall._object3);
                }
                else
                {
                    out.out << "WARNING_UNKNOWN_OBJECT";
                }
            }
        }

        bool isMultiLine = _ShouldBeMultiline(&sendCall);
        SET_MULTILINEMODE(isMultiLine);

        // If we're multilineing, we should indent.
        {
            if ((sendCall.GetParams().size() == 1) && !sendCall.GetParams()[0]->IsMethod() && sendCall.GetParams()[0]->GetSelectorParams().empty())
            {
                // Property retrieval.
                MAYBE_INDENT_BLOCK;
                _MaybeNewLineIndent();
                out.out << sendCall.GetParams()[0]->GetName() << "?";
            }
            else
            {
                _MaybeIndentAcceptChildren(sendCall.GetParams());
                if (sendCall._rest)
                {
                    MAYBE_INDENT_BLOCK;
                    _IndentAccept(*sendCall._rest);
                }
            }
        }

        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    void Visit(const ProcedureCall &procCall) override
    {
        bool multiLine = _ShouldBeMultiline(&procCall);
        _MaybeNewLineIndent();
        out.out << "(" << procCall.GetName();
        if (multiLine)
        {
            GO_MULTILINE;
            _IndentAcceptChildren(procCall.GetStatements());
            _MaybeNewLineIndent();
            out.out << ")";
        }
        else
        {
            GO_INLINE;
            _IndentAcceptChildren(procCall.GetStatements());
            out.out << ")";
        }
    }

    void Visit(const ReturnStatement &ret) override
    {
        bool multiLine = ret.GetStatement1() && _ShouldBeMultiline(ret.GetStatement1());
        _MaybeNewLineIndent(); // Space, or newline and tab.
        out.out << "(return";
        if (multiLine)
        {
            GO_MULTILINE;
            _IndentAccept(*ret.GetStatement1());
            _MaybeNewLineIndent();
            out.out << ")";
        }
        else
        {
            GO_INLINE;
            if (ret.GetStatement1())
            {
                ret.GetStatement1()->Accept(*this);
            }
            out.out << ")";
        }
    }

    void Visit(const ForLoop &forLoop) override
    {
        _MaybeNewLineIndent();

        out.out << "(for (";

        bool firstBlobMultiline = _ShouldBeMultiline(forLoop.GetInitializer()) || _ShouldBeMultiline(forLoop.GetCondition().get()) || _ShouldBeMultiline(forLoop._looper.get());

        {
            SET_MULTILINEMODE(firstBlobMultiline);
            if (forLoop.GetInitializer())
            {
                _MaybeIndentAccept(*forLoop.GetInitializer());
                _MaybeNewLineIndentNoSpace();
            }
            out.out << ")";

            _MaybeIndentAccept(*forLoop.GetCondition());

            if (forLoop._looper)
            {
                _MaybeNewLineIndent();
                out.out << " (";
                _MaybeIndentAccept(*forLoop._looper);
                _MaybeNewLineIndentNoSpace();
                out.out << ")";
            }
        }

        // Now the code
        SET_MULTILINEMODE(firstBlobMultiline);
        _MaybeIndentAcceptChildren(forLoop.GetStatements());
        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    bool _IsConditionalExpressionTRUE(const ConditionalExpression &condExp)
    {
        uint16_t result;
        return condExp.Evaluate(simpleDefineLookup, result, false) && (result == 1);
    }

    // Returns the inside of the negation
    const SyntaxNode *_IsConditionalExpressionLogicallyNegated(const ConditionalExpression &condExp)
    {
        const SyntaxNode *inner = nullptr;
        if (condExp.GetStatements().size() == 1)
        {
            const UnaryOp *unary = SafeSyntaxNode<UnaryOp>(condExp.GetStatements()[0].get());
            if (unary && (unary->Operator == UnaryOperator::LogicalNot))
            {
                inner = unary->GetStatement1();
            }
        }
        return inner;
    }

    void Visit(const WhileLoop &whileLoop) override
    {
        _MaybeNewLineIndent();

        if (_IsConditionalExpressionTRUE(*whileLoop.GetCondition()))
        {
            out.out << "(repeat";
        }
        else
        {
            out.out << "(while";
            bool isConditionMultiline = _ShouldBeMultiline(whileLoop.GetCondition().get());
            SET_MULTILINEMODE(isConditionMultiline);
            _MaybeIndentAccept(*whileLoop.GetCondition());
        }

        bool isMultiline = _ShouldBeMultiline(&whileLoop);
        assert(isMultiline); // It's important that this returns true so that parents are multiline too.
        SET_MULTILINEMODE(isMultiline);

        // Now the code, indented.
        _MaybeIndentAcceptChildren(whileLoop.GetStatements());
        _MaybeNewLineIndentNoSpace();
        out.out << ")";
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

        _MaybeNewLineIndent();
        out.out << "(repeat";

        bool isMultiline = _ShouldBeMultiline(&doLoop);
        assert(isMultiline); // It's important that this returns true so that parents are multiline too.
        SET_MULTILINEMODE(isMultiline);

        // Now the code, indented.
        _MaybeIndentAcceptChildren(doLoop.GetStatements());

        // Apply the condition as a breakif, if necessary.
        if (!_IsConditionalExpressionTRUE(*doLoop.GetCondition()))
        {
            INDENT_BLOCK;
            _MaybeNewLineIndent();
            bool isConditionMultiline = _ShouldBeMultiline(doLoop.GetCondition().get());
            SET_MULTILINEMODE(isConditionMultiline);
            out.out << "(breakif";

            // We need to negate this condition. It's likely already negated though. So we can:
            //  - Check to see if the condition is a negation, and just visit the inner.
            //  - Otherwise, write a (not ...) 
            const SyntaxNode *inner = _IsConditionalExpressionLogicallyNegated(*doLoop.GetCondition());
            if (inner)
            {
                _MaybeIndentAccept(*inner);
            }
            else
            {
                // Negate
                _MaybeNewLineIndentNoSpace();
                out.out << "(not";
                _MaybeIndentAccept(*doLoop.GetCondition());
                _MaybeNewLineIndentNoSpace();
                out.out << ")";
            }

            _MaybeNewLineIndentNoSpace();
            out.out << ")";
        }

        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    void Visit(const BreakStatement &breakStatement) override
    {
        _MaybeNewLineIndent();
        out.out << "(break)";
    }

    void Visit(const ContinueStatement &breakStatement) override
    {
        _MaybeNewLineIndent();
        out.out << "(continue)";
    }

    void Visit(const CaseStatement &caseStatement) override
    {
        _MaybeNewLineIndent();

        bool isMultiline = _ShouldBeMultiline(&caseStatement, MaxCaseWidth);
        SET_MULTILINEMODE(isMultiline);
        {
            if (caseStatement.IsDefault())
            {
                out.out << "(else ";
            }
            else
            {
                out.out << "(";
                bool isCaseConditionMultiline = _ShouldBeMultiline(caseStatement.GetStatement1());
                SET_MULTILINEMODE(isCaseConditionMultiline);
                _SkipNextSpace();
                _MaybeIndentAccept(*caseStatement.GetStatement1());
            }
        }

        _MaybeIndentAcceptChildren(caseStatement.GetCodeSegments());
        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    void Visit(const CondStatement &condStatement) override
    {
        if (condStatement.GetStatement1()) { condStatement.GetStatement1()->Accept(*this); }
    }

    void Visit(const SwitchStatement &switchStatement) override
    {
        bool isMultiline = _ShouldBeMultiline(&switchStatement);
        assert(isMultiline || _isCalculateSizePass); // It's important that this returns true so that parents are multiline too.
        SET_MULTILINEMODE(isMultiline);

        _MaybeNewLineIndent();

        {
            out.out << "(switch";
            bool isValueMultiline = _ShouldBeMultiline(switchStatement.GetStatement1());
            SET_MULTILINEMODE(isValueMultiline);
            _MaybeIndentAccept(*switchStatement.GetStatement1());
        }

        // Now the cases, indented.
        _MaybeIndentAcceptChildren(switchStatement._cases);

        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    void Visit(const Assignment &assignment) override
    {
        _MaybeNewLineIndent();
        bool isMultiline = _ShouldBeMultiline(&assignment);
        out.out << "(" << OperatorToName(assignment.Operator, sciNameToAssignmentOp);
        {
            GO_INLINE;
            assignment._variable->Accept(*this);
        }
        SET_MULTILINEMODE(isMultiline);
        _IndentAccept(*assignment.GetStatement1());
        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    // Returns thing, or if thing is a CodeBlock and has only one internal
    // statement, then returns that statement.
    const SyntaxNode *_GetSingleItemInCodeBlock(const SyntaxNode *thing)
    {
        if (thing && thing->GetNodeType() == NodeTypeCodeBlock)
        {
            const CodeBlock *block = SafeSyntaxNode<CodeBlock>(thing);
            if (block->GetStatements().size() == 1)
            {
                return block->GetStatements()[0].get();
            }
        }
        return thing;
    }

    void _CoalesceBinaryOps(const BinaryOperator &operatorName, const SyntaxNode *operand, vector<const SyntaxNode*> &terms)
    {
        const BinaryOp *subOp = SafeSyntaxNode<BinaryOp>(_GetSingleItemInCodeBlock(operand));
        if (subOp && (subOp->Operator == operatorName))
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
        _MaybeNewLineIndent();

        vector<const SyntaxNode*> terms;
        bool canCoalesce = coalesceBinaryOps.find(binaryOp.Operator) != coalesceBinaryOps.end();
        if (canCoalesce)
        {
            _CoalesceBinaryOps(binaryOp.Operator, binaryOp.GetStatement1(), terms);
            _CoalesceBinaryOps(binaryOp.Operator, binaryOp.GetStatement2(), terms);
        }
        else
        {
            terms.push_back(binaryOp.GetStatement1());
            terms.push_back(binaryOp.GetStatement2());
        }

        string name = OperatorToName(binaryOp.Operator, sciNameToBinaryOp);
        out.out << "(" + name;

        SET_MULTILINEMODE(_ShouldBeMultiline(&binaryOp));
        _IndentAcceptChildren(terms);
        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    void Visit(const NaryOp &naryOp) override
    {
        _MaybeNewLineIndent();
        out.out << "(" << OperatorToName(naryOp.Operator, sciNameToBinaryOp);
        SET_MULTILINEMODE(_ShouldBeMultiline(&naryOp));
        _IndentAcceptChildren(naryOp.GetStatements());
        _MaybeNewLineIndentNoSpace();
        out.out << ")";
    }

    void Visit(const UnaryOp &unaryOp) override
    {
        _MaybeNewLineIndent();
        out.out << "(" << OperatorToName(unaryOp.Operator, sciNameToUnaryOp);
        SET_MULTILINEMODE(_ShouldBeMultiline(&unaryOp));
        _IndentAccept(*unaryOp.GetStatement1());
        _MaybeNewLineIndentNoSpace();
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

    const bool _GetSingleBreakContinueStatementInThenBlock(const IfStatement &ifStatement, std::string &breakOrContinueIf)
    {
        const CodeBlock *thenBlock = SafeSyntaxNode<CodeBlock>(ifStatement.GetStatement1());
        const CodeBlock *elseBlock = SafeSyntaxNode<CodeBlock>(ifStatement.GetStatement2());
        if ((!elseBlock || elseBlock->GetStatements().empty()) &&
            thenBlock &&
            (thenBlock->GetStatements().size() == 1))
        {
            if (SafeSyntaxNode<BreakStatement>(thenBlock->GetStatements()[0].get()))
            {
                breakOrContinueIf = "breakif";
                return true;
            }
            else if (SafeSyntaxNode<ContinueStatement>(thenBlock->GetStatements()[0].get()))
            {
                breakOrContinueIf = "contif";
                return true;
            }
        }
        return false;
    }

    void Visit(const IfStatement &ifStatement) override
    {
        _MaybeNewLineIndent();
        bool isMultiline = _ShouldBeMultiline(&ifStatement);

        std::string breakOrContinueIf;
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
            {
                out.out << "(cond ";

                // Now the main part
                SET_MULTILINEMODE(isMultiline);

                {
                    INDENT_BLOCK;
                    for (auto &conditionAndCode : conditionAndCodes)
                    {
                        _MaybeNewLineIndent();
                        out.out << "(";
                        bool isConditionMultiline = _ShouldBeMultiline(conditionAndCode.first);
                        // Condition value
                        {
                            _SkipNextSpace();
                            SET_MULTILINEMODE(isConditionMultiline); // Generally not multiline
                            _MaybeIndentAccept(*conditionAndCode.first);
                        }

                        // This is generally multiline. It definitely is if the condition is though.
                        {
                            SET_MULTILINEMODE(isConditionMultiline || _ShouldBeMultiline(conditionAndCode.second)); // Generally not multiline
                            _MaybeIndentAccept(*conditionAndCode.second);

                            // Move the closing paren in here, since we want it to be inline if everything was inline.
                            _MaybeNewLineIndentNoSpace();
                            out.out << ")";
                        }
                    }

                    if (finalElse)
                    {
                        _MaybeNewLineIndent();
                        SET_MULTILINEMODE(_ShouldBeMultiline(finalElse));
                        out.out << "(else";
                        {
                            // This is generally multiline
                            {
                                _MaybeIndentAccept(*finalElse);
                            }
                        }
                        _MaybeNewLineIndentNoSpace();
                        out.out << ")";
                    }
                }

                _MaybeNewLineIndentNoSpace();
                out.out << ")";
            }
        }
        else if (_GetSingleBreakContinueStatementInThenBlock(ifStatement, breakOrContinueIf))
        {
            // This is:
            //      (if (...) break)    ; or continue
            //
            // We change it to
            //      (breakif (...))     ; or contif
            out.out << "(" << breakOrContinueIf;

            SET_MULTILINEMODE(isMultiline);
            _MaybeIndentAccept(*ifStatement.GetCondition());
            _MaybeNewLineIndentNoSpace();
            out.out << ")";
        }
        else
        {
            out.out << "(if";

            {
                bool isConditionMultiline = _ShouldBeMultiline(ifStatement.GetCondition().get());
                SET_MULTILINEMODE(isConditionMultiline);
                _MaybeIndentAccept(*ifStatement.GetCondition());
            }

            // Now the main part
            SET_MULTILINEMODE(isMultiline);

            {
                _MaybeIndentAccept(*ifStatement.GetStatement1());
            }

            // Is there an else?
            if (ifStatement.HasElse())
            {
                _MaybeNewLineIndent();
                out.out << "else";
                _MaybeIndentAccept(*ifStatement.GetStatement2());
            }
            _MaybeNewLineIndentNoSpace();
            out.out << ")";
        }
    }

    void Visit(const Asm &asmStatement) override
    {
        // Let's use a more custom text formatting.
        int labelSize = 0;
        if (!asmStatement.GetLabel().empty())
        {
            _MaybeNewLineNoIndent();
            out.out << asmStatement.GetLabel() << ":";
        }

        // Move forward to the current indent level.
        _MaybeNewLineIndent();

        // output the instruction
        out.out << asmStatement.GetName();

        // Move forward to a common column
        string spaces;
        spaces.append(max(0, 8 - asmStatement.GetName().size()), ' ');
        out.out << spaces;

        {
            GO_INLINE;
            Forward(asmStatement.GetStatements(), ", ");
        }
    }

    void Visit(const AsmBlock &asmSection) override
    {
        out.disallowedTokens = &GetOpcodeSet();

        GO_MULTILINE;
        _MaybeNewLineIndent();
        out.out << "(asm";
        _MaybeIndentAcceptChildren(asmSection.GetStatements());
        _MaybeNewLineIndent();
        out.out << ")";

        // Can't have asmblock inside another, so it's ok to not have a stack here, and just force values.
        out.disallowedTokens = nullptr;
    }

    void Visit(const ExportEntry &exportEntry) override
    {
        _MaybeNewLineIndent();
        out.out << CleanTokenSCI(exportEntry.Name) << " " << exportEntry.Slot;
    }

    void Visit(const ClassDefDeclaration &classDef)
    {
        GO_MULTILINE;
        _MaybeNewLineIndent();
        out.out << "(classdef " << classDef.GetName();
        {
            INDENT_BLOCK;
            _MaybeNewLineIndent();
            out.out << "script# ";
            _OutputNumber(out.out, classDef.ScriptNumber, false, true);
            _MaybeNewLineIndent();
            out.out << "class# ";
            _OutputNumber(out.out, classDef.ClassNumber, false, true);
            _MaybeNewLineIndent();
            out.out << "super# ";
            _OutputNumber(out.out, classDef.SuperNumber, false, true);
            _MaybeNewLineIndent();
            out.out << "file# " << "\"" << classDef.File << "\"";
            _MaybeNewLineIndent();
            _MaybeNewLineIndent();

            out.out << "(properties";
            {
                INDENT_BLOCK;
                for (auto &prop : classDef.Properties)
                {
                    _MaybeNewLineIndent();
                    out.out << prop.first << " ";
                    _OutputNumber(out.out, prop.second, false, true);
                }
            }
            _MaybeNewLineIndent();
            out.out << ")";

            _MaybeNewLineIndent();
            _MaybeNewLineIndent();
            out.out << "(methods";
            {
                INDENT_BLOCK;
                for (auto &method : classDef.Methods)
                {
                    _MaybeNewLineIndent();
                    out.out << method;
                }
            }
            _MaybeNewLineIndent();
            out.out << ")";
        }
        _MaybeNewLineIndent();
        out.out << ")";
    }
    void Visit(const GlobalDeclaration &global)
    {
        _MaybeNewLineIndent();
        GO_INLINE;
        out.out << global.GetName() << " " << global.Index;
        if (global.InitialValue)
        {
            out.out << " =";
            _MaybeIndentAccept(*global.InitialValue);
        }
    }
    void Visit(const ExternDeclaration &theExtern)
    {
        _MaybeNewLineIndent();
        out.out << theExtern.GetName();
        GO_INLINE;
        theExtern.ScriptNumber.Accept(*this);
        _MaybeNewLineIndent();
        out.out << theExtern.Index;
    }
    void Visit(const SelectorDeclaration &selector)
    {
        _MaybeNewLineIndent();
        out.out << selector.GetName() << " " << selector.Index;
    }


    // Measure the size of code output in a first pass, so we can better

    void Enter(const SyntaxNode &node) override
    {
        if (_isCalculateSizePass)
        {
            _nodeSize[&node] = (size_t)out.out.tellp();
        }
        else
        {
            // Keep position updated
            if (_currentPosition < node.GetPosition())
            {
                _currentPosition = node.GetPosition();
            }
        }
    }
    void Leave(const SyntaxNode &node) override
    {
        if (_isCalculateSizePass)
        {
            bool forceNonInline = false;
            switch (node.GetNodeType())
            {
                // For things that are always non-inline, put a large number here.
                // This will make it so that, for instance, a procedure call that contains
                // a switch statement for a parameter will automatically go non-inline.
                case NodeTypeSwitch:
                case NodeTypeWhileLoop:
                case NodeTypeDoLoop:
                case NodeTypeForLoop:
                    forceNonInline = true;
                    break;
                default:
                    bool doDefault = true;
                    if (node.GetNodeType() == NodeTypeIf)
                    {
                        // This will turn into a cond statement, which is always non-inline
                        if (_GetSingleIfStatementInElseBlock(static_cast<const IfStatement&>(node)))
                        {
                            forceNonInline = true;
                        }
                    }
            }

            if (forceNonInline)
            {
                _nodeSize[&node] = 1000;
            }
            else
            {
                _nodeSize[&node] = (size_t)out.out.tellp() - _nodeSize[&node];
            }

        }
        else
        {
            // Keep position updated
            if (_currentPosition < node.GetPosition())
            {
                _currentPosition = node.GetPosition();
            }
        }
    }

private:
    bool _isCalculateSizePass;
    std::unordered_map<const SyntaxNode*, size_t> _nodeSize;
};

void OutputSourceCode_SCI(const sci::Script &script, sci::SourceCodeWriter &out)
{
    SCISourceCodeFormatter output(out);
    output.DoTheThing(script);
}

void OutputSourceCode_SCI(const sci::ClassDefinition &classDef, sci::SourceCodeWriter &out)
{
    SCISourceCodeFormatter output(out);
    output.DoTheThing(classDef);
}

void OutputSourceCode_SCI(const sci::MethodDefinition &method, sci::SourceCodeWriter &out)
{
    SCISourceCodeFormatter output(out);
    output.DoTheThing(method);
}

void OutputSourceCode_SCI(const sci::ClassProperty &classProp, sci::SourceCodeWriter &out)
{
    SCISourceCodeFormatter output(out);
    output.DoTheThing(classProp);
}