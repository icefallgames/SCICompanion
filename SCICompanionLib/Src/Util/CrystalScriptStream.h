#pragma once

#include "CCrystalTextBuffer.h"
#include "CompileInterfaces.h"

class ISyntaxParserCallback
{
public:
    // Returns true if we should continue, or false if we should bail
    virtual bool Done() = 0;
};

class ReadOnlyTextBuffer
{
public:
    ReadOnlyTextBuffer(CCrystalTextBuffer *pBuffer);
    ReadOnlyTextBuffer(CCrystalTextBuffer *pBuffer, CPoint limit, int extraSpace);

    int GetLineCount() { return _lineCount; }
    int GetLineLength(int nLine);
    PCTSTR GetLineChars(int nLine);
    CPoint GetLimit() { return _limit; }
    void Extend(const std::string &extraChars);

private:
    struct StartAndLength
    {
        int Start;
        int Length;
    };

    int _lineCount;
    std::unique_ptr<StartAndLength[]> _lineStartsAndLengths;
    std::vector<char> _text;
    int _extraSpace;
    CPoint _limit;
};

//
// Limits the text buffer to a particular line/char
// (used for autocomplete and tooltips)
//
class CScriptStreamLimiter
{
public:
    CScriptStreamLimiter(CCrystalTextBuffer *pBuffer);
    CScriptStreamLimiter(CCrystalTextBuffer *pBuffer, CPoint ptLimit, int extraSpace);

    ~CScriptStreamLimiter()
    {
    }

    void Extend(const std::string &extraChars) { _pBuffer->Extend(extraChars); }

    CPoint GetLimit()
    {
        return _pBuffer->GetLimit();
    }

    void SetCallback(ISyntaxParserCallback *pCallback)
    {
        _pCallback = pCallback;
    }

    // Reflect some methods on CCrystalTextBuffer:
	int GetLineCount()
    {
        return _pBuffer->GetLineCount(); // REVIEW: Might be off by one.
        // + 1 since we really want to include the current line in the calc
        // return min(_nLineLimit + 1, );
    }

    int _id;
    int *GetIdPtr() { return &_id; }    // REVIEW: What's this for?

	int GetLineLength(int nLine)
    {
        return _pBuffer->GetLineLength(nLine);
        // REVIEW: Might have some off by one errors here.
        /*
        int iLength = 0;
        if (nLine == _nLineLimit + 1)
        {
            ASSERT(FALSE);
        }
        if (nLine == _nLineLimit)
        {
            // Last line.  Limit it.
            // nCharLimit + 1, since we really want to include the point *after* the cursor pos.
            iLength = min(_nCharLimit, _pBuffer->GetLineLength(nLine));
        }
        else
        {
            iLength = _pBuffer->GetLineLength(nLine);
        }
        return iLength;*/
    }

    LPCTSTR GetLineChars(int nLine) { return _pBuffer->GetLineChars(nLine); } // Just fwd through.  We're limited by _nCharLimit
    void GetMoreData(int &nChar, int &nLine, int &nLength, PCSTR &pszLine);
    std::string GetLastWord();
    std::string GetLookAhead(int nLine, int nChar, int nChars);

private:
    std::unique_ptr<ReadOnlyTextBuffer> _pBuffer;
    bool _fCancel;

    // Or tooltips
    ISyntaxParserCallback *_pCallback;
};




class CCrystalScriptStream
{
public:
    CCrystalScriptStream(CScriptStreamLimiter *pLimiter);

    class const_iterator
    {
    public:
		typedef std::forward_iterator_tag iterator_category;
		typedef char value_type;
		typedef std::ptrdiff_t difference_type; // ??
		typedef char& reference;
		typedef char* pointer;

        const_iterator() : _limiter(NULL), _pidStream(NULL), _id(0) {}
        const_iterator(CScriptStreamLimiter *limiter, LineCol dwPos = LineCol());
        char operator*();
        const_iterator& operator++();
        bool operator<(const const_iterator& _Right) const;
        std::string tostring() const;
        LineCol GetPosition() const;
        int GetLineNumber() const;
        int GetColumnNumber() const;
        bool operator==(const const_iterator& value) const;
        bool operator!=(const const_iterator& value) const;
        void Restore(const const_iterator &prev);

        // For debugging
        std::string GetLookAhead(int nChars);

    private:
        CScriptStreamLimiter *_limiter;
        int _nLine;
        int _nChar;
        PCTSTR _pszLine;
        int _nLength;
        int _id;
        int *_pidStream;
    };

    const_iterator begin() { return const_iterator(_pLimiter); }
    const_iterator get_at(LineCol dwPos) { return const_iterator(_pLimiter, dwPos); }

protected:
    CScriptStreamLimiter *_pLimiter;
};



