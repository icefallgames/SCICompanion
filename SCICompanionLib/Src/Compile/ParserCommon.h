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

#include "ParseAutoCompleteContext.h"

// Common parser routines


#ifdef DEBUG
// Uncomment this to get parsing debug spew. It's still very flaky
// #define PARSE_DEBUG 1
#endif

//
// Syntax parser inspired by boost's Spirit (but much less flexible and more performant)
// 

#ifdef DEBUG
#define _DEBUG_PARSER(x) (x).SetName(#x);
#else
#define _DEBUG_PARSER(x)
#endif

namespace sci
{
    enum class CommentType;
}

//
// A comment was detected - add it (text and endpoints) to the script.
//
template<typename TContext, typename _It>
inline void _DoComment(TContext *pContext, const _It &streamBegin, const _It &streamEnd, sci::CommentType type)
{
    std::string comment;
    // Transfer to string:
    copy(streamBegin, streamEnd, back_inserter(comment));
    // Create a new Comment syntax node and add it to the script
    std::unique_ptr<sci::Comment> pComment = std::make_unique<sci::Comment>(comment, type); // TODO
    pComment->SetPosition(streamBegin.GetPosition());
    pComment->SetEndPosition(streamEnd.GetPosition());
    pContext->TryAddCommentDirectly(pComment);
    if (pComment)
    {
        pContext->Script().AddComment(move(pComment));
    }
}

class EatCommentCpp
{
public:
    // c++ style comments:
    // // and /***/
    template<typename TContext, typename _It>
    static void EatWhitespaceAndComments(TContext *pContext, _It &stream)
    {
        bool fDone = false;
        while (!fDone)
        {
            fDone = true;
            // Eat whitespace
            while (isspace(*stream))
            {
                fDone = false;
                ++stream;
            }
            _It streamSave(stream);
            if (*stream == '/')
            {
                char ch = *(++stream);
                if (ch == '/')
                {
                    // Indicate we're in a comment for autocomplete's sake
                    if (pContext)
                    {
                        pContext->PushParseAutoCompleteContext(BlockAllChannels);
                    }

                    // Go until end of line
                    while ((ch = *(++stream)) && (ch != '\n')) {} // Look for \n or EOF
                    fDone = false; // Check for whitespace again

                    // Comment gathering.  This may be expensive, so only do this if pContext is non-NULL
                    if (pContext)
                    {
                        pContext->PopParseAutoCompleteContext();
                        // If there were previous non-whitespace chars on this line, consider this comment "positioned".
                        bool foundNonWhitespace = false;
                        _It streamLineBegin(streamSave);
                        streamLineBegin.ResetLine();
                        while (!foundNonWhitespace && (streamLineBegin != streamSave))
                        {
                            foundNonWhitespace = !isspace(*streamLineBegin);
                            ++streamLineBegin;
                        }
                        sci::CommentType type = foundNonWhitespace ? sci::CommentType::Positioned : sci::CommentType::Indented;
                        _DoComment(pContext, streamSave, stream, type);
                    }

                    if (ch == '\n') // As opposed to EOF
                    {
                        ++stream; // Move past \n now that we've done _DoComment. Otherwise the comment will be tagged as ending on the next line.
                    }
                }
                else if (ch == '*')
                {
                    // Indicate we're in a comment for autocomplete's sake
                    if (pContext)
                    {
                        pContext->PushParseAutoCompleteContext(BlockAllChannels);
                    }

                    // Go until */
                    bool fLookingForSlash = false;
                    while (TRUE)
                    {
                        ch = *(++stream);
                        if (ch == 0)
                        {
                            break;
                        }
                        else if (ch == '*')
                        {
                            fLookingForSlash = true;
                        }
                        else if (fLookingForSlash)
                        {
                            if (ch == '/')
                            {
                                break; // We found a */
                            }
                            else
                            {
                                fLookingForSlash = false;
                            }
                        }
                    }
                    if (ch == '/') // As opposed to 0
                    {
                        ++stream; // Move past '/'
                    }

                    if (pContext)
                    {
                        pContext->PopParseAutoCompleteContext();
                        _DoComment(pContext, streamSave, stream, sci::CommentType::LeftJustified);
                    }
                    fDone = false; // Check for whitespace again
                }
                else
                {
                    // We're done.
                    stream = streamSave;
                    fDone = true;
                }
            }
        }
    }
};

