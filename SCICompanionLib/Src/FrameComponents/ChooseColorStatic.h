#pragma once


// CChooseColorStatic

#define INVALID_COLORINDEX (-1)

#include "ColorDialogCallback.h"

//class CChooseColorStatic : public CExtLabel
class CChooseColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CChooseColorStatic)

public:
    CChooseColorStatic();
	virtual ~CChooseColorStatic();
    void SetCallback(IColorDialogCallback *pCallback) { _pCallback = pCallback; }
    // EGACOLOR is used to tell the client which index was picked. Its size should be cRows * cColumns.
    // RGBQUAD is so we can draw. Its size is paletteColorCount.
    void SetPalette(int cRows, int cColumns, const EGACOLOR *pPalette, int paletteColorCount, const RGBQUAD *pPaletteColors, bool dithered = true);
    void SetPrintIndex(BOOL fPrintIndex) { _fPrintIndex = fPrintIndex; }
    void ShowSelection(BOOL fShowSelection) { _fShowSelection = fShowSelection; }
    void ShowSelectionNumbers(BOOL fShow) { _fSelectionNumbers = fShow; }
    void ShowFocusBoxes(bool fShow) { _showSelectionBoxes = fShow; }
    void SetSelection(BYTE bIndex);
    void SetSelection(bool *multipleSelection);
    void GetMultipleSelection(bool *multipleSelection);
    void SetActualUsedColors(bool *actualUsedColors);
    void SetShowHover(bool showHover) { _showHover = showHover; }
    void SetAuxSelection(BYTE bIndex);
    void SetAutoHandleSelection(bool autoHandle) { _autoHandleSelection = autoHandle; }
    void ShowIndices(BOOL fShowIndices) { _fShowIndices = fShowIndices; }
    void OnPaletteUpdated();
    void ShowUnused(bool showUnused) { _showUnused = showUnused; }

protected:
	DECLARE_MESSAGE_MAP()

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void _OnButtonDown(UINT nFlags, CPoint point, BOOL fLeft);
    void OnMouseMove(UINT nFlags, CPoint point);

    int _MapPointToIndex(CPoint point);
    void _MapRowColToRect(int iRow, int iColumn, RECT *prcOut);
    void _MapIndexToRect(BYTE bIndex, RECT *prc);
    virtual void _DrawItem(CDC *pDC, int cx, int cy); // for double-buffering
    virtual void _DrawIndex(CDC *pDC, BYTE bIndex);
    virtual void _DrawTextAtIndex(CDC *pDC, BYTE bIndex, PCTSTR psz);
    virtual void _DrawHover(CDC *pDC);
    virtual void _DrawSelection(CDC *pDC);
    void _DrawMultiSelection(CDC *pDC);
    void _DrawActualUsedColors(CDC *pDC);
    void _Draw0x3Colors(CDC *pDC);
    void _DrawUnused(CDC *pDC);

protected:
    uint16_t _bHoverIndex;
    int _cRows;
    int _cColumns;
    BOOL _fPrintIndex;
    BOOL _fShowSelection;
    BOOL _fShowIndices;
    BYTE _bSelectedColorIndex;
    BYTE _bAuxSelectedColorIndex;
    BOOL _fSelectionNumbers;
    bool _showSelectionBoxes;
    BOOL _fShowAuxSel;

    bool _showHover;
    bool _showUnused;
    int _focusIndex;
    bool _autoHandleSelection;
    bool _allowMultipleSelection;
    bool _multipleSelection[256];

    bool _showActualUsedColors;
    bool _actualUsedColors[256];

    EGACOLOR _pPalette[256];
    RGBQUAD _pPaletteColors[256];

    bool _dithered;
    IColorDialogCallback *_pCallback;
    int _paletteSquareInset;
};

//
// Overrides drawing functionality, to draw brushes instead.
// Also ignores the SetPalette method.
// This should really be re-factored more.
//
class CChooseBrushStatic : public CChooseColorStatic
{
	DECLARE_DYNAMIC(CChooseBrushStatic)

public:
    CChooseBrushStatic();
	virtual ~CChooseBrushStatic();

protected:
	DECLARE_MESSAGE_MAP()

    virtual void _DrawItem(CDC *pDC, int cx, int cy);

private:
};

RGBQUAD RGBQUADFromCOLORREF(COLORREF color);

// Returns inclusive start/end pairs
std::vector<std::pair<uint8_t, uint8_t>> GetSelectedRanges(CChooseColorStatic &wndStatic);
std::string GetRangeText(const std::vector<std::pair<uint8_t, uint8_t>> &ranges);