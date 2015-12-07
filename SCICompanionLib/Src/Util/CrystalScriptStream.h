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
        void ResetLine();

        // For debugging
        std::string GetLookAhead(int nChars);
		int CountPosition(int tabSize) const;

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

private:
    CScriptStreamLimiter *_pLimiter;
};




