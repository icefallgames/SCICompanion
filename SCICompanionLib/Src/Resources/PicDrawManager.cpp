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
#include "stdafx.h"
#include "AppState.h"
#include "PicDrawManager.h"
#include "Pic.h"
#include "PicOperations.h"
#include "PaletteOperations.h"
#include "format.h"
#include "PicCommands.h"

using namespace Gdiplus;

PicScreenFlags PicScreenToFlags(PicScreen screen)
{
    return (PicScreenFlags)(0x1 << (int)screen);
}

PicPositionFlags PicPositionToFlags(PicPosition pos)
{
    return (PicPositionFlags)(0x1 << (int)pos);
}

PicDrawManager::PicDrawManager(const PicComponent *pPic, const PaletteComponent *pPalette)
    : _pPicWeak(pPic),
    _paletteVGA{},
    _isVGA(pPalette != nullptr),
    _isContinuousPri(pPic && pPic->Traits->ContinuousPriority),
    _screenBuffers{}
{
    _viewPorts = std::make_unique<ViewPort[]>(3);
    _Reset();
    _paletteVGA[255].rgbRed = 255;
    _paletteVGA[255].rgbGreen = 255;
    _paletteVGA[255].rgbBlue = 255;
    _paletteVGA[255].rgbReserved = 0x1;

    if (pPalette)
    {
        _ApplyVGAPalette(pPalette);
    }
}

void PicDrawManager::_EnsureBufferPool(size16 size)
{
    size_t byteSize = size.cx * size.cy;
    if (!_bufferPool || (_bufferPool->GetSize() != byteSize))
    {
        _bufferPool = std::make_unique<BufferPool<12>>(byteSize);
        Invalidate();
    }
}

RGBQUAD *PicDrawManager::_GetPalette()
{
    return (_isVGA ? _paletteVGA : nullptr);
}

void PicDrawManager::_ApplyVGAPalette(const PaletteComponent *pPalette)
{
    for (int i = 1; i < 255; i++)
    {
        _paletteVGA[i] = pPalette->Colors[i];
    }
}

void PicDrawManager::_Reset()
{
    _fValidScreens = PicScreenFlags::None;
    _iDrawPos = -1;
    _fValidPalette = false;
    _fValidState = false;
    _bPaletteNumber = 0;
    //_currentState.Reset(_bPaletteNumber);
    _iInsertPos = -1;
}

void PicDrawManager::SetPic(const PicComponent *pPic, const PaletteComponent *pPalette)
{
    _isVGA = (pPalette != nullptr);
    _isContinuousPri = pPic && pPic->Traits->ContinuousPriority;
    if (pPic != _pPicWeak)
    {
        _Reset();
    }
    _pPicWeak = pPic;
    if (pPalette)
    {
        _ApplyVGAPalette(pPalette);
    }
}

//
// Caller needs to free HBITMAP
//
HBITMAP PicDrawManager::CreateBitmap(PicScreen screen, PicPosition pos, size16 size, int cx, int cy, SCIBitmapInfo *pbmi, uint8_t **pBitsDest)
{
    _EnsureBufferPool(size);
    _RedrawBuffers(nullptr, PicScreenToFlags(screen), PicPositionToFlags(pos));
    RGBQUAD *colors = _GetPalette();
    int colorCount = 256;
    if (screen == PicScreen::Visual)
    {
        colors = _GetPalette();
        colorCount = 256;
    }
    else
    {
        if (_pPicWeak && _pPicWeak->Traits->ContinuousPriority)
        {
            colors = g_continuousPriorityColors;
            colorCount = ARRAYSIZE(g_continuousPriorityColors);
        }
        else
        {
            colors = g_egaColors;
            colorCount = ARRAYSIZE(g_egaColors);
        }
    }
    return _CreateBitmap(GetScreenData(screen, pos), cx, cy, colors, colorCount, pbmi, pBitsDest);
}

const uint8_t *PicDrawManager::GetPicBits(PicScreen screen, PicPosition pos, size16 size)
{
    _EnsureBufferPool(size);
    _RedrawBuffers(nullptr, PicScreenToFlags(screen), PicPositionToFlags(pos));
    return GetScreenData(screen, pos);
}

