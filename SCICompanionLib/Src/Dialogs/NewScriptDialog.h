#pragma once

// fwd decl
class ScriptId;

//
// CNewScriptDialog
//
// This dialog will return the script name (script id).  It will also write the appropriate entry in
// the game.ini
//
class CNewScriptDialog : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CNewScriptDialog)

public:
	CNewScriptDialog(CWnd* pParent = NULL);   // standard constructor
    CNewScriptDialog(UINT nID, CWnd* pParent);

    std::string &GetScriptBuffer() { return _strBuffer; }
    ScriptId &GetScript() { return _scriptId; }
    int GetNumber() { return _scriptId.GetResourceNumber(); }
    void GetFriendlyScriptName(CString &strName) { strName = _szScriptName; }

// Dialog Data
	enum { IDD = IDD_NEWSCRIPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    virtual void _PrepareDialog();
    virtual void _DiscoveredScriptName(PCTSTR pszName);
    virtual void _PrepareBuffer();
    virtual void _AttachControls(CDataExchange *pDX);
    virtual int _GetSuggestedScriptNumber();
    virtual int _GetMinSuggestedScriptNumber() { return 0; }
    BOOL _ValidateScriptNumber();
	DECLARE_MESSAGE_MAP()

    TCHAR _szScriptName[100];
    std::string _strBuffer;
    ScriptId _scriptId;

    CExtEdit m_wndEditScriptName;
    CExtEdit m_wndEditScriptNumber;

    std::set<int> _usedScriptNumbers;
    std::vector<std::string> _usedNames;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndScriptName;
    CExtLabel m_wndScriptNumber;
};
