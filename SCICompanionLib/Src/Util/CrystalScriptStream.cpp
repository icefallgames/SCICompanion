#include "stdafx.h"
#include "CrystalScriptStream.h"
#include "format.h"

ReadOnlyTextBuffer::ReadOnlyTextBuffer(CCrystalTextBuffer *pBuffer)
{
    _lineCount = pBuffer->GetLineCount();
    int totalCharCount = 0;
    for (int i = 0; i < _lineCount; i++)
    {
        int charCount = pBuffer->GetLineLength(i);
        totalCharCount += charCount;
    }

    int start = 0;
    _text = std::make_unique<char[]>(totalCharCount);
    _lineStartsAndLengths = std::make_unique<StartAndLength[]>(_lineCount);
    for (int i = 0; i < _lineCount; i++)
    {
        int charCount = pBuffer->GetLineLength(i);
        memcpy(&_text[start], pBuffer->GetLineChars(i), charCount * sizeof(char));
        _lineStartsAndLengths[i].Start = start;
        _lineStartsAndLengths[i].Length = charCount;
        start += charCount;
    }
}
int ReadOnlyTextBuffer::GetLineLength(int nLine)
{
    return _lineStartsAndLengths[nLine].Length;
}
PCTSTR ReadOnlyTextBuffer::GetLineChars(int nLine)
{
    return &_text[_lineStartsAndLengths[nLine].Start];
}

CScriptStreamLimiter::CScriptStreamLimiter(CCrystalTextBuffer *pBuffer)
{
    _pBuffer = std::make_unique<ReadOnlyTextBuffer>(pBuffer);
    // By default it's not limited:
    _nLineLimit = pBuffer->GetLineCount();
    _nCharLimit = pBuffer->GetLineLength(_nLineLimit - 1);
    _hDoAC = nullptr;
    _hACDone = nullptr;
    _fCancel = false;
    _id = 0;
    _pCallback = nullptr;
}

// CCrystalScriptStream

CCrystalScriptStream::CCrystalScriptStream(CScriptStreamLimiter *pLimiter)
{
    _pLimiter = pLimiter;
}

// Ensure we are on the line with nChar and nLine.
void CScriptStreamLimiter::GetMoreData(int &nChar, int &nLine, int &nLength, PCSTR &pszLine)
{
    while (nChar > nLength)
    {
        // Need to move to the next line
        int cLines = GetLineCount();
        bool fAtLimitedEnd = true;
        while (nLine < cLines)
        {
            nLine++;

            //OutputDebugString(fmt::format("Moved to line {0} of {1}\n", nLine, cLines).c_str());

            if (nLine < cLines)
            {
                fAtLimitedEnd  = false; // Indicate this definitely isn't a limited end.
                nLength = GetLineLength(nLine);
                pszLine = GetLineChars(nLine);
                assert(pszLine != nullptr);
                nChar = 0;
                break; // done
            }
        }
        if (nLine == cLines)
        {
            bool fGenerateEOF = true;
            // We ran out of data.
            if (fAtLimitedEnd)
            {
                if (_hACDone && !_fCancel)
                {
                    fGenerateEOF = false;
                    // We're at a "limited end".  Let's tell the client and block.
                    OutputDebugString("setting hACDone (because at limit)\n");
                    SetEvent(_hACDone); // Tell the client
                    // Wait for future instructions
                    if (WAIT_OBJECT_0 == WaitForSingleObject(_hDoAC, INFINITE))
                    {
                        fGenerateEOF = _fCancel;
                        if (!_fCancel)
                        {
                            nLine--;
                            // 1) Pretend we're back where we were when we entered.
                            //    nLength should be unchanged, nLine should be --, nChar should be unchanged.
                            //    We should refetch nLength (since it may have changed) (don't think we need a new pszLine???)
                            pszLine = GetLineChars(nLine); // REVIEW: is this even necessary?
                            nLength = GetLineLength(nLine);
                            //    And then go back to teh beginning of the top level loop.
                        }
                    }
                    else
                    {
                        assert(false);
                    }
                }
                else if (_pCallback)
                {
                    _pCallback->Done();
                }
            }
            else
            {
                int x = 0;
            }
            if (fGenerateEOF)
            {
                pszLine = "\0"; // EOF
                nLength = 1;
                nChar = 0;
            }
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

CCrystalScriptStream::const_iterator& CCrystalScriptStream::const_iterator::operator++()
{
    assert(*_pszLine != 0); // EOF
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

