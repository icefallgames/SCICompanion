#pragma once

// ViewUIElement, A control that shows a view

struct PaletteComponent;
struct RasterComponent;

class ViewUIElement : public CStatic
{
    DECLARE_DYNAMIC(ViewUIElement)

public:
    ViewUIElement();
    virtual ~ViewUIElement();

    void SetResource(const ResourceEntity *view, const PaletteComponent *optionalPalette);

    void SetLoop(int nLoop);
    void SetCel(int nCel);
    void SetBackground(COLORREF color);
    void SetPatterned(bool isPatterned);
    CSize GetSizeNeeded() { return _sizeWeDrawIn; }

private:
    afx_msg int OnCreate(CREATESTRUCT *createStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

    void _OnDraw(CDC *pDC, LPRECT prc);
    void _GenerateDoubleBuffer(CDC *pDC, LPRECT prc);
    CSize _RecalcSizeNeeded();
    const Cel &GetCel();

    DECLARE_MESSAGE_MAP()

    const ResourceEntity *_viewResource;
    const PaletteComponent *_optionalPalette;

    int _nLoop;
    CSize _sizeAnimate;     // size of us
    CSize _sizeWeDrawIn;    // The area within us in which we draw
    CRect _rectFullBounds;  // Not zoomed
    int _iZoom;
    CPoint _ptOrigin;
    bool _fDoubleBuffer;
    std::unique_ptr<CBitmap> _pbitmapDoubleBuf;
    CSize _sizeDoubleBuf;
    int _nCel;
    bool _fInitialized;

    bool _isBackgroundPatterned;
    COLORREF _bgColor;
};
