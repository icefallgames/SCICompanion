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

#include "ScriptOM.h"
#include "CrystalScriptStream.h"
#include "ParserCommon.h"
#include "AutoCompleteContext.h"
#include "PMachine.h"
#include "SyntaxContext.h"
#include <stack>
#include <deque>

class MatchResult
{
public:
    MatchResult() = delete;
    MatchResult(bool fResult) : _fResult(fResult) {}
    MatchResult(const MatchResult &src) = default;
    MatchResult& operator=(const MatchResult& src) = default;

    ~MatchResult() { }
    void ChangeResult(bool result) {
        _fResult = result;
    }
    bool Result() { return _fResult; }
private:
    bool _fResult;
};


template<typename _TContext, typename _It, typename _CommentPolicy>
class ParserBase : private _CommentPolicy
{
    using _CommentPolicy::EatWhitespaceAndComments;

public:
#ifdef DEBUG
    void SetName(const std::string &name) { Name = name; }
    std::string Name;
#endif

    typedef bool(*MATCHINGFUNCTION)(const ParserBase *pParser, _TContext *pContext, _It &stream);
    typedef void(*DEBUGFUNCTION)(bool fEnter, bool fResult);
    typedef void(*ACTION)(MatchResult &match, const ParserBase *pParser, _TContext *pContext, const _It &stream);

    struct ActionAndContext
    {
        ACTION pfn;
        ParseAutoCompleteContext pacc;
        PCSTR pszDebugName;
    };

    // Fwd decl (Used for circular references in grammer descriptions)
    // If an empty Parser is created (since you need to refer to it subsequently, but you can't
    // define it yet), it will be endowed with this matching function.
    template<typename _It>
    bool static ReferenceForwarderP(const ParserBase *pParser, _TContext *pContext, _It &stream)
    {
        assert(FALSE);
        return false; // Just a dummy return value
    }

    template<typename _It>
    void static CopyParserVector(std::vector<std::unique_ptr<ParserBase>> &out, const std::vector<std::unique_ptr<ParserBase>> &in)
    {
        out.clear();
        for (auto &parser : in)
        {
            std::unique_ptr<ParserBase> pCopy = std::make_unique<ParserBase>(*parser);
            out.push_back(std::move(pCopy));
        }
    }

    ParserBase(const ParserBase& src)
    {
        // Allocate new Parser objects
        if (src._fOnlyRef)
        {
            _pa.reset(nullptr);
            _psz = nullptr;
            _pRef = &src;
            _pfn = ReferenceForwarderP<_It>;
            _pfnA = nullptr;
            _pacc = ParseAutoCompleteContext::None;
            _pfnDebug = nullptr;
            _fLiteral = false; // Doesn't matter
            _fOnlyRef = false; // We're a ref, so people can copy us.
            //assert(src._pfn != ReferenceForwarderP<_It>); 
        }
        else
        {
            _pa.reset(src._pa ? new ParserBase(*src._pa) : nullptr);
            _psz = src._psz; // Ok, because this is always a static string
            _pfn = src._pfn;
            _pfnA = src._pfnA;
            _pacc = src._pacc;
            _pfnDebug = src._pfnDebug;
            _pRef = src._pRef; // Don't make a new object.
            _fLiteral = src._fLiteral;
            _fOnlyRef = false; // Copied parsers are temporary objects, generally, and so we shouldn't take references to them.
            if (src._pfn == nullptr)
            {
                // No matching function means this is an empty parser... pass a ref to the source
                _pRef = &src;
                _pfn = ReferenceForwarderP<_It>;
                assert(src._pfn != ReferenceForwarderP<_It>); 
            }
            CopyParserVector<_It>(_parsers, src._parsers);
        }
#ifdef DEBUG
        Name = src.Name;
#endif
    }

