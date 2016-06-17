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
#pragma once

#include "CrystalScriptStream.h"
#include "AutoCompleteSourceTypes.h"
#include "ParseAutoCompleteContext.h"

// The kind of iterator we use.
typedef CCrystalScriptStream::const_iterator streamIt;

enum class IfDefDefineState
{
    None,
    False,  // In a clause that is false
    True,   // In a clause that is true
};

class SyntaxContext
{
public:
    SyntaxContext(streamIt beginning, sci::Script &script, std::unordered_set<std::string> preProcessorDefines, bool addCommentsDirectly, bool collectComments) : _beginning(beginning), _script(script), extraKeywords(nullptr), ifDefDefineState(IfDefDefineState::None), _preProcessorDefines(preProcessorDefines), _addCommentsToOM(addCommentsDirectly), _collectComments(collectComments), CurrentStringType(0)
#ifdef PARSE_DEBUG
        , ParseDebug(false), ParseDebugIndent(0)
#endif
    {}

    ~SyntaxContext()
    {
        assert(_statements.empty()); // Or else someone messed up, or there could have been an exception
    }
    void ReportError(const std::string &error, streamIt pos);

    std::string GetErrorText()
    {
        return _error;
    }
    streamIt GetErrorPosition()
    {
        return _beginning;
    }

    char CurrentStringType;

    void PushParseAutoCompleteContext(ParseACChannels pacc)
    {
        // For now, just set. We'll use a stack if necessary.
        _parseAutoCompleteContext.push_back(pacc);
    }
    void PopParseAutoCompleteContext()
    {
        _parseAutoCompleteContext.pop_back();
    }

    std::set<ParseAutoCompleteContext> GetParseAutoCompleteContext() const
    {
        ParseAutoCompleteContext channels[4] = { };
        bool done[4] = { };
        for (auto it = _parseAutoCompleteContext.rbegin(); it != _parseAutoCompleteContext.rend(); ++it)
        {
            for (int i = 0; i < 4; i++)
            {
                if (!done[i])
                {
                    ParseAutoCompleteContext acc = ExtractChannel(*it, (ParseAutoCompleteChannel)i);
                    if (acc == ParseAutoCompleteContext::Block)
                    {
                        done[i] = true;
                    }
                    if (acc != ParseAutoCompleteContext::None)
                    {
                        channels[i] = acc;
                        done[i] = true;
                    }
                }
            }
        }
        return std::set<ParseAutoCompleteContext>(std::begin(channels), std::end(channels));
    }

    // Accessors
    sci::Script &Script() { return _script; }
    std::string &ScratchString() { return _scratch; }
    std::string &ScratchString2() { return _scratch2; }
    void SetInteger(int i, bool fNeg, bool fHex, streamIt pos)
    {
        if ((i > 0xffff) || (i < -32768))
        {
            ReportError("Integer value is too large.", pos);
        }
        Integer = (WORD)i;
        NegInt = fNeg;
        HexInt = fHex;
    }
    uint16_t Integer;   // Last encountered number 
    bool NegInt;        // Was the number a result of negation
    bool HexInt;        // Was the number expressed in hex
    uint16_t Integer2;

    void CreateSynonym() { this->SynonymPtr = std::make_unique<sci::Synonym>(); }
    std::unique_ptr<sci::Synonym> SynonymPtr;

    void CreateDefine() { DefinePtr = std::make_unique<sci::Define>(); }
    std::unique_ptr<sci::Define> DefinePtr;

    void CreateProcedure() {
        FunctionPtr = std::make_unique<sci::ProcedureDefinition>(); FunctionPtr->AddSignature(std::move(std::make_unique<sci::FunctionSignature>()));
    }
    void CreateMethod() {
        FunctionPtr = std::make_unique<sci::MethodDefinition>(); FunctionPtr->AddSignature(std::move(std::make_unique<sci::FunctionSignature>()));
    }
    std::unique_ptr<sci::FunctionBase> FunctionPtr;
    std::unique_ptr<sci::ProcedureDefinition> GetFunctionAsProcedure() {
        return std::unique_ptr<sci::ProcedureDefinition>(static_cast<sci::ProcedureDefinition*>(FunctionPtr.release()));
    }
    std::unique_ptr<sci::MethodDefinition> GetFunctionAsMethod() {
        return std::unique_ptr<sci::MethodDefinition>(static_cast<sci::MethodDefinition*>(FunctionPtr.release()));
    }

    void CreateClass() { ClassPtr = std::make_unique<sci::ClassDefinition>(); }
    std::unique_ptr<sci::ClassDefinition> ClassPtr;

    void CreateClassProperty() { ClassProp = std::make_unique<sci::ClassProperty>(); }
    std::unique_ptr<sci::ClassProperty> ClassProp;

    void StartVariableDecl(const std::string &name)
    {
        VariableDecl = std::make_unique<sci::VariableDecl>();
        VariableDecl->SetName(name);
        VariableDecl->SetSize(1); // Vars are size 1 by default.
        VariableDecl->SetIsUnspecifiedSize(true);
    }

    // 1) Each statement must begin with a push onto the statement stack.
    // 2) The subrule will fill that in with the required node (forloop, assignment, etc...)
    // 3) Once the subrule is complete, we'll either
    //      a) transfer the node to a new statement object, and pop the node of the statement stack (if successful), or
    //      b) delete the node, and pop it off the statement stack.
    void PushSyntaxNode()
    {
        // Put a null on the statement stack to make room for a statement
        _statements.push(std::move(std::unique_ptr<sci::SyntaxNode>()));
    }