void PicDrawManager::GetBitmapInfo(PicScreen screen, BITMAPINFO **ppbmi)
{
    size16 size = _GetPicSize();
    _EnsureBufferPool(size);
    if (screen == PicScreen::Visual)
    {
        *ppbmi = new SCIBitmapInfo(size.cx, size.cy, _GetPalette(), 256);
    }
    else
    {
        if ((screen == PicScreen::Priority) && _pPicWeak && _pPicWeak->Traits->ContinuousPriority)
        {
            *ppbmi = new SCIBitmapInfo(size.cx, size.cy, g_continuousPriorityColors, ARRAYSIZE(g_continuousPriorityColors));
        }
        else
        {
            // These are always EGA bitmaps
            *ppbmi = new SCIBitmapInfo(size.cx, size.cy);
        }
    }
}

//
// Copies one of our bitmaps (specified by dwDrawEnable) into pdataDisplay,
// and the aux bitmap into pdataAux.  So callers can take what we have,
// and add things.
// Caller must delete the optional parameter *ppbmi
//
void PicDrawManager::CopyBitmap(PicScreen screen, PicPosition pos, size16 size, uint8_t *pdataDisplay, uint8_t *pdataAux, BITMAPINFO **ppbmi)
{
    assert(size == _GetPicSize());
    _EnsureBufferPool(size);

    PicScreenFlags flags = PicScreenToFlags(screen);
    _RedrawBuffers(nullptr, flags, PicPositionToFlags(pos));
    assert(IsFlagSet(_fValidScreens, flags));

    memcpy(pdataDisplay, GetScreenData(screen, pos), size.cx * size.cy);
    if (ppbmi)
    {
        if (screen == PicScreen::Visual)
        {
            *ppbmi = new SCIBitmapInfo(size.cx, size.cy, _GetPalette(), 256);
        }
        else
        {
            // These are always EGA bitmaps
            *ppbmi = new SCIBitmapInfo(size.cx, size.cy);
        }
    }

    memcpy(pdataAux, GetScreenData(PicScreen::Aux, pos), size.cx * size.cy);
}

void PicDrawManager::SetPalette(uint8_t bPaletteNumber)
{
    if (bPaletteNumber != _bPaletteNumber)
    {
        _bPaletteNumber = bPaletteNumber;
        _fValidPalette = false; // Since the palette changed.
        _fValidScreens = PicScreenFlags::None;
        _fValidState = false;
    }
}

uint8_t *PicDrawManager::GetScreenData(PicScreen screen, PicPosition pos)
{
    return _screenBuffers[static_cast<int>(pos)][static_cast<int>(screen)];
}

void PicDrawManager::SetScreenData(PicScreen screen, PicPosition pos, uint8_t *data)
{
    _screenBuffers[static_cast<int>(pos)][static_cast<int>(screen)] = data;
}

void PicDrawManager::_EnsureInitialBuffers(PicScreenFlags screenFlags)
{
    for (int i = 0; i < 4; i++)
    {
        if (IsFlagSet(screenFlags, (PicScreenFlags)(0x1 << i)))
        {
            if (!_screenBuffers[0][i])
            {
                _screenBuffers[0][i] = _bufferPool->AllocateBuffer();

                // REVIEW test -> indicate uninitlaize data.
                *_screenBuffers[0][i] = 0xe;
            }
        }
        else
        {
            // Return this and null out
            _ReturnOldBufferIfNotUsedAnywhere((PicScreen)i, PicPosition::PrePlugin);
        }
    }
}

static int g_redrawDebug = 0;

