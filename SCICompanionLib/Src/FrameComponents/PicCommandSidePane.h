#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"

class CPicDoc;
struct PicComponent;

class PicCommandListBox : public CListBox
{
protected:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

// PicCommandSidePane dialog

class PicCommandSidePane : public CExtDialogFwdCmd, public INonViewClient
{
public:
    PicCommandSidePane(bool showPalette, CWnd* pParent = NULL);   // standard constructor
	virtual ~PicCommandSidePane();

// Dialog Data
	enum
    {
        IDD = IDD_PICCOMMANDS,
        IDD_VGA = IDD_PICCOMMANDS_VGA,
    };

    void OnDeleteCommands();

    void SetDocument(CDocument *pDoc);
    CPicDoc *GetDocument() { return _pDoc; }
    int GetCurrentPosition() { return _iCurrentPicPos; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    LRESULT OnOcmDrawItem(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSelChange();
    afx_msg void OnGotoScript();
    BOOL OnEraseBkgnd(CDC *pDC);
    void OnCropCommands();
    void OnCopyCommands();
    void OnCutCommands();
    void _OnUpdateCommands();

private:
    void _OnDelete(bool fCut, bool fCopy);
    void _UpdateItemCount();
    const PicComponent *_GetEditPic();
    void _InvalidateCurrentCaretPos();
    void _UpdatePalette();
    
    CPicDoc *_pDoc;
    int _iUserSelectedPos;
    int _iCurrentPicPos;        // Current position in CommandView (equal to pic position)
    bool _fAttached;

    CFont m_font;

    PicCommandListBox m_wndList;
    CExtNoFlickerStatic m_wndPalette;
    bool _showPalette;

    CExtHyperLinkButton m_wndGotoScript;

    // Visual
    CExtButton m_wndEditPaletteButton;
    CExtButton m_wndDelete;
    CExtButton m_wndCopy;
    CExtButton m_wndCrop;
    HACCEL _hAccel;
};
