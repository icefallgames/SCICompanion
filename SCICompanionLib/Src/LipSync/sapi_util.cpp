/* 
sapi_util.cpp

Copyright (C) 2005 Annosoft, LLC. Richardson, Texas. 
All rights reserved.  
	
Permission is hereby granted, free of charge, to use and distribute
this software and its documentation without restriction, including   
without limitation the rights to use, copy, modify, merge, publish,  
distribute, sublicense, and/or sell copies of this work, and to      
permit persons to whom this work is furnished to do so, subject to   
the following conditions:                                            
1. The code must retain the above copyright notice, this list of    
	conditions and the following disclaimer.                        
2. Any modifications must be clearly marked as such.                
3. Original authors' names are not deleted.                         
4. The name "Annosoft" and the authors' names can be not used to endorse or 
   promote products derived from this software without specific prior written       
   permission.                                            

ANNOSOFT AND THE CONTRIBUTORS TO THIS WORK DISCLAIM ALL WARRANTIES 
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS, IN NO EVENT ANNOSOFT NOR THE CONTRIBUTORS 
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

*/

/**
@file sapi_util.cpp
@brief Implementation for various utility functions
*/

#include "stdafx.h"
#include <sphelper.h>
#include <comdef.h>	// for nice bstr_t thing!
#include "sapi_util.h"

///////////////////////////////////////////////////////////////////////////////
std::wstring TCHAR_2_wstring(const TCHAR *str)
{
#if _UNICODE
	return std::wstring(str);
#else
	bstr_t bst = str;
	std::wstring ret = bst;
	return (ret);
#endif
}

///////////////////////////////////////////////////////////////////////////////
std::string  wstring_2_string(const std::wstring& str)
{
	bstr_t bst = str.c_str();
	std::string ret = bst;
	return (ret);
}



///////////////////////////////////////////////////////////////////////////////
/** 
@ingroup utility_functions

Load the content of a text file into a buffer.  The
buffer is converted into Unicode using the default
engine codepage if it isn't already in Unicode

This code was borrowed from SAPI 5.1 sample CRecogDlgClass::GetTextFile
@param pszFileName - [in] source file name
@param ppwszCoMem - [out] the loaded text buffer
@param pcch - [out] size of text buffer
@return HRESULT hr 
**/
HRESULT GetTextFile(
    const TCHAR* pszFileName,
    WCHAR ** ppwszCoMem,
    ULONG * pcch)
{

    HRESULT hr = S_OK;
    HANDLE hf = INVALID_HANDLE_VALUE;
    DWORD cBytes;
    BOOL fUnicodeFile = FALSE;
    USHORT uTemp;
    WCHAR * pwszCoMem = 0;
    ULONG cch = 0;
    DWORD dwRead;

       

    if (SUCCEEDED(hr))
    {
        hf = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        hr = (hf != INVALID_HANDLE_VALUE) ? S_OK : HRESULT_FROM_WIN32(CommDlgExtendedError());
    }

    if (SUCCEEDED(hr))
    {
        cBytes = GetFileSize(hf, NULL); // 64K limit

        hr = (cBytes != -1) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }

    if (SUCCEEDED(hr))
    {
        hr = ReadFile(hf, &uTemp, 2, &dwRead, NULL) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }

    if (SUCCEEDED(hr))
    {
        fUnicodeFile = uTemp == 0xfeff;

        if (fUnicodeFile)
        {
            cBytes -= 2;

            pwszCoMem = (WCHAR *)CoTaskMemAlloc(cBytes);

            if (pwszCoMem)
            {
                hr = ReadFile(hf, pwszCoMem, cBytes, &dwRead, NULL) ? S_OK : HRESULT_FROM_WIN32(GetLastError());

                cch = cBytes / sizeof(WCHAR);
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }
        else
        {
            SPRECOGNIZERSTATUS stat;
            ZeroMemory(&stat, sizeof(stat));
            hr = S_OK;
            if (SUCCEEDED(hr))
            {
                UINT uiCodePage = SpCodePageFromLcid(MAKELCID(LANG_ENGLISH, SORT_DEFAULT));

                char * pszBuffer = (char *)malloc(cBytes);

                hr = pszBuffer ? S_OK : E_OUTOFMEMORY;

                if (SUCCEEDED(hr))
                {
                    SetFilePointer(hf, 0, NULL, FILE_BEGIN); // rewind
        
                    hr = ReadFile(hf, pszBuffer, cBytes, &dwRead, NULL) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
                }

                if (SUCCEEDED(hr))
                {
                    cch = MultiByteToWideChar(uiCodePage, 0, pszBuffer, cBytes, NULL, NULL);

                    if (cch)
                    {
                        pwszCoMem = (WCHAR *)CoTaskMemAlloc(sizeof(WCHAR) * cch);
                    }
                    else
                    {
                        hr = E_FAIL;
                    }
                }

                if (SUCCEEDED(hr))
                {
                    MultiByteToWideChar(uiCodePage, 0, pszBuffer, cBytes, pwszCoMem, cch);
                }

                if (pszBuffer)
                {
                    free(pszBuffer);
                }
            }
        }
    }

    if (INVALID_HANDLE_VALUE != hf)
    {
        CloseHandle(hf);
    }

    *ppwszCoMem = pwszCoMem;
    *pcch = cch;
    
    return hr;
}