void PicDrawManager::_RedrawBuffers(ViewPort *pState, PicScreenFlags screenFlags, PicPositionFlags picPositionFlags, bool assertIfCausedRedraw)
{
    size16 size = _GetPicSize();
    _EnsureBufferPool(size);
    ViewPort state(_bPaletteNumber);
    if (pState == nullptr)
    {
        pState = &state;
    }
    else
    {
        pState->Reset(_bPaletteNumber);
    }

    // If we know someone is going to want other screens, may as well include them now.
    for (IPicDrawPlugin *plugin : _plugins)
    {
        screenFlags |= plugin->GetRequiredScreens();
        picPositionFlags |= plugin->GetRequiredPicPosition();
    }

    // control and priority require visual for accurate fills, so add that in
    if (IsFlagSet(screenFlags, PicScreenFlags::Control | PicScreenFlags::Priority))
    {
        screenFlags |= PicScreenFlags::Visual;
    }
    screenFlags |= PicScreenFlags::Aux; // Always

    if (AreAllFlagsSet(_fValidScreens, screenFlags) && (AreAllFlagsSet(_validPositions, picPositionFlags)))
    {
        // Nothing to do
        return;
    }

    // If we need some new screens, make it so that no positions are valid. We'll just redraw from scratch. Otherwise
    // it's too complicated.
    if (!AreAllFlagsSet(_fValidScreens, screenFlags))
    {
        _validPositions = PicPositionFlags::None;
    }

    // An important distinction:
    // PicPositionFlags indicate which buffers people will be requesting. It doesn't mean we don't need
    // to draw those to get to subsequent buffers, obviously.

    // If anyone requests something earlier than we have, then we need to invalidate everything from then on.
    // For instance, if someone needs PrePlugin, and that is not valid, we need to invalidate everything
    // If someone needs PostPlugin, and we only have
    PicPosition clearOutFromHereOn = PicPosition::PrePlugin;
    if (IsFlagSet(_validPositions, PicPositionFlags::PrePlugin))
    {
        // We can re-use PrePlugin
        clearOutFromHereOn = PicPosition::PostPlugin;
    }
    if (IsFlagSet(_validPositions, PicPositionFlags::PostPlugin))
    {
        clearOutFromHereOn = PicPosition::Final;
    }

    if (clearOutFromHereOn <= PicPosition::PrePlugin)
    {
        _ReturnOldBufferIfNotUsedAnywhere(PicPosition::PrePlugin);
    }
    if (clearOutFromHereOn <= PicPosition::PostPlugin)
    {
        _ReturnOldBufferIfNotUsedAnywhere(PicPosition::PostPlugin);
    }
    if (clearOutFromHereOn <= PicPosition::Final)
    {
        _ReturnOldBufferIfNotUsedAnywhere(PicPosition::Final);
    }

    _EnsureInitialBuffers(screenFlags);

    bool atFinalPosition = (_iDrawPos == -1) || (_iDrawPos == _pPicWeak->commands.size());

    // Can we re-use some of our per-position buffers? This optimization is essential, it will be used
    // when we're drawing lines and such in the editor.
    if (!IsFlagSet(_validPositions, PicPositionFlags::PrePlugin))
    {
        // Our initial viewport state.
        _viewPorts[0] = *pState;

        // Default states
        if (IsFlagSet(screenFlags, PicScreenFlags::Priority))
        {
            memset(GetScreenData(PicScreen::Priority, PicPosition::PrePlugin), 0x00, _bufferPool->GetSize());
        }
        if (IsFlagSet(screenFlags, PicScreenFlags::Control))
        {
            memset(GetScreenData(PicScreen::Control, PicPosition::PrePlugin), 0x00, _bufferPool->GetSize());
        }
        if (IsFlagSet(screenFlags, PicScreenFlags::Visual))
        {
            memset(GetScreenData(PicScreen::Visual, PicPosition::PrePlugin), _isVGA ? 0xff : 0x0f, _bufferPool->GetSize());
        }
        memset(GetScreenData(PicScreen::Aux, PicPosition::PrePlugin), 0x00, _bufferPool->GetSize());

        PicData data =
        {
            screenFlags,
            GetScreenData(PicScreen::Visual, PicPosition::PrePlugin), // Visual always needs to be provided (for fill)
            GetScreenData(PicScreen::Priority, PicPosition::PrePlugin),
            GetScreenData(PicScreen::Control, PicPosition::PrePlugin),
            GetScreenData(PicScreen::Aux, PicPosition::PrePlugin),
            _isVGA,
            _GetPicSize(),
            _isContinuousPri
        };

        // Now draw!
        Draw(*_pPicWeak, data, _viewPorts[0], 0, _iDrawPos);
    }

    // Perf optimization: if no one is drawing on the pic, then we can "skip" this step
    bool willDrawOnPic = false;
    for (IPicDrawPlugin *plugin : _plugins)
    {
        willDrawOnPic = willDrawOnPic || plugin->WillDrawOnPic();
    }
    if (willDrawOnPic)
    {
        // Now do the plugins
        _MoveToNextStep(picPositionFlags, PicPosition::PrePlugin);

        // TODO: NEeds to be an if statement here
        // Basically, if someone needs PostPlugin or Final, and PostPlugin is not valid
        if (!IsFlagSet(_validPositions, PicPositionFlags::PostPlugin) &&
            IsFlagSet(picPositionFlags, PicPositionFlags::PostPlugin | PicPositionFlags::Final))
        {
            PicData data =
            {
                screenFlags,
                GetScreenData(PicScreen::Visual, PicPosition::PostPlugin), // Visual always needs to be provided (for fill)
                GetScreenData(PicScreen::Priority, PicPosition::PostPlugin),
                GetScreenData(PicScreen::Control, PicPosition::PostPlugin),
                GetScreenData(PicScreen::Aux, PicPosition::PostPlugin),
                _isVGA,
                _GetPicSize(),
                _isContinuousPri
            };

            // OutputDebugString("Drawing plguins\n");

            for (IPicDrawPlugin *plugin : _plugins)
            {
                plugin->DrawOnPic(_viewPorts[1], data, screenFlags);
            }
        }
    }
    else
    {
        // Pretend no one needs "post plugin", and just use the previous buffers.
        _MoveToNextStep(picPositionFlags & ~PicPositionFlags::PrePlugin, PicPosition::PrePlugin);
    }

    // Perf optimization: if the current position is the final, then "skip" this step and just re-use buffers.
    if (atFinalPosition)
    {
        _MoveToNextStep(picPositionFlags & ~PicPositionFlags::PostPlugin, PicPosition::PostPlugin);
    }
    else
    {
        _MoveToNextStep(picPositionFlags, PicPosition::PostPlugin);
    }

    if (!IsFlagSet(_validPositions, PicPositionFlags::Final) &&
        IsFlagSet(picPositionFlags, PicPositionFlags::Final))
    {
        if (!atFinalPosition && IsFlagSet(picPositionFlags, PicPositionFlags::Final))
        {
            // OutputDebugString("Drawing final\n");

            PicData data =
            {
                screenFlags,
                GetScreenData(PicScreen::Visual, PicPosition::Final), // Visual always needs to be provided (for fill)
                GetScreenData(PicScreen::Priority, PicPosition::Final),
                GetScreenData(PicScreen::Control, PicPosition::Final),
                GetScreenData(PicScreen::Aux, PicPosition::Final),
                _isVGA,
                _GetPicSize(),
                _isContinuousPri
            };

            // Now draw!
            Draw(*_pPicWeak, data, _viewPorts[2], _iDrawPos, -1);
        }
    }

    _validPositions = picPositionFlags;
    _fValidScreens = screenFlags;
}

