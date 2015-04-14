#pragma once

#include "PicDrawManager.h"

class IBitmapEditor;

class ZoomCelStatic : public CStatic
{
    DECLARE_DYNAMIC(ZoomCelStatic)

public:
    ZoomCelStatic(int zoom, bool drawCrossHairs, PicPosition picPosition);
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

    CPoint _cursorPos;

    int _zoom;
    bool _drawCrossHairs;
    PicPosition _picPosition;

    PicScreen _currentZoomScreen;
    IBitmapEditor *_be;
    PicDrawManager *_pdm;
};
