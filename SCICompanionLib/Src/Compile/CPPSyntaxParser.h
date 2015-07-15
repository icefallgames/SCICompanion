#pragma once

/***************************************************/
// Currently, the CPP-syntax parser is not fully
// functional, so it is not accessible through the UI
/***************************************************/

#include "ScriptOM.h"
#include "CrystalScriptStream.h"
#include "ParserCommon.h"
#include <stack>

#ifndef noexcept
#define noexcept
#endif

//#define PARSE_DEBUG

// The kind of iterator we use.
typedef CrystalScriptTokenStream::const_iterator tokenIt;

#ifdef DEBUG
// On 6/13/2007, the biggest any nodebay got was 48.  It suprises me it's this big.
extern size_t g_biggestNodeBay;
#endif



// AST trees


#define AST_NODE_TYPES \
    ENUM_OR_STRING(None),   \
    ENUM_OR_STRING(Script),   \
    ENUM_OR_STRING(Define),   \
    ENUM_OR_STRING(Include),   \
    ENUM_OR_STRING(ScriptNumber),   \
    ENUM_OR_STRING(Synonym),   \
    ENUM_OR_STRING(ScriptVariable),   \
    ENUM_OR_STRING(Name),   \
    ENUM_OR_STRING(VariableSize),   \
    ENUM_OR_STRING(VariableDeclarationsOfType),   \
    ENUM_OR_STRING(AlphaNumeric),   \
    ENUM_OR_STRING(Integer),   \
    ENUM_OR_STRING(Said),   \
    ENUM_OR_STRING(String),   \
    ENUM_OR_STRING(Public),   \
    ENUM_OR_STRING(Private),   \
    ENUM_OR_STRING(Instance),   \
    ENUM_OR_STRING(Class),   \
    ENUM_OR_STRING(SuperClass),   \
    ENUM_OR_STRING(Method),   \
    ENUM_OR_STRING(Parameter),   \
    ENUM_OR_STRING(TypeSpecifier),   \
    ENUM_OR_STRING(Property),   \
    ENUM_OR_STRING(Assignment),   \
    ENUM_OR_STRING(LValue),   \
    ENUM_OR_STRING(RValue),   \
    ENUM_OR_STRING(Statement),   \
    ENUM_OR_STRING(Binary),   \
    ENUM_OR_STRING(Block),   \
    ENUM_OR_STRING(Use), \
    ENUM_OR_STRING(Unary), \
    ENUM_OR_STRING(Procedure), \
    ENUM_OR_STRING(If), \
    ENUM_OR_STRING(Else), \
    ENUM_OR_STRING(Switch), \
    ENUM_OR_STRING(Do), \
    ENUM_OR_STRING(While), \
    ENUM_OR_STRING(For), \
    ENUM_OR_STRING(Initializer), \
    ENUM_OR_STRING(Condition), \
    ENUM_OR_STRING(Looper), \
    ENUM_OR_STRING(Break), \
    ENUM_OR_STRING(Return), \
    ENUM_OR_STRING(ExpressionSource), \
    ENUM_OR_STRING(IndexedVariable), \
    ENUM_OR_STRING(ProcedureCall), \
    ENUM_OR_STRING(PropertyAccess), \
    ENUM_OR_STRING(MethodCall), \
    ENUM_OR_STRING(Index), \
    ENUM_OR_STRING(Case), \
    ENUM_OR_STRING(Selector), \
    ENUM_OR_STRING(Pointer), \
    ENUM_OR_STRING(Rest), \
    ENUM_OR_STRING(Default), \
    ENUM_OR_STRING(ExpressionRoot), \
    ENUM_OR_STRING(MoreParameters), \
    ENUM_OR_STRING(FunctionSignature), \
    ENUM_OR_STRING(Optional), \
    ENUM_OR_STRING(Cast), \
    ENUM_OR_STRING(Res), \
    ENUM_OR_STRING(End), \
    ENUM_OR_STRING(Ternary), \

#undef ENUM_OR_STRING
#define ENUM_OR_STRING(x) x

class ASTNode
{
public:
    // Can't find a good pool allocator.
    /*
    static void *operator new(unsigned int size);
    void operator delete(void *p);
    */
    enum Type
    {
        AST_NODE_TYPES
    };