    void TryAddCommentDirectly(std::unique_ptr<sci::Comment> &comment)
    {
        if (_addCommentsToOM)   // For now...
        {
            if (!_statements.empty())
            {
                _statements.push(move(comment));
            }
            else if (FunctionPtr)
            {
                FunctionPtr->AddStatement(move(comment));
            }
        }
    }

    // This is the "return value" from a generic statement
    std::unique_ptr<sci::SyntaxNode> StatementPtrReturn;

    void FinishStatement(bool fSuccess)
    {
        if (fSuccess)
        {
            // The statement is moved to the "return value"
            StatementPtrReturn = std::move(_statements.top());
        }
        else
        {
            // We shall never hear of this statement again. Pop will automatically delete it.
        }
        _statements.pop(); // It's now an empty one?
    }

    // Make a syntax node (forloop, assignment, etc...) at the top of the statement stack
    // (use PushSyntaxNode to push a new spot onto the stack)
    template<typename _T>
    void CreateSyntaxNode(const streamIt &stream)
    {
        assert(!_statements.empty());       // That would mean there is no statement stack frame at all.
        if (_statements.top())
        {
            // Someone created a statement, then didn't clean up (e.g. property value was created, but then failed!)
            _statements.top().reset(nullptr);
        }

        std::unique_ptr<_T> p = std::make_unique<_T>();
        // Assign a line/char #
        p->SetPosition(stream.GetPosition());
        _statements.top() = std::move(p);
    }

    template<typename _statementT>
    void ReplaceSyntaxNode(std::unique_ptr<_statementT> node)
    {
        _statements.top().reset(static_cast<SyntaxNode*>(node.release()));
    }

    // Returns a pointer to the first syntax node of this type
    const sci::SyntaxNode *GetSyntaxNode(sci::NodeType type) const;
    // Returns the type of the topmost non-null syntax node
    sci::NodeType GetTopKnownNode() const;

    // Get the syntax node at the top of the statement stack
    template<typename _statementT>
    _statementT* GetSyntaxNode() const
    {
        const std::unique_ptr<sci::SyntaxNode> &node = _statements.top();
        assert(node->GetNodeType() == _statementT::MyNodeType);
        return static_cast<_statementT*>(node.get());
    }

    template<typename _statementT>
    std::unique_ptr<_statementT> StealStatementReturn()
    {
        assert(StatementPtrReturn->GetNodeType() == _statementT::MyNodeType);
        return std::unique_ptr<_statementT>(static_cast<_statementT*>(StatementPtrReturn.release()));
    }

    template<typename _statementT>
    _statementT* GetPrevSyntaxNode() const
    {
        // Get the item that isn't at the top of the stack... it's one up.
        auto prevIt = _statements._Get_container().rbegin();
        prevIt++;
        assert((*prevIt)->GetNodeType() == _statementT::MyNodeType);
        return static_cast<_statementT*>((*prevIt).get());
    }
    // Like GetSyntaxNode, but sets the top of the stack to nullptr (doesn't pop),
    // assuming that ownership has been transfered to the caller.
    template<typename _statementT>
    std::unique_ptr<_statementT> StealSyntaxNode()
    {
        // _statements.top should always be non-null. We could assert, but we do this anyway:
        assert(_statements.top()->GetNodeType() == _statementT::MyNodeType);
        // We just steal it from the top of the stack:
        return std::unique_ptr<_statementT>(static_cast<_statementT*>(_statements.top().release()));
    }
    std::unique_ptr<sci::SyntaxNode> StealSyntaxNode()
    {
        // We just steal it from the top of the stack:
        return std::move(_statements.top());
    }
    void DeleteAndPopSyntaxNode()
    {
        _statements.pop();
    }

    void EvaluateIfDefScratch(const streamIt &stream, const std::string &value)
    {
        if (ifDefDefineState != IfDefDefineState::None)
        {
            ReportError("Already in an #ifdef.", stream);
        }

        if (_preProcessorDefines.find(value) != _preProcessorDefines.end())
        {
            ifDefDefineState = IfDefDefineState::True;
        }
        else
        {
            ifDefDefineState = IfDefDefineState::False;
        }
    }
    void EndIf(const streamIt &stream)
    {
        if (ifDefDefineState == IfDefDefineState::None)
        {
            ReportError("Not in an #ifdef.", stream);
        }
        ifDefDefineState = IfDefDefineState::None;
    }

    void TransferScriptVariable()
    {
        VariableDeclPtr = std::move(VariableDecl);
        VariableDecl = std::make_unique<sci::VariableDecl>();
    }
    std::unique_ptr<sci::VariableDecl> VariableDeclPtr;

    // A generic values for everyone to use (one at a time!)
    // These kinds of objects are "dead ends" on the parse tree, so it should be ok if
    // there is only instance of them.
    sci::PropertyValue PropertyValue;
    bool PropertyValueWasSet = false;
    std::unique_ptr<sci::VariableDecl> VariableDecl;

    // Hack for certain situations where we want to check for more keywords
    std::unordered_set<std::string> *extraKeywords;
    // Hacky/quick way of supported ifdefs for header defines.
    IfDefDefineState ifDefDefineState;

    bool CollectComments() {
        return _collectComments;
    }

private:
    std::unordered_set<std::string> _preProcessorDefines;

    std::string _error;
    streamIt _beginning;
    bool _collectComments;  // Do we care baout comments at all?
    bool _addCommentsToOM;  // And if so, do we add them to the OM?
    sci::Script &_script;
    std::string _scratch;
    std::string _scratch2;
    std::vector<ParseACChannels> _parseAutoCompleteContext;
    std::stack<std::unique_ptr<sci::SyntaxNode>> _statements;

public:
#ifdef PARSE_DEBUG
    bool ParseDebug;
    int ParseDebugIndent = 0;
#endif

};