class EatCommentSemi
{
public:
    // Semi-colon style comments:
    // ;
    template<typename TContext, typename _It>
    static void EatWhitespaceAndComments(TContext *pContext, _It &stream)
    {
        bool fDone = false;
        while (!fDone)
        {
            fDone = true;
            // Eat whitespace
            while (isspace(*stream))
            {
                fDone = false;
                ++stream;
            }
            _It streamSave(stream);
            if (*stream == ';')
            {
                // Indicate we're in a comment for autocomplete's sake
                if (pContext)
                {
                    pContext->PushParseAutoCompleteContext(BlockAllChannels);
                }

                char ch;
                // Go until end of line, after counting the number of semicolons
                int semiCount = 0;
                while ((ch = *stream) && (ch == ';'))
                {
                    semiCount++;
                    ++stream;
                }
                // Now stream points to after the semicolons
                while ((ch = *stream) && (ch != '\n')) { ++stream; } // Look for \n or EOF
                // Now stream points to after the newline
                fDone = false; // Check for whitespace again

                // Comment gathering.  This may be expensive, so only do this if pContext is non-NULL
                if (pContext)
                {
                    pContext->PopParseAutoCompleteContext();
                    // ;    -> positinoed
                    // ;;   -> indented to code level
                    // ;;;  -> left-justified
                    sci::CommentType type = (semiCount == 1) ? sci::CommentType::Positioned : ((semiCount == 2) ? sci::CommentType::Indented : sci::CommentType::LeftJustified);
                    _DoComment(pContext, streamSave, stream, type);
                }

                if (ch == '\n') // As opposed to EOF
                {
                    ++stream; // Move past \n now that we've done _DoComment. Otherwise the comment will be tagged as ending on the next line.
                }
            }
        }
    }
};


//
// Optimized delimiter reader for SCI Script
//
// Unlike for SCI Studio script, this continues until the closing quote (instead of dealing with '+')
//
// Forced spaces are indicated by underscores.
// Underscores are indicated by \_
//
template<typename _TContext, typename _It, char Q1, char Q2>
bool _ReadStringSCI(_It &stream, std::string &str)
{
    str.clear();
    if (Q1 == *stream)
    {
        char chPrev = 0;
        char ch;
        // Continue while no EOF, and while the character isn't the closing quote
        // (unless the previous character was an escape char)
        bool fEscape = false;
        bool addedSpace = false;
        bool lookingForSecondHex = false;
        int chHex = 0;
        while ((ch = *(++stream)) && ((ch != Q2) || (chPrev == '\\')))
        {
            chPrev = ch;
            bool processCharNormally = true;
            if (lookingForSecondHex)
            {
                if (isxdigit(ch))
                {
                    chHex *= 16;
                    chHex += charToI(ch);
                    str += (char)chHex;
                } // Otherwise, just ignore the whole thing
                processCharNormally = false;
            }
            lookingForSecondHex = false;
            if (fEscape)
            {
                processCharNormally = false;
                // Last char was an escape
                switch (ch)
                {
                    case 'n':
                        str += '\n';
                        break;
                    case 't':
                        str += '\t';
                        break;
                    case '_':
                        // \_ is an underscore.
                        str += "_";
                        break;
                        // TODO: HExes
                    default:
                        if (isxdigit(ch))
                        {
                            chHex = charToI(ch);
                            lookingForSecondHex = true;
                        }
                        else
                        {
                            str += "\\"; // Add it, the following char was not an escape char
                            processCharNormally = true; // Add the current char too
                        }
                }
            }
            fEscape = false;
            if (processCharNormally)
            {
                if (isspace(ch))
                {
                    // Add one space for all continguous whitespace
                    if (!addedSpace)
                    {
                        str += ' ';
                    }
                    addedSpace = true;
                }
                else
                {
                    addedSpace = false; // Reset this
                    if (ch == '\\')
                    {
                        // Escape char, don't add to string.
                        fEscape = true;
                    }
                    else if (ch == '_')
                    {
                        // Underscores are forced spaces
                        str += ' ';
                    }
                    else
                    {
                        str += ch;
                    }
                }
            }
        }
        if (ch == Q2)
        {
            ++stream;
            return true; // We're done
        }
        else
        {
            assert(ch == 0); // EOF
            return false;
        }
    }
    return false;
}