    ASTNode(const ASTNode &src) = delete;
    ASTNode &operator=(const ASTNode &src) = delete;

    ASTNode()
    {
        _type = Type::None;
        _number = 0;
        _intFlags = IFNone;
    }
    ASTNode(Type type, LineCol startPos, LineCol endPos)
    {
        _type = type;
        _number = 0;
        _intFlags = IFNone;
        _startPos = startPos;
        _endPos = endPos;
    }
    ASTNode(const std::string &text, Type type, LineCol startPos, LineCol endPos)
    {
        _type = type;
        _text = text;
        _number = 0;
        _intFlags = IFNone;
        _startPos = startPos;
        _endPos = endPos;
    }
    ASTNode(int number, IntegerFlags intFlags, Type type, LineCol startPos, LineCol endPos)
    {
        _type = type;
        _number = number;
        _intFlags = intFlags;
        _startPos = startPos;
        _endPos = endPos;
    }
    ~ASTNode()
    {
        // Some data 2007/07/29 from compiling Game.scp
        // _childNodes.capacity()   frequency
        // 0                        4547
        // 1                        4088
        // 2                        1074
        // 3                        133
        // 4                        181
        // 5                        43
        // PERF: Considering that with MS's vector reallocs occur from 0->1, 1->2, 2->3
        // it might make sense to preallocate a bit when adding our first item.
        // It could save about 20% of the allocations here (assuming they're all from push_back,
        // which is not true).
    }

#ifdef DEBUG
    void EnsureConsistency()
    {
        for (size_t i = 0; i < _childNodes.size(); ++i)
        {
            assert(_childNodes[i].get() != this); // REVIEW, this fired when I built help while browse info was being compiled
        }
    }
#else
    void EnsureConsistency() {}
#endif

    Type GetType() { return _type; }
    const std::string &GetText() { return _text; }
    int GetNumber() { return _number; }
    IntegerFlags GetIntegerFlags() { return _intFlags; }
    std::vector<std::unique_ptr<ASTNode>> &Children() { return _childNodes; }
    LineCol GetPosition() const { return _startPos; }

private:
    std::vector<std::unique_ptr<ASTNode>> _childNodes;
    Type _type;
    std::string _text;
    int _number;
    IntegerFlags _intFlags;
    LineCol _startPos;
    LineCol _endPos;
};

class CPPSyntaxContext
{
public:
    class ParseLevel
    {
    public:
        // Can't find a good pool allocator, but we need one for speed.
        /*
        static void *operator new(unsigned int size)
        {
            assert(size == sizeof(ParseLevel));
            void *p = _ParseLevelPool.malloc();
            return p;
        }
        void operator delete(void *p)
        {
            ParseLevel *pToDelete = ((ParseLevel*)p);
            _ParseLevelPool.free(pToDelete);
        }
        */

        ParseLevel() : _pRoot(nullptr) {}

        ~ParseLevel()
        {
            // Strange - _nodeBay.size is never more than 1 here (and usually 0)
            for (ASTNode *pNode : _nodeBay)
            {
                delete pNode;
            }
            delete _pRoot;
        }

