#pragma once


#include "NonViewClient.h"
#include "ColorButton.h"
#include "ChooseColorStatic.h"
#include "ColorButton.h"
#include "DialogFactory.h"
#include "ExtDialogFwdCmd.h"
#include "ResourceMap.h"

// fwd decl
class CResourceDocument;
class CNewRasterResourceDocument;

// RasterSidePane

class RasterSidePane : public CExtDialogFwdCmd, public INonViewClient, public IColorDialogCallback, public IDialogFactory
{
public:
	RasterSidePane(CWnd *pParent = NULL);
	virtual ~RasterSidePane();

    void SetDocument(CDocument *pDoc);

    // INonViewClient
    virtual void UpdateNonView(CObject *pObject) override;

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeft);

    // IDialogFactory
    virtual void InvokeDialog(UINT nID, RECT *prcTarget);

    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    BOOL PreTranslateMessage(MSG* pMsg);

private:
    void DoDataExchange(CDataExchange* pDX);
    void OnMirrorSelection();
    void OnPenWidthSelection();
    void OnPaletteSelection();
    void OnChooseLoop();
    void OnAddCelBefore();
    void OnAddCelAfter();
    void OnDeleteCel();
    void OnAddLoopBefore();
    void OnAddLoopAfter();
    void OnDeleteLoop();
    void OnEditCelWidth();
    void OnEditCelHeight();
    void OnEditCelX();
    void OnEditCelY();
    void OnEditLineHeight();
    void OnEditSampleText();
    void OnUp();
    void OnDown();
    void OnRight();
    void OnLeft();
    void OnMakeFont();
    void OnApplyToAll();

    void _UpdatePaletteChoices();
    void _UpdatePaletteResources();
    void _OnUpdateCommandUIs();
    void OnTreeSelectionChanged(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
    void _SyncCelPane();
    void _SyncLoopPane();
    void _SyncMirror();
    void _SyncSampleText();
    void _OnAddCel(bool before);
    void _OnAddLoop(bool before);
    void _OnEditSize(UINT nID, CWnd *pwnd);
    void _OnPenWidthChanged();
    void _PreparePenWidth();
    void _OnDirectionButton(CPoint point);
    void _SyncLoopTree();
    void _RebuildLoopTree();
    void _SyncPalette();

private:
    CNewRasterResourceDocument *_pDoc;

    // Cels:
    bool _fSupportsSize;
    CExtEdit m_wndCelWidth;
    CExtEdit m_wndCelHeight;
    bool _fSupportsPlacement;
    CExtEdit m_wndCelX;
    CExtEdit m_wndCelY;

    bool _fTransparency;
    CColorPickerButton m_wndTransparent;

    // Loops:
    bool _fSupportsLoops;
    CExtComboBox m_wndMirrorOf;
    CExtLabel m_wndStaticMirrorOf;

    // View-specific
    CExtComboBox m_wndPaletteChoice;

    // Color: (supported by all)
    CChooseColorStatic m_wndPalette;
    CExtLabel m_wndChosenColors;
    CExtComboBox m_wndPenWidth;

    // Font related things
    bool _fSupportsFonts;
    CEdit m_wndLineHeight;
    CEdit m_wndSampleText;

    // Wingdings font for directional buttons
    CFont m_font;

    // "Apply to all" checkbox.
    bool _fSupportsApplyAll;
    CExtCheckBox m_wndApplyToAll;

    CTreeCtrl m_wndTree;

    // This classes needed purely for visuals
    CExtButton m_wndLeft;
    CExtButton m_wndRight;
    CExtButton m_wndUp;
    CExtButton m_wndDown;
    CExtButton m_wndLoopAfter;
    CExtButton m_wndLoopBefore;
    CExtButton m_wndDeleteLoop;
    CExtButton m_wndCelAfter;
    CExtButton m_wndCelBefore;
    CExtButton m_wndDeleteCel;
    CExtButton m_wndMakeFont;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic4;
    CExtLabel m_wndStatic5;
    CExtLabel m_wndStatic6;
    CExtLabel m_wndStatic7;
    CExtGroupBox m_wndGroup1;
    CExtGroupBox m_wndGroup2;

    int _cRows;
    int _cColumns;

    bool _fRebuildingTree;

    HACCEL _hAccel;
};


