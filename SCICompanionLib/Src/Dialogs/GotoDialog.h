#pragma once
#include "afxwin.h"


// CGotoDialog dialog

class CGotoDialog : public CExtResizableDialog
{
public:
	CGotoDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGotoDialog();
    int GetLineNumber() { return _iLineNumber; }

// Dialog Data
	enum { IDD = IDD_DIALOG_GOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CExtEdit m_LineNumber;
    int _iLineNumber;
    std::string Title;
    std::string Label;

    // Visuals
    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
};