        std::vector<ASTNode*> &Bay() { return _nodeBay; }
        ASTNode *GetRoot() { return _pRoot; }
        ASTNode *StealRoot() { ASTNode *pTemp = _pRoot; _pRoot = nullptr; return pTemp; }
        // Add a node and designate it as root of this level
        void AssignRoot(ASTNode *pNode)
        {
            assert(_pRoot == nullptr); // We'll probably have to change this.
            _pRoot = pNode;
            if (!_nodeBay.empty())
            {
                _pRoot->Children().insert(_pRoot->Children().end(), _nodeBay.begin(), _nodeBay.end());
                _nodeBay.clear(); // Don't need to delete, since they are owned by the root's children.
                _pRoot->EnsureConsistency();
                // If this assert fires, and pNode is a freshly allocated thing, it means that one of
                // the nodes in _nodeBay has been deleted, but we didn't remove it from _nodeBay.
            }
        }
        // Replace the root of this level with this node (and make the old root become a child of the new node)
        void InsertRoot(ASTNode *pNode)
        {
            if (_pRoot)
            {
                // pNode becomes the new root.  The root becomes a child of _pRoot.
                assert(_nodeBay.empty());
                std::unique_ptr<ASTNode> temp(_pRoot);
                pNode->Children().push_back(std::move(temp));
                pNode->EnsureConsistency();
                _pRoot = pNode; // Takes ownership
            }
            else
            {
                AssignRoot(pNode);
            }
        }
        // Add a regular node.
        void AddNode(ASTNode *pNode)
        {
            if (_pRoot)
            {
                std::unique_ptr<ASTNode> temp(pNode);
                _pRoot->Children().push_back(std::move(temp));
                _pRoot->EnsureConsistency();
            }
            else
            {
                _nodeBay.push_back(pNode);
            }
#ifdef DEBUG
            g_biggestNodeBay = max(g_biggestNodeBay, _nodeBay.size());
#endif
            assert(_nodeBay.size() != 48);
        }
        // Accumulate the nodes from another level.
        void AddFromOther(ParseLevel *pLevel)
        {
            if (pLevel->GetRoot())
            {
                std::unique_ptr<ASTNode> pRootOther(pLevel->StealRoot());
                if (_pRoot)
                {
                    _pRoot->Children().push_back(std::move(pRootOther));
                    _pRoot->EnsureConsistency();
                }
                else
                {
                    _nodeBay.push_back(pRootOther.get());
                }
                pRootOther.release();
            }
            else
            {
                // Then add what was in its bay to us.
                std::vector<ASTNode*> bayTemp;
                bayTemp.swap(pLevel->Bay()); // Transfer ownership to us.
                if (_pRoot)
                {
                    _pRoot->Children().insert(_pRoot->Children().end(), bayTemp.begin(), bayTemp.end());
                    _pRoot->EnsureConsistency();
                }
                else
                {
                    _nodeBay.insert(_nodeBay.end(), bayTemp.begin(), bayTemp.end());
                }
                // Don't need to delete the pointers in bayTemp, because we own them now.
            }
#ifdef DEBUG
            g_biggestNodeBay = max(g_biggestNodeBay, _nodeBay.size());
#endif
        }
    private:
        std::vector<ASTNode*> _nodeBay; // Temporary storage. PERF: can vector have a piece of static alloc?
        ASTNode *_pRoot;
    };



    CPPSyntaxContext(tokenIt beginning, sci::Script &script, ICompileLog &log, ScriptId &scriptId) :
        _beginning(beginning), _script(script), _log(log), _scriptId(scriptId)
    {
#ifdef DEBUG
        // On 6/13/2007, for compiling the template game, max depth was 113
        // (of course this changes as the user codes)
        _depth = 0;
        _maxDepth = 0;
#endif
        _pTree = nullptr;
    }
    ~CPPSyntaxContext()
    {
        delete _pTree;
    }

    void ReportError(const char *error, tokenIt pos)
    {
        // Prefer previous errors at the same spot
        if ((_beginning < pos) || (_error == nullptr))
        {
            _error = error;
            _beginning = pos;
        }
    }
    void ReportExpectedChar(char theChar, tokenIt pos)
    {
        // Prefer previous errors at the same spot
        if ((_beginning < pos) || (_error == nullptr))
        {
            _expectedChar = theChar;
            _error = &_expectedChar;
            _beginning = pos;
        }
    }

    void ReportASTError(std::string error, LineCol dwPos)
    {
        // These errors are from the AST evaluation, and accumulate
        std::string errorText = " [Error] ";
        errorText += error;
        // Add 1 to the line number (parser is 0-based, display is 1-based)
        _log.ReportResult(CompileResult(errorText, _scriptId, dwPos.Line() + 1, dwPos.Column(), CompileResult::CRT_Error));
    }
    std::string GetErrorText()
    {
        std::stringstream ss;
        if (_error == &_expectedChar)
        {
            ss << "Expected '" << _expectedChar << "' " << "Line: " << _beginning.GetLineNumber() << "  Column: " << _beginning.GetColumnNumber();
        }
        else
        {
            ss << _error << "Line: " << _beginning.GetLineNumber() << "  Column: " << _beginning.GetColumnNumber();
        }
        return ss.str();
    }
    tokenIt GetErrorPosition()
    {
        return _beginning;
    }

    void TryAddCommentDirectly(std::unique_ptr<sci::Comment> &comment) {}

