#pragma once

#include "Components.h"

#define ANIMATE_TIMER 1234

struct PaletteComponent;
struct RasterComponent;

// CAnimateDialog dialog

class CAnimateDialog : public CExtResizableDialog
{
public:
    CAnimateDialog(CWnd* pParent, const RasterComponent &raster, PaletteComponent *palette);   // standard constructor

    void SetTitle(PCTSTR pszTitle) { _strTitle = pszTitle; }
    void SetLoop(int nLoop) { _nLoop = nLoop; }

    void OnSize(UINT nType, int cx, int cy);
    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    void OnTimer(UINT_PTR nIDEvent);
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);

// Dialog Data
	enum { IDD = IDD_DIALOGANIMATE };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnPlay();


    void _AutoSize();
    void _OnDraw(CDC *pDC, LPRECT prc);
    void _GenerateDoubleBuffer(CDC *pDC, LPRECT prc);
    CSize _RecalcSizeNeeded();
    void _UpdateButton();
    int _GetCelCount();
    const Cel &GetCel();

	DECLARE_MESSAGE_MAP()

    const RasterComponent &_raster;
    PaletteComponent *_palette;
    CString _strTitle;
    int _nLoop;
    CStatic m_wndAnimate;
    CExtSliderWnd m_wndSlider;
    CExtButton m_wndButton;
    CSize _sizeAnimate;     // of the animate ctl. Zoomed
    CSize _sizeWeDrawIn;    // Part of the animate control we draw in (centered based on _SizeAnimate). Zoomed
    //CRect _rectPlacements;  // Not zoomed
    CRect _rectFullBounds;  // Not zoomed
    int _iZoom;
    CPoint _ptOrigin;
    CRect _rectDialogSize;
    bool _fDoubleBuffer;
    std::unique_ptr<CBitmap> _pbitmapDoubleBuf;
    CSize _sizeDoubleBuf;
    int _nCel;
    bool _fAnimating;
    bool _fInitialized;

    // Visuals
    CExtButton m_wndOK;
};
