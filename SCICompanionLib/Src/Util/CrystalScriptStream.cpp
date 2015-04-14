#include "stdafx.h"
#include "CrystalScriptStream.h"

// CCrystalScriptStream

CCrystalScriptStream::CCrystalScriptStream(CScriptStreamLimiter *pBuffer)
{
    _pBuffer = pBuffer;
}


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
            if (nLine < cLines)
            {
                fAtLimitedEnd  = false; // Indicate this definitely isn't a limited end.
                nLength = GetLineLength(nLine);
                pszLine = GetLineChars(nLine);
                ASSERT(pszLine != NULL);
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
                        ASSERT(FALSE);
                    }
                }
                else if (_pCallback)
                {
                    _pCallback->Done();
                }
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

CCrystalScriptStream::const_iterator::const_iterator(CScriptStreamLimiter *pBuffer, LineCol dwPos) : _pBuffer(pBuffer), _pidStream(pBuffer->GetIdPtr())
{
    _nLine = dwPos.Line();
    _nChar = dwPos.Column();
    if (_nLine < _pBuffer->GetLineCount())
    {
        _nLength = _pBuffer->GetLineLength(_nLine);
        _pszLine = _pBuffer->GetLineChars(_nLine);
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
        _pszLine = _pBuffer->GetLineChars(_nLine);
        _id = *_pidStream;
        return (_nChar == _nLength) ? '\n' : _pszLine[_nChar];
    }
}

CCrystalScriptStream::const_iterator& CCrystalScriptStream::const_iterator::operator++()
{
    ASSERT(*_pszLine != 0); // EOF
    _nChar++;
    if ((_nChar > _nLength) ||
        ((_nChar == _nLength) && (_nLine == (_pBuffer->GetLineCount() - 1)))) // for == we use '\n', unless this is the last line
    {
        _pBuffer->GetMoreData(_nChar, _nLine, _nLength, _pszLine);
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

