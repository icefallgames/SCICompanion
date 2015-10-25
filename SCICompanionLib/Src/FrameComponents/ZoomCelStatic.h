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

#include "PicDrawManager.h"

class IBitmapEditor;

class ZoomCelStatic : public CStatic
{
    DECLARE_DYNAMIC(ZoomCelStatic)

public:
    ZoomCelStatic(int zoom, bool drawCrossHairs, bool ensureAllVisible, PicPosition picPosition);
    virtual ~ZoomCelStatic();

    void OnUpdateBitmapEditor(IBitmapEditor *be);
    PicScreen GetCurrentScreen() { return _currentZoomScreen; }
    void SetCurrentScreen(PicScreen screen);
    void SetDrawManager(PicDrawManager *pdm);

protected:
    DECLARE_MESSAGE_MAP()

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

private:

    void _Update();
    CSize _GetBitmapSize();

    CPoint _cursorPos;

    int _zoom;
    bool _drawCrossHairs;
    bool _ensureAllVisible;
    PicPosition _picPosition;

    PicScreen _currentZoomScreen;
    IBitmapEditor *_be;
    PicDrawManager *_pdm;
};
