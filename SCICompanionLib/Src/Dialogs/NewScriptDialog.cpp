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
    // Now find an "empty" slot, starting from the top down
    for (int i = ARRAYSIZE(_rgUsed) - 1; i >= 0; i--)
    {
        if (0x00 == _rgUsed[i])
        {
            iRet = i;
            break;
        }
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
        std::string iniFileName = appState->GetResourceMap().GetGameIniFileName();
        if (!iniFileName.empty())
        {
            DWORD nLength =  GetPrivateProfileSection(TEXT("Script"), pszNameValues, nSize, iniFileName.c_str());
            if (nLength > 0 && ((nSize - 2) != nLength)) // returns (nSize - 2) in case of insufficient buffer 
            {
                // Keep track of which script numbers have been used.
                ZeroMemory(_rgUsed, sizeof(_rgUsed));

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
                    if ((iScript >= 0) && (iScript < ARRAYSIZE(_rgUsed)))
                    {
                        _rgUsed[iScript] = 0xff;
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

    // Values from 0 to 999
    m_wndEditScriptNumber.SetLimitText(3);
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
    _scriptId.SetResourceNumber(StrToInt(strNumber));
    if (_rgUsed[_scriptId.GetResourceNumber()])
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
    ASSERT(nScript < 1000);

    if (fClose)
    {
        // Prepare the script name.
        // (Without the .sc extension)
        CString strName;
        m_wndEditScriptName.GetWindowText(strName);
        if (!strName.IsEmpty())
        {
            StringCchCopy(_szScriptName, ARRAYSIZE(_szScriptName), (PCTSTR)strName);
            _scriptId.SetFullPath(appState->GetResourceMap().GetScriptFileName(_szScriptName, appState->GetResourceMap().GetGameLanguage()));
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