//
// Optimized delimiter reader
//
template<typename _TContext, typename _CommentPolicy, typename _It, char Q1, char Q2>
bool _ReadStringStudio(_It &stream, std::string &str)
{
    str.clear();
    while (Q1 == *stream)
    {
        char chPrev = 0;
        char ch;
        // Continue while no EOF, and while the character isn't the closing quote
        // (unless the previous character was an escape char)
        bool fEscape = false;
        while ((ch = *(++stream)) && ((ch != Q2) || (chPrev == '\\')))
        {
            chPrev = ch;
            if (!fEscape)
            {
                if (ch == '\\')
                {
                    // Escape char, don't add to strign.
                    fEscape = true;
                }
                else
                {
                    str += ch;
                }
            }
            else
            {
                // Last char was an escape char.
                fEscape = false;
                switch(ch)
                {
                case 'n':
                    str += '\n';
                    break;
                case 't':
                    str += '\t';
                    break;
                default:
                    str += ch;
                    break;
                }
            }
        }
        if (ch == Q2)
        {
            ++stream;
            if (*stream == '+')
            {
                // If we encountered a '+', then skip whitespace until we hit another open quote
                // "huwehuierger"+   "gejriger"
                ++stream;
                _CommentPolicy::EatWhitespaceAndComments<_TContext, _It>(nullptr, stream);
                // ... and continue...
            }
            else
            {
                return true; // We're done
            }
        }
        else
        {
            ASSERT(ch == 0); // EOF
            return false;
        }
    }
    return false;
}

int charToI(char ch);



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
            _pacc = NoChannels;
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
    ParserBase() : _pfn(nullptr), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(true), _psz(nullptr), _pacc(NoChannels) {}
    ParserBase(MATCHINGFUNCTION pfn) : _pfn(pfn), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(false), _psz(nullptr), _pacc(NoChannels)  {}
    ParserBase(MATCHINGFUNCTION pfn, const ParserBase &a) : _pfn(pfn), _pa(new ParserBase(a)), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(false), _psz(nullptr), _pacc(NoChannels)  {}
    ParserBase(MATCHINGFUNCTION pfn, const char *psz) : _pfn(pfn), _psz(psz), _pfnA(nullptr), _pfnDebug(nullptr), _pRef(nullptr), _fLiteral(false), _fOnlyRef(false), _pacc(NoChannels)
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
        newOne._pacc = SetChannel(newOne._pacc, ParseAutoCompleteChannel::One, aac.pacc);
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
    bool ReadStringStudio(_It &stream, std::string &str) const
    {
        return _ReadStringStudio<_TContext, _CommentPolicy, _It, Q1, Q2>(stream, str);
    }

    std::unique_ptr<ParserBase> _pa;
    std::vector<std::unique_ptr<ParserBase>> _parsers;
    const char *_psz;
    // PERF: perhaps we could optimize for some cases here, and not have a matching functino (e.g. char)
    MATCHINGFUNCTION _pfn;
    ACTION _pfnA;
    ParseACChannels _pacc;
    DEBUGFUNCTION _pfnDebug;
    const ParserBase *_pRef;
    bool _fLiteral; // Don't skip whitespace
    bool _fOnlyRef; // Only references to this parser... it's lifetime is guaranteed.
};

extern const int AltKeys[26];


//
// Handles negation, hex, binary, character literals, etc...
//
template<typename _It, typename _TContext, typename _CommentPolicy>
bool IntegerExpandedP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
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
    else if (*stream == '%')
    {
        // Binary
        fHex = true; // Seems reasonable.
        ++stream;
        auto value = *stream;
        while (value == '0' || value == '1')
        {
            i *= 2;
            i += charToI(value);
            fRet = true;
            ++stream;
            value = *stream;
        }
    }
    else if (*stream == '`')
    {
        // Character literal.
        ++stream;
        auto value = *stream;
        ++stream;
        switch (value)
        {
            case '@':
            {
                // This is an alt key
                int index = toupper(*stream) - 'A';
                ++stream;
                if ((index >= 0) && (index < ARRAYSIZE(AltKeys)))
                {
                    i = AltKeys[index] << 8;
                    fRet = true;
                }
                break;
            }
            case '#':
            {
                // A function key
                i = toupper(*stream);
                ++stream;
                if (i == '0') // F10 appears to be `#0
                {
                    i += 10;
                }
                i = (i - '1' + 0x3b) << 8;
                fRet = true;
                break;
            }
            case '^':
            {
                // A control key
                i = toupper(*stream) - '@';
                ++stream;
                fRet = true;
                break;
            }
            default:
                i = toupper(value);
                fRet = true;
                break;

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
