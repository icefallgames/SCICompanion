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
// VocabAutoCompleteDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "VocabAutoCompleteDialog.h"
#include "WordEnumString.h"
#include "Vocab000.h"

// CVocabAutoCompleteDialog dialog

CVocabAutoCompleteDialog::CVocabAutoCompleteDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CVocabAutoCompleteDialog::IDD, pParent)
{
    _hrCoCreate = CoInitialize(NULL); 
    _fInited = false;
}

CVocabAutoCompleteDialog::~CVocabAutoCompleteDialog()
{
    if (SUCCEEDED(_hrCoCreate))
    {
        CoUninitialize();
    }
}

BOOL CVocabAutoCompleteDialog::OnInitDialog()
{
    __super::OnInitDialog();

    if (!_strWord.IsEmpty())
    {
        // Set focus to the synonym box, if a word was provided.
        m_wndEdit.SetFocus();
        return FALSE;
    }

    // Otherwise, the word box is the first one, so focus will be there.
    return TRUE;
}


void CVocabAutoCompleteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    if (!_fInited)
    {
        _fInited = true;

        ShowSizeGrip(FALSE);
        DDX_Control(pDX, IDC_WORD, m_wndEditWord);
        m_wndEditWord.SetWindowText(_strWord);

        DDX_Control(pDX, IDC_EDITAC, m_wndEdit);
        HWND hwndEdit = m_wndEdit.GetSafeHwnd();

        if (SUCCEEDED(_hrCoCreate))
        {
            IAutoComplete *pac;
            if (SUCCEEDED(CoCreateInstance(CLSID_AutoComplete, NULL, CLSCTX_INPROC_SERVER, IID_IAutoComplete, (void**)&pac)))
            {
                IUnknown *pSource;
                if (SUCCEEDED(CWordEnumString_CreateInstance(IID_IUnknown, (void**)&pSource)))
                {
                    if (SUCCEEDED(pac->Init(hwndEdit, pSource, NULL, NULL)))
                    {
                        IAutoComplete2 *pAC2;
                        if (SUCCEEDED(pac->QueryInterface(IID_IAutoComplete2, (void**)&pAC2)))
                        {
                            if (appState->_fUseAutoSuggest)
                            {
                                pAC2->SetOptions(ACO_AUTOSUGGEST);
                            }
                            pAC2->Release();
                        }
                    }
                    pSource->Release();
                }
                pac->Release();
            }
        }

        // Visuals
        DDX_Control(pDX, IDOK, m_wndOk);
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
        DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
    }
}

void CVocabAutoCompleteDialog::OnOK()
{
    m_wndEditWord.GetWindowText(_strWord);
    m_wndEdit.GetWindowText(_strSynonym);

    if (IsValidVocabString(_strWord, TRUE))
    {
        __super::OnOK();
    }
}

BEGIN_MESSAGE_MAP(CVocabAutoCompleteDialog, CDialog)
END_MESSAGE_MAP()


// CVocabAutoCompleteDialog message handlers