    ParserBase& operator=(const ParserBase& src)
    {
        if (this != &src)
        {
            // Allocate new Parser objects
            _pa.reset(src._pa ? new ParserBase(*src._pa) : nullptr);
            _psz = src._psz; // Ok, because this is always a static string
            _pfn = src._pfn;
            _pfnA = src._pfnA;
            _pacc = src._pacc;
            _pfnDebug = src._pfnDebug;
            _pRef = src._pRef; // Don't make a new object.
            _fLiteral = src._fLiteral;
            assert(_fOnlyRef);
            if ((src._pfn == nullptr) || src._fOnlyRef)
            {
                //assert(FALSE); // We probably never hit this - but if we do, we should check if things are right.
                // No matching function means this is an empty parser... pass a ref to the source
                _pRef = &src;
                _pfn = ReferenceForwarderP<_It>;
            }
            else
            {
                CopyParserVector<_It>(_parsers, src._parsers);
            }
#ifdef DEBUG
            Name = src.Name;
#endif
        }
        return *this;
    }

    // The default constructor will create an object that can only be copied by reference (see the copy constructor
    // and == operator, and _pRef)
    ParserBase() : _pfn(nullptr), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(true), _psz(nullptr), _pacc(ParseAutoCompleteContext::None) {}
    ParserBase(MATCHINGFUNCTION pfn) : _pfn(pfn), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(false), _psz(nullptr), _pacc(ParseAutoCompleteContext::None)  {}
    ParserBase(MATCHINGFUNCTION pfn, const ParserBase &a) : _pfn(pfn), _pa(new ParserBase(a)), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(false), _psz(nullptr), _pacc(ParseAutoCompleteContext::None)  {}
    ParserBase(MATCHINGFUNCTION pfn, const char *psz) : _pfn(pfn), _psz(psz), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(false), _pacc(ParseAutoCompleteContext::None)
    {
    }
    MatchResult Match(_TContext *pContext, _It &stream) const
    {
        assert(_pfn);
        if (!_fLiteral)
        {
            EatWhitespaceAndComments<_TContext, _It>(pContext, stream);
        }
        _It streamSave(stream);
#ifdef PARSE_DEBUG
        string text;

        if (pContext->ParseDebug && (!Name.empty() || _psz))
        {
            text = stream.GetLookAhead(5);
            string name = !Name.empty() ? Name : _psz;
            std::stringstream ss;
            string spaces;
            spaces.append(pContext->ParseDebugIndent, ' ');
            ss << spaces << "-->Matching " << name << " against " << text << "\n";
            OutputDebugString(ss.str().c_str());
        }
        pContext->ParseDebugIndent++;
#endif
        pContext->PushParseAutoCompleteContext(_pacc);
        MatchResult result(_pRef ? _pRef->Match(pContext, stream) : (*_pfn)(this, pContext, stream));
        pContext->PopParseAutoCompleteContext();
#ifdef PARSE_DEBUG
        pContext->ParseDebugIndent--;
        if (pContext->ParseDebug && (!Name.empty() || _psz))
        {
            string name = !Name.empty() ? Name : _psz;
            std::stringstream ss;
            string spaces;
            spaces.append(pContext->ParseDebugIndent, ' ');
            ss << spaces << (result.Result() ? "   TRUE" : "   FALSE") << "\n";
            OutputDebugString(ss.str().c_str());
        }
#endif
        if (_pfnA)
        {
            (*_pfnA)(result, this, pContext, stream);
        }
        if (!result.Result())
        {
            // Revert the stream to what it was when we came in this function (after whitespace)
            stream.Restore(streamSave);
        }
        return result;
    }

    // This is for actions.
    ParserBase operator[](ACTION pfn)
    {
        assert(_pfnA == nullptr); // Ensure we're not overwriting any action.
        ParserBase newOne(*this);
        newOne._pfnA = pfn;
        return newOne;
    }

    ParserBase operator[](ActionAndContext aac)
    {
        assert(_pfnA == nullptr); // Ensure we're not overwriting any action.
        ParserBase newOne(*this);
        newOne._pfnA = aac.pfn;
        newOne._pacc = aac.pacc;
#ifdef PARSE_DEBUG
        if (aac.pszDebugName)
        {
            newOne.Name = aac.pszDebugName;
        }
#endif
        return newOne;
    }

