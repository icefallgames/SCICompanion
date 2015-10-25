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

#include "WordEnumString.h"
#include "Vocab000.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceEntity.h"

using namespace std;

class CWordEnumString : public IEnumString
{
public:
    CWordEnumString();
    ~CWordEnumString();

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // IEnumString
    HRESULT STDMETHODCALLTYPE Next(ULONG celt, LPOLESTR *rgelt, ULONG *pceltFetched);
    HRESULT STDMETHODCALLTYPE Skip(ULONG celt);
    HRESULT STDMETHODCALLTYPE Reset();
    HRESULT STDMETHODCALLTYPE Clone(IEnumString **ppenum);

    // CWordEnumString
    HRESULT Init();

private:
    LONG _cRef;

    Vocab000 _vocab;

    size_t _vocabIndex;
};


CWordEnumString::CWordEnumString()
{
    _cRef = 1;
    _vocabIndex = 0;
}

CWordEnumString::~CWordEnumString()
{
}

ULONG CWordEnumString::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CWordEnumString::Release()
{
    LONG cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT CWordEnumString::QueryInterface(REFIID riid, void **ppv)
{
    *ppv = NULL;
    HRESULT hr = E_NOINTERFACE;
    if (IsEqualIID(IID_IUnknown, riid))
    {
        *ppv = (IUnknown*)this;
        hr = S_OK;
    }
    else if (IsEqualIID(IID_IEnumString, riid))
    {
        *ppv = (IEnumString*)this;
        hr = S_OK;
    }

    if (SUCCEEDED(hr))
    {
        AddRef();
    }

    return hr;
}

HRESULT CWordEnumString::Next(ULONG celt, LPOLESTR *rgelt, ULONG *pceltFetched)
{
    HRESULT hr = S_FALSE;
    *pceltFetched = 0;
    vector<string> &words = _vocab.GetWords();
    while ((celt > 0) && SUCCEEDED(hr) && (_vocabIndex < words.size()))
    {
        string &strWord = words[_vocabIndex];
        int cch = (int)strWord.length() + 1;
        rgelt[*pceltFetched] = (LPOLESTR)CoTaskMemAlloc(cch * sizeof(WCHAR));
        hr = rgelt[*pceltFetched] ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hr))
        {
            MultiByteToWideChar(CP_ACP, 0, strWord.c_str(), -1, rgelt[*pceltFetched], cch);
            (*pceltFetched)++;
            celt--;
        }
        _vocabIndex++;
    }
    return hr;
}

HRESULT CWordEnumString::Skip(ULONG celt)
{
    _vocabIndex += celt;
    return S_OK;
}

HRESULT CWordEnumString::Reset()
{
    _vocabIndex = 0;
    return S_OK;
}

HRESULT CWordEnumString::Clone(IEnumString **ppenum)
{
    return CWordEnumString_CreateInstance(IID_IEnumString, (void**)ppenum);
}

HRESULT CWordEnumString::Init()
{
    _vocab = *appState->GetResourceMap().GetVocab000();
    return S_OK;
}

HRESULT CWordEnumString_CreateInstance(REFIID riid, void **ppv)
{
    *ppv = NULL;
    HRESULT hr = E_OUTOFMEMORY;
    CWordEnumString *pEnumString = new CWordEnumString();
    if (pEnumString)
    {
        hr = pEnumString->Init();
        if (SUCCEEDED(hr))
        {
            hr = pEnumString->QueryInterface(riid, ppv);
        }
        pEnumString->Release();
    }
    return hr;

}