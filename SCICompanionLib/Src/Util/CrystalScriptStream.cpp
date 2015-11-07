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
#include "CrystalScriptStream.h"
#include "format.h"

// Limit: y is inclusize, x is exclusive.
// e.g. a limit of (1, 2) would mean that we can read all of line 0, and 2 characters of line 1.

CPoint GetNaturalLimit(CCrystalTextBuffer *pBuffer)
{
    CPoint limit;
    limit.y = pBuffer->GetLineCount() - 1;
    if (limit.y >= 0)
    {
        limit.x = pBuffer->GetLineLength(limit.y);
    }
    return limit;
}

ReadOnlyTextBuffer::ReadOnlyTextBuffer(CCrystalTextBuffer *pBuffer) : ReadOnlyTextBuffer(pBuffer, GetNaturalLimit(pBuffer), 0) {}

ReadOnlyTextBuffer::ReadOnlyTextBuffer(CCrystalTextBuffer *pBuffer, CPoint limit, int extraSpace)
{
    _limit = limit;
    _extraSpace = extraSpace;

    int lineCountMinusOne = limit.y;
    _lineCount = lineCountMinusOne + 1;
    int totalCharCount = 0;
    for (int i = 0; i < lineCountMinusOne; i++)
    {
        int charCount = pBuffer->GetLineLength(i);
        totalCharCount += charCount;
    }
    totalCharCount += limit.x;

    int start = 0;
    _text.reserve(totalCharCount + extraSpace);
    _lineStartsAndLengths = std::make_unique<StartAndLength[]>(_lineCount);
    for (int i = 0; i < lineCountMinusOne; i++)
    {
        int charCount = pBuffer->GetLineLength(i);
        std::copy(pBuffer->GetLineChars(i), pBuffer->GetLineChars(i) + charCount, std::back_inserter(_text));
        _lineStartsAndLengths[i].Start = start;
        _lineStartsAndLengths[i].Length = charCount;
        start += charCount;
    }
    assert(pBuffer->GetLineLength(lineCountMinusOne) >= limit.x);
    std::copy(pBuffer->GetLineChars(lineCountMinusOne), pBuffer->GetLineChars(lineCountMinusOne) + limit.x, std::back_inserter(_text));
    _lineStartsAndLengths[lineCountMinusOne].Start = start;
    _lineStartsAndLengths[lineCountMinusOne].Length = limit.x;
}

void ReadOnlyTextBuffer::Extend(const std::string &extraChars)
{
    if ((int)extraChars.length() < _extraSpace)
    {
        OutputDebugString("extending with '");
        OutputDebugString(extraChars.c_str());
        OutputDebugString("'\n");

        std::copy(extraChars.begin(), extraChars.end(), std::back_inserter(_text));
        _lineStartsAndLengths[_lineCount - 1].Length += extraChars.length();
        _extraSpace -= extraChars.length();
        _limit.x += extraChars.length();
    }
}

int ReadOnlyTextBuffer::GetLineLength(int nLine)
{
    return _lineStartsAndLengths[nLine].Length;
}
PCTSTR ReadOnlyTextBuffer::GetLineChars(int nLine)
{
    if (_lineStartsAndLengths[nLine].Length > 0)
    {
        return &_text[_lineStartsAndLengths[nLine].Start];
    }
    return nullptr;
}

CScriptStreamLimiter::CScriptStreamLimiter(CCrystalTextBuffer *pBuffer)
{
    _pBuffer = std::make_unique<ReadOnlyTextBuffer>(pBuffer);
    _pCallback = nullptr;
    _fCancel = false;
}

CScriptStreamLimiter::CScriptStreamLimiter(CCrystalTextBuffer *pBuffer, CPoint ptLimit, int extraSpace)
{
    _pBuffer = std::make_unique<ReadOnlyTextBuffer>(pBuffer, ptLimit, extraSpace);
    _pCallback = nullptr;
    _fCancel = false;
}

// CCrystalScriptStream

CCrystalScriptStream::CCrystalScriptStream(CScriptStreamLimiter *pLimiter)
{
    _pLimiter = pLimiter;
}

// For debugging
std::string CScriptStreamLimiter::GetLookAhead(int nLine, int nChar, int cChars)
{
    PCSTR pszLine = _pBuffer->GetLineChars(nLine);
    int cLineChars = _pBuffer->GetLineLength(nLine);
    std::string text;
    while (pszLine && (nChar < cLineChars) && pszLine[nChar] && (cChars > 0))
    {
        text += pszLine[nChar++];
        --cChars;
    }
    return text;
}