    // PERF: doing a new and delete here at every level is very expensive, so ParseLevel uses
    // a pool allocator.
    void StartLevel()
    {
        _levels.push(new ParseLevel);
#ifdef DEBUG
        _depth++;
        _maxDepth = max(_maxDepth, _depth);
#endif;
    }
    void EndLevel(bool fSuccess)
    {
        std::unique_ptr<ParseLevel> pOld(_levels.top());
        _levels.pop();
        if (fSuccess)
        {
            // Transfer the previous level's nodes to this one.
            if (_levels.empty())
            {
                // Get the root of the tree.  We're done.
                assert(_pTree == nullptr);
                _pTree = pOld->StealRoot();
            }
            else
            {
                _levels.top()->AddFromOther(pOld.get());
            }
        }
#ifdef DEBUG
        _depth--;
#endif;
    }
    ParseLevel *LevelPtr() { return _levels.top(); }

    std::string &ScratchString() { return _scratch; }
    sci::Script &Script() { return _script; }
    int Integer;
    IntegerFlags IntFlags;

    void DebugOutputAST();
    void EvaluateAST();

private:
    ICompileLog &_log;
    const char *_error;     // Use char* not std::string (for perf - called thousands of times during a parse)
    char _expectedChar;     // _error may == &_expectedChar.  Again, for perf, but it doesn't seem to make much diff.
    sci::Script &_script;
    ScriptId _scriptId;
    std::string _scratch;

    tokenIt _beginning;

    // For AST (abstract syntax tree)
    std::stack<ParseLevel*> _levels;
#ifdef DEBUG
        int _depth;
        int _maxDepth;
#endif
    ASTNode *_pTree;
};

#ifdef PARSE_DEBUG
extern int g_ParseIndent2;
#endif



class TokenParser
{
public:

    class MatchResult
    {
    public:
        MatchResult() = delete;
        MatchResult(bool fResult) : _fResult(fResult) {}
        MatchResult(const MatchResult &src) = default;
        MatchResult& operator=(MatchResult& src) = default;
        ~MatchResult() { }
        bool Result() { return _fResult; }
    private:
        bool _fResult;
    };

#ifdef DEBUG
    void SetName(const std::string &name) { Name = name; }
    std::string Name;
#endif

