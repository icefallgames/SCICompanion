#pragma once

#include "CompileContext.h"

// CCompileDialog dialog

class CNewCompileDialog : public CExtResizableDialog
{
public:
	CNewCompileDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewCompileDialog();
    bool GetResult() { return _fResult; }
    bool GetAborted() { return _fAbort; }
    virtual void OnCancel();

// Dialog Data
	enum { IDD = IDD_COMPILEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    LRESULT CompileAll(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();
    virtual void OnDestroy();
	DECLARE_MESSAGE_MAP()

    CExtProgressWnd m_wndProgress;
    CExtEdit m_wndDisplay;
    bool _fResult;
    bool _fAbort;
    bool _fDone;
    int _nScript;
    std::vector<ScriptId> _scripts;
    CompileTables _tables;
    PrecompiledHeaders _headers;
    CompileLog _log;

    // Visuals
    CExtButton m_wndCancel;
};
