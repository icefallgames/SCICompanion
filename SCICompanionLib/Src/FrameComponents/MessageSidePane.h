#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"

class CTextDoc;
struct TextComponent;

class MessageSidePane : public CExtDialogFwdCmd, public INonViewClient
{
public:
    MessageSidePane(CWnd* pParent = NULL);   // standard constructor
    virtual ~MessageSidePane();

    // Dialog Data
    enum
    {
        IDD = IDD_MESSAGETOOLBOX,
    };

    void SetDocument(CDocument *pDoc);
    CTextDoc *GetDocument() { return _pDoc; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoTalkers();
    afx_msg void OnGotoVerbs();
       

    DECLARE_MESSAGE_MAP()

    BOOL OnEraseBkgnd(CDC *pDC);

private:
    CTextDoc *_pDoc;

    CExtHyperLinkButton m_wndGotoVerbs;
    CExtHyperLinkButton m_wndGotoTalkers;
    CListCtrl m_wndNouns;
    CListCtrl m_wndConditions;

    // Visual
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;

    HACCEL _hAccel;
};