    // This is for wrapping a parser in another, such as when we want its
    // Match function to always match and act as a pre-action.
    // e.g. syntaxnode_d[value[FinishValueA]]
    ParserBase operator[](const ParserBase &parser)
    {
        // This should take a parser
        assert(_pa.get() == nullptr);    // Ensure we're not overwriting a parser.
        ParserBase newOne(*this);
        // We'll contain the provided parser in ourselves.
        newOne._pa.reset(new ParserBase(parser));
        return newOne;
    }

    // Returns a parser that is literal (doesn't skip whitespace prior to matching)
    ParserBase operator+()
    {
        ParserBase newOne(*this);
        newOne._fLiteral = true;
        return newOne;
    }

    void SetDebug(DEBUGFUNCTION pfnDebug)
    {
        _pfnDebug = pfnDebug;
    }

    void AddParser(const ParserBase &add)
    {
        // This is for matching functions that need an array
        // Be careful the matching function hasn't been replaced with a reference forwarder!
        assert(_pfn != ReferenceForwarderP<_It>);
        _parsers.push_back(std::move(std::make_unique<ParserBase>(add)));
    }

    template<typename _TContext, typename _It, char Q1, char Q2>
    bool ReadString(_It &stream, std::string &str) const
    {
        return _ReadString<_TContext, _CommentPolicy, _It, Q1, Q2>(stream, str);
    }


    std::unique_ptr<ParserBase> _pa;
    std::vector<std::unique_ptr<ParserBase>> _parsers;
    const char *_psz;
    // PERF: perhaps we could optimize for some cases here, and not have a matching functino (e.g. char)
    MATCHINGFUNCTION _pfn;
    ACTION _pfnA;
    ParseAutoCompleteContext _pacc;
    DEBUGFUNCTION _pfnDebug;
    const ParserBase *_pRef;
    bool _fLiteral; // Don't skip whitespace
    bool _fOnlyRef; // Only references to this parser... it's lifetime is guaranteed.
};

//
// Common parsing primitives
//
template<typename _It, typename _TContext, typename _CommentPolicy>
bool AlphanumP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    std::string &str = pContext->ScratchString();
    str.clear();
    char ch = *stream;
    if (isalpha(ch) || (ch == '_'))     // First character must be a letter or _
    {
        fRet = true;
        str += ch;
        ch = *(++stream);
        while (isalnum(ch) || (ch == '_'))  // Then any alphanumeric character is fine.
        {
            fRet = true;
            str += ch;
            ch = *(++stream);
        }
    }
    return fRet;
}

// Asm instructions can include '?' in the name too
template<typename _It, typename _TContext, typename _CommentPolicy>
bool AsmInstructionP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    std::string &str = pContext->ScratchString();
    str.clear();
    char ch = *stream;
    if (isalpha(ch) || (ch == '_') || (ch == '&') || (ch == '-') || (ch == '+'))     // First character must be a letter or _ or & or + or - (for the rest instruction, or inc/dec)
    {
        fRet = true;
        str += ch;
        ch = *(++stream);
        while (isalnum(ch) || (ch == '_') || (ch == '?'))  // Then any alphanumeric character is fine, or ?
        {
            fRet = true;
            str += ch;
            ch = *(++stream);
        }
    }

    if (fRet)
    {
        fRet = IsOpcode(str);
    }

    return fRet;
}



extern const char *g_keywords[4];