    typedef bool(*MATCHINGFUNCTION)(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
    typedef void(*DEBUGFUNCTION)(bool fEnter, bool fResult);
    typedef void(*ACTION)(MatchResult &match, const TokenParser *pParser, CPPSyntaxContext *pContext, const CrystalScriptTokenStream::const_iterator &stream);

    // Fwd decl (Used for circular references in grammer descriptions)
    // If an empty Parser is created (since you need to refer to it subsequently, but you can't
    // define it yet), it will be endowed with this matching function.
    bool static ReferenceForwarderP(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream)
    {
        assert(FALSE);
        return false; // Just a dummy return value
    }

    void static CopyParserVector(std::vector<std::unique_ptr<TokenParser>> &out, const std::vector<std::unique_ptr<TokenParser>> &in)
    {
        out.clear();
        for (auto &parser : in)
        {
            std::unique_ptr<TokenParser> pCopy = std::make_unique<TokenParser>(*parser);
            out.push_back(std::move(pCopy));
        }
    }

    TokenParser(const TokenParser& src)
    {
        // Allocate new Parser objects
        if (src._fOnlyRef)
        {
            _pa.reset(nullptr);
            _token = TokenInvalid;
            _pRef = &src;
            _pfn = ReferenceForwarderP;
            _pfnA = nullptr;
            _pfnDebug = nullptr;
            _fOnlyRef = false; // We're a ref, so people can copy us.
            //assert(src._pfn != ReferenceForwarderP<CrystalScriptTokenStream>); 
        }
        else
        {
            _pa.reset(src._pa ? new TokenParser(*src._pa) : nullptr);
            _token = src._token;
            _pfn = src._pfn;
            _pfnA = src._pfnA;
            _pfnDebug = src._pfnDebug;
            _pRef = src._pRef; // Don't make a new object.
            _fOnlyRef = false; // Copied parsers are temporary objects, generally, and so we shouldn't take references to them.
            if (src._pfn == nullptr)
            {
                // No matching function means this is an empty parser... pass a ref to the source
                _pRef = &src;
                _pfn = ReferenceForwarderP;
                assert(src._pfn != ReferenceForwarderP); 
            }
            CopyParserVector(_parsers, src._parsers);
        }
#ifdef DEBUG
        Name = src.Name;
#endif
        //assert(_pfn != ReferenceForwarderP<CrystalScriptTokenStream> || _pRef->_pfn != ReferenceForwarderP<CrystalScriptTokenStream>);
    }

    TokenParser& operator=(const TokenParser& src)
    {
        if (this != &src)
        {
            // Allocate new Parser objects
            _pa.reset(src._pa ? new TokenParser(*src._pa) : nullptr);
            _token = src._token;
            _pfn = src._pfn;
            _pfnA = src._pfnA;
            _pfnDebug = src._pfnDebug;
            _pRef = src._pRef; // Don't make a new object.
            assert(_fOnlyRef);
            if ((src._pfn == nullptr) || src._fOnlyRef)
            {
                //assert(FALSE); // We probably never hit this - but if we do, we should check if things are right.
                // No matching function means this is an empty parser... pass a ref to the source
                _pRef = &src;
                _pfn = ReferenceForwarderP;
            }
            else
            {
                CopyParserVector(_parsers, src._parsers);
            }
#ifdef DEBUG
            Name = src.Name;
#endif
        }
        return *this;
    }

    // The default constructor will create an object that can only be copied by reference (see the copy constructor
    // and == operator, and _pRef)
    TokenParser() : _pfn(nullptr), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fOnlyRef(true), _token(TokenInvalid) {}
    TokenParser(MATCHINGFUNCTION pfn) : _pfn(pfn), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fOnlyRef(false), _token(TokenInvalid) {}
    TokenParser(MATCHINGFUNCTION pfn, const TokenParser &a) : _pfn(pfn), _pa(new TokenParser(a)), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fOnlyRef(false), _token(TokenInvalid) {}
    TokenParser(MATCHINGFUNCTION pfn, SyntaxToken token) : _pfn(pfn), _token(token), _pfnA(nullptr), _pfnDebug(nullptr), _pa(nullptr), _pRef(nullptr), _fOnlyRef(false) {}
    MatchResult Match(CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream) const
    {
        assert(_pfn);
        if (_fOnlyRef)
        {
            pContext->StartLevel();
        }
        CrystalScriptTokenStream::const_iterator streamSave(stream);
#ifdef PARSE_DEBUG
        if (!Name.empty())
        {
            std::string name = !Name.empty() ? Name : StringFromSyntaxTokenDEBUG(_token);
            std::string text;
            CrystalScriptTokenStream::const_iterator streamTemp = stream;
            text += StringFromSyntaxTokenDEBUG((*streamTemp).type);
            if (text[0])
                text += StringFromSyntaxTokenDEBUG((*++streamTemp).type);
            if (text[1])
                text += StringFromSyntaxTokenDEBUG((*++streamTemp).type);
            std::stringstream ss;
            std::string spaces;
            spaces.append(g_ParseIndent2, ' ');
            ss << spaces << "-->Matching " << name << ": " << text << "\n";
            OutputDebugString(ss.str().c_str());
        }
        g_ParseIndent2++;
#endif
        MatchResult result(_pRef ? _pRef->Match(pContext, stream) :
            (*_pfn)(this, pContext, stream));
        
#ifdef PARSE_DEBUG
        g_ParseIndent2--;
        if (!Name.empty() || (_token != TokenInvalid))
        {
            std::string name = !Name.empty() ? Name : StringFromSyntaxTokenDEBUG(_token);
            std::string text;
            CrystalScriptTokenStream::const_iterator streamTemp = stream;
            text += StringFromSyntaxTokenDEBUG((*streamTemp).type);
            if (text[0])
                text += StringFromSyntaxTokenDEBUG((*++streamTemp).type);
            if (text[1])
                text += StringFromSyntaxTokenDEBUG((*++streamTemp).type);
            std::stringstream ss;
            std::string spaces;
            spaces.append(g_ParseIndent2, ' ');
            ss << spaces << "<--Matching " << name << " " << (result.Result() ? "true: " : "false: ") << text << "\n";
            OutputDebugString(ss.str().c_str());
        }
#endif
        if (_pfnA)
        {
            (*_pfnA)(result, this, pContext, streamSave); // Provide starting point of stream to action (stream currently points to end)
        }
        if (!result.Result())
        {
            // Revert the stream to what it was when we came in this function (after whitespace)
            stream = streamSave;
        }
        if (_fOnlyRef)
        {
            pContext->EndLevel(result.Result());
        }
        return result;
    }

    // This is for actions.
    TokenParser operator[](ACTION pfn)
    {
        assert(_pfnA == nullptr); // Ensure we're not overwriting any action.
        TokenParser newOne(*this);
        newOne._pfnA = pfn;
        return newOne;
    }

    // This is for wrapping a parser in another, such as when we want its
    // Match function to always match and act as a pre-action.
    // e.g. syntaxnode_d[value[FinishValueA]]
    TokenParser operator[](const TokenParser &parser)
    {
        // This should take a parser
        assert(_pa.get() == nullptr);    // Ensure we're not overwriting a parser.
        TokenParser newOne(*this);
        // We'll contain the provided parser in ourselves.
        newOne._pa.reset(new TokenParser(parser));
        return newOne;
    }

    void SetDebug(DEBUGFUNCTION pfnDebug)
    {
        _pfnDebug = pfnDebug;
    }

    void AddParser(const TokenParser &add)
    {
        // This is for matching functions that need an array
        // Be careful the matching function hasn't been replaced with a reference forwarder!
        assert(_pfn != ReferenceForwarderP);
        _parsers.push_back(std::move(std::make_unique<TokenParser>(add)));
    }

    std::unique_ptr<TokenParser> _pa;
    std::vector<std::unique_ptr<TokenParser>> _parsers;
    SyntaxToken _token;
    // PERF: perhaps we could optimize for some cases here, and not have a matching functino (e.g. char)
    MATCHINGFUNCTION _pfn;
    ACTION _pfnA;
    DEBUGFUNCTION _pfnDebug;
    const TokenParser *_pRef;
    bool _fOnlyRef; // Only references to this parser... it's lifetime is guaranteed.
};


class ICompileLog; // fwd decl
class IReportError;

class CPPSyntaxParser
{
public:
    bool Parse(sci::Script &script, CrystalScriptTokenStream::const_iterator &stream, ICompileLog *pError);
    bool ParseHeader(sci::Script &script, CrystalScriptTokenStream::const_iterator &stream, ICompileLog *pError);
    void Load();

private:
    bool _fLoaded;

