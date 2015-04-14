// DontShowAgainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "DontShowAgainDialog.h"


// CDontShowAgainDialog dialog

CDontShowAgainDialog::CDontShowAgainDialog(PCTSTR pszMessage, int &iValue, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CDontShowAgainDialog::IDD, pParent), _iValue(iValue)
{
    _pszMessage = pszMessage;
}

CDontShowAgainDialog::~CDontShowAgainDialog()
{
}

void CDontShowAgainDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_DONTSHOWAGAIN, _iValue);

    DDX_Control(pDX, IDC_MESSAGE, m_wndStaticMessage);
    m_wndStaticMessage.SetWindowText(_pszMessage);

    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDC_DONTSHOWAGAIN, m_wndDontShow);
}


BEGIN_MESSAGE_MAP(CDontShowAgainDialog, CExtResizableDialog)
END_MESSAGE_MAP()


// CDontShowAgainDialog message handlers