// TODO: Refactor with above
template<typename _It, typename _TContext, typename _CommentPolicy>
bool AlphanumPNoKeyword(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = AlphanumP(pParser, pContext, stream);
    if (fRet)
    {
        std::string &str = pContext->ScratchString();
        for (int i = 0; fRet && (i < ARRAYSIZE(g_keywords)); i++)
        {
            fRet = (str != g_keywords[i]);
        }
        if (fRet && pContext->extraKeywords)
        {
            fRet = pContext->extraKeywords->find(str) == pContext->extraKeywords->end();
        }
    }
    return fRet;
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool SequenceP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    for (auto &parser : pParser->_parsers)
    {
        if (!parser->Match(pContext, stream).Result())
        {
            return false;
        }
    }
    return true;
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool AlternativeP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    for (auto &parser : pParser->_parsers)
    {
        if (parser->Match(pContext, stream).Result())
        {
            return true;
        }
    }
    return false;
    return false;
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool KleeneP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    while (pParser->_pa->Match(pContext, stream).Result())
    {
        while (isspace(*stream))
        {
            ++stream;
        }
    }
    return true; // Always matches
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool ZeroOrOnceP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    pParser->_pa->Match(pContext, stream);
    return true; // Always matches
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool NotP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return !pParser->_pa->Match(pContext, stream).Result();
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool QuotedStringP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return pParser->ReadString<_TContext, _It, '"', '"'>(stream, pContext->ScratchString());
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool SQuotedStringP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return pParser->ReadString<_TContext, _It, '\'', '\''>(stream, pContext->ScratchString());
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool BraceStringP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return pParser->ReadString<_TContext, _It, '{', '}'>(stream, pContext->ScratchString());
}

//
// Handles negation, hex, etc...
//
template<typename _It, typename _TContext, typename _CommentPolicy>
bool IntegerP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    int i = 0;
    bool fNeg = false;
    bool fRet = false;
    bool fHex = false;
    if (*stream == '$')
    {
        fHex = true;
        ++stream;
        while (isxdigit(*stream))
        {
            i *= 16;
            i += charToI(*stream);
            fRet = true;
            ++stream;
        }
    }
    else
    {
        if (*stream == '-')
        {
            fNeg = true;
            ++stream;
        }
        while (isdigit(*stream))
        {
            i *= 10;
            i += charToI(*stream);
            fRet = true;
            ++stream;
        }
    }
    if (fRet)
    {
        // Make sure that the number isn't followed by an alphanumeric char
        fRet = !isalnum(*stream);
    }
    if (fNeg)
    {
        i = -i;
    }
    if (fRet)
    {
        // Let the context know so people can use it.
        pContext->SetInteger(i, fNeg, fHex, stream);
    }
    return fRet;
}


template<typename _It, typename _TContext, typename _CommentPolicy>
bool AlphanumOpenP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    std::string &str = pContext->ScratchString();
    str.clear();
    char ch = *stream;
    if (isalpha(ch) || (ch == '_'))
    {
        fRet = true;
        str += ch;
        ch = *(++stream);
        while (isalnum(ch) || (ch == '_'))
        {
            fRet = true;
            str += ch;
            ch = *(++stream);
        }
    }
    if (fRet && (*stream == '('))
    {
        ++stream;
        return true;
    }
    else
    {
        return false;
    }
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool AlwaysMatchP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return true; // Always matches
}


//
// Matches a single character
//
template<typename _It, typename _TContext, typename _CommentPolicy>
bool CharP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    const char *psz = pParser->_psz;
    while (*psz && (*stream == *psz))
    {
        ++stream;
        ++psz;
    }
    return (*psz == 0);
}

