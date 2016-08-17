/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#pragma once

#include "BufferPool.h"

// fwd decl
struct PicData;
struct PicComponent;
struct PaletteComponent;
struct Cel;

enum class PicPosition
{
    PrePlugin = 0,
    PostPlugin = 1,
    Final = 2,
};

enum class PicPositionFlags
{
    None = 0,
    PrePlugin = 1,
    PostPlugin = 2,
    Final = 4,
};
DEFINE_ENUM_FLAGS(PicPositionFlags, uint32_t)

PicScreenFlags PicScreenToFlags(PicScreen screen);
PicPositionFlags PicPositionToFlags(PicPosition pos);

class IPicDrawPlugin
{
public:
    virtual void DrawOnPic(ViewPort &viewPort, PicData &picData, PicScreenFlags flags) = 0;
    virtual bool WillDrawOnPic() = 0;
    virtual PicScreenFlags GetRequiredScreens() = 0;
    virtual PicPositionFlags GetRequiredPicPosition() = 0;
};

// Holds onto the results of a pic resource being rendered.
class PicDrawManager
{
public:
    PicDrawManager(const PicComponent *pPic = nullptr, const PaletteComponent *pPalette = nullptr);
    void SetPic(const PicComponent *pPic, const PaletteComponent *pPalette, bool isEGAUndithered);
    const PicComponent *GetPic() const { return _pPicWeak; }

    // Use these to get the pic image:
    HBITMAP CreateBitmap(PicScreen screen, PicPosition position, size16 size, int cx, int cy, SCIBitmapInfo *pbmi = nullptr, uint8_t **pBitsDest = nullptr);
    const uint8_t *GetPicBits(PicScreen screen, PicPosition position, size16 size);
    void CopyBitmap(PicScreen screen, PicPosition position, size16 size, uint8_t *pdataDisplay, uint8_t *pdataAux, BITMAPINFO **ppbmi);
    void GetBitmapInfo(PicScreen screen, BITMAPINFO **ppbmi);
    std::unique_ptr<Cel> MakeCelFromPic(PicScreen screen, PicPosition position);

    void SetPalette(uint8_t bPaletteNumber);
    uint8_t GetPalette() { return _bPaletteNumber; }
    const ViewPort *GetViewPort(PicPosition pos);
    bool SeekToPos(ptrdiff_t iPos); // true if changed
    ptrdiff_t PosFromPoint(int x, int y, ptrdiff_t iStart);
    ptrdiff_t GetPos() const { return _iInsertPos; }
    void SetPreview(bool fPreview);
    void Invalidate();

    void AddPicPlugin(IPicDrawPlugin *plugin);

    // Call this if you know you're going to obtain multiple screens right away
    void RefreshAllScreens(PicScreenFlags picScreenFlags, PicPositionFlags picPositionFlags);
    void InvalidatePlugins();
    bool IsVGA() {
        return _isVGA;
    }
    RGBQUAD *GetVGAPalette() { assert(_isVGA); return _paletteVGA; }

private:
    void _Reset();
    void _Init();
    void _MoveToNextStep(PicPositionFlags requestedFlags, PicPosition posCompleted);
    ptrdiff_t _GetDrawPos() const { return _iDrawPos; }
    void _RedrawBuffers(ViewPort *pState, PicScreenFlags dwRequestedMaps, PicPositionFlags picPositionFlags, bool assertIfCausedRedraw = false);
    HBITMAP _CreateBitmap(uint8_t *pData, size16 sizePic, int cxRequest, int cyRequest, const RGBQUAD *palette, int paletteCount, SCIBitmapInfo *pbmi = nullptr, uint8_t **pBitsDest = nullptr) const;
    HBITMAP _GetBitmapGDIP(uint8_t *pData, int cx, int cy, const RGBQUAD *palette, int paletteCount) const;
    void _OnPosChanged(bool fNotify = true);
    size16 _GetPicSize() const;

    uint8_t *GetScreenData(PicScreen screen, PicPosition pos);
    void SetScreenData(PicScreen screen, PicPosition pos, uint8_t *data);
    void _ReturnOldBufferIfNotUsedAnywhere(PicScreen screen, PicPosition pos);
    void _ReturnOldBufferIfNotUsedAnywhere(PicPosition pos);
    void _EnsureInitialBuffers(PicScreenFlags screenFlags);
    void _ApplyVGAPalette(const PaletteComponent *pPalette);
    RGBQUAD *PicDrawManager::_GetPalette();
    void _EnsureBufferPool(size16 size);

    const PicComponent *_pPicWeak;
    RGBQUAD _paletteVGA[256];

    std::unique_ptr<BufferPool<12>> _bufferPool;
    // These are the screens (PicPosition is the first dimension, PicScreen is the second)
    // These are not necessarily all unique. If we only need the final version, then all 3
    // will be the same.
    uint8_t* _screenBuffers[3][4];
    // Cached view port state for each of the 3 position buffers.
    std::unique_ptr<ViewPort[]> _viewPorts;

    // Are the bitmaps valid? (note, if any of these are valid, then the aux is valid too)
    PicScreenFlags _fValidScreens;
    PicPositionFlags _validPositions;
    bool _fValidPalette;
    uint8_t _bPaletteNumber; // 0-3: the palette number in which to draw.
    bool _fValidState; // Is picstate valid?

    // command to which the buffers have been drawn. (-1 for all the way)
    // It does NOT include _pCommand(_iDrawPos).  So if _iValidPos is 0, then
    // nothing is drawn.
    ptrdiff_t _iDrawPos;
    ptrdiff_t _iInsertPos;
    std::vector<IPicDrawPlugin*> _plugins;

    bool _isVGA;
	bool _isUndithered;
    bool _isContinuousPri;
};