#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "ExtTabControl.h"

class CPicDoc;
class SCIPolygon;
struct PicComponent;

class PicCommandListBox : public CListBox
{
protected:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

class PolygonListBox : public CListBox
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
        IDD_VGA = IDD_PICCOMMANDS_VGA1,
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
    afx_msg void OnPolySelChange();
    afx_msg void OnGotoScript();
    afx_msg void OnClickPolygons();
    afx_msg void OnClickCommands();
    afx_msg void OnBnClickedShowPolys();
    afx_msg void OnCbnSelchangeComboPolyType();
    afx_msg void PushEditPointsToPoly();
    afx_msg void PushNameToPoly();
    afx_msg void OnTcnSelchangeTabWhichList(NMHDR *pNMHDR, LRESULT *pResult);
    BOOL OnEraseBkgnd(CDC *pDC);
    void OnCropCommands();
    void OnCopyCommands();
    void OnCutCommands();
    void _OnUpdateCommands();

private:
    void _ShowPolyOrCommands();
    void _OnDelete(bool fCut, bool fCopy);
    void _UpdateItemCount();
    void _UpdatePolyItemCount();
    const PicComponent *_GetEditPic();
    void _InvalidateCurrentCaretPos();
    void _UpdatePalette();
    void _SyncPolyTypeCombo();
    void _SyncPolyChoice();
    const SCIPolygon *_GetCurrentPolygon();
    bool _ShowingPolygons();
    
    CPicDoc *_pDoc;
    int _iUserSelectedPos;
    int _iCurrentPicPos;        // Current position in CommandView (equal to pic position)
    bool _fAttached;

    CFont m_font;

    PicCommandListBox m_wndListCommands;
    CExtNoFlickerStatic m_wndPalette;
    bool _showPalette;

    PolygonListBox m_wndListPolygons;
    CExtLabel m_wndStaticPolyType;
    CExtLabel m_wndStaticPolyName;
    CExtComboBox m_wndComboPolyType;
    CExtCheckBox m_wndCheckShowPolys;
    CExtEdit m_wndEditPolyPoints;
    CExtEdit m_wndEditPolyName;

    CExtHyperLinkButton m_wndGotoScript;

    ExtTabControl m_wndTabWhichList;

    // Visual
    CExtButton m_wndUploadPointsButton;
    CExtButton m_wndUploadNameButton;
    CExtButton m_wndEditPaletteButton;
    CExtButton m_wndDelete;
    CExtButton m_wndCopy;
    CExtButton m_wndCrop;
    CExtButton m_wndSetBackground;
    HACCEL _hAccel;
};