void PicDrawManager::_ReturnOldBufferIfNotUsedAnywhere(PicPosition pos)
{
    for (int screenIndex = 0; screenIndex < 4; screenIndex++)
    {
        PicScreen screen = (PicScreen)screenIndex;
        _ReturnOldBufferIfNotUsedAnywhere(screen, pos);
    }
}

void PicDrawManager::_ReturnOldBufferIfNotUsedAnywhere(PicScreen screen, PicPosition pos)
{
    uint8_t *buffer = GetScreenData(screen, pos);

    bool usedElsewhere = false;
    for (int posIndex = 0; !usedElsewhere && (posIndex < 3); posIndex++)
    {
        PicPosition cur = (PicPosition)posIndex;
        usedElsewhere = (cur != pos) && (buffer == GetScreenData(screen, cur));
    }
    if (!usedElsewhere)
    {
        _bufferPool->FreeBuffer(buffer);
    }
    SetScreenData(screen, pos, nullptr);
}

void PicDrawManager::_MoveToNextStep(PicPositionFlags requestedFlags, PicPosition posCompleted)
{
    bool debugFlag = false;

    PicPositionFlags posFlagsCompleted = PicPositionToFlags(posCompleted);
    PicPosition nextPos = (PicPosition)((int)posCompleted + 1);

    _viewPorts[(int)nextPos] = _viewPorts[(int)posCompleted];

    _ReturnOldBufferIfNotUsedAnywhere(nextPos);
    // If the next position has different buffers, copy from the previous position.
    for (int screenIndex = 0; screenIndex < 4; screenIndex++)
    {
        PicScreen screen = (PicScreen)screenIndex;
        uint8_t *prev = GetScreenData(screen, posCompleted);
        if (prev)
        {
            // If someone needs the position we just completed, we need to "branch" our buffers.
            // Otherwise just assign pointers.
            if (IsFlagSet(requestedFlags, posFlagsCompleted))
            {
                SetScreenData(screen, nextPos, _bufferPool->AllocateBuffer());
                size16 size = _GetPicSize();
                memcpy(GetScreenData(screen, nextPos), prev, size.cx * size.cy);
                debugFlag = true;
            }
            else
            {
                // Just keep drawing on the same surface
                SetScreenData(screen, nextPos, prev);
            }
        }
        else
        {
            SetScreenData(screen, nextPos, nullptr);
        }
    }

    if (debugFlag)
    {
        // OutputDebugString("memcpy'd\n");
    }
}


