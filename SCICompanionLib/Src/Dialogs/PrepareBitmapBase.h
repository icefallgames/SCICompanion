#pragma once

struct PaletteComponent;

class PrepareBitmapBase
{
public:
    PrepareBitmapBase(int convertButtonId, int staticOriginalImageId);
    virtual ~PrepareBitmapBase();

protected:
    void _UpdateOrigBitmap(CWnd *pwnd);
    void _PrepareBitmapForConversion();
    BOOL _ReallocateCRBitmap(CSize size);
    BOOL _Init(std::unique_ptr<Gdiplus::Bitmap> pImage, CWnd *pwnd);
    void _ApplySettingsToCurrentBitmap();
    void _CalculateContrastCenter(Gdiplus::Bitmap *pBitmap, BYTE *pbContrastCenter);
    void _OnPasteFromClipboard(CWnd *pwnd);
    void _OnBrowse(CWnd *pwnd);

    int _convertButtonId;
    int _staticOriginalImageId;

    int _iScaleImage;

    int _nBrightness;
    int _nContrast;
    int _nSaturation;

    // Straight through
    COLORREF *_pCRBitmap;
    CSize _size;

    Gdiplus::Bitmap *_pbmpNormal;
    int _nContrastCenter;
    Gdiplus::Bitmap *_pbmpScaled;
    Gdiplus::Bitmap *_pbmpCurrent;
    std::unique_ptr<Gdiplus::Bitmap> _pbmpOrig;
    std::unique_ptr<PaletteComponent> _originalPalette;
    int _numberOfPaletteEntries;
    uint8_t _bContrastCenterNormal;
    uint8_t _bContrastCenterScaled;

    bool _disableAllEffects;
};