    // Grammar rules
    TokenParser property_decl;
    TokenParser properties_decl;
    TokenParser instance_property_decl;
    TokenParser instance_properties_decl;
    TokenParser array_init;
    TokenParser general_token;
    TokenParser assignment_operator;
    TokenParser simple_value;
    TokenParser immediateValue;
    TokenParser code_block;
    TokenParser primary_expression;
    TokenParser statement;
    TokenParser expression;
    TokenParser array_index;
    TokenParser method_call;
    TokenParser function_call;
    TokenParser argument_expression_list;
    TokenParser property_access;
    TokenParser postfix_expression;
    TokenParser unary_operator;
    TokenParser binary_operator;
    TokenParser unary_expression;
    TokenParser unary_expression_core;

    TokenParser expression_statement;
    TokenParser assignment_expression;
    TokenParser assignment_expression_core;
    TokenParser lvalue_expression;
    TokenParser rvalue_expression;
    TokenParser conditional_expression;
    TokenParser ternary_operation;
    TokenParser expression_ternary;
    TokenParser conditional_expression_ternary;
    TokenParser logical_or_expression;
    TokenParser logical_and_expression;
    TokenParser inclusive_or_expression;
    TokenParser cast_operator;
    TokenParser multiplicative_expression;
    TokenParser additive_expression;
    TokenParser shift_expression;
    TokenParser relational_expression;
    TokenParser equality_expression;
    TokenParser and_expression;
    TokenParser exclusive_or_expression;
    TokenParser parameter;
    TokenParser parameter_sequence;
    TokenParser initial_optional_parameter_sequence;
    TokenParser parameter_base;
    TokenParser labeled_statement;
    TokenParser selection_statement;
    TokenParser iteration_statement;
    TokenParser for_loop_initializer;
    TokenParser for_loop_condition;
    TokenParser for_loop_looper;
    TokenParser loop_condition;
    TokenParser jump_statement;
    TokenParser statement_list;
    TokenParser switch_statement;
    TokenParser switch_condition;
    TokenParser break_statement;
    TokenParser rest_statement;
    TokenParser return_statement;
    TokenParser procedure_call;
    TokenParser send_param_call;
    TokenParser send_call;
    TokenParser assignment;
    TokenParser binary_operation;
    TokenParser unary_operation;
    TokenParser variable;
    TokenParser base_conditional;
    TokenParser conditional;
    TokenParser do_loop;
    TokenParser while_loop;
    TokenParser if_statement;
    TokenParser for_loop;
    TokenParser case_statement;
    TokenParser default_statement;
    TokenParser class_decl;
    TokenParser procedure_decl;
    TokenParser entire_script;
    TokenParser entire_header;
    TokenParser function_var_decl;
    TokenParser method_decl;
    TokenParser procedure_base;
    TokenParser classbase_decl;
    TokenParser instancebase_decl;
    TokenParser include;
    TokenParser use;
    TokenParser define;
    TokenParser instance_decl;
    TokenParser synonyms;
    TokenParser script_var;
    TokenParser script_string;
    TokenParser property_value;
    TokenParser var_decl;
    TokenParser function_signature;
    TokenParser function_array_init;
};

//
// Common parsing primitives
//

bool BasicMatch(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool MatchString(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool MatchNumber(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool AlwaysMatch(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool TSequenceP(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool TAlternativeP(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool TKleeneP(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool TZeroOrOnceP(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);
bool TOneOrMoreP(const TokenParser *pParser, CPPSyntaxContext *pContext, CrystalScriptTokenStream::const_iterator &stream);

//
// *a
// 
// Matches n instances of a  (where n can be 0)
//
inline TokenParser operator*(const TokenParser &a)
{
    return TokenParser(TKleeneP, a);
}

//
// +a
// 
// 1 or more instances of a
//
inline TokenParser operator+(const TokenParser &a)
{
    return TokenParser(TOneOrMoreP, a);
}

//
// a >> b
// 
// Matches a followed by b
//
inline TokenParser operator>>(const TokenParser &a, const TokenParser &b)
{
    // We must create a new "sequence" thing if a is not a sequence, or if a has an action.
    // If a has an action, then we'll mis-place it if we don't create a new sequence.
    // e.g.
    // c = a >> b;
    // e = c[act1] >> d[act2]
    // would result in
    // Seq[act1]
    //      a
    //      b
    //      d[act2]
    // The result would be act2 gets called prior to act1.
    // It looks like this optimization won't be useful, since most things have actions.
    if ((a._pfn != TSequenceP) || (a._pfnA))
    {
        TokenParser alternative(TSequenceP);
        alternative.AddParser(a);
        alternative.AddParser(b);
        return alternative;
    }
    else
    {
        // A is already an alternative.  Add b to it.
        TokenParser parserA(a);
        if (parserA._pfn == TSequenceP)
        {
            parserA.AddParser(b);
            return parserA;
        }
        else
        {
            // The matching function changed (reference forwarding...) we can't just add it...
            TokenParser alternative(TSequenceP);
            alternative.AddParser(a);
            alternative.AddParser(b);
            return alternative;
        }
    }
}

//
// a | b
// 
// Matches a or b
//
inline TokenParser operator|(const TokenParser &a, const TokenParser &b)
{
    // See operator>>
    if ((a._pfn != TAlternativeP) || (a._pfnA))
    {
        TokenParser alternative(TAlternativeP);
        alternative.AddParser(a);
        alternative.AddParser(b);
        return alternative;
    }
    else
    {
        // A is already an alternative.  Add b to it.
        TokenParser parserA(a);
        if (parserA._pfn == TAlternativeP)
        {
            parserA.AddParser(b);
            return parserA;
        }
        else
        {
            // The matching function of a changed... we can no longer just add.
            TokenParser alternative(TAlternativeP);
            alternative.AddParser(a);
            alternative.AddParser(b);
            return alternative;
        }
    }
}

//
// (a % b)     
//
// equivalent to    (a >> *(b >> a))
//
inline TokenParser operator%(const TokenParser &a, const TokenParser &b)
{
    return a >> *(b >> a);
}

//
// !a
// 
// 0 or 1 instances of a
//
inline TokenParser operator!(const TokenParser &a)
{
    return TokenParser(TZeroOrOnceP, a);
}