HBITMAP _ScaleByHalf(uint8_t *pData, size16 sizeSource, int cx, int cy, const RGBQUAD *palette, int paletteCount)
{
    HBITMAP hbm = nullptr;

    SCIBitmapInfo bmiSCI(sizeSource.cx, sizeSource.cy, palette, paletteCount);
    RGBQUAD *newData = nullptr;
    HDC hDC = GetDC(nullptr);
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    hbm = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&newData, nullptr, 0);

    ReleaseDC(nullptr, hDC);

    for (int y = 0; y < cy; y++)
    {
        uint8_t *source = pData + (y * 2 * sizeSource.cx);
        uint8_t *sourceNext = pData + ((y * 2 + 1) * sizeSource.cx);
        RGBQUAD *dest = newData + ((cy - (y + 1)) * cx);
        for (int x = 0; x < cx; x++)
        {
            RGBQUAD a = bmiSCI.bmiColors[*source];
            RGBQUAD b = bmiSCI.bmiColors[*(source + 1)];
            RGBQUAD c = bmiSCI.bmiColors[*(sourceNext)];
            RGBQUAD d = bmiSCI.bmiColors[*(sourceNext + 1)];
            int blue = (a.rgbBlue + b.rgbBlue + c.rgbBlue + d.rgbBlue) >> 2;
            int red = (a.rgbRed + b.rgbRed + c.rgbRed + d.rgbRed) >> 2;
            int green = (a.rgbGreen + b.rgbGreen + c.rgbGreen + d.rgbGreen) >> 2;
            RGBQUAD final = {
                (BYTE)blue, (BYTE)green, (BYTE)red, 0
            };
            *dest = final;
            dest++;
            source += 2;
            sourceNext += 2;
        }
    }

    return hbm;
}

HBITMAP PicDrawManager::_GetBitmapGDIP(uint8_t *pData, int cx, int cy, const RGBQUAD *palette, int paletteCount) const
{
    size16 size = _GetPicSize();
    HBITMAP hbm = nullptr;
    SCIBitmapInfo bmi(size.cx, size.cy, palette, paletteCount);
    std::unique_ptr<Gdiplus::Bitmap> pimgVisual(Gdiplus::Bitmap::FromBITMAPINFO(&bmi, pData));
    if (pimgVisual)
    {
        if ((cx == size.cx) && (cy == size.cy))
        {
            // Exact size.
            pimgVisual->GetHBITMAP(Color::Black, &hbm);
        }
        else
        {
            if ((cx == size.cx / 2) && (cy == size.cy / 2))
            {
                hbm =  _ScaleByHalf(pData, size, cx, cy, palette, paletteCount);
            }
            else
            {
                std::unique_ptr<Bitmap> newBitmap = std::make_unique<Bitmap>(cx, cy, PixelFormat24bppRGB);
                Graphics graphics(newBitmap.get());

                graphics.SetCompositingMode(CompositingModeSourceCopy);
                graphics.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
                graphics.SetSmoothingMode(SmoothingModeHighSpeed);
                graphics.SetInterpolationMode(InterpolationModeHighQualityBilinear);

                graphics.DrawImage(pimgVisual.get(), 0, 0, cx, cy);
                newBitmap->GetHBITMAP(Color::Black, &hbm);
            }
        }
    }
    return hbm;
}

