// RemoveScriptDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "RemoveScriptDialog.h"


// CRemoveScriptDialog dialog

CRemoveScriptDialog::CRemoveScriptDialog(WORD wScript, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CRemoveScriptDialog::IDD, pParent), _wScript(wScript), _fAlsoDelete(true)
{
}

CRemoveScriptDialog::~CRemoveScriptDialog()
{
}

void CRemoveScriptDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_CHECKDELETE, m_wndCheckDelete);
    DDX_Control(pDX, IDC_STATICQUESTION, m_wndStaticQuestion);

    // Visuals
    DDX_Control(pDX, IDOK, m_wndYes);
    DDX_Control(pDX, IDCANCEL, m_wndNo);
}

BOOL CRemoveScriptDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    
    // Ask the question
    CResourceMap &rm = appState->GetResourceMap();
    std::string scriptTitle = rm.Helper().GetIniString("Script", default_reskey(_wScript));
    ScriptId script = rm.Helper().GetScriptId(scriptTitle);
    std::stringstream ss;
    ss << "Do you want to remove " << script.GetTitle() << " from the game completely?";
    m_wndStaticQuestion.SetWindowText(ss.str().c_str());

    // And if the file exists, ask if it should be deleted.
    if (PathFileExists(script.GetFullPath().c_str()))
    {
        std::stringstream ss;
        ss << "Also delete script file: " << script.GetFullPath();
        m_wndCheckDelete.SetWindowText(ss.str().c_str());
    }
    else
    {
        m_wndCheckDelete.ShowWindow(SW_HIDE);
    }

    ShowSizeGrip(FALSE);
    return fRet;
}

BEGIN_MESSAGE_MAP(CRemoveScriptDialog, CExtResizableDialog)
    ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()


// CRemoveScriptDialog message handlers
void CRemoveScriptDialog::OnOK()
{
    _fAlsoDelete = (m_wndCheckDelete.GetCheck() != 0);
    __super::OnOK();
}