//
// For tokenizing for CPP syntax parser
//
enum SyntaxToken
{
    // REVIEW: I commented some of these out... they result in us tokenizing things when we shouldn't.
    // e.g. x = scriptNumber.  the parser expects scriptNumber to be an alphanumeric thing.  Not a "token".
    // So I don't tokenize it.  An alternative is that we could match some tokens to alphanumeric stuff...
    // but they would have to be marked specially (e.g. we don't want to map "include" for instance).

    TokenComment = 128,
    TokenAlphanum,
    //TokenBool,
    TokenSaidString,
    TokenString,
    TokenSaid,
    TokenBreak,
    TokenCase,
    TokenClass2,
    TokenDefault,
    TokenDo,
    TokenElse,
    TokenFor,
    TokenIf,
    TokenInclude,
    TokenInstance2,
    //TokenInt,
    //TokenUInt,
    TokenPublic,
    TokenPrivate,
    TokenRest,
    TokenRes,
    TokenReturn,
    //TokenScriptNumber,
    TokenScript,
    //TokenSelf2,
    //TokenSuper,
    TokenSwitch,
    TokenUse,
    TokenDefine,
    TokenSynonym,
    //TokenVoid,
    //TokenVar,
    TokenWhile,
    TokenEllipsis,
    TokenRightAssign,
    TokenLeftAssign,
    TokenAddAssign,
    TokenSubAssign,
    TokenMulAssign,
    TokenDivAssign,
    TokenModAssign,
    TokenAndAssign,
    TokenOrAssign,
    TokenXorAssign,
    TokenRightOp,
    TokenLeftOp,
    TokenIncOp,
    TokenDecOp,
    TokenAndOp,
    TokenOrOp,
    TokenLeOp,
    TokenGeOp,
    TokenEqOp,
    TokenNeOp,

    TokenQuotedString,
    TokenSingleQuotedString,
    TokenInteger,
    // etc...

    TokenCast,

    TokenInvalid,
    TokenEnd,
};

class TokenInfo
{
public:
    TokenInfo()
    {
        type = TokenInvalid;
        integer = 0;
        intFlags = IFNone;
    }
    SyntaxToken type;
    LineCol start;      // Start position
    LineCol end;        // One after the end position
    int integer;
    IntegerFlags intFlags;
    std::string string;
};

SyntaxToken SyntaxTokenFromString(const char *psz);
const char *StringFromSyntaxToken(SyntaxToken token);
const char *StringFromSyntaxTokenDEBUG(SyntaxToken token);

namespace sci
{
    class Script;
};


//
// This aggregates a CCrystalScriptStream while being a stream itself.
// It basically provides a tokenized stream.
// THIS IS CURRENTLY UNUSED, AS CPP-STYLE SYNTAX IS NOT SUPPORTED
//
class CrystalScriptTokenStream
{
public:
    CrystalScriptTokenStream(CCrystalScriptStream &buffer) : _stream(buffer) {}
    void Tokenize(sci::Script *pScriptForComments = NULL);
    void DebugPrint();

    class const_iterator
    {
    private:
        std::vector<TokenInfo>::const_iterator _it;
        std::vector<TokenInfo>::const_iterator _itEnd;

    public:
        const_iterator(std::vector<TokenInfo> &tokens, size_t iIndex = 0) :
            //_it(&tokens[iIndex]), _itEnd(tokens.end())
			_it(tokens.begin() + iIndex), _itEnd(tokens.end())
        {
        }
        TokenInfo operator*()
        {
            return *_it;
        }
        const_iterator& operator++()
        {
            ASSERT((*_it).type != TokenEnd); // ASSERT no one advanced past end.
            _it++;
            return *this;
        }
        bool operator<(const const_iterator& _Right) const
        {
            return _it < _Right._it;
        }
        //std::string tostring() const;
        LineCol GetPosition() const
        {
            return _it->start;
        }
        LineCol GetEndPosition() const
        {
            return _it->end;
        }
        int GetLineNumber() const
        {
            return GetPosition().Line();
        }
        int GetColumnNumber() const
        {
            return GetPosition().Column();
        }
        bool operator==(const const_iterator& value) const
        {
            return (_it == value._it);
        }
        bool operator!=(const const_iterator& value) const
        {
            return (_it != value._it);
        }
        const_iterator &operator=(const const_iterator& value)
        {
            _itEnd = value._itEnd;
            _it = value._it;
            return *this;
        }
    };

    const_iterator begin() { return const_iterator(_tokens); }


private:
    void _ConvertAliasedTokens(std::string &token);

    // PERF: provide good starting size based on script size (e.g. reserve).
    // Cost: about 2% of compile time.
    std::vector<TokenInfo> _tokens;
    CCrystalScriptStream &_stream;
};
std::string GetTokenAlias(const std::string &token);