HBITMAP PicDrawManager::_CreateBitmap(uint8_t *pData, int cx, int cy, const RGBQUAD *palette, int paletteCount, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest) const
{
    HBITMAP hbm = nullptr;
    if (!appState->_fNoGdiPlus && (!pbmi || !ppBitsDest))
    {
        // Use GDI+ if we can - the images are much better.
        hbm = _GetBitmapGDIP(pData, cx, cy, palette, paletteCount);
    }
    else
    {
        // Here we'll use some ugly GDI scaling.
        CDC dc;
        if (dc.CreateCompatibleDC(nullptr))
        {
            int cxBmp = 320;
            int cyBmp = 190;
            SCIBitmapInfo bmi;
            uint8_t *pBitsDest;
            if (!pbmi)
            {
                ppBitsDest = &pBitsDest;
                pbmi = &bmi;
            }
            *pbmi = SCIBitmapInfo(cx, cy, palette, paletteCount);
            CBitmap bitmap;
            if (bitmap.Attach(CreateDIBSection(dc, pbmi, DIB_RGB_COLORS, (void**)ppBitsDest, nullptr, 0)))
            {
                SCIBitmapInfo bmiSrc(cxBmp, cyBmp, palette, paletteCount);
                HGDIOBJ hObjOld = dc.SelectObject((HBITMAP)bitmap);

                if ((cx != cxBmp) || (cy != cyBmp))
                {
                    // NOTE: HALFTONE is not supported on Win 98/Me
                    // As a result, the images will be resized poorly for dithered colours.
                    SetStretchBltMode(dc, HALFTONE);
                    SetBrushOrgEx(dc, 0, 0, nullptr);
                    StretchDIBits(dc, 0, 0, cx, cy,
                        0, 0, cxBmp, cyBmp, pData, &bmiSrc, DIB_RGB_COLORS, SRCCOPY);
                }
                else
                {
                    SetStretchBltMode(dc, COLORONCOLOR);
                    SetBrushOrgEx(dc, 0, 0, nullptr);
                    StretchDIBits(dc, 0, 0, cx, cy,
                        0, 0, cxBmp, cyBmp, pData, &bmiSrc, DIB_RGB_COLORS, SRCCOPY);
                }
                dc.SelectObject(hObjOld);
                hbm = (HBITMAP)bitmap.Detach();
            }
        }
    }
    return hbm;
}

void PicDrawManager::RefreshAllScreens(PicScreenFlags picScreenFlags, PicPositionFlags picPositionFlags)
{
    _RedrawBuffers(nullptr, picScreenFlags, picPositionFlags);
}

const ViewPort *PicDrawManager::GetViewPort(PicPosition pos)
{
    _RedrawBuffers(nullptr, _fValidScreens, PicPositionToFlags(pos), true);
    return &_viewPorts[(int)pos];
}

//
// Given a point, determines the position in the pic, before iState (-1 = end)
// where that position changed.
// Returns -1 if that point was never changed.
// Currently only works for visual screen.
//
ptrdiff_t PicDrawManager::PosFromPoint(int x, int y, ptrdiff_t iStart)
{
    ViewPort state(0);
    size16 size = _GetPicSize();
    size_t byteSize = size.cx * size.cy;
    // Clear out our cached bitmaps.
    PicScreenFlags dwMapsToRedraw = PicScreenFlags::None;
    std::vector<uint8_t> pdataVisual(byteSize, 0x0f);
    std::vector<uint8_t> pdataAux(byteSize, 0x00);
    dwMapsToRedraw |= PicScreenFlags::Visual;
    PicData data =
    {
        dwMapsToRedraw,
        &pdataVisual[0], // Visual always needs to be provided (for fill)
        nullptr,
        nullptr,
        &pdataAux[0], // Aux always needs to be provided (for fill)
        _isVGA,
        _GetPicSize(),
        _isContinuousPri
    };

    return GetLastChangedSpot(*_pPicWeak, data, state, x, y);
}

//
// Seek to a particular position.
// The picture is drawn up to and including that position.
// 
//
bool PicDrawManager::SeekToPos(ptrdiff_t iPos)
{
    bool fChanged = ((iPos != _iInsertPos) || (iPos != _iDrawPos));
    if (fChanged)
    {
        _iInsertPos = iPos;
        _iDrawPos = iPos;
        _OnPosChanged();
    }
    return fChanged;
}

void PicDrawManager::AddPicPlugin(IPicDrawPlugin *plugin)
{
    _plugins.push_back(plugin);
}

void PicDrawManager::Invalidate()
{
    _fValidScreens = PicScreenFlags::None;
    _fValidState = false;
}

void PicDrawManager::_OnPosChanged(bool fNotify)
{
    Invalidate();
}

void PicDrawManager::InvalidatePlugins()
{
    _validPositions &= ~(PicPositionFlags::Final | PicPositionFlags::PostPlugin);
}

size16 PicDrawManager::_GetPicSize() const
{
    size16 size(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT);
    if (_pPicWeak)
    {
        size = _pPicWeak->Size;
    }
    return size;
}