//
// Matches a keyword (e.g. a piece of text NOT followed by an alphanumeric character).
//
template<typename _It, typename _TContext, typename _CommentPolicy>
bool KeywordP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    const char *psz = pParser->_psz;
    while (*psz && (*stream == *psz))
    {
        ++stream;
        ++psz;
    }
    if (*psz == 0)
    {
        // We used up the whole keyword.  Make sure it isn't followed by another alpha numeric char.
        return !isalnum(*stream);
    }
    return false;
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool OperatorP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    const char *psz = pParser->_psz;
    while (*psz && (*stream == *psz))
    {
        ++stream;
        ++psz;
    }
    if (*psz == 0)
    {
        pContext->ScratchString() = pParser->_psz;

        char chNext = *stream;
        // REVIEW: we're relying on careful ordering of rules to have this parser work.
        // The "operator" parser needs to make sure there isn't another "operator char" following
        // this:
        if ((chNext != '|') && (chNext != '&') && (chNext != '=') && (chNext != '<') && (chNext != '>'))
        {
            // Probably more...
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}



//
// Parser operators
//

//
// *a
// 
// Matches n instances of a  (where n can be 0)
//
template<typename _It, typename _TContext, typename _CommentPolicy>
inline ParserBase<_TContext, _It, _CommentPolicy> operator*(const ParserBase<_TContext, _It, _CommentPolicy> &a)
{
    return ParserBase<_TContext, _It, _CommentPolicy>(KleeneP<_It, _TContext, _CommentPolicy>, a);
}

//
// a >> b
// 
// Matches a followed by b
//
template<typename _It, typename _TContext, typename _CommentPolicy>
inline ParserBase<_TContext, _It, _CommentPolicy> operator>>(const ParserBase<_TContext, _It, _CommentPolicy> &a, const ParserBase<_TContext, _It, _CommentPolicy> &b)
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
    if ((a._pfn != SequenceP<_It, _TContext, _CommentPolicy>) || (a._pfnA))
    {
        ParserBase<_TContext, _It, _CommentPolicy> alternative(SequenceP<_It, _TContext, _CommentPolicy>);
        alternative.AddParser(a);
        alternative.AddParser(b);
        return alternative;
    }
    else
    {
        // A is already an alternative.  Add b to it.
        ParserBase<_TContext, _It, _CommentPolicy> parserA(a);
        if (parserA._pfn == SequenceP<_It, _TContext, _CommentPolicy>)
        {
            parserA.AddParser(b);
            return parserA;
        }
        else
        {
            // The matching function changed (reference forwarding...) we can't just add it...
            ParserBase<_TContext, _It, _CommentPolicy> alternative(SequenceP<_It, _TContext, _CommentPolicy>);
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
template<typename _It, typename _TContext, typename _CommentPolicy>
inline ParserBase<_TContext, _It, _CommentPolicy> operator|(const ParserBase<_TContext, _It, _CommentPolicy> &a, const ParserBase<_TContext, _It, _CommentPolicy> &b)
{
    // See operator>>
    if ((a._pfn != AlternativeP<_It, _TContext, _CommentPolicy>) || (a._pfnA))
    {
        ParserBase<_TContext, _It, _CommentPolicy> alternative(AlternativeP<_It, _TContext, _CommentPolicy>);
        alternative.AddParser(a);
        alternative.AddParser(b);
        return alternative;
    }
    else
    {
        // A is already an alternative.  Add b to it.
        ParserBase<_TContext, _It, _CommentPolicy> parserA(a);
        if (parserA._pfn == AlternativeP<_It, _TContext, _CommentPolicy>)
        {
            parserA.AddParser(b);
            return parserA;
        }
        else
        {
            // The matching function of a changed... we can no longer just add.
            ParserBase<_TContext, _It, _CommentPolicy> alternative(AlternativeP<_It, _TContext, _CommentPolicy>);
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
template<typename _It, typename _TContext, typename _CommentPolicy>
inline ParserBase<_TContext, _It, _CommentPolicy> operator%(const ParserBase<_TContext, _It, _CommentPolicy> &a, const ParserBase<_TContext, _It, _CommentPolicy> &b)
{
    return a >> *(b >> a);
}

//
// -a
// 
// 0 or 1 instances of a
//
template<typename _It, typename _TContext, typename _CommentPolicy>
inline ParserBase<_TContext, _It, _CommentPolicy> operator-(const ParserBase<_TContext, _It, _CommentPolicy> &a)
{
    return ParserBase<_TContext, _It, _CommentPolicy>(ZeroOrOnceP<_It, _TContext, _CommentPolicy>, a);
}

//
// !a
// 
// true if a fails to match
//
template<typename _It, typename _TContext, typename _CommentPolicy>
inline ParserBase<_TContext, _It, _CommentPolicy> operator!(const ParserBase<_TContext, _It, _CommentPolicy> &a)
{
    return ParserBase<_TContext, _It, _CommentPolicy>(NotP<_It, _TContext, _CommentPolicy>, a);
}


//
// Common error primitives
//
template<typename _It, typename _TContext, typename _CommentPolicy>
void GeneralE(MatchResult &match, const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, const _It &stream)
{
    assert(pParser->_psz); // Not valid to call this if there isn't something we can say was expected.
    if (!match.Result())
    {
        std::string error = "Expected \"";
        error += pParser->_psz;
        error += "\" ";
        pContext->ReportError(error, stream);
    }
}

// Our parser...
typedef ParserBase<SyntaxContext, streamIt, EatCommentCpp> Parser;

class ICompileLog; // fwd decl

class IReportError
{
public:
    virtual void ReportError(PCTSTR pszError) = 0;
};

class StudioSyntaxParser
{
public:
    bool Parse(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool addCommentsToOM);
    bool Parse(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, SyntaxContext &context);
    bool ParseHeader(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError);
    void Load();

private:
    bool _fLoaded;

    // Grammar rules

    Parser statement;
    Parser immediateValue;
    Parser string_immediateValue;
    Parser string_immediateValue2;
    Parser general_token;
    Parser pointer;
    Parser selector;
    Parser value;
    Parser simple_value;
    Parser property_value;
    Parser property_value_expanded;
    Parser property_decl;
    Parser properties_decl;
    Parser array_init;
    Parser string_array_init;

    // Operators
    Parser binary_operator;
    Parser unary_operator;
    Parser assignment_operator;
    // Variable declaration
    Parser var_decl;

    // General code pieces
    Parser variable;
    Parser base_conditional_v1;
    Parser base_conditional_v2;
    Parser conditional;
    Parser conditional_v1;
    Parser conditional_v2;
    Parser do_loop;
    Parser while_loop;
    Parser if_statement;
    Parser asm_statement;
    Parser asm_block;
    Parser asm_label;
    Parser asm_arg;
    Parser ternary_expression;
    Parser for_loop;
    Parser case_statement;
    Parser default_statement;
    Parser switch_statement;
    Parser break_statement;
    Parser rest_statement;
    Parser return_statement;
    // Force the oppar to come right after the alphanum_token, to eliminate ambiguity
    // = gWnd clBlack (send gEgo:x)        = gWnd Print("foo")         (= button (+ 5 5))
    Parser procedure_call;
    Parser send_param_call;
    Parser send_call;
    Parser code_block;
    Parser code_block_no_paren;
    Parser assignment;
    Parser binary_operation;
    Parser unary_operation;

    // Functions
    Parser function_var_decl;
    Parser function_var_decl_inner;
    Parser function_var_decl_begin;
    Parser method_base;
    Parser method_decl;
    Parser procedure_base;

    // Classes
    Parser classbase_decl;
    Parser instancebase_decl;

    // Main script section.
    Parser include;
    Parser use;
    Parser version;
    Parser define;
    Parser exports;
    Parser export_entry;
    Parser scriptNum;
    Parser instance_decl;
    Parser class_decl;
    Parser procedure_decl;
    Parser synonyms;
    Parser script_var;
    Parser script_string;
  
    Parser entire_script;
    Parser entire_header;


    // Additional ones required for c++ syntax
    Parser expression_statement;
    Parser argument_expression_list;
    Parser expression;
    Parser assignment_expression;
    Parser assignment_expression_core;
    Parser lvalue_expression;
    Parser rvalue_expression;
    Parser unary_expression;
    Parser unary_expression_core;
    Parser conditional_expression;
    Parser logical_or_expression;
    Parser logical_and_expression;
    Parser inclusive_or_expression;
    Parser primary_expression;
    Parser postfix_expression;
    Parser cast_expression;
    Parser multiplicative_expression;
    Parser additive_expression;
    Parser shift_expression;
    Parser relational_expression;
    Parser equality_expression;
    Parser and_expression;
    Parser exclusive_or_expression;
    Parser parameter;
    Parser labeled_statement;
    Parser selection_statement;
    Parser iteration_statement;
    Parser for_loop_initializer;
    Parser for_loop_condition;
    Parser for_loop_looper;
    Parser jump_statement;
    Parser array_index;
    Parser method_call;
    Parser function_call;
    Parser property_access;
    Parser postfix_expression_root;
    Parser statement_list;
};
