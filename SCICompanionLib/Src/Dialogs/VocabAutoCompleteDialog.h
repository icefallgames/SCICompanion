#pragma once


// CVocabAutoCompleteDialog dialog

class CVocabAutoCompleteDialog : public CExtResizableDialog
{
public:
	CVocabAutoCompleteDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVocabAutoCompleteDialog();

    void SetWord(PCTSTR pszWord) { _strWord = pszWord; }
    CString &GetSynonym() { return _strSynonym; }
    CString &GetWord() { return _strWord; }

// Dialog Data
	enum { IDD = IDD_VOCABAUTOCOMPLETE };

protected:
    virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    CExtEdit m_wndEdit;
    CExtEdit m_wndEditWord;
    HRESULT _hrCoCreate;

    CString _strSynonym;
    CString _strWord;

    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;

    bool _fInited;
};