// For autocomplete
std::string CScriptStreamLimiter::GetLastWord()
{
    CPoint pt = _pBuffer->GetLimit();
    PCSTR pszLine = _pBuffer->GetLineChars(pt.y);
    int nChar = pt.x - 1;
    std::string word;
    while (nChar > 0)
    {
        uint8_t theChar = (uint8_t)pszLine[nChar];
        if (isalnum(theChar) || (theChar == '_'))
        {
            word += theChar;
        }
        else
        {
            break;
        }
        nChar--;
    }
    std::reverse(word.begin(), word.end());
    return word;
}

// Ensure we are on the line with nChar and nLine.
void CScriptStreamLimiter::GetMoreData(int &nChar, int &nLine, int &nLength, PCSTR &pszLine)
{
    // If we're limited, call the callback
    if (_pCallback && !_fCancel && (nChar == nLength) && (nLine == (GetLineCount() - 1)))
    {
        // We're limited.
        _fCancel = !_pCallback->Done();
        if (!_fCancel)
        {
            nLength = GetLineLength(nLine);
            pszLine = GetLineChars(nLine);
        }
        else
        {
            // We ran out of data. "go to next line" and specify EOF
            pszLine = "\0"; // EOF
            nLine++;
            nLength = 1;
            nChar = 0;
        }
    }

    while (nChar > nLength)
    {
        // Need to move to the next line
        int cLines = GetLineCount();
        while (nLine < cLines)
        {
            nLine++;

            //OutputDebugString(fmt::format("Moved to line {0} of {1}\n", nLine, cLines).c_str());

            if (nLine < cLines)
            {
                nLength = GetLineLength(nLine);
                pszLine = GetLineChars(nLine);
                assert(pszLine != nullptr || nLength == 0);
                nChar = 0;
                break; // done
            }
        }
        if (nLine == cLines)
        {
            // We ran out of data.
            pszLine = "\0"; // EOF
            nLength = 1;
            nChar = 0;
        }
        // else we're all good.
    }
}

CCrystalScriptStream::const_iterator::const_iterator(CScriptStreamLimiter *limiter, LineCol dwPos) : _limiter(limiter), _pidStream(limiter->GetIdPtr())
{
    _nLine = dwPos.Line();
    _nChar = dwPos.Column();
    if (_nLine < _limiter->GetLineCount())
    {
        _nLength = _limiter->GetLineLength(_nLine);
        _pszLine = _limiter->GetLineChars(_nLine);
    }
    else
    {
        _nLength = 0;
    }
    _id = *_pidStream;
}

char CCrystalScriptStream::const_iterator::operator*()
{
    if (_id == *_pidStream)
    {
        return (_nChar == _nLength) ? '\n' : _pszLine[_nChar];
    }
    else
    {
        _pszLine = _limiter->GetLineChars(_nLine);
        _id = *_pidStream;
        return (_nChar == _nLength) ? '\n' : _pszLine[_nChar];
    }
}

std::string CCrystalScriptStream::const_iterator::GetLookAhead(int nChars)
{
   return  _limiter->GetLookAhead(_nLine, _nChar, nChars);
}

CCrystalScriptStream::const_iterator& CCrystalScriptStream::const_iterator::operator++()
{
    assert((_pszLine == nullptr) || (*_pszLine != 0)); // EOF
    _nChar++;
    if ((_nChar > _nLength) ||
        ((_nChar == _nLength) && (_nLine == (_limiter->GetLineCount() - 1)))) // for == we use '\n', unless this is the last line
    {
        _limiter->GetMoreData(_nChar, _nLine, _nLength, _pszLine);
    }
    return *this;
}

bool CCrystalScriptStream::const_iterator::operator<(const const_iterator& _Right) const
{
    return (_nLine < _Right._nLine) || ((_nLine == _Right._nLine) && ((_nChar < _Right._nChar)));
}

std::string CCrystalScriptStream::const_iterator::tostring() const
{
    char sz[50];
    sprintf_s(sz, sizeof(sz), "Line %d, column %d", _nLine + 1, _nChar);
    return sz;
}

bool CCrystalScriptStream::const_iterator::operator==(const const_iterator& value) const
{
    return (_nLine == value._nLine) && (_nChar == value._nChar);
}

bool CCrystalScriptStream::const_iterator::operator!=(const const_iterator& value) const
{
    return (_nChar != value._nChar) || (_nLine != value._nLine);
}

void CCrystalScriptStream::const_iterator::Restore(const const_iterator &prev)
{
    // This is a hack for autocomplete. Ideally, we should retrieve line length each time.
    // If on the same line, keep the furthest length, since it may have been modified by AC.
    int furthestLength = (prev._nLine == _nLine) ? _nLength : prev._nLength;
    (*this) = prev;
    _nLength = furthestLength;
}

LineCol CCrystalScriptStream::const_iterator::GetPosition() const
{
    return LineCol(_nLine, _nChar);
}
int CCrystalScriptStream::const_iterator::GetLineNumber() const
{
    return _nLine;
}
int CCrystalScriptStream::const_iterator::GetColumnNumber() const
{
    return _nChar;
}

