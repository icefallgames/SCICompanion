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
// NewRoomDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NewScriptDialog.h"
#include "resource.h"

// CNewScriptDialog dialog

IMPLEMENT_DYNAMIC(CNewScriptDialog, CDialog)

CNewScriptDialog::CNewScriptDialog(UINT nID, CWnd* pParent) : CExtResizableDialog(nID, pParent)
{
}

CNewScriptDialog::CNewScriptDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CNewScriptDialog::IDD, pParent)
{
}

void CNewScriptDialog::_DiscoveredScriptName(PCTSTR pszName)
{
    // Nothing.
}

int CNewScriptDialog::_GetSuggestedScriptNumber()
{
    int iRet = 0;
    // Now find an "empty" slot. 
    // REVIEW: Need a better algorithm here.
    int lastUsed = 0;
    for (int used : _usedScriptNumbers)
    {
        if ((used > (lastUsed + 1)) && (lastUsed > _GetMinSuggestedScriptNumber()))
        {
            return lastUsed + 1;
        }
        lastUsed = used;
    }
    return iRet;
}

void CNewScriptDialog::_PrepareDialog()
{
    int iSuggestedRoom = 0;

    // Look through game.ini for a vacant room number.
    DWORD nSize = 5000;
    TCHAR *pszNameValues = new TCHAR[nSize];
    if (pszNameValues)
    {
        std::string iniFileName = appState->GetResourceMap().Helper().GetGameIniFileName();
        if (!iniFileName.empty())
        {
            DWORD nLength =  GetPrivateProfileSection(TEXT("Script"), pszNameValues, nSize, iniFileName.c_str());
            if (nLength > 0 && ((nSize - 2) != nLength)) // returns (nSize - 2) in case of insufficient buffer 
            {
                // Keep track of which script numbers have been used.
                _usedScriptNumbers.clear();

                TCHAR *psz = pszNameValues;
                while(*psz)
                {
                    // The format is
                    // n000=ScriptName
                    size_t cch = strlen(psz);
                    TCHAR *pszEq = StrChr(psz, TEXT('='));
                    if (pszEq)
                    {
                        _DiscoveredScriptName(pszEq + 1);
                    }

                    // Take note of the script number.
                    int iScript = StrToInt(psz + 1);
                    if (iScript >= 0)
                    {
                        // We can end up with turd entries lying around in game.ini, so check that the file actually exists:
                        std::string filename = appState->GetResourceMap().Helper().GetScriptFileName((uint16_t)iScript);
                        if (PathFileExists(filename.c_str()))
                        {
                            _usedScriptNumbers.insert(iScript);
                        }
                    }

                    // Advance to next string.
                    psz += (cch + 1);
                }

                iSuggestedRoom = _GetSuggestedScriptNumber();
            }
        }
        delete [] pszNameValues;
    }

    // Suggest a room:
    TCHAR szNumber[5];
    StringCchPrintf(szNumber, ARRAYSIZE(szNumber), TEXT("%d"), iSuggestedRoom);
    m_wndEditScriptNumber.SetWindowText(szNumber);

}
void CNewScriptDialog::_AttachControls(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_SCRIPTNAME, m_wndEditScriptName);
    DDX_Control(pDX, IDC_EDITROOMNUMBER, m_wndEditScriptNumber);

    DDX_Control(pDX, IDC_STATICSCRIPTNUMBER, m_wndScriptName);
    DDX_Control(pDX, IDC_STATICSCRIPTNAME, m_wndScriptNumber);

    m_wndEditScriptNumber.SetLimitText(5);
}

void CNewScriptDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);

    _AttachControls(pDX);
    _PrepareDialog();
}

void CNewScriptDialog::_PrepareBuffer()
{
    _strBuffer += 
    TEXT("/******************************************************************************/\r\n");

    if (appState->GetVersion().SeparateHeapResources)
    {
        // e.g. for SCI0, keep SCIStudio compatible. Otherwise, use version 2
        _strBuffer += "(version 2)\r\n";
    }

    // The script number
    TCHAR szTemp[MAX_PATH];
    StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("(script %d)\r\n"), _scriptId.GetResourceNumber());
    _strBuffer += szTemp;
    _strBuffer += TEXT("/******************************************************************************/\r\n");
}


BEGIN_MESSAGE_MAP(CNewScriptDialog, CDialog)
END_MESSAGE_MAP()

//
// Returns TRUE if the chosen script number is valid
//
BOOL CNewScriptDialog::_ValidateScriptNumber()
{
    BOOL fValid = FALSE;
    // Validate the script number
    CString strNumber;
    m_wndEditScriptNumber.GetWindowText(strNumber);
    _scriptId = ScriptId(); // Reset, or else it asserts if we already set a number on it.
    _scriptId.SetResourceNumber(StrToInt(strNumber));
    int value = _scriptId.GetResourceNumber();
    if (contains(_usedScriptNumbers, value))
    {
        TCHAR szMessage[MAX_PATH];
        StringCchPrintf(szMessage, ARRAYSIZE(szMessage), TEXT("Script %03d already exists.  Please use another number."), _scriptId.GetResourceNumber());
        AfxMessageBox(szMessage, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
        fValid = TRUE;
    }
    return fValid;
}

// CNewScriptDialog message handlers

void CNewScriptDialog::OnOK()
{
    BOOL fClose = !_ValidateScriptNumber();

    WORD nScript = _scriptId.GetResourceNumber();

    if (fClose)
    {
        // Prepare the script name.
        // (Without the .sc extension)
        CString strName;
        m_wndEditScriptName.GetWindowText(strName);
        if (!strName.IsEmpty())
        {
            StringCchCopy(_szScriptName, ARRAYSIZE(_szScriptName), (PCTSTR)strName);
            _scriptId.SetFullPath(appState->GetResourceMap().Helper().GetScriptFileName(_szScriptName, appState->GetResourceMap().Helper().GetDefaultGameLanguage()));
        }
        else
        {
            fClose = FALSE;
            AfxMessageBox(TEXT("Please enter a script name."), MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        }
    }

    if (fClose)
    {
        _PrepareBuffer();
        __super::OnOK();
    }
}