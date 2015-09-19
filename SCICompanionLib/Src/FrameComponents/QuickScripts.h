#pragma once

#include "NoFlickerStatic.h"
#include "NonViewClient.h"
// QuickScriptsSidePane dialog bar

class ResourceEntity;

class CScriptDocument;

class QuickScriptsSidePane : public CExtResizableDialog, public INonViewClient
{
public:
	QuickScriptsSidePane(CWnd* pParent = nullptr);   // standard constructor
	virtual ~QuickScriptsSidePane();

    void SetDocument(CDocument *pDoc);

    void Refresh() { _UpdateEntries(); }

    // INonViewClient
    virtual void UpdateNonView(CObject *pObject) override;

// Dialog Data
	enum { IDD = IDD_QUICKSCRIPTS };

    void Initialize();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog();
    void OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
    void OnItemEnter(NMHDR* pNMHDR, LRESULT* pResult);
    BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
private:
    afx_msg void OnGotoPic();
    afx_msg void OnGotoMessage();
    afx_msg BOOL OnGotoRoom(UINT nID);
    afx_msg void OnGotoView(UINT nID);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnUpdateAlwaysOn(CCmdUI *pCmdUI) { pCmdUI->Enable(TRUE); }

    void _InitColumns();
    void _UpdateEntries();
    void _AddFilesOfType(int &iItem, PCTSTR pszWildcard);
    void _OpenItem(int iItem);
    void _ResetUI();
    void _PrepareViewCommands(int iIndex, const ResourceEntity &view, int nLoop, int nCel);
    void _PrepareViewMenu(int cItems);

    CListCtrl m_wndList;

    CExtNoFlickerStatic m_wndPic;
    WORD _wPic;
    CScriptDocument *_pDoc;
    CExtHyperLinkButton m_wndGotoPic;
    WORD _wNorth;
    CExtHyperLinkButton m_wndNorth;
    WORD _wEast;
    CExtHyperLinkButton m_wndEast;
    WORD _wSouth;
    CExtHyperLinkButton m_wndSouth;
    WORD _wWest;
    CExtHyperLinkButton m_wndWest;
    CExtHyperLinkButton m_wndGotoMessage;

    CExtButton m_wndButtonViews;
    std::vector<WORD> _viewNumbers;

    CExtLabel m_wndScriptNum;

    bool _needsUpdate;
    int _nFirstScript;
    bool _needInfoFromThisScript;

    std::string _srcFolder;
};
