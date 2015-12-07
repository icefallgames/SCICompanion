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

// PicEditorView.cpp : implementation of the CPicView class
//

#include "stdafx.h"
#include "AppState.h"
#include "PicChildFrame.h"
#include "PicDoc.h"
#include "PicView.h"
#include "ChooseColorDialog.h"
#include "PaletteDefinitionDialog.h"
#include "ChoosePenStyleDialog.h"
#include "DontShowAgainDialog.h"
#include "BitmapToPicDialog.h"
#include "BitmapToVGADialog.h"
#include "PicDrawManager.h"
#include <math.h>
#include "CObjectWrap.h"
#include "ColorQuantization.h"
#include "View.h"
#include "ImageUtil.h"
#include "format.h"
#include "View.h"
#include "PaletteEditorDialog.h"
#include "PicDimensionsDialog.h"
#include "GotoDialog.h"
#include "PicCommands.h"
#include "EditCelDataDialog.h"
#include "ClipboardUtil.h"

const int PicGutter = 5;
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const key_value_pair<CPicView::ToolType, UINT> CPicView::c_toolToID [] =
{
    { Command, ID_PENTOOL },
    { Command, ID_FILL },
    { Command, ID_CIRCLE },
    { Command, ID_LINE },
    { History, ID_HISTORY },
    { Zoom, ID_ZOOM },
    { Polygons, ID_POLYPATH },
    { None, ID_DRAWOFF },
    // Add more here...
};

CExtAlphaSlider *g_pPicAlphaSlider = NULL;

const int PolygonPointHitTestMargin = 5;
const COLORREF ColorPolyHighlight = RGB(255, 64, 64);

const COLORREF PolygonColorsAlt[] =
{
    RGB(98, 128, 128),
    RGB(98, 128, 128),
    RGB(128, 128, 128),
    RGB(98, 128, 98)
};

const COLORREF PolygonColors[] =
{
    RGB(162, 214, 214),
    RGB(162, 214, 214),
    RGB(214, 214, 214),
    RGB(162, 214, 162)
};


template<typename _TPoint>
_TPoint ScreenResolutionToGameResolution(_TPoint point)
{
    return ScreenResolutionToGameResolution(point, appState->GetVersion().DefaultResolution);
}

struct CommandModifier
{
    PicCommand original;
    PicCommand originalAlt;
    ptrdiff_t index;
    bool firstCoordSet;
    PicCommand::CommandType type;

    void Begin(const PicComponent &pic, PicCommand::CommandType type, ptrdiff_t bestIndex, bool first);
    void ApplyDifference(PicComponent &pic, int dx, int dy);
    void Reset();
    void GetTransformCoord(const PicComponent &pic, int &x, int &y);
    void Delete(PicComponent &pic);
};


void CPicView::_ClampPoint(CPoint &point)
{
    size16 size = _GetPicSize();
    point.x = max(0, point.x);
    point.y = max(0, point.y);
    point.x = min(size.cx - 1, point.x);
    point.y = min(size.cy - 1, point.y);
}
void CPicView::_ClampPoint(point16 &point)
{
    size16 size = _GetPicSize();
    point.x = max(0, point.x);
    point.y = max(0, point.y);
    point.x = min(size.cx - 1, point.x);
    point.y = min(size.cy - 1, point.y);
}

void CommandModifier::Reset()
{
    index = 0;
    type = PicCommand::CommandType::None;
}

void CommandModifier::Begin(const PicComponent &pic, PicCommand::CommandType typeIn, ptrdiff_t indexIn, bool first)
{
    type = typeIn;
    index = indexIn;
    original.type = PicCommand::CommandType::None;
    originalAlt.type = PicCommand::CommandType::None;
    if (type == PicCommand::CommandType::Line)
    {
        if (first)
        {
            originalAlt = pic.commands[index - 1];
            if (index < (ptrdiff_t)pic.commands.size())
            {
                PicCommand nextCommand = pic.commands[index];
                if ((nextCommand.type == type) && (nextCommand.drawLine.xFrom == originalAlt.drawLine.xTo) && ((nextCommand.drawLine.yFrom == originalAlt.drawLine.yTo)))
                {
                    original = nextCommand;
                }
            }
        }
        else
        {
            original = pic.commands[index];
            if ((index - 1) >= 0)
            {
                PicCommand prevCommand = pic.commands[index - 1];
                if ((prevCommand.type == type) && (prevCommand.drawLine.xTo == original.drawLine.xFrom) && ((prevCommand.drawLine.yTo == original.drawLine.yFrom)))
                {
                    originalAlt = prevCommand;
                }
            }
        }
    }
    else
    {
        original = original = pic.commands[index];
        firstCoordSet = first;
    }

}

void CommandModifier::Delete(PicComponent &pic)
{
    if ((type == PicCommand::CommandType::Fill) || (type == PicCommand::CommandType::Pattern) || (type == PicCommand::CommandType::Circle))
    {
        // Easy
        if (index < (ptrdiff_t)pic.commands.size())
        {
            pic.commands.erase(pic.commands.begin() + index);
        }
    }
    else if (type == PicCommand::CommandType::Line)
    {
        if (original.type != PicCommand::CommandType::None)
        {
            uint16_t x = pic.commands[index].drawLine.xTo;
            uint16_t y = pic.commands[index].drawLine.yTo;
            pic.commands.erase(pic.commands.begin() + index);
            pic.commands[index - 1].drawLine.xTo = x;
            pic.commands[index - 1].drawLine.yTo = y;
        }
        else
        {
            pic.commands.erase(pic.commands.begin() + (index - 1));
        }
    }
}

void CommandModifier::GetTransformCoord(const PicComponent &pic, int &x, int &y)
{
    x = 0; y = 0;
    if (type == PicCommand::CommandType::Line)
    {
        if (original.type != PicCommand::CommandType::None)
        {
            x = pic.commands[index].drawLine.xFrom;
            y = pic.commands[index].drawLine.yFrom;
        }
        else
        {
            x = pic.commands[index - 1].drawLine.xTo;
            y = pic.commands[index - 1].drawLine.yTo;
        }
    }
    else if (type == PicCommand::CommandType::Circle)
    {
        if (firstCoordSet)
        {
            x = pic.commands[index].circle.xFrom;
            y = pic.commands[index].circle.yFrom;
        }
        else
        {
            x = pic.commands[index].circle.xTo;
            y = pic.commands[index].circle.yTo;
        }
    }
    else
    {
        x = pic.commands[index].fill.x;
        y = pic.commands[index].fill.y;
    }
}

void CommandModifier::ApplyDifference(PicComponent &pic, int dx, int dy)
{
    if (type == PicCommand::CommandType::Line)
    {
        // Line is tricky due to the way we store individual lines
        // The rule for lines is that "original" will always be the "from", and originalAlt the previous "to"
        if (original.type != PicCommand::CommandType::None)
        {
            PicCommand command = original;
            command.drawLine.xFrom += dx;
            command.drawLine.yFrom += dy;
            pic.commands[index] = command;
        }
        if (originalAlt.type != PicCommand::CommandType::None)
        {
            PicCommand command = originalAlt;
            command.drawLine.xTo += dx;
            command.drawLine.yTo += dy;
            pic.commands[index - 1] = command;
        }
    }
    else if (type == PicCommand::CommandType::Circle)
    {
        PicCommand command = original;
        if (firstCoordSet)
        {
            command.circle.xFrom += dx;
            command.circle.yFrom += dy;
        }
        else
        {
            command.circle.xTo += dx;
            command.circle.yTo += dy;
        }
        pic.commands[index] = command;
    }
    else
    {
        // Easy
        PicCommand command = original;
        // Use fill... it'll work for pen too.
        command.fill.x += dx;
        command.fill.y += dy;
        pic.commands[index] = command;
    }
}

// CPicView

IMPLEMENT_DYNCREATE(CPicView, CView)

BEGIN_MESSAGE_MAP(CPicView, CScrollingThing<CView>)
    // Standard printing commands
    ON_COMMAND(ID_PENTOOL, CPicView::OnPenCommand)
    ON_COMMAND(ID_ZOOM, CPicView::OnZoomTool)
    ON_COMMAND(ID_HISTORY, CPicView::OnHistoryTool)
    ON_COMMAND(ID_LINE, CPicView::OnLineCommand)
    ON_COMMAND(ID_FILL, CPicView::OnFillCommand)
    ON_COMMAND(ID_CIRCLE, CPicView::OnCircleCommand)
    ON_COMMAND(ID_DRAWOFF, CPicView::OnDrawOff)
    ON_COMMAND(ID_TRACEBITMAP, CPicView::OnTraceBitmap)
    ON_COMMAND(ID_SHOWTRACEIMAGE, CPicView::OnShowTraceImage)
    ON_COMMAND(IDC_TOGGLEVISUAL, CPicView::OnToggleVisual)
    ON_COMMAND(IDC_TOGGLEPRIORITY, CPicView::OnTogglePriority)
    ON_COMMAND(ID_TOGGLEGRIDLINES, CPicView::OnToggleGridLines)
    ON_COMMAND(IDC_TOGGLECONTROL, CPicView::OnToggleControl)
    ON_COMMAND(ID_SHOWVISUALSCREEN, CPicView::OnShowVisual)
    ON_COMMAND(ID_EDIT_RENAME, CPicView::OnShowVisual)  // A complete hack, but we use one accelerator list F2
    ON_COMMAND(ID_SHOWPRIORITYSCREEN, CPicView::OnShowPriority)
    ON_COMMAND(ID_SHOWCONTROLSCREEN, CPicView::OnShowControl)
    ON_COMMAND(ID_SHOWPALETTE0, CPicView::OnShowPalette0)
    ON_COMMAND(ID_SHOWPALETTE1, CPicView::OnShowPalette1)
    ON_COMMAND(ID_SHOWPALETTE2, CPicView::OnShowPalette2)
    ON_COMMAND(ID_SHOWPALETTE3, CPicView::OnShowPalette3)
    ON_COMMAND(ID_TOGGLEPRIORITYLINES, CPicView::OnTogglePriorityLines)
    ON_COMMAND(ID_POLYPATH, CPicView::OnPolyPath)
    ON_COMMAND(ID_TOGGLEEGO, CPicView::OnToggleEgo)
    ON_COMMAND(ID_LIGHTUPCOORDS, CPicView::OnLightUpCoords)
    ON_COMMAND(ID_DEFINEPALETTES, CPicView::OnSetPalette)
    ON_COMMAND(ID_EDIT_COPY, CPicView::OnCopyPic)
    ON_COMMAND(ID_EDIT_PASTE, CPicView::OnPaste)
    ON_COMMAND(ID_EDIT_IMPORTPICBACKGROUND, CPicView::OnPasteIntoPic)
    ON_COMMAND(ID_VIEW_ZOOMIN, CPicView::OnZoomIn)
    ON_COMMAND(ID_VIEW_ZOOMOUT, CPicView::OnZoomOut)
    ON_COMMAND(ID_OBSERVECONTROLLINES, CPicView::OnObserveControlLines)
    ON_COMMAND(ID_OBSERVEPOLYGONS, CPicView::OnObservePolygons)
    ON_COMMAND(ID_EGO_COPY, CPicView::OnCopyFakeEgoAttributes)
    ON_COMMAND(ID_PIC_EXPORT8, CPicView::OnExportPalettizedBitmap)
    ON_COMMAND(ID_PIC_EDITPALETTE, CPicView::EditVGAPalette)
    ON_COMMAND(ID_PIC_DELETEPOINT, CPicView::OnDeletePoint)
    ON_COMMAND(ID_PIC_CHANGEDIMENSIONS, CPicView::ChangeDimensions)
    ON_COMMAND(ID_PIC_REMOVESETVISUAL, CPicView::RemoveSetVisual)
    ON_COMMAND(ID_PIC_EDITCELDATA, CPicView::EditCelData)
    ON_COMMAND_RANGE(ID_DEFAULTPRIORITY, ID_MAIN_PRI15, CPicView::OnSetEgoPriority)
    ON_COMMAND_RANGE(ID_FAKEEGO0, ID_FAKEEGO12, CPicView::OnEnableFakeEgo)
    ON_COMMAND(ID_FAKEEGONUMBER, CPicView::OnEnableFakeEgoCustom)
    ON_UPDATE_COMMAND_UI(ID_PENTOOL, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_LINE, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_FILL, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_CIRCLE, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_DRAWOFF, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_ZOOM, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_HISTORY, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_TRACEBITMAP, CPicView::OnUpdateIsGDIPAvailable)
    ON_UPDATE_COMMAND_UI(ID_SHOWTRACEIMAGE, CPicView::OnUpdateShowTraceImage)
    ON_UPDATE_COMMAND_UI(ID_ALPHASLIDER, OnUpdateAlphaSlider)
    ON_UPDATE_COMMAND_UI(ID_SETPRIORITY, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_COORDS, CPicView::OnCommandUIStatus)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_PRI, CPicView::OnCommandUIStatus)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_CONTROLCOLOR, CPicView::OnCommandUIStatus)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_PRICOLOR, CPicView::OnCommandUIStatus)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_PIXELCOLOR, CPicView::OnCommandUIStatus)
    ON_UPDATE_COMMAND_UI(ID_SHOWVISUALSCREEN, CPicView::OnUpdateShowScreenControl)
    ON_UPDATE_COMMAND_UI(ID_SHOWPRIORITYSCREEN, CPicView::OnUpdateShowScreenControl)
    ON_UPDATE_COMMAND_UI(ID_SHOWCONTROLSCREEN, CPicView::OnUpdateShowScreenControl)
    ON_UPDATE_COMMAND_UI(ID_POLYPATH, CPicView::OnUpdateAllPicCommands)
    ON_UPDATE_COMMAND_UI(ID_TOGGLEPRIORITYLINES, CPicView::OnUpdateTogglePriorityLines)
    ON_UPDATE_COMMAND_UI(ID_TOGGLEGRIDLINES, CPicView::OnUpdateToggleGridLines)
    ON_UPDATE_COMMAND_UI(ID_SHOWPALETTE0, CPicView::OnUpdateShowPaletteControl)
    ON_UPDATE_COMMAND_UI(ID_SHOWPALETTE1, CPicView::OnUpdateShowPaletteControl)
    ON_UPDATE_COMMAND_UI(ID_SHOWPALETTE2, CPicView::OnUpdateShowPaletteControl)
    ON_UPDATE_COMMAND_UI(ID_SHOWPALETTE3, CPicView::OnUpdateShowPaletteControl)
    ON_UPDATE_COMMAND_UI(ID_TOGGLEPRIORITYLINES, CPicView::OnUpdateShowPaletteControl)
    ON_UPDATE_COMMAND_UI(ID_TOGGLEEGO, CPicView::OnUpdateToggleEgo)
    ON_UPDATE_COMMAND_UI(ID_LIGHTUPCOORDS, CPicView::OnUpdateLightUpCoords)
    ON_UPDATE_COMMAND_UI(IDC_TOGGLEVISUAL, CPicView::OnUpdateToggleScreen)
    ON_UPDATE_COMMAND_UI(IDC_TOGGLEPRIORITY, CPicView::OnUpdateToggleScreen)
    ON_UPDATE_COMMAND_UI(IDC_TOGGLECONTROL, CPicView::OnUpdateToggleScreen)
    ON_UPDATE_COMMAND_UI(ID_DEFINEPALETTES, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, CPicView::OnCommandUpdatePastePic)
    ON_UPDATE_COMMAND_UI(ID_EDIT_IMPORTPICBACKGROUND, CPicView::OnUpdateIsGDIPAvailable)
    ON_UPDATE_COMMAND_UI(ID_DEFAULTPRIORITY, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI_RANGE(ID_DEFAULTPRIORITY, ID_MAIN_PRI15, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_ZOOMSLIDER, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_OBSERVECONTROLLINES, CPicView::OnUpdateObserveControlLines)
    ON_UPDATE_COMMAND_UI(ID_OBSERVEPOLYGONS, CPicView::OnUpdateObservePolygons)
    ON_UPDATE_COMMAND_UI(ID_PIC_EXPORT8, CPicView::OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_PIC_EDITPALETTE, CPicView::OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_PIC_CHANGEDIMENSIONS, CPicView::OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_PIC_DELETEPOINT, CPicView::OnCommandUIAlwaysValid)  // Since it's in a context menu we only bring up when it's available.
    ON_UPDATE_COMMAND_UI(ID_PIC_SPLITEDGE, CPicView::OnCommandUIAlwaysValid)  // Since it's in a context menu we only bring up when it's available.
    ON_UPDATE_COMMAND_UI(ID_FAKEEGONUMBER, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_PIC_EDITCELDATA, CPicView::OnUpdateEditCelData)
    ON_WM_ERASEBKGND()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
    ON_WM_KEYDOWN()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

// CPicView construction/destruction

CPicView::CPicView()
{
    _fGridLines = false;
    _transformCommandMod = std::make_unique<CommandModifier>();
    _currentPolyPointIndexInEdit = -1;
    _currentHoverPolyPointIndex = -1;
    _currentHoverPolyEdgeIndex = -1;
    _polyDragPointIndex = -1;

    _priBarMoveIndex = -1;
    _originalPriValue = -1;
    memset(&_originalPriValueCommand, 0, sizeof(_originalPriValueCommand));

    _isDrawUpToDate = PicScreenFlags::None;

    _attachedPlugin = false;

    _currentCommand = PicCommand::Pattern; // just something
    _currentTool = None;
    _fShowTraceImage = false;
    _fHaveTraceImage = false;
    _fShowPriorityLines = FALSE;
    _fShowingEgo = FALSE;

    _fCapturing = FALSE;
    _pointCapture.x = 0;
    _pointCapture.y = 0;
    _fakeEgoAttributes.Location.x = 140; // Good starting values.
    _fakeEgoAttributes.Location.y = 80;
    _fCanBeHere = false;

    _fDrawingLine = FALSE;
    _fDrawingCircle = FALSE;
    _fPreviewPen = FALSE;
    _fMouseWithin = FALSE;
    _xOld = -1;
    _yOld = -1;
    _ptCurrentHover = CPoint(0, 0);

    // Scrolling:
    _yOrigin = 0;
    _xOrigin = 0;

    // Default size of a pic.
    _cyPic = DEFAULT_PIC_HEIGHT * 2;
    _cxPic = DEFAULT_PIC_WIDTH * 2;

    // Start with the visual screen.
    _mainViewScreen = PicScreen::Visual;

    _fRegenerateTrace = false;
    _bRandomNR = 0;
    _transformingCoords = false;
    _iTraceAlpha = 50;
}

// Scrolling
int CPicView::_GetViewWidth()
{
    int zoom = _cxPic / _GetPicSize().cx;
    return _cxPic + PriorityBarWidth + 2 * zoom * PicGutter;
}
int CPicView::_GetViewHeight()
{
    int zoom = _cyPic / _GetPicSize().cy;
    return GetPicScreenHeight() + 2 * zoom * PicGutter;
}

int CPicView::GetPicScreenHeight() const
{
    int height = _cyPic;
    if (appState->_fUseOriginalAspectRatio)
    {
        height = appState->AspectRatioY(height);
    } 
    return height;
}

CPoint CPicView::GetZoomOffset() const
{
    return CPoint((_cyPic / _GetPicSize().cx) / 2, (GetPicScreenHeight() / _GetPicSize().cy) / 2);
}

PicDrawManager &CPicView::_GetDrawManager()
{
    return GetDocument()->GetDrawManager();
}

CPicView::~CPicView()
{
    _CleanUpPaste();
}

//
// This one takes into account origins
//
CPoint CPicView::_MapClientPointToPic(CPoint ptScreen)
{
    return CPoint((ptScreen.x + _xOrigin) * _GetPicSize().cx / _cxPic - PicGutter,
        (ptScreen.y + _yOrigin) * _GetPicSize().cy / GetPicScreenHeight() - PicGutter);
}

//
// From pic coordinates to window client coordinates
// (taking into account origins)
//
CPoint CPicView::_MapPicPointToClient(CPoint ptPic)
{
    return CPoint(((ptPic.x + PicGutter) * _cxPic / _GetPicSize().cx) - _xOrigin,
        ((ptPic.y + PicGutter) * GetPicScreenHeight() / _GetPicSize().cy) - _yOrigin);
}

void CPicView::OnPenCommand()
{
    // When this is pressed, choose it.
    _currentTool = Command;
    _currentCommand = PicCommand::Pattern;
    _UpdateCursor();
    _OnCommandChanged();
    _MakeRandomNR();
}

void CPicView::OnLineCommand()
{
    // When this is pressed, choose it.
    _currentTool = Command;
    _currentCommand = PicCommand::Line;
    _UpdateCursor();
    _OnCommandChanged();
}

void CPicView::OnFillCommand()
{
    // When this is pressed, choose it.
    _currentTool = Command;
    _currentCommand = PicCommand::Fill;
    _UpdateCursor();
    _OnCommandChanged();
}

void CPicView::OnCircleCommand()
{
    // When this is pressed, choose it.
    _currentTool = Command;
    _currentCommand = PicCommand::Circle;
    _UpdateCursor();
    _OnCommandChanged();
}

void CPicView::OnDrawOff()
{
    _currentTool = None;
    _UpdateCursor();
    _OnCommandChanged();
}

void CPicView::OnZoomTool()
{
    // When this is pressed, choose it.
    _currentTool = Zoom;
    _UpdateCursor();
    _OnCommandChanged();
}

void CPicView::OnHistoryTool()
{
    // When this is pressed, choose it.
    _currentTool = History;
    _UpdateCursor();
    _OnCommandChanged();
}

void CPicView::OnEnableFakeEgo(UINT nID)
{
    if (GetDocument())
    {
        int resourceNumber = appState->GetRecentViews()[nID - ID_FAKEEGO0];
        GetDocument()->SetFakeEgo(resourceNumber);
        _fShowingEgo = false;
        OnToggleEgo();
    }
}

void CPicView::OnSetEgoPriority(UINT nID)
{
    if (nID == ID_DEFAULTPRIORITY)
    {
        _fakeEgoAttributes.Pri = -1;
    }
    else
    {
        _fakeEgoAttributes.Pri = (nID - ID_MAIN_PRI0);
    }
}

void CPicView::OnObserveControlLines()
{
    appState->_fObserveControlLines = !appState->_fObserveControlLines;
}

void CPicView::OnObservePolygons()
{
    appState->_fObservePolygons = !appState->_fObservePolygons;
}

void CPicView::OnCopyFakeEgoAttributes()
{
    if (GetDocument())
    {
        _fakeEgoAttributes.View = GetDocument()->GetFakeEgo();

        sci::ostream stream;
        stream << _fakeEgoAttributes;
        OpenAndSetClipboardDataFromStream(this, appState->ViewAttributesClipboardFormat, stream);
    }
}

void CPicView::OnDeletePoint()
{
    GetDocument()->ApplyChanges<PicComponent>(
        [this](PicComponent &pic)
    {
        _transformCommandMod->Delete(pic);
        return WrapHint(PicChangeHint::EditPicInvalid);
    }
    );
    _transformCommandMod->Reset();
}

void CPicView::OnEnableFakeEgoCustom()
{
    if (GetDocument())
    {
        CGotoDialog resourceNumDialog;
        resourceNumDialog.Title = "Choose view number";
        resourceNumDialog.Label = "View number:";
        if (resourceNumDialog.DoModal() == IDOK)
        {
            GetDocument()->SetFakeEgo(resourceNumDialog.GetLineNumber());
            _fShowingEgo = false;
            OnToggleEgo();
        }
    }
}

void CPicView::ChangeDimensions()
{
    const PicComponent *pic = _GetEditPic();
    if (pic)
    {
        PicDimensionsDialog dialog(pic->Size);
        if ((IDOK == dialog.DoModal()) && (dialog.GetSize() != pic->Size))
        {
            size16 newSize = dialog.GetSize();
            GetDocument()->ApplyChanges<PicComponent>(
                [newSize](PicComponent &pic)
            {
                pic.Size = newSize;
                return WrapHint(PicChangeHint::NewPic);
            }
            );
        }
    }
}

void CPicView::EditVGAPalette()
{
    GetDocument()->SwitchToEmbeddedPalette();

    const PaletteComponent *palette = _GetPalette();
    if (palette)
    {
        vector<const Cel*> cels;
        for (const auto &command : _GetEditPic()->commands)
        {
            if (command.type == PicCommand::CommandType::DrawBitmap)
            {
                cels.push_back(command.drawVisualBitmap.pCel);
            }
        }

        GetDocument()->ApplyChanges<PaletteComponent>(
            [&cels, this](PaletteComponent &palette)
        {
            GetDocument()->SetPreviewPalette(&palette);
            PicChangeHint hint = PicChangeHint::None;
            PaletteComponent copy = palette;
            PaletteEditorDialog paletteEditor(this, palette, cels, true);
            if (IDOK == paletteEditor.DoModal())
            {
                if (copy != palette)
                {
                    hint |= PicChangeHint::Palette;
                }
            }
            GetDocument()->SetPreviewPalette(nullptr);
            return WrapHint(hint);
        }
        );
    }
}

void CPicView::OnExportPalettizedBitmap()
{
    const PicComponent *pic = _GetEditPic();
    const PaletteComponent *palette = _GetPalette();
    if (pic)
    {
        for (const auto &command : pic->commands)
        {
            if (command.type == PicCommand::CommandType::DrawBitmap)
            {
                // Default extension should be the first one in the list for g_szGdiplus8BitSaveFilter
                CFileDialog fileDialog(FALSE, ".bmp", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, g_szGdiplus8BitSaveFilter);
                if (IDOK == fileDialog.DoModal())
                {
                    CString strFileName = fileDialog.GetPathName();
                    Save8BitBmpGdiP(strFileName, *command.drawVisualBitmap.pCel, *palette);
                }
                break;
            }
        }
    }
}

void CPicView::OnUpdateObserveControlLines(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(appState->_fObserveControlLines);
}

void CPicView::OnUpdateObservePolygons(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(appState->_fObservePolygons);
}

void CPicView::OnUpdateIsVGA(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(_GetEditPic() && _GetEditPic()->Traits->IsVGA);
}


//
// Takes either a pszFileName, or a combination of pbmi and pBits
//
void CPicView::_MakeNewMasterTraceImage(PCTSTR pszFileName, BITMAPINFO *pbmi, void *pBits)
{
    // Remove any old one.
    _fShowTraceImage = false;
    _fHaveTraceImage = false;

    if (pszFileName)
    {
#ifdef UNICODE
        _pgdiplusTrace.reset(Bitmap::FromFile(pszFileName));
#else
        // GDI+ only deals with unicode.
        int a = lstrlenA(pszFileName);
        BSTR unicodestr = SysAllocStringLen(NULL, a);
        MultiByteToWideChar(CP_ACP, 0, pszFileName, a, unicodestr, a);
        _pgdiplusTrace.reset(Bitmap::FromFile(unicodestr));
        //... when done, free the BSTR
        SysFreeString(unicodestr);
#endif    
    }
    else if (pbmi && pBits)
    {
        _pgdiplusTrace.reset(Bitmap::FromBITMAPINFO(pbmi, pBits));
    }

    // If we got a new one, then set bits indicating we have one, and that
    // we need to regenerate the 320x190 bitmap on the next draw cycle.
    if (_pgdiplusTrace)
    {
        if ((_pgdiplusTrace->GetWidth() != _GetPicSize().cx) || (_pgdiplusTrace->GetHeight() != _GetPicSize().cy))
        {
            if (!appState->_fDontShowTraceScaleWarning)
            {
                TCHAR szPath[MAX_PATH];
                StringCchPrintf(szPath, ARRAYSIZE(szPath), TEXT("The image is not 320x190, and will be %s.\nTo change these settings, go to the preferences dialog."), appState->_fScaleTracingImages ? TEXT("scaled") : TEXT("cropped"));
                CDontShowAgainDialog dialog(szPath, appState->_fDontShowTraceScaleWarning);
                dialog.DoModal(); // Don't care about the thing.
            }
        }

        _fRegenerateTrace = true;
        _fShowTraceImage = true;
        _fHaveTraceImage = true;
        InvalidateOurselves();
    }
}

void CPicView::_CleanUpPaste()
{
    // Bail on the paste.
    _pastedCommands.clear();
}


// These are the formats that GDI+ uses.
extern TCHAR g_szGdiplusFilter[] = "Image Files|*.jpg;*.jpeg;*.bmp;*.gif;*.png;*.tif;*.tiff;*.emf|All Files|*.*|";

void CPicView::OnTraceBitmap()
{
    // Create a file dialog.
    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR, g_szGdiplusFilter, this);
    fileDialog.m_ofn.lpstrTitle = TEXT("Loading tracing image");
    if (IDOK == fileDialog.DoModal())
    {
        // Tell EditPic about the image.
		CString fileName = fileDialog.GetPathName();
        PCTSTR pszFileName = (PCTSTR)fileName;
        _MakeNewMasterTraceImage(pszFileName, NULL, NULL);
    }
}

void CPicView::OnShowTraceImage()
{
    if (_fHaveTraceImage)
    {
        _fShowTraceImage = !_fShowTraceImage;
        InvalidateOurselves();
    }
}


void CPicView::OnToggleVisual()
{
    const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
    PicCommand command;
    if (IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Visual))
    {
        command = PicCommand::CreateDisableVisual();
    }
    else
    {
        // When turning back on, use the last set values in the pic state:
        command = PicCommand::CreateSetVisual(pstate->bPaletteNumber, pstate->bPaletteOffset);
    }
    GetDocument()->InsertCommand(&command);
}

void CPicView::OnToggleGridLines()
{
    _fGridLines = !_fGridLines;
    InvalidateOurselves();
}

void CPicView::OnTogglePriority()
{
    const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
    PicCommand command;
    if (IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Priority))
    {
        command = PicCommand::CreateDisablePriority();
    }
    else
    {
        // When turning back on, use the last set values in the pic state:
        command = PicCommand::CreateSetPriority(pstate->bPriorityValue);
    }
    GetDocument()->InsertCommand(&command);
}

void CPicView::OnToggleControl()
{
    const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
    PicCommand command;
    if (IsFlagSet(pstate->dwDrawEnable, PicScreenFlags::Control))
    {
        command = PicCommand::CreateDisableControl();
    }
    else
    {
        // When turning back on, use the last set values in the pic state:
        command = PicCommand::CreateSetControl(pstate->bControlValue);
    }
    GetDocument()->InsertCommand(&command);
}


void CPicView::OnShowVisual()
{
    if (_mainViewScreen != PicScreen::Visual)
    {
        _mainViewScreen = PicScreen::Visual;
        GetDocument()->InformBitmapEditor(PicChangeHint::ViewScreenChange, this);
        InvalidateOurselves();
    }
}

void CPicView::OnShowPriority()
{
    if (_mainViewScreen != PicScreen::Priority)
    {
        _mainViewScreen = PicScreen::Priority;
        GetDocument()->InformBitmapEditor(PicChangeHint::ViewScreenChange, this);
        InvalidateOurselves();
    }
}

void CPicView::OnShowControl()
{
    if (_mainViewScreen != PicScreen::Control)
    {
        _mainViewScreen = PicScreen::Control;
        GetDocument()->InformBitmapEditor(PicChangeHint::ViewScreenChange, this);
        InvalidateOurselves();
    }
}

void CPicView::_OnShowPalette(BYTE b)
{
    GetDocument()->SetPalette(b);
}

void CPicView::OnShowPalette0() { _OnShowPalette(0); }
void CPicView::OnShowPalette1() { _OnShowPalette(1); }
void CPicView::OnShowPalette2() { _OnShowPalette(2); }
void CPicView::OnShowPalette3() { _OnShowPalette(3); }


void CPicView::OnTogglePriorityLines()
{
    _fShowPriorityLines = !_fShowPriorityLines;
    InvalidateOurselves();
}

void CPicView::OnPolyPath()
{
    // When this is pressed, choose it.
    _currentTool = Polygons;
    _UpdateCursor();
    _OnCommandChanged();
    if (GetDocument())
    {
        GetDocument()->SetShowPolygons(true);
    }
}

void CPicView::OnToggleEgo()
{
    _fShowingEgo = !_fShowingEgo;
    if (_fShowingEgo)
    {
        GetDocument()->EnsureFakeEgoNumber();

        // Generally, when showing the ego, we'll assume the user no longer wants
        // to use a drawing tool.  This behaviour is a little strange.
        // The alternative is to have draw commands not work when the mouse is over the
        // ego (and change the cursor).  But the user might want to draw in and around the ego.
        OnDrawOff();
    }
    _GetDrawManager().InvalidatePlugins();
    InvalidateOurselves();
}

void CPicView::OnLightUpCoords()
{
    _transformingCoords = !_transformingCoords;
    _UpdateCursor();
    InvalidateOurselves();
}

void CPicView::OnSetPalette()
{
    if (_GetEditPic()->Traits->IsVGA)
    {
        EditVGAPalette();
    }
    else
    {
        const PicComponent *pic = _GetEditPic();
        ptrdiff_t pos = GetDocument()->GetDrawManager().GetPos();
        if (pos == -1)
        {
            pos = pic->commands.size() - 1;
        }

        uint8_t paletteNumber = _GetDrawManager().GetPalette();


        GetDocument()->PreviewChanges<PicComponent>(
            [&, pos, paletteNumber](PicComponent &pic)
        {
            CPaletteDefinitionDialog dialog(*this, pic, pos, paletteNumber);
            INT_PTR result = dialog.DoModal();
            PicChangeHint hint = PicChangeHint::None;
            if ((result == IDOK) && dialog.GetChanged())
            {
                hint |= PicChangeHint::EditPicInvalid;
            }
            return WrapHint(hint);
        }
        );

        this->SetPosition(pos);
    }
}

void CPicView::OnCopyPic()
{
    OpenClipboardGuard clipBoard(this);
    if (clipBoard.IsOpen())
    {
        if (EmptyClipboard())
        {
            assert((HBITMAP)_bitmapDoubleBuf);
            // Quick and dirty: use the bitmap we generated for double-buffering.
            // We'll lose it to the clipboard, so we detach it.
            SetClipboardData(CF_BITMAP, (HBITMAP)_bitmapDoubleBuf.Detach());
            InvalidateOurselves();
        }
    }
}

BOOL _HaveCompatibleClipboardFormat(CWnd *pWnd, UINT *puFormat)
{
    BOOL bRet = FALSE;
    OpenClipboardGuard clipBoard(pWnd);
    if (clipBoard.IsOpen())
    {
        // See if we have a DIB, or a pic command object
        UINT rgFormats[] = { CF_DIB, appState->GetCommandClipboardFormat() };
        *puFormat = GetPriorityClipboardFormat(rgFormats, ARRAYSIZE(rgFormats));
        // This returns 0 if the clipboard is empty, and -1 if the format isn't available.
        bRet = *puFormat && (*puFormat != -1);
    }
    return bRet;
}

void CPicView::_InsertPastedCommands()
{
    // Perform the paste
    if (!_pastedCommands.empty())
    {
        // Adjust the commands:
        PICCOMMAND_ADJUST adjust = { 0 };
        _InitCommandAdjust(&adjust);
        PastedCommands_Adjust(_GetPicSize(), _pastedCommands, &adjust);

        // And then insert them.
        GetDocument()->InsertCommands(_pastedCommands.size(), &_pastedCommands[0]);
        _CleanUpPaste();
    }
}

//
// Returns the current paste rect, constrained.
//
void CPicView::_GetPasteRect(CRect &rect)
{
    rect.left = _xPasted + _xPasteOffset;
    rect.left = max(0, rect.left);
    rect.right = _xPasted + _xPasteOffset + _cxPastedScale;
    rect.right = min(_GetPicSize().cx, rect.right);
    rect.top = _yPasted + _yPasteOffset;
    rect.top = max(0, rect.top);
    rect.bottom = _yPasted + _yPasteOffset + _cyPastedScale;
    rect.bottom = min(_GetPicSize().cy, rect.bottom);
}

bool _AllowCommand(bool supportsPen, bool isVGA, bool canChangePriLines, const PicCommand &command)
{
    if (!supportsPen && (command.type == PicCommand::CommandType::Pattern))
    {
        return false;
    }
    if (isVGA && ((command.type == PicCommand::CommandType::SetPalette) || (command.type == PicCommand::CommandType::SetPaletteEntry)))
    {
        return false;
    }
    if (canChangePriLines && (command.type == PicCommand::CommandType::SetPriorityBars))
    {
        return false;
    }
    return true;
}

// This is some very basic SCI2 support for moving pic layers around.
void CPicView::OnUpdateEditCelData(CCmdUI *pCmdUI)
{
    const PicComponent *pic = _GetEditPic();
    BOOL enable = FALSE;
    if (pic->Traits->AllowMultipleBitmaps)
    {
        // A drawvisual command must be selected
        ptrdiff_t position = GetDocument()->GetPosition() - 1;
        if ((position >= 0) && (position < (ptrdiff_t)pic->commands.size()))
        {
            enable = pic->commands[position].type == PicCommand::CommandType::DrawBitmap;
        }
    }
    pCmdUI->Enable(enable);
}

// This is some very basic SCI2 support for moving pic layers around.
void CPicView::EditCelData()
{
    const PicComponent *pPic = _GetEditPic();
    BOOL enable = FALSE;
    if (pPic->Traits->AllowMultipleBitmaps)
    {
        // And a drawvisual command must be selected
        ptrdiff_t position = GetDocument()->GetPosition() - 1;
        if ((position >= 0) && (position < (ptrdiff_t)pPic->commands.size()))
        {
            GetDocument()->ApplyChanges<PicComponent>(
                [position](PicComponent &pic)
            {
                PicChangeHint hint = PicChangeHint::None;
                PicCommand &command = pic.commands[position];
                if (command.type == PicCommand::CommandType::DrawBitmap)
                {
                    EditCelDataDialog dialog(command.drawVisualBitmap.priority, *command.drawVisualBitmap.pCel);
                    if (dialog.DoModal() == IDOK)
                    {
                        hint = PicChangeHint::EditPicInvalid;
                    }
                }
                return WrapHint(hint);
            }
                );
        }
    }
}

// Useful for importing EGA pics into a VGA game.
void CPicView::RemoveSetVisual()
{
    if (GetDocument())
    {
        GetDocument()->ApplyChanges<PicComponent>(
            [](PicComponent &pic)
        {
            std::vector<PicCommand> newCommands;
            PicScreenFlags drawEnable = PicScreenFlags::None;
            for (const PicCommand &command : pic.commands)
            {
                bool discard = false;
                switch (command.type)
                {
                    case PicCommand::CommandType::SetVisual:
                        discard = true;
                        break;
                    case PicCommand::CommandType::SetControl:
                        drawEnable |= PicScreenFlags::Control;
                        break;
                    case PicCommand::CommandType::SetPriority:
                        drawEnable |= PicScreenFlags::Priority;
                        break;
                    case PicCommand::CommandType::DisableControl:
                        ClearFlag(drawEnable, PicScreenFlags::Control);
                        break;
                    case PicCommand::CommandType::DisablePriority:
                        ClearFlag(drawEnable, PicScreenFlags::Priority);
                        break;

                    // Drawing commands:
                    case PicCommand::CommandType::Circle:
                    case PicCommand::CommandType::Fill:
                    case PicCommand::CommandType::Pattern:
                    case PicCommand::CommandType::Line:
                        // Discard this drawing command if it now does nothing.
                        discard = (drawEnable == PicScreenFlags::None);
                        break;

                }

                if (!discard)
                {
                    newCommands.push_back(command);
                }
            }

            pic.commands = newCommands;
            return WrapHint(PicChangeHint::EditPicInvalid);
        }
            );

    }
}

void CPicView::_OnPasteCommands(HGLOBAL hMem)
{
    if (_currentTool != Pasting) // If we're already "pasting", we can't do it again.
    {
        // Remove any old ones
        _pastedCommands.clear();

        size_t cb = GlobalSize(hMem);
        GlobalLockGuard<BYTE*> globalLock(hMem);
        BYTE *pBits = globalLock.Object;
        if (pBits)
        {
            bool supportsPen = (_GetEditPic()->Traits->SupportsPenCommands);
            bool isVGA = (_GetEditPic()->Traits->IsVGA);
            bool canChangePriLines = (_GetEditPic()->Traits->CanChangePriorityLines);

            // Create a byte stream with this data.
            sci::ostream stream;
            stream.WriteBytes(pBits, (int)cb);
            sci::istream reader = istream_from_ostream(stream);
            bool fOk = true;
            while (fOk)
            {
                PicCommand command;
                fOk = command.Initialize(reader);
                if (fOk && _AllowCommand(supportsPen, isVGA, canChangePriLines, command))
                {
                    _pastedCommands.push_back(command);
                }
            }

            sRECT rcPasted;
            PastedCommands_GetBounds(_GetPicSize(), &_pastedCommands[0], _pastedCommands.size(), &rcPasted);
            _cxPasted = rcPasted.right - rcPasted.left;
            _cyPasted = rcPasted.bottom - rcPasted.top;
            _xPasteOffset = 0;
            _yPasteOffset = 0;
            _xPasted = rcPasted.left;
            _yPasted = rcPasted.top;
            _fHFlip = false;
            _fVFlip = false;
            _iAngle = 0;
            _cxPastedScale = _cxPasted;
            _cyPastedScale = _cyPasted;

            if (PastedCommands_ContainDrawCommands(&_pastedCommands[0], _pastedCommands.size()))
            {
                // Go into a mode where they can drag around the commands
                _currentTool = Pasting;
                _currentCommand = PicCommand::None;
                _UpdateCursor();
                _OnCommandChanged();
                _GetDrawManager().InvalidatePlugins();
                InvalidateOurselves();
            }
            else
            {
                // Otherwise, just paste right now.
                _InsertPastedCommands();
            }
        }
    }
}

void *_GetBitsPtrFromBITMAPINFO(const BITMAPINFO *pbmi)
{
    void *pBits = NULL;
    if (pbmi->bmiHeader.biCompression == BI_RGB)
    {
        // The bits start right after the BITMAPINFO, plus (maybe?) any colour table.
        pBits = ((BYTE*)pbmi + (uint16_t)(pbmi->bmiHeader.biSize) + pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD));
    }
    else if (pbmi->bmiHeader.biCompression == BI_BITFIELDS)
    {
        ASSERT((pbmi->bmiHeader.biBitCount == 16) || (pbmi->bmiHeader.biBitCount == 32));
        // In this case, there are 3 DWORDS before the bits actually start.
        pBits = ((BYTE*)pbmi + (uint16_t)(pbmi->bmiHeader.biSize) + (3 * sizeof(DWORD)));
    }
    return pBits;
}

void _ReplaceDrawVisual(PicComponent &pic, const Cel &cel)
{
    // Prep the command
    PicCommand newCommand;
    newCommand.CreateDrawVisualBitmap(cel, true);

    // Now find where the (first) old bitmap was and stick it in there.
    bool found = false;
    for (auto &command : pic.commands)
    {
        if (command.type == PicCommand::CommandType::DrawBitmap)
        {
            command = newCommand;
            found = true;
            break;
        }
    }

    if (!found)
    {
        // There was no old one. Just stick it in front then.
        pic.commands.insert(pic.commands.begin(), newCommand);
    }
}

void CPicView::OnPasteIntoPic()
{
    if (_GetEditPic()->Traits->IsVGA)
    {
        // Provide the current background, in case they wish to overlay.
        const Cel *backgroundCurrent = nullptr;
        for (const auto &command : _GetEditPic()->commands)
        {
            if (command.type == PicCommand::CommandType::DrawBitmap)
            {
                backgroundCurrent = command.drawVisualBitmap.pCel;
            }
        }

        bool defaultInsertAtCurrent = _GetEditPic()->Traits->AllowMultipleBitmaps;
        CBitmapToVGADialog dialog(nullptr, backgroundCurrent, _GetPalette(), false, 256, _GetEditPic()->Traits->AllowMultipleBitmaps, _GetEditPic()->Size, 255, PaletteAlgorithm::Quantize, DefaultPaletteUsage::UnusedColors, nullptr, defaultInsertAtCurrent);
        if (IDOK == dialog.DoModal())
        {
            std::unique_ptr<Cel> result = dialog.GetFinalResult();
            std::unique_ptr<PaletteComponent> paletteResult = dialog.GetFinalResultPalette();
            bool insertAtCurrent = dialog.GetInsertAtCurrentPosition();
            if (insertAtCurrent)
            {
                // We don't bother with the palette here. We just plop in a pic command.
                PicCommand newCommand;
                newCommand.CreateDrawVisualBitmap(*result, true);
                GetDocument()->InsertCommand(&newCommand);
            }
            else
            {
                // Time for some manipulation
                GetDocument()->ApplyChanges<PicComponent, PaletteComponent>(
                    [&](PicComponent &pic, PaletteComponent &palette)
                {
                    // Replace the palette with the new one.
                    palette = *paletteResult;

                    _ReplaceDrawVisual(pic, *result);

                    return WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::Palette);
                },
                    [&paletteResult](ResourceEntity &resource)
                {
                    // Ensure we actually have a palette.
                    if (!resource.TryGetComponent<PaletteComponent>())
                    {
                        resource.AddComponent<PaletteComponent>(std::make_unique<PaletteComponent>(*paletteResult));
                    }
                }
                );
            }

        }
    }
    else
    {
        CBitmapToPicDialog dialog;

        // Initialize the dialog with the current palette 0.
        const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
        dialog.SetPicPalette(&pstate->pPalettes[0]);

        if (IDOK == dialog.DoModal())
        {
            // Insert the commands in to the pic if successful.
            dialog.InsertIntoPic(GetDocument());
        }
    }
}

void CPicView::OnPaste()
{
    UINT uFormat;
    if (_HaveCompatibleClipboardFormat(this, &uFormat))
    {
        OpenClipboardGuard openClipboard(this);
        if (openClipboard.IsOpen())
        {
            HGLOBAL hMem = GetClipboardData(uFormat);
            // We don't need to GlobalFree this.
            if (hMem)
            {
                switch (uFormat)
                {
                case CF_DIB:
                    {
                        GlobalLockGuard<BITMAPINFO*> globalLock(hMem);
                        BITMAPINFO *pbmi = globalLock.Object;
                        if (pbmi)
                        {
                            void *pDIBBits32 = nullptr;;
                            CBitmap bitmap;
                            bitmap.Attach(BitmapToRGB32Bitmap(pbmi, 320, 190, &pDIBBits32));
                            globalLock.Unlock();    // Unlock and close ASAP, before we do the expensive quantization
                            openClipboard.Close();

                            void *pDIBBits = _GetBitsPtrFromBITMAPINFO(pbmi);
                            if (pDIBBits)
                            {
                                _MakeNewMasterTraceImage(NULL, pbmi, pDIBBits);
                            }
                        }
                    }
                    break;

                default:
                    {
                        _OnPasteCommands(hMem);
                        break;
                    }
                }
            }
        }
    }
}


void CPicView::OnUpdateSetVisual(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(TRUE);
}

void CPicView::OnCommandUpdatePastePic(CCmdUI *pCmdUI)
{
    UINT uFormat;
    pCmdUI->Enable((_currentTool != Pasting) && !appState->_fNoGdiPlus && _HaveCompatibleClipboardFormat(this, &uFormat));
}

void CPicView::OnUpdateIsGDIPAvailable(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!appState->_fNoGdiPlus);
}

void CPicView::InvalidateOurselves()
{
    _isDrawUpToDate = PicScreenFlags::None;
    Invalidate(FALSE);
}

void CPicView::InvalidateOurselvesImmediately()
{
    _isDrawUpToDate = PicScreenFlags::None;
    RedrawWindow();
}

LRESULT CPicView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    _fMouseWithin = FALSE;
    if (_fPreviewPen || _fDrawingLine || _fDrawingCircle)
    {
        InvalidateOurselves();
    }
    return 0;
}

void CPicView::OnUpdateShowTraceImage(CCmdUI *pCmdUI)
{
    // The show/hide trace image button is disabled if we don't have a trace image.
    ASSERT(pCmdUI->m_nID == ID_SHOWTRACEIMAGE);
    pCmdUI->SetCheck(_fShowTraceImage);
    pCmdUI->Enable(_fHaveTraceImage);
}

void CPicView::OnUpdateAlphaSlider(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(_fHaveTraceImage);
    g_pPicAlphaSlider->SetView(this);
}

void CPicView::SetOpacity(int iOpacity)
{
    ASSERT(iOpacity >= 0 && iOpacity <= 100);
    if (_iTraceAlpha != iOpacity)
    {
        _iTraceAlpha = iOpacity;
        InvalidateOurselves(); // Redraw
    }
}


//
// Update the coordinate display
//
void CPicView::OnCommandUIStatus(CCmdUI *pCmdUI)
{
    size16 picSize = _GetPicSize();
    TCHAR szText[50];
    szText[0] = 0;
    if (pCmdUI->m_nID == ID_INDICATOR_COORDS)
    {
        int x = min(_ptCurrentHover.x, picSize.cx - 1);
        int y = min(_ptCurrentHover.y, picSize.cy - 1);
        pCmdUI->Enable(); 
        point16 screenPosition = point16(x, y);
        point16 position = ScreenResolutionToGameResolution(screenPosition);
        if (screenPosition != position)
        {
            StringCchPrintf(szText, ARRAYSIZE(szText), "%3d,%3d (%3d, %3d)", position.x, position.y, screenPosition.x, screenPosition.y);
        }
        else
        {
            StringCchPrintf(szText, ARRAYSIZE(szText), "%3d,%3d", screenPosition.x, screenPosition.y);
        }
        pCmdUI->SetText(szText);
    }
    else if (pCmdUI->m_nID == ID_INDICATOR_PRI)
    {
        const PicComponent *pic = _GetEditPic();
        if (pic && pic->Traits->ContinuousPriority)
        {
            pCmdUI->SetText("");
        }
        else
        {
            int y = min(_ptCurrentHover.y, picSize.cy - 1);
            pCmdUI->Enable();
            const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
            StringCchPrintf(szText, ARRAYSIZE(szText), "Pri bar: %2d", PriorityFromY((uint16_t)y, *pstate));
            pCmdUI->SetText(szText);
        }
    }
    else if ((_ptCurrentHover.x >= 0) && (_ptCurrentHover.y >= 0) && (_ptCurrentHover.x < picSize.cx) && (_ptCurrentHover.y < picSize.cy))
    {
        const uint8_t *picBits = _GetDrawManager().GetPicBits(_mainViewScreen, PicPosition::PrePlugin, _GetPicSize());
        size16 displaySize = _GetPicSize();
        uint8_t bColor = *(picBits + BUFFEROFFSET_NONSTD(displaySize.cx, displaySize.cy, _ptCurrentHover.x, _ptCurrentHover.y));
        if ((pCmdUI->m_nID == ID_INDICATOR_CONTROLCOLOR) && (_mainViewScreen == PicScreen::Control))
        {
            StringCchCopy(szText, ARRAYSIZE(szText), c_rgControlColourNames[bColor % 16]);
        }
        else if ((pCmdUI->m_nID == ID_INDICATOR_PRICOLOR) && (_mainViewScreen == PicScreen::Priority))
        {
            const PicComponent *pic = _GetEditPic();
            if (pic && pic->Traits->ContinuousPriority)
            {
                StringCchPrintf(szText, ARRAYSIZE(szText), "Priority: %3d", ColorIndexToContinuousPriorityValue(bColor));
            }
            else
            {
                StringCchPrintf(szText, ARRAYSIZE(szText), "Priority: %3d", bColor);
            }
        }
        else if ((pCmdUI->m_nID == ID_INDICATOR_PIXELCOLOR) && (_mainViewScreen == PicScreen::Visual))
        {
            const PaletteComponent *palette = _GetPalette();
            if (palette)
            {
                RGBQUAD rgb = palette->Colors[bColor];
                StringCchPrintf(szText, ARRAYSIZE(szText), TEXT("Color: %03d (R=%d, G=%d, B=%d)"), bColor, rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
            }
            else
            {
                StringCchPrintf(szText, ARRAYSIZE(szText), "Color: %3d", bColor);
            }
        }
    }

    pCmdUI->SetText(szText);
}

void CPicView::OnUpdateShowScreenControl(CCmdUI *pCmdUI)
{
    BOOL fChecked = FALSE;
    switch (pCmdUI->m_nID)
    {
    case ID_SHOWVISUALSCREEN:
        fChecked = (_mainViewScreen == PicScreen::Visual);
        break;
    case ID_SHOWPRIORITYSCREEN:
        fChecked = (_mainViewScreen == PicScreen::Priority);
        break;
    case ID_SHOWCONTROLSCREEN:
        fChecked = (_mainViewScreen == PicScreen::Control);
        break;
    }

    pCmdUI->SetCheck(fChecked ? 1 : 0);

    const PicComponent *pic = _GetEditPic();
    if ((pCmdUI->m_nID == ID_SHOWCONTROLSCREEN) && pic && !pic->Traits->SupportsControlScreen)
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        // Otherwise always enabled
        pCmdUI->Enable();
    }
}

void CPicView::OnUpdateShowPaletteControl(CCmdUI *pCmdUI)
{
    if (_GetEditPic()->Traits->IsVGA)
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable();
        BYTE bPaletteButton = 0;
        switch (pCmdUI->m_nID)
        {
        case ID_SHOWPALETTE0:
            bPaletteButton = 0;
            break;
        case ID_SHOWPALETTE1:
            bPaletteButton = 1;
            break;
        case ID_SHOWPALETTE2:
            bPaletteButton = 2;
            break;
        case ID_SHOWPALETTE3:
            bPaletteButton = 3;
            break;
        default:
            assert(false);
        }

        pCmdUI->SetCheck((_GetDrawManager().GetPalette() == bPaletteButton) ? 1 : 0);
    }
}

void CPicView::OnUpdateTogglePriorityLines(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(_fShowPriorityLines);
    pCmdUI->Enable(_GetEditPic() && !_GetEditPic()->Traits->ContinuousPriority);
}

void CPicView::OnUpdateToggleGridLines(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(_fGridLines);
    pCmdUI->Enable();
}

void CPicView::OnUpdateToggleEgo(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(_fShowingEgo);
    pCmdUI->Enable();
}

void CPicView::OnUpdateLightUpCoords(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(_transformingCoords);
    pCmdUI->Enable(_GetEditPic() && _GetEditPic()->Traits->SupportsVectorCommands);
}


void CPicView::OnUpdateToggleScreen(CCmdUI *pCmdUI)
{
    const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
    PicScreenFlags dwDrawEnableMask = PicScreenFlags::None;
    switch (pCmdUI->m_nID)
    {
    case IDC_TOGGLEVISUAL:
        dwDrawEnableMask |= PicScreenFlags::Visual;        
        break;

    case IDC_TOGGLEPRIORITY:
        dwDrawEnableMask |= PicScreenFlags::Priority;
        break;

    case IDC_TOGGLECONTROL:
        dwDrawEnableMask |= PicScreenFlags::Control;
        break;
    }

    pCmdUI->SetCheck(IsFlagSet(dwDrawEnableMask, pstate->dwDrawEnable) ? 1 : 0);
    pCmdUI->Enable();
}

const key_value_pair<PicCommand::CommandType, UINT> c_commandToID[] =
{
    { PicCommand::Pattern, ID_PENTOOL },
    { PicCommand::Line, ID_LINE },
    { PicCommand::Fill, ID_FILL },
    { PicCommand::Circle, ID_CIRCLE },
    // Add more here...
};
PicCommand::CommandType _IDToCommand(UINT nID)
{
    return LookupKey(c_commandToID, ARRAYSIZE(c_commandToID), nID, PicCommand::None);
}



CPicView::ToolType CPicView::_IDToTool(UINT nID)
{
    return LookupKey(c_toolToID, ARRAYSIZE(c_toolToID), nID, None);
}


const key_value_pair<UINT, int> c_IDToCursor [] =
{
    { ID_PENTOOL, IDC_CURSORPEN },
    { ID_HISTORY, IDC_CURSORHISTORY },
    { ID_FILL, IDC_CURSORFILL },
    { ID_CIRCLE, IDC_CURSORCIRCLE },
    { ID_LINE, IDC_CURSORLINE },
    { ID_ZOOM, IDC_CURSORZOOM },
    { ID_POLYPATH, IDC_CURSORPOLYGON },
};
int _IDToCursor(UINT nID)
{
    return LookupKeyValue(c_IDToCursor, ARRAYSIZE(c_IDToCursor), nID, -1);
}

int CalcAngle(CPoint pt1, CPoint pt2)
{
    if (pt2.y == pt1.y) // avoid / 0
    {
        if (pt2.x > pt2.y)
        {
            return 270;
        }
        else
        {
            return 90;
        }
    }
    else
    {
        int i = (int)(360 * atan(((double)(pt2.x - pt1.x)) / (double)(pt2.y - pt1.y)) / 6.28);
        if (pt2.y > pt1.y)
        {
            i += 180;
        }
        return i;
    }
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void CPicView::_SnapCoordinate(CPoint &pt)
{
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        int dx = abs(_xOld - pt.x);
        int dy = abs(_yOld - pt.y);
        if (_fDrawingLine)
        {
            // Snap along axes
            if (dx > dy)
            {
                // Snap along y
                pt.y = _yOld;
            }
            else
            {
                pt.x = _xOld;
            }
        }
        else if (_fDrawingCircle)
        {
            int d = min(dx, dy);
            pt.x = _xOld + sgn(dx) * d;
            pt.y = _yOld + sgn(dy) * appState->InverseAspectRatioY(d);
        }
    }
}

void CPicView::OnMouseMove(UINT nFlags, CPoint point)
{
    // Adjust to pic coords.
    _ptCurrentHover = _MapClientPointToPic(point);
    _SnapCoordinate(_ptCurrentHover);

    bool needsImmediateUpdate = false;

    if (_fDrawingLine || _fPreviewPen || _fDrawingCircle)
    {
        _GetDrawManager().InvalidatePlugins();
    }

    if (_fDrawingLine || _fPreviewPen || _transformingCoords || _fDrawingCircle)
    {
        // FEATURE: we could be more selective with this too
        // Just invalidate the region that covers our from-to
        // CPU spikes to 100% when moving around here.
        needsImmediateUpdate = true;
    }

    if ((_currentTool == Command) && _fGridLines)
    {
        // Invalidate on every mouse move if we're showing gridlines.
        _GetDrawManager().InvalidatePlugins();
        needsImmediateUpdate = true;
    }

    bool fUpdateCursor = false;

    if (_fCapturing)
    {
        int dx = _ptCurrentHover.x - _pointCapture.x;
        int dy = _ptCurrentHover.y - _pointCapture.y;

        if (_priBarMoveIndex != -1)
        {
            _MovePriorityBar(false, dy);
        }
        else if (_currentTool == Pasting)
        {
            if (_fRotating)
            {
                // Rotate
                // Figure out the angle for the original (pointCapture to center rect)
                CRect rc(_xPasted, _yPasted, _xPasted + _cxPasted, _yPasted + _cyPasted);
                rc.OffsetRect(_xPasteOffset, _yPasteOffset);
                int iAngleOrig = CalcAngle(rc.CenterPoint(), _pointCapture);
                int iAngleNow = CalcAngle(rc.CenterPoint(), _ptCurrentHover);
                _iAngle = _iAngleCapture + iAngleOrig - iAngleNow;
            }
            else
            {
                // Adjust our delta
                _xPasteOffset = dx + _xPasteOffsetFromCapture;
                _yPasteOffset = dy + _yPasteOffsetFromCapture;
            }
            needsImmediateUpdate = true;
            _GetDrawManager().InvalidatePlugins();
        }
        else if (_currentTool == Polygons)
        {
            assert(_polyDragPointIndex != -1);
            _currentDragPolyPoint = _startDragPolyPoint;
            _currentDragPolyPoint.x += dx;
            _currentDragPolyPoint.y += dy;
            _ClampPoint(_currentDragPolyPoint);
            _currentDragPolyPoint = ScreenResolutionToGameResolution(_currentDragPolyPoint);
            needsImmediateUpdate = true;
        }
        else if (_transformingCoords && (_currentTool != None))
        {
            _TransformPoint(false, _ptCurrentHover);
            _GetDrawManager().Invalidate();
            GetDocument()->ExplicitNotify(PicChangeHint::EditPicInvalid);
            needsImmediateUpdate = true;
        }
        else if (_fShowingEgo)
        {
            // Update the ego position, if the mouse button is down.  But only if we're not "pasting"
            // And only if the ego can be at this position
            CPoint ptNew = _pointEgoOrig + CPoint(dx, dy);
            bool fCanBeHere = _EvaluateCanBeHere(ptNew);
            if (fCanBeHere)
            {
                _fCanBeHere = fCanBeHere;
                _fakeEgoAttributes.Location = ptNew;
                appState->_ptFakeEgo = _fakeEgoAttributes.Location;
                InvalidateOurselves();
            }
            else if (fCanBeHere != _fCanBeHere)
            {
                _fCanBeHere = fCanBeHere;
                InvalidateOurselves();
            }
            needsImmediateUpdate = true;
            _GetDrawManager().InvalidatePlugins();
        }
    }
    else
    {
        if (_currentTool == Polygons)
        {
            _OnPolyMouseMove(ScreenResolutionToGameResolution(_ptCurrentHover));
        }

        if (_fShowPriorityLines)
        {
            _HitTestPriorityBar(_ptCurrentHover, &_priBarMoveIndex);
            fUpdateCursor = true;
        }

        if ((_currentTool == Pasting) || (_fShowingEgo))
        {
            fUpdateCursor = true;
            // If we're not capturing, but hovering, and we're pasting or showing the ego
            // then change the cursor when the user is over the paste box (or over the ego)
            BOOL fInPasteRect;
            if (_currentTool == Pasting)
            {
                CRect rectPasted;
                _GetPasteRect(rectPasted);
                fInPasteRect = PtInRect(&rectPasted, CPoint(_ptCurrentHover.x, _ptCurrentHover.y));
            }
            else
            {
                assert(_fShowingEgo);
                fInPasteRect = _HitTestFakeEgo(_ptCurrentHover);
            }
            
            if (fInPasteRect)
            {
                if (!_fInPasteRect)
                {
                    _fInPasteRect = true;
                    if (_fShowingEgo)
                    {
                        // Since we show coordinates when hovered.
                        InvalidateOurselves();
                    }
                }
            }
            else
            {
                if (_fInPasteRect)
                {
                    _fInPasteRect = false;
                    if (_fShowingEgo)
                    {
                        // Since we show coordinates when hovered.
                        InvalidateOurselves();
                    }
                }
            }
        }
    }

    if (!_fMouseWithin)
    {
        // Track mouseleaves.
        TRACKMOUSEEVENT eventTrack;
        eventTrack.cbSize = sizeof(eventTrack);
        eventTrack.dwFlags = TME_LEAVE;
        eventTrack.hwndTrack = m_hWnd;
        _fMouseWithin = TrackMouseEvent(&eventTrack);
        fUpdateCursor = true;
    }

    GetDocument()->InformBitmapEditor(PicChangeHint::CursorPosition, this);

    if (needsImmediateUpdate)
    {
        InvalidateOurselvesImmediately();
    }

    if (fUpdateCursor)
    {
        _UpdateCursor();
    }
}

void CPicView::OnUpdateAllPicCommands(CCmdUI *pCmdUI)
{
    BOOL enabled = true;
    if (pCmdUI->m_nID == ID_PENTOOL && _GetEditPic())
    {
        enabled = _GetEditPic()->Traits->SupportsPenCommands;
    }
    if (pCmdUI->m_nID == ID_POLYPATH && _GetEditPic())
    {
        enabled = _GetEditPic()->Traits->IsVGA;
    }

    if (!_GetEditPic()->Traits->SupportsVectorCommands)
    {
        switch (pCmdUI->m_nID)
        {
            case ID_PENTOOL:
            case ID_LINE:
            case ID_FILL:
            case ID_CIRCLE:
                enabled = false;
                break;
        }
    }
        
    pCmdUI->Enable(enabled);

    // Which one is checked though?
    PicCommand::CommandType command = _IDToCommand(pCmdUI->m_nID);
    ToolType tool = _IDToTool(pCmdUI->m_nID);
    int iCheck = (tool == _currentTool);
    if (iCheck && (tool == Command))
    {
        // Even though the tools are equal, the commands might not.
        iCheck = (command == _currentCommand);
    }

    pCmdUI->SetCheck(iCheck);
}

void CPicView::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    CRect rect;
    GetClientRect(&rect);
    // Set our initial zoom level to the biggest we can fit (or at least 2)
    // NOTE: We do NOT yet have a pic resource here, so we'll be using the defaults for cx/cy
    int iZoom = min(rect.Width() / _GetPicSize().cx, rect.Height() / _GetPicSize().cy);
    iZoom = max(iZoom, 2);
    _cyPic = iZoom * _GetPicSize().cy;
    _cxPic = iZoom * _GetPicSize().cx;
    GetDocument()->SetZoom(iZoom);
    GetDocument()->InformBitmapEditor(PicChangeHint::CursorPosition, this);
}

int CPicView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Remove any cursor associated with this window class, so we can change the cursor as we
    // change tools
    SetClassLongPtr(m_hWnd, GCL_HCURSOR, NULL);

    return 0;
}

BOOL CPicView::PreCreateWindow(CREATESTRUCT& cs)
{
    return __super::PreCreateWindow(cs);
}

// CPicView drawing

void CPicView::_EnsureDoubleBuffer()
{
    size16 size = _GetPicSize();
    CBitmap &bitmap = _bitmapDoubleBuf;
    if (((HBITMAP)bitmap == nullptr) || (bitmap.GetBitmapDimension().cx != size.cx) || (bitmap.GetBitmapDimension().cy != size.cy))
    {
        bitmap.DeleteObject();
        CDC *pDC = this->GetDC();
        bitmap.CreateCompatibleBitmap(pDC, _GetPicSize().cx, _GetPicSize().cy);
        bitmap.SetBitmapDimension(_GetPicSize().cx, _GetPicSize().cy);
        this->ReleaseDC(pDC);
    }
}

void CPicView::_GenerateTraceImage(CDC *pDC)
{
    _bitmapTrace.DeleteObject();
    
    // Take our master copy, and generate a 320x190 version of it.
    if (_pgdiplusTrace)
    {
        _bitmapTrace.CreateCompatibleBitmap(pDC, _GetPicSize().cx, _GetPicSize().cy);

        // Now copy the trace image into it.
        CDC dcDest;
        if (dcDest.CreateCompatibleDC(pDC))
        {
            HGDIOBJ hgdiObj = dcDest.SelectObject(_bitmapTrace);
            Graphics graphics((HDC)dcDest);
            if (appState->_fScaleTracingImages)
            {
                graphics.DrawImage(_pgdiplusTrace.get(), 0, 0, _GetPicSize().cx, _GetPicSize().cy);
            }
            else
            {
                int cx = min(_GetPicSize().cx, _pgdiplusTrace->GetWidth());
                int cy = min(_GetPicSize().cy, _pgdiplusTrace->GetHeight());
                graphics.DrawImage(_pgdiplusTrace.get(), 0, 0, 0, 0, cx, cy, Gdiplus::UnitPixel);
            }
            dcDest.SelectObject(hgdiObj);

            _fRegenerateTrace = false;
        }
    }
}

void CPicView::_DrawEgoCoordinates(CDC *pDC)
{
    if ((appState->_fUseBoxEgo && (_fakeEgoAttributes.Pri != -1)) || (!appState->_fUseBoxEgo && _GetFakeEgo()))
    {
        // The cursor is over the ego.  Draw the coordinates.
        // First we need to convert the ego coordinates to "client" coordinates
        CPoint ptEgoClient = _MapPicPointToClient(_fakeEgoAttributes.Location);
        TCHAR szCoords[20];
        if (appState->_fUseBoxEgo)
        {
            assert(_fakeEgoAttributes.Pri != -1);
            // The only time we draw anything for "box" egos is if the user set a pri.
            StringCchPrintf(szCoords, ARRAYSIZE(szCoords), TEXT("pri %d"), _fakeEgoAttributes.Pri);
        }
        else
        {
            if (_fakeEgoAttributes.Pri == -1)
            {
                StringCchPrintf(szCoords, ARRAYSIZE(szCoords), TEXT("(%d,%d)"), _fakeEgoAttributes.Location.x, _fakeEgoAttributes.Location.y);
            }
            else
            {
                StringCchPrintf(szCoords, ARRAYSIZE(szCoords), TEXT("(%d,%d) - pri %d"), _fakeEgoAttributes.Location.x, _fakeEgoAttributes.Location.y, _fakeEgoAttributes.Pri);
            }
        }
        CRect rectTextSize;
        int cyText = pDC->DrawText(szCoords, &rectTextSize, DT_SINGLELINE | DT_CALCRECT);

        // Center the text:
        CPoint ptEgoCenter = _MapPicPointToClient(_FindCenterOfFakeEgo());
        int cxHalf = rectTextSize.Width() / 2;
        cyText /= 2;
        CRect rectFinal(ptEgoCenter.x - cxHalf, ptEgoCenter.y - cyText, ptEgoCenter.x + cxHalf, ptEgoCenter.y + cyText);

        // darken the area under the letters so we can see them better.
        HBITMAP hbmOverlay = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_ALPHABLEND));
        if (hbmOverlay)
        {
            CDC dcMem;
            if (dcMem.CreateCompatibleDC(pDC))
            {
                HGDIOBJ hOld = dcMem.SelectObject(hbmOverlay);
                BLENDFUNCTION blend = { 0 };
                blend.AlphaFormat = 0;
                blend.SourceConstantAlpha = 128;
                blend.BlendFlags = 0;
                blend.BlendOp = AC_SRC_OVER;
                pDC->AlphaBlend(rectFinal.left, rectFinal.top, rectFinal.Width(), rectFinal.Height(),
                    &dcMem, 0, 0, 32, 4, blend);
                dcMem.SelectObject(hOld);
            }
            DeleteObject(hbmOverlay);
        }

        // Draw it
        int iOldMode = pDC->SetBkMode(TRANSPARENT);
        COLORREF crOld = pDC->SetTextColor(RGB(0, 0, 0));
        pDC->DrawText(szCoords, &rectFinal, DT_SINGLELINE);
        // Offset 1x1, and draw in white (or red, if we can't be here)
        rectFinal.OffsetRect(-1, -1);
        pDC->SetTextColor(_fCanBeHere ? RGB(255, 255, 255) : RGB(250, 0, 0));
        pDC->DrawText(szCoords, &rectFinal, DT_SINGLELINE);

        // Restore stuff
        pDC->SetTextColor(crOld);
        pDC->SetBkMode(iOldMode);
    }
}

void AdjustPolyPointsForScreen(std::vector<POINT> &points)
{
    NativeResolution resolution = appState->GetVersion().DefaultResolution;
    for (auto &point : points)
    {
        point = GameResolutionToScreenResolution(point, resolution);
    }
}

void CPicView::_DrawPolygon(CDC *pDC, const SCIPolygon *polygon, bool isActive)
{
    COLORREF colorPoly = PolygonColors[(int)polygon->Type];
    COLORREF colorPolyAlt = PolygonColorsAlt[(int)polygon->Type];
    if (!isActive)
    {
        colorPoly = CExtBitmap::stat_HLS_Adjust(colorPoly, 0.0, -0.5, -0.8);
    }

    int penStyle = appState->_fShowPolyDotted ? PS_DOT : PS_SOLID;

    CPen penPoly(penStyle, 1, colorPoly);
    HGDIOBJ hOldPen = pDC->SelectObject(penPoly);
    int oldBkMode = pDC->SetBkMode(OPAQUE);
    COLORREF oldBkColor = pDC->SetBkColor(colorPolyAlt);

    std::vector<POINT> points;
    int index = 0;
    for (point16 point : polygon->Points())
    {
        if (isActive && (_polyDragPointIndex == index))
        {
            points.push_back(PointToCPoint(_currentDragPolyPoint));
        }
        else
        {
            points.push_back({ point.x, point.y });
        }
        index++;
    }

    if (!isActive || !_currentPolyInEdit)
    {
        // Seal the loop for complete polygons.
        points.push_back({ points[0].x, points[0].y });
    }
    else
    {
        // For currently edited polygon:
        points.push_back({ _nextPolyPoint.x, _nextPolyPoint.y });
    }

    AdjustPolyPointsForScreen(points);
    pDC->Polyline(&points[0], (int)points.size());

    if (isActive)
    {
        // Now the highlight point
        points.clear();
        if ((_currentHoverPolyPointIndex == -1) && (_currentHoverPolyEdgeIndex != -1) && (_currentHoverPolyEdgeIndex < (int)polygon->Points().size()))
        {
            CPen penEdge(penStyle, 1, ColorPolyHighlight);
            pDC->SelectObject(penEdge);
            point16 a = polygon->Points()[_currentHoverPolyEdgeIndex];
            point16 b = polygon->Points()[(_currentHoverPolyEdgeIndex + 1) % polygon->Points().size()];
            points.push_back(PointToCPoint(a));
            points.push_back(PointToCPoint(b));
            AdjustPolyPointsForScreen(points);
            pDC->Polyline(&points[0], (int)points.size());
        }

        if ((_currentHoverPolyPointIndex != -1) && (_currentHoverPolyPointIndex < (int)polygon->Points().size()))
        {
            CBrush brush(RGB(222, 222, 222));   // Mitigation: Pure white doesn't show up against white backgrounds, which is the default pic background.
            HGDIOBJ hOldBrush = pDC->SelectObject(brush);
            point16 hoverPoint = (_currentHoverPolyPointIndex == _polyDragPointIndex) ? _currentDragPolyPoint : polygon->Points()[_currentHoverPolyPointIndex];
            hoverPoint = GameResolutionToScreenResolution(hoverPoint, appState->GetVersion().DefaultResolution);
            CRect rectEllipse(hoverPoint.x, hoverPoint.y, hoverPoint.x, hoverPoint.y);
            rectEllipse.InflateRect(3, 3);
            pDC->Ellipse(&rectEllipse);
            pDC->SelectObject(hOldBrush);
        }
    }
    pDC->SelectObject(hOldPen);
    pDC->SetBkMode(oldBkMode);
    pDC->SetBkColor(oldBkColor);
}

void CPicView::_DrawPolygons(CDC *pDC)
{
    CPicDoc *pDoc = GetDocument();
    if (pDoc)
    {
        if (pDoc->GetShowPolygons())
        {
            const PolygonComponent *polygonComponent = pDoc->GetPolygonComponent();
            if (polygonComponent)
            {
                for (size_t i = 0; i < polygonComponent->Polygons().size(); i++)
                {
                    _DrawPolygon(pDC, &polygonComponent->Polygons()[i], ((int)i == pDoc->GetCurrentPolygonIndex()));
                }
            }
        }

        if (_currentPolyInEdit)
        {
            _DrawPolygon(pDC, _currentPolyInEdit.get(), true);
        }
    }
}

void CPicView::_DrawPriorityLines(CDC *pDC)
{
    if (!(HBITMAP)_bitmapPriLines)
    {
        // Create bitmap with 15 priority lines.
        // We create a bitmap for this so that we can alpha blend (GDI only supports
        // alpha blending from bitmaps, not brushes)
        _bitmapPriLines.CreateCompatibleBitmap(pDC, 1, NumPriorityBars + 1);
        CDC dcDest;
        if (dcDest.CreateCompatibleDC(pDC))
        {
            HGDIOBJ hgdiObj = dcDest.SelectObject(_bitmapPriLines);
            for (int i = 0; i <= NumPriorityBars; i++)
            {
                CBrush brush(g_egaColorsCR[i]);
                CRect rect(CPoint(0, i), CSize(1, 1));
                dcDest.FillRect(&rect, &brush);
            }
            dcDest.SelectObject(hgdiObj);
        }
    }

    // Blend the priority bars over the screen
    BLENDFUNCTION blendFunc;
    blendFunc.BlendOp = AC_SRC_OVER;
    blendFunc.BlendFlags = 0;
    blendFunc.SourceConstantAlpha = 192; // 0 to 255
    blendFunc.AlphaFormat = 0;//AC_SRC_ALPHA;

    const uint16_t *priLines = _GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines;
    size_t priLineCount = ARRAYSIZE(_GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines);

    CDC dcMem2;
    if (dcMem2.CreateCompatibleDC(pDC))
    {
        HGDIOBJ hgdiObj2 = dcMem2.SelectObject(_bitmapPriLines);
        for (int i = 0; i < NumPriorityBars; i++)
        {
            int yPriLine = priLines[i] - 1;
            pDC->AlphaBlend(0, yPriLine, _GetPicSize().cx, 2, &dcMem2, 0, i, 1, 2, blendFunc);
        }
        dcMem2.SelectObject(hgdiObj2);
    }
}

PicCommand CPicView::_EnsurePriorityBarCommand()
{
    assert(_GetEditPic()->Traits->CanChangePriorityLines);
    for (const PicCommand &command : _GetEditPic()->commands)
    {
        if (command.type == PicCommand::CommandType::SetPriorityBars)
        {
            return command;
        }
    }

    // Insert one at the beginning
    GetDocument()->ApplyChanges<PicComponent>(
        [](PicComponent &pic)
    {
        PicCommand setPriBarsCommand;
        setPriBarsCommand.CreateSetPriorityBars(g_defaultPriBands16Bit, pic.Traits->SixteenBitPri, pic.Traits->IsVGA);
        pic.commands.insert(pic.commands.begin(), setPriBarsCommand);
        return WrapHint(PicChangeHint::EditPicInvalid);
    }
        );

    return _GetEditPic()->commands[0];
}

void CPicView::_MovePriorityBar(bool commit, int dy)
{
    int newPriValue = _originalPriValue + dy;
    const uint16_t *priLines = _originalPriValueCommand.setPriorityBars.pPriorityLines;
    int priLineCount = (int)NumPriorityBars;
    // For now, don't allow dragging onto another priority line
    // Not sure if SCI technically allows overlapping pri lines, but I'm pretty sure some pics have them.
    int minBound = (_priBarMoveIndex > 0) ? priLines[_priBarMoveIndex - 1] : 0;
    int maxBound = (_priBarMoveIndex < (priLineCount - 1)) ? priLines[_priBarMoveIndex + 1] : (_GetPicSize().cy - 1);
    newPriValue = max(minBound, min(maxBound, newPriValue));

    PicCommand newPriBarsCommand = _originalPriValueCommand;
    newPriBarsCommand.setPriorityBars.pPriorityLines[_priBarMoveIndex] = newPriValue;

    // Find the pri bar command
    // HACK: We're modifying the pic commands directly.
    vector<PicCommand> &commands = const_cast<vector<PicCommand>&>(GetDocument()->GetPic()->commands);
    size_t i = 0;
    for (i = 0; i < commands.size(); i++)
    {
        if (commands[i].type == PicCommand::CommandType::SetPriorityBars)
        {
            break;
        }
    }
    assert(i < commands.size());

    if (commit)
    {
        // Restore the original before we commit the change to the clone of the resource
        commands[i] = _originalPriValueCommand;

        GetDocument()->ApplyChanges<PicComponent>(
            [&newPriBarsCommand, i](PicComponent &pic)
        {
            pic.commands[i] = newPriBarsCommand;
            return WrapHint(PicChangeHint::EditPicInvalid);
        }
            );
    }
    else
    {
        commands[i] = newPriBarsCommand;
    }

    // Invalidate everything - we need to redraw from scratch.
    _GetDrawManager().Invalidate();
    InvalidateOurselvesImmediately();
}

bool CPicView::_HitTestPriorityBar(CPoint pt, int *barIndex)
{
    *barIndex = -1;
    CRect rect(_GetPicSize().cx, 0, _GetPicSize().cx + PriorityBarWidth, _GetPicSize().cy);
    bool hit = !!rect.PtInRect(pt);
    if (hit && _GetEditPic()->Traits->CanChangePriorityLines)
    {
        const uint16_t *priLines = _GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines;
        size_t priLineCount = ARRAYSIZE(_GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines);
        // Hit on current line and previous
        for (size_t i = 0; i < priLineCount; i++)
        {
            int priLine = (i < priLineCount) ? priLines[i] : _GetPicSize().cy;
            if ((pt.y >= (priLine - 1)) && (pt.y <= priLine))
            {
                *barIndex = (int)i;
                break;
            }
        }
    }
    return hit;
}

void CPicView::_DrawPriorityBar(CDC *pDC, CPoint offset)
{
    const uint16_t *priLines = _GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines;
    size_t priLineCount = ARRAYSIZE(_GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines);
    int zoom = _cxPic / _GetPicSize().cx;

    size_t i = 0;
    int previousPriorityY = -1;
    for (; i <= priLineCount; i++, previousPriorityY++)
    {
        int priLine = (i < priLineCount) ? priLines[i] : _GetPicSize().cy;
        priLine--;
        CBrush brush(g_egaColorsCR[i]);
        CRect rect(CPoint(_cxPic - _xOrigin + offset.x, appState->AspectRatioY(previousPriorityY * zoom) - _yOrigin + offset.y), CSize(PriorityBarWidth, appState->AspectRatioY(zoom * (priLine - previousPriorityY + 1))));
        pDC->FillRect(&rect, &brush);
        previousPriorityY = priLine;
    }
}

CPoint CPicView::GetCursorPos()
{
    return _ptCurrentHover;
}

CSize CPicView::GetBitmapSize()
{
    size16 size = _GetPicSize();
    return CSize(size.cx, size.cy);
}

void CPicView::SetPosition(ptrdiff_t position)
{
    GetDocument()->SeekToPos(position);
    _GetDrawManager().Invalidate();
    CPicDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PreviewPalette));
    }
}

void CPicView::OnVGAPaletteChanged()
{
    CPicDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PreviewPalette));
    }
}

bool CPicView::EnsureBitmapUpToDate(CDC *pDCMem, PicScreen screen, bool force)
{
    _EnsureDoubleBuffer();

    CBitmap &bitmap = _bitmapDoubleBuf;
    PicScreenFlags dwDrawEnable = PicScreenToFlags(screen);
    if ((!IsFlagSet(_isDrawUpToDate, dwDrawEnable) || force) && (HBITMAP)bitmap)
    {
        HGDIOBJ hgdiObj = pDCMem->SelectObject(bitmap);
        _OnDraw(pDCMem, screen);
        pDCMem->SelectObject(hgdiObj);
        _isDrawUpToDate |= dwDrawEnable;
        return true;
    }
    return false;
}

void CPicView::_AttachPicPlugin()
{
    if (!_attachedPlugin)
    {
        _GetDrawManager().AddPicPlugin(this);
        _attachedPlugin = true;
    }
}

template<typename _Function>
void CPicView::_EnsurateCoordinates(PicCommand::CommandType commandType, _Function func)
{
    if (_currentTool == Command)
    {
        const PicComponent *pic = _GetEditPic();
        ptrdiff_t pos = GetDocument()->GetDrawManager().GetPos();
        if (pos == -1)
        {
            pos = pic->commands.size() - 1;
        }
        if (pic)
        {
            pos = min(pos, (ptrdiff_t)(pic->commands.size() - 1));
            while (pos > 0)
            {
                const PicCommand &command = pic->commands[pos];
                if (command.type == commandType)
                {
                    bool goOn = false;
                    if (command.type == PicCommand::CommandType::Line)
                    {
                        // We'll just enumerate line points twice. It's easiest, even though it's a little inefficient.
                        goOn = func(pos, command.drawLine.xFrom, command.drawLine.yFrom, false) &&
                            func(pos + 1, command.drawLine.xTo, command.drawLine.yTo, true);
                    }
                    else if(command.type == PicCommand::CommandType::Circle)
                    {
                        goOn = func(pos, command.circle.xFrom, command.circle.yFrom, true) &&
                            func(pos, command.circle.xTo, command.circle.yTo, false);
                    }
                    else
                    {
                        goOn = func(pos, command.fill.x, command.fill.y, false);
                    }
                      
                    if (!goOn)
                    {
                        break;
                    }
                }
                pos--;
            }
        }
    }
}

const int LightUpLinesDistanceSq = 900;
const int HitTestDistanceSqThreshold = 16;

bool CPicView::_IsCoordCloseEnough(int x, int y)
{
    int dx = _ptCurrentHover.x - x;
    int dy = _ptCurrentHover.y - y;
    return (dx * dx + dy * dy) < LightUpLinesDistanceSq;
}

int CPicView::_GetTransformHitTestDistance(int x, int y)
{
    int dx = _ptCurrentHover.x - x;
    int dy = _ptCurrentHover.y - y;
    return (dx * dx + dy * dy);
}

CPoint CPicView::_GetGutterOffset()
{
    int zoom = _cxPic / _GetPicSize().cx;
    return CPoint(zoom * PicGutter, zoom * PicGutter);
}

void CPicView::OnDraw(CDC *pDC)
{
    RECT rcClient;
    GetClientRect(&rcClient);

    _AttachPicPlugin();

    CPoint gutter = _GetGutterOffset();

    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        // Draw the picture. All the stuff drawn in here must use the same palette as the SCI pic:
        bool needToDrawStuffOnTop = EnsureBitmapUpToDate(&dcMem, _mainViewScreen, false);
        HGDIOBJ hgdiObj = dcMem.SelectObject(_bitmapDoubleBuf);

        if (needToDrawStuffOnTop)
        {
            if (_fShowTraceImage && _fHaveTraceImage && _fRegenerateTrace)
            {
                // Blit this is one time - it will be there from then on.
                _GenerateTraceImage(pDC);
            }

            if (_fShowTraceImage)
            {
                CPicDoc *pDoc = GetDocument();

                BLENDFUNCTION blendFunc;
                blendFunc.BlendOp = AC_SRC_OVER;
                blendFunc.BlendFlags = 0;
                blendFunc.SourceConstantAlpha = (BYTE)(_iTraceAlpha * 255 / 100); // 0 to 255
                blendFunc.AlphaFormat = 0;//AC_SRC_ALPHA;

                CDC dcMem2;
                if (dcMem2.CreateCompatibleDC(pDC))
                {
                    HGDIOBJ hgdiObj2 = dcMem2.SelectObject(_bitmapTrace);
                    // Draw this.
                    dcMem.AlphaBlend(0, 0, _GetPicSize().cx, _GetPicSize().cy, &dcMem2, 0, 0, _GetPicSize().cx, _GetPicSize().cy, blendFunc);
                    dcMem2.SelectObject(hgdiObj2);
                }
            }

            if (_fMouseWithin &&  _fGridLines && (_currentTool == Command))
            {
                // The mouse is within the window, and we are currently drawing.
                // If the user wants guide lines, give them to him.
                _DrawGuideLines(dcMem);
            }

            if (_fShowingEgo && _GetEditPic()->Traits->IsVGA)
            {
                _DrawShowingEgoVGA(dcMem, _GetDrawManager());
            }

            if (_fShowPriorityLines)
            {
                _DrawPriorityLines(&dcMem);
            }

            _DrawPolygons(&dcMem);
        }

        // Now blt back to the real DC.
        // Now we want to copy it back to the real dc.
        pDC->StretchBlt(-_xOrigin + gutter.x, -_yOrigin + gutter.y, _cxPic, GetPicScreenHeight(), &dcMem, 0, 0, _GetPicSize().cx, _GetPicSize().cy, SRCCOPY);

        dcMem.SelectObject(hgdiObj);
    }

    int picRightEdge = _cxPic + gutter.x;
    if (_fShowPriorityLines)
    {
        _DrawPriorityBar(pDC, gutter);
        picRightEdge += PriorityBarWidth;
    }

    // Finish off by drawing a navy background around the pic.
    CBrush brush(RGB(0, 0, 128));
    // All along the bottom - extend to window edges
    int yBottom = _GetViewHeight() - gutter.y - _yOrigin;
    if (rcClient.bottom > yBottom)
    {
        CRect rectBottom(CPoint(0, yBottom), CSize(RECTWIDTH(rcClient), rcClient.bottom - yBottom));
        pDC->FillRect(&rectBottom, &brush);
    }
    // Along the top
    CRect rectTop(CPoint(0, -_yOrigin), CSize(RECTWIDTH(rcClient), gutter.y));
    pDC->FillRect(&rectTop, &brush);

    int yRight = picRightEdge - _xOrigin;
    if (rcClient.right > yRight)
    {
        CRect rect2(CPoint(yRight, 0), CSize(rcClient.right - yRight, RECTHEIGHT(rcClient)));
        pDC->FillRect(&rect2, &brush);
    }
    // left:
    CRect rect2(CPoint(-_xOrigin, 0), CSize(gutter.x, RECTHEIGHT(rcClient)));
    pDC->FillRect(&rect2, &brush);


    // This will flicker here, but oh well.
    if (_transformingCoords)
    {
        const ViewPort *pState = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
        CPen pen(PS_SOLID, 1, RGB(255, 192, 128));
        HGDIOBJ hObj = pDC->SelectObject(&pen);

        int xBest = 0, yBest = 0;
        int bestDistance = INT_MAX;
        if ((_currentTool == Command) && (_currentCommand != PicCommand::CommandType::None))
        {
            if (_fCapturing)
            {
                // Only draw the dragged one in this case
                bestDistance = 0;
                _transformCommandMod->GetTransformCoord(*_GetEditPic(), xBest, yBest);
            }
            else
            {
                _EnsurateCoordinates(_currentCommand,
                    [this, pDC, &bestDistance, &xBest, &yBest](ptrdiff_t index, int x, int y, bool first)
                {
                    if (_IsCoordCloseEnough(x, y))
                    {
                        _DrawCoord(pDC, x, y);
                    }
                    int hitTestDistanceSq = _GetTransformHitTestDistance(x, y);
                    if ((hitTestDistanceSq < HitTestDistanceSqThreshold) && (hitTestDistanceSq < bestDistance))
                    {
                        xBest = x; yBest = y;
                        bestDistance = hitTestDistanceSq;
                    }
                    return true;
                }
                );
            }
        }

        if (bestDistance != INT_MAX)
        {
            CPen penRed(PS_SOLID, 1, RGB(255, 0, 0));
            pDC->SelectObject(&penRed);
            _DrawCoord(pDC, xBest, yBest, 1);
        }

        pDC->SelectObject(hObj);
    }

    // A final touch.  This will cause some flicker, but it shouldn't be too bad.
    if (_fShowingEgo && _HitTestFakeEgo(_ptCurrentHover))
    {
        _DrawEgoCoordinates(pDC);
    }
    __super::OnDraw(pDC);
}

CRect CPicView::_DrawShowingEgoWorker(const ViewPort &viewPort, uint8_t *pdataVisual, const uint8_t *pdataPriority, PicScreenFlags flags)
{
    int16_t egoPriority = PriorityFromY((uint16_t)_fakeEgoAttributes.Location.y, viewPort);
    if (_GetEditPic() && _GetEditPic()->Traits->ContinuousPriority)
    {
        // TODO: Don't use global DefaultResolution, but instead pic resolution
        point16 position = ScreenResolutionToGameResolution(CPointToPoint(_fakeEgoAttributes.Location));
        egoPriority = position.y;
        // Now turn it into something we can compare with the priority screen
        egoPriority = PriorityValueToColorIndex(true, egoPriority);
    }
    
    uint8_t bEgoPriority = (_fakeEgoAttributes.Pri) == -1 ? (uint8_t)egoPriority : (uint8_t)_fakeEgoAttributes.Pri;
    // Now we need to draw a box (or whatever), but only where
    // the bEgoPriority is equal to or greater than the priority
    // of the priority screen.
    CRect rc;
    if (pdataPriority)
    {
        // Alright, we're part way there.
        if (!appState->_fUseBoxEgo && _GetFakeEgo())
        {
            // Draw a view.
            rc = DrawViewWithPriority(_GetPicSize(), pdataVisual, pdataPriority, bEgoPriority, (uint16_t)_fakeEgoAttributes.Location.x, (uint16_t)_fakeEgoAttributes.Location.y, _GetFakeEgo(), _fakeEgoAttributes.Loop, _fakeEgoAttributes.Cel, _HitTestFakeEgo(_ptCurrentHover));
        }
        else
        {
            // Just draw a box.
            DrawBoxWithPriority(_GetPicSize(), pdataVisual, pdataPriority, bEgoPriority, (uint16_t)_fakeEgoAttributes.Location.x, (uint16_t)_fakeEgoAttributes.Location.y, appState->_cxFakeEgo, appState->_cyFakeEgo);
        }
    }
    return rc;
}

void CPicView::_DrawShowingEgoEGA(ViewPort &viewPort, PicData &picData, PicScreenFlags flags)
{
    // EGA uses the same palette for display, control and priority. Easy.
    _DrawShowingEgoWorker(viewPort, picData.pdataVisual, picData.pdataPriority, flags);
}

void CPicView::_DrawShowingEgoVGA(CDC &dc, PicDrawManager &pdm)
{
    const PaletteComponent *palette = _GetPalette();
    ResourceEntity *fakeEgo = _GetFakeEgo();
    if (fakeEgo && palette)
    {
        CDC dcMem;
        if (dcMem.CreateCompatibleDC(&dc))
        {
            // Prepare a pic-sized buffer to draw the ego into 
            const Cel &cel = GetCel(fakeEgo, _fakeEgoAttributes.Loop, _fakeEgoAttributes.Cel);
            const PaletteComponent *egoPalette = fakeEgo->TryGetComponent<PaletteComponent>();
            PaletteComponent paletteCopy;
            if (egoPalette)
            {
                paletteCopy = *egoPalette;
                paletteCopy.MergeFromOther(palette);
                palette = &paletteCopy;
            }

            // VGA display screen uses a different palette than priority or control.
            // When drawing the ego, we don't want to be limited by the palette of the screen
            // we're currently showing.
            
            uint8_t transparentColor = cel.TransparentColor;
            RGBQUAD transparentRGB = palette->Colors[cel.TransparentColor];
            SCIBitmapInfo bmi(_GetPicSize().cx, _GetPicSize().cy, (appState->_fUseBoxEgo ? g_egaColorsPlusOne : palette->Colors), appState->_fUseBoxEgo ? ARRAYSIZE(g_egaColorsPlusOne) : ARRAYSIZE(palette->Colors));
            if (appState->_fUseBoxEgo)
            {
                transparentColor = 16;
                transparentRGB = g_egaColorsPlusOne[transparentColor];

                uint8_t *pBitsDest;
                CBitmap bitmap;
                bitmap.Attach(CreateDIBSection((HDC)dcMem, &bmi, DIB_RGB_COLORS, (void**)&pBitsDest, nullptr, 0));
                if ((HBITMAP)bitmap)
                {
                    memset(pBitsDest, transparentColor, _GetPicSize().cx * _GetPicSize().cy);
                    _DrawShowingEgoWorker(*_GetDrawManager().GetViewPort(PicPosition::PostPlugin), pBitsDest, _GetDrawManager().GetPicBits(PicScreen::Priority, PicPosition::PostPlugin, _GetPicSize()), PicScreenFlags::Visual);

                    // Now blt it
                    HGDIOBJ hold = dcMem.SelectObject(bitmap);
                    COLORREF transparent = RGB(transparentRGB.rgbRed, transparentRGB.rgbGreen, transparentRGB.rgbBlue);
                    TransparentBlt(dc, 0, 0, _GetPicSize().cx, _GetPicSize().cy, dcMem, 0, 0, _GetPicSize().cx, _GetPicSize().cy, transparent);
                    dcMem.SelectObject(hold);
                }
            }
            else
            {
                // We have an issue when we have a palette with an RGB index identical to the transparent index's RGB value, but earlier in the palette.
                // Even setting it to a random color doesn't make a difference though, so something else is up. The result is that the area surrounding the
                // ego is opaque and the pic is no longer visible. Instead, we'll need to use AlphaBlend.
                std::unique_ptr<uint8_t[]> bitsDest = std::make_unique<uint8_t[]>(_GetPicSize().cx * _GetPicSize().cy);
                memset(bitsDest.get(), transparentColor, _GetPicSize().cx * _GetPicSize().cy);
                CRect rcEgo = _DrawShowingEgoWorker(*_GetDrawManager().GetViewPort(PicPosition::PostPlugin), bitsDest.get(), _GetDrawManager().GetPicBits(PicScreen::Priority, PicPosition::PostPlugin, _GetPicSize()), PicScreenFlags::Visual);
                if (!rcEgo.IsRectEmpty())
                {
                    CBitmap bitmap;
                    uint8_t *start = bitsDest.get() + (_GetPicSize().cy - rcEgo.bottom) * _GetPicSize().cx + rcEgo.left;
                    bitmap.Attach(Create32bbpBitmap(start, _GetPicSize().cx, rcEgo.Width(), rcEgo.Height(), cel.TransparentColor, bmi.bmiColors, bmi.bmiHeader.biClrUsed));
                    HGDIOBJ hOld = dcMem.SelectObject(bitmap);
                    BLENDFUNCTION blend = {};
                    blend.AlphaFormat = AC_SRC_ALPHA;
                    blend.SourceConstantAlpha = 255;
                    blend.BlendFlags = 0;
                    blend.BlendOp = AC_SRC_OVER;
                    AlphaBlend(dc, rcEgo.left, rcEgo.top, rcEgo.Width(), rcEgo.Height(), dcMem, 0, 0, rcEgo.Width(), rcEgo.Height(), blend);
                    dcMem.SelectObject(hOld);
                }
            }

        }
    }
}

void CPicView::_DrawLineDraw(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags)
{
    // The user is drawing a line.  We'll draw it right into the bits, by asking
    // a line command to draw.
    PicCommand command = PicCommand::CreateLine((uint16_t)_xOld, (uint16_t)_yOld, (uint16_t)_ptCurrentHover.x, (uint16_t)_ptCurrentHover.y);

    if (IsFlagSet(viewPort.dwDrawEnable, screenFlags))
    {
        // Finally draw (whew!)
        LineCommand_DrawOnly(&command, &data, &viewPort);
    }
}

void CPicView::_DrawCircleDraw(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags)
{
    // The user is drawing a line.  We'll draw it right into the bits, by asking
    // a line command to draw.
    ASSERT(_xOld != -1);
    ASSERT(_yOld != -1);

    PicCommand command;
    command.CreateCircle((uint16_t)_xOld, (uint16_t)_yOld, (uint16_t)_ptCurrentHover.x, (uint16_t)_ptCurrentHover.y);

    if (IsFlagSet(viewPort.dwDrawEnable, screenFlags))
    {
        CircleCommand_DrawOnly(&command, &data, &viewPort);
    }
    else
    {
        // We need to draw this some other way.  We'll wait until we've copied
        // our buffer into the memory dc.
        // TODO
    }
}

void CPicView::_DrawPenPreview(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags)
{
    // We're drawing with a pen.  Preview it.

    // The user is using the pen tool.
    CPicDoc *pDoc = GetDocument();
    if (pDoc)
    {
        PenStyle penStyle = pDoc->GetPenStyle();
        PicCommand command = PicCommand::CreatePattern((uint16_t)_ptCurrentHover.x,
                                (uint16_t)_ptCurrentHover.y,
                                penStyle.bPatternSize,
                                _bRandomNR, // Always use _bRandomNR - if random change isn't set, we just don't cycle it.
                                penStyle.fPattern,
                                penStyle.fRectangle);

        PatternCommand_Draw_DrawOnly(&command, &data, &viewPort);
    }
}


void CPicView::_DrawGuideLines(CDC &dcMem)
{
    RECT rcAroundThis = { _ptCurrentHover.x - 10, _ptCurrentHover.y - 10, _ptCurrentHover.x + 10, _ptCurrentHover.y + 10 };

    CPen pen;
    pen.CreateStockObject(BLACK_PEN);

    if (rcAroundThis.left >= 0)
    {
        dcMem.MoveTo(0, _ptCurrentHover.y);
        dcMem.LineTo(rcAroundThis.left, _ptCurrentHover.y);
    }
    if (rcAroundThis.right < _GetPicSize().cx)
    {
        dcMem.MoveTo(rcAroundThis.right, _ptCurrentHover.y);
        dcMem.LineTo(_GetPicSize().cx, _ptCurrentHover.y);
    }
    if (rcAroundThis.top >= 0)
    {
        dcMem.MoveTo(_ptCurrentHover.x, 0);
        dcMem.LineTo(_ptCurrentHover.x, rcAroundThis.top);
    }
    if (rcAroundThis.bottom < _GetPicSize().cy)
    {
        dcMem.MoveTo(_ptCurrentHover.x, rcAroundThis.bottom);
        dcMem.LineTo(_ptCurrentHover.x, _GetPicSize().cy);
    }
}

void CPicView::_InitCommandAdjust(PICCOMMAND_ADJUST *pAdjust)
{
    pAdjust->rcBounds.left = _xPasted;
    pAdjust->rcBounds.top = _yPasted;
    pAdjust->rcBounds.right = _xPasted + _cxPasted;
    pAdjust->rcBounds.bottom = _yPasted + _cyPasted;
    pAdjust->rcNew.left = _xPasted + _xPasteOffset;
    pAdjust->rcNew.top = _yPasted + _yPasteOffset;
    pAdjust->rcNew.right = pAdjust->rcNew.left + _cxPastedScale;
    pAdjust->rcNew.bottom = pAdjust->rcNew.top + _cyPastedScale;

    pAdjust->fHFlip = _fHFlip;
    pAdjust->fVFlip = _fVFlip;

    pAdjust->iAngle = _iAngle;
}

void CPicView::_DrawPasteCommands(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags)
{
    for (size_t i = 0; i < _pastedCommands.size(); i++)
    {
        PICCOMMAND_ADJUST adjust = { 0 };
        _InitCommandAdjust(&adjust);
        Command_DrawWithOffset(&_pastedCommands[i], &data, const_cast<ViewPort*>(&viewPort), &adjust);
    }
}

PicScreenFlags CPicView::GetRequiredScreens()
{
    PicScreenFlags flags = PicScreenToFlags(_mainViewScreen);
    if (_fShowingEgo)
    {
        flags |= PicScreenFlags::Priority;
    }
    return flags;
}

// This is useful for a performance optimization
bool CPicView::WillDrawOnPic()
{
    if (_fShowingEgo || _fDrawingLine || _fDrawingCircle)
    {
        return true;
    }
    if (_fMouseWithin && !_transformingCoords)
    {
        if (_fPreviewPen || (_fGridLines && (_currentTool == Command)))
        {
            return true;
        }
    }
    return !_pastedCommands.empty();
}

void CPicView::DrawOnPic(ViewPort &viewPort, PicData &picData, PicScreenFlags flags)
{
    if (_fShowingEgo && !_GetEditPic()->Traits->IsVGA)
    {
        _DrawShowingEgoEGA(viewPort, picData, flags);
    }

    if (_fDrawingLine)
    {
        _DrawLineDraw(viewPort, picData, flags);
    }
    if (_fDrawingCircle)
    {
        _DrawCircleDraw(viewPort, picData, flags);
    }

    if (_fPreviewPen && _fMouseWithin && !_transformingCoords)
    {
        _DrawPenPreview(viewPort, picData, flags);
    }

    // If we're currently pasting, draw those commands now.
    if (!_pastedCommands.empty())
    {
        _DrawPasteCommands(viewPort, picData, flags);
    }
}

// Every thing that is in here must adhere to the current palette and operates within "native" SCI
// resolution.
void CPicView::_OnDraw(CDC* pDC, PicScreen screen)
{
    BITMAPINFO *pbmi;

    /*
#ifdef DEBUG
    // Show aux screen when shift is held.
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        screen = PicScreen::Aux;
    }
#endif
    */

    const uint8_t *displayBits = _GetDrawManager().GetPicBits(screen, PicPosition::PostPlugin, _GetPicSize());
    _GetDrawManager().GetBitmapInfo(screen, &pbmi);

    // FEATURE: this could be optimized (when zoomed, by limiting how much we copy)
    // - it involves palette conversion, so it could be expensive.
    if (pbmi)
    {
        StretchDIBits((HDC)*pDC, 0, 0, _GetPicSize().cx, _GetPicSize().cy, 0, 0, _GetPicSize().cx, _GetPicSize().cy, displayBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
        delete pbmi;

        // Draw a special line for when line-drawing is "invisible".  We can't do this directly
        // on the bits, because the line drawing functions don't support dotted lines.
        const ViewPort *viewPort = _GetDrawManager().GetViewPort(PicPosition::PostPlugin); // Just to coincide with above
        if (_fDrawingLine && !IsFlagSet(viewPort->dwDrawEnable, PicScreenToFlags(screen)))
        {
            // Use a black and white dotted line.
            CPen pen(PS_DOT, 1, RGB(255, 255, 255));
            COLORREF colorOld = pDC->SetBkColor(RGB(0, 0, 0));
            HGDIOBJ hObj = pDC->SelectObject(&pen);
            pDC->MoveTo(_xOld, _yOld);
            pDC->LineTo(_ptCurrentHover);
            pDC->SelectObject(hObj);
            pDC->SetBkColor(colorOld);
        }

        if (!_pastedCommands.empty())
        {
            // Use a black and white dotted line.
            CPen pen(PS_DOT, 1, RGB(255, 255, 255));
            COLORREF colorOld = pDC->SetBkColor(RGB(0, 0, 0));
            HGDIOBJ hObj = pDC->SelectObject(&pen);

            CRect rectPaste;
            _GetPasteRect(rectPaste);

            pDC->MoveTo(rectPaste.left, rectPaste.top);
            pDC->LineTo(rectPaste.right - 1, rectPaste.top);
            pDC->LineTo(rectPaste.right - 1, rectPaste.bottom - 1);
            pDC->LineTo(rectPaste.left, rectPaste.bottom - 1);
            pDC->LineTo(rectPaste.left, rectPaste.top);
            pDC->SelectObject(hObj);
            pDC->SetBkColor(colorOld);
        }
    }
}

//
// Draws lines that highlight the lines previously drawn in the editor.
//
void CPicView::s_CloseCoordCallback(void *pv, CDC *pDC, int x1, int y1, int x2, int y2)
{
    CPicView *ppev = static_cast<CPicView*>(pv);
    CPoint zoomOffset = ppev->GetZoomOffset();

    CPoint ptClient1 = ppev->_MapPicPointToClient(CPoint(x1, y1));
    ptClient1.Offset(zoomOffset);
    pDC->MoveTo(ptClient1.x - 3, ptClient1.y - 3);
    pDC->LineTo(ptClient1.x + 4, ptClient1.y - 3);
    pDC->LineTo(ptClient1.x + 4, ptClient1.y + 4);
    pDC->LineTo(ptClient1.x - 3, ptClient1.y + 4);
    pDC->LineTo(ptClient1.x - 3, ptClient1.y - 3);

    CPoint ptClient2 = ppev->_MapPicPointToClient(CPoint(x2, y2));
    ptClient2.Offset(zoomOffset);
    pDC->MoveTo(ptClient2.x - 3, ptClient2.y - 3);
    pDC->LineTo(ptClient2.x + 4, ptClient2.y + 4);
    pDC->MoveTo(ptClient2.x + 3, ptClient2.y - 3);
    pDC->LineTo(ptClient2.x - 4, ptClient2.y + 4);

    pDC->MoveTo(ptClient2.x, ptClient2.y);
    pDC->LineTo(ptClient1.x, ptClient1.y);
}

void CPicView::_DrawCoord(CDC *pDC, int x, int y, int additionalSize)
{
    CPoint zoomOffset = GetZoomOffset();
    CPoint ptClient1 = _MapPicPointToClient(CPoint(x, y));
    ptClient1.Offset(zoomOffset);
    int bottom = ptClient1.y + 4 + additionalSize;
    int top = ptClient1.y - 3 - additionalSize;
    int right = ptClient1.x + 4 + additionalSize;
    int left = ptClient1.x - 3 - additionalSize;
    pDC->MoveTo(left, top);
    pDC->LineTo(right, top);
    pDC->LineTo(right, bottom);
    pDC->LineTo(left, bottom);
    pDC->LineTo(left, top);
}

void CPicView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    // We have a new thing.
    __super::OnUpdate(pSender, lHint, pHint);

    PicChangeHint hint = GetHint<PicChangeHint>(pHint);

    if (IsFlagSet(hint, PicChangeHint::ZoomLevel))
    {
        int iZoom = GetDocument()->GetZoom();
        if (iZoom)
        {
            int iOldZoom = _cxPic / _GetPicSize().cx;
            if (iOldZoom != iZoom)
            {
                int iMultiplier;
                if (iZoom > iOldZoom)
                {
                    iMultiplier = iZoom / iOldZoom;
                }
                else
                {
                    iMultiplier = -iOldZoom / iZoom;
                }
                // Figure out the center of the zoom
                CPoint ptZoom;
                CRect rcClient;
                GetClientRect(&rcClient);
                // Does the pic occupy less than the entire client area?
                if (_cxPic < rcClient.Width())
                {
                    // Yes... zoom around the center of the pic
                    ptZoom.SetPoint(_GetPicSize().cx / 2, _GetPicSize().cy / 2);
                }
                else
                {
                    // Otherwise zoom around the center of the client area
                    ptZoom = _MapClientPointToPic(rcClient.CenterPoint());
                }
                _OnZoomClick(&ptZoom, iMultiplier);
            }
        }
    }

    if (IsFlagSet(hint, PicChangeHint::NewPic))
    {
        // Need to update our size, potentially.
        int iZoom = GetDocument()->GetZoom();
        _cyPic = iZoom * _GetPicSize().cy;
        _cxPic = iZoom * _GetPicSize().cx;
    }

    if (IsFlagSet(hint, PicChangeHint::NewPic | PicChangeHint::EditPicPos | PicChangeHint::Palette | PicChangeHint::EditPicInvalid | PicChangeHint::PreviewPalette))
    {
        // Need to redraw the pic, yup
        InvalidateOurselvesImmediately();
    }

    if (IsFlagSet(hint, PicChangeHint::PolygonChoice | PicChangeHint::PolygonsChanged))
    {
        InvalidateOurselves();
    }

    if (IsFlagSet(hint, PicChangeHint::Preferences))
    {
        // The scale/crop for trace images might have changed, so regenrate that
        _fRegenerateTrace = true;
        InvalidateOurselves();
    }

    if (IsFlagSet(hint, PicChangeHint::PenStyle))
    {
        // Be smart and switch into pen mode.
        OnPenCommand();
    }

    if (IsFlagSet(hint, PicChangeHint::FakeEgo))
    {
        // Force ourselves to get it again...
        _fakeEgo.reset(nullptr);
        InvalidateOurselves();
    }
}

//
// prevent flicker
//
BOOL CPicView::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

// FEATURE: Do the replace-colour tool
// PosFromPoint, then do a setvisual of the current visual colour
// then after the point (now 1 after) restore it.
// Before doing a setvisual, see if you can go before and remove a setvisual
// e.g. if not drawing commands were done before the last setvisual

void CPicView::_OnHistoryLClick(CPoint point)
{
    size_t iPos = _GetDrawManager().PosFromPoint(point.x, point.y, -1);

    // We'll set the pos!
    GetDocument()->SeekToPos(iPos);
}

void CPicView::_OnHistoryRClick(CPoint point)
{
    size_t iPos = _GetDrawManager().PosFromPoint(point.x, point.y, -1);
    iPos++;

    // We'll set the pos!
    GetDocument()->SeekToPos(iPos);
}

void CPicView::_MakeRandomNR()
{
    // If we're doing random bit patterns, make a new one now.
    CPicDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);        
    if (pDoc)
    {
        if (pDoc->GetPenStyle().fRandomNR)
        {
            _bRandomNR = (rand() % 128);
        }
    }
}

void CPicView::_OnPatternLClick(CPoint point)
{
    CPicDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);        
    if (pDoc)
    {
        PenStyle penStyle = pDoc->GetPenStyle();
        PicCommand command = PicCommand::CreatePattern((uint16_t)point.x, (uint16_t)point.y,
                            penStyle.bPatternSize, _bRandomNR, // Use _bRandomNR even if we're not randomly cycling it.
                            penStyle.fPattern, penStyle.fRectangle);
        pDoc->InsertCommand(&command);

        _MakeRandomNR();
    }
}

void CPicView::_OnPatternRClick()
{
    // cycle patternNR.
    _bRandomNR++;
    _bRandomNR = _bRandomNR % 128;

    // FEATURE: we can really optimize the clip region here.
    _GetDrawManager().InvalidatePlugins();
    GetDocument()->ExplicitNotify(PicChangeHint::EditPicInvalid);
    InvalidateOurselvesImmediately();
}

void CPicView::_OnCircleLClick(CPoint point)
{
    if (!_fDrawingCircle)
    {
        _fDrawingCircle = TRUE;
        // Start the line here:
        _xOld = point.x;
        _yOld = point.y;

        _GetDrawManager().InvalidatePlugins();
        GetDocument()->ExplicitNotify(PicChangeHint::EditPicInvalid);
    }
    else
    {
        // Actually draw a line. (Insert a line command)
        assert(_xOld != -1);
        assert(_yOld != -1);
        _SnapCoordinate(point);

        PicCommand command;
        command.CreateCircle((uint16_t)_xOld, (uint16_t)_yOld, (uint16_t)point.x, (uint16_t)point.y);
        GetDocument()->InsertCommand(&command);
        _fDrawingCircle = FALSE; // Start anew
    }    
}

void CPicView::_OnLineLClick(CPoint point)
{
    if (!_fDrawingLine)
    {
        _fDrawingLine = TRUE;
        // Start the line here:
        _xOld = point.x;
        _yOld = point.y;

        _GetDrawManager().InvalidatePlugins();
        GetDocument()->ExplicitNotify(PicChangeHint::EditPicInvalid);
    }
    else
    {
        // Actually draw a line. (Insert a line command)
        assert(_xOld != -1);
        assert(_yOld != -1);
        _SnapCoordinate(point);

        PicCommand command = PicCommand::CreateLine((uint16_t)_xOld, (uint16_t)_yOld, (uint16_t)point.x, (uint16_t)point.y);
        GetDocument()->InsertCommand(&command);

        // And update our coords.
        _xOld = point.x;
        _yOld = point.y;
    }
}

void CPicView::_OnLineRClick(CPoint point)
{
    if (_fDrawingLine)
    {
        // To clean up the line drawing turd.
        InvalidateOurselves();
        _fDrawingLine = FALSE;
        _GetDrawManager().InvalidatePlugins();
        GetDocument()->ExplicitNotify(PicChangeHint::EditPicInvalid);
    }
    _xOld = -1;
    _yOld = -1;
}
void CPicView::_OnCircleRClick(CPoint point)
{
    if (_fDrawingCircle)
    {
        // To clean up the line drawing turd.
        InvalidateOurselves();
        _fDrawingCircle = FALSE;
        _GetDrawManager().InvalidatePlugins();
        GetDocument()->ExplicitNotify(PicChangeHint::EditPicInvalid);
    }
    _xOld = -1;
    _yOld = -1;
}

ResourceEntity *CPicView::_GetFakeEgo()
{
    if (!_fakeEgo && GetDocument())
    {
        _fakeEgoAttributes.View = GetDocument()->GetFakeEgo();
        _fakeEgo = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, _fakeEgoAttributes.View);
    }
    return _fakeEgo.get();
}

const SCIPolygon *CPicView::_GetCurrentPolygon()
{
    const SCIPolygon *polygon = _currentPolyInEdit.get();
    if (!polygon)
    {
        if (GetDocument())
        {
            int index = GetDocument()->GetCurrentPolygonIndex();
            const PolygonComponent *source = GetDocument()->GetPolygonComponent();
            if (source)
            {
                polygon = source->GetAt(index);
            }
        }
    }
    return polygon;
}

void CPicView::_OnPolygonLClick(CPoint point)
{
    if (!_currentPolyInEdit)
    {
        int currentHover = _HitTestCurrentPolyPoint(point);
        if (currentHover != -1)
        {
            // Start dragging the point
            const SCIPolygon *polygon = _GetCurrentPolygon();
            _fCapturing = TRUE;
            SetCapture();
            _polyDragPointIndex = currentHover;
            _startDragPolyPoint = polygon->Points()[currentHover];
            _currentDragPolyPoint = _startDragPolyPoint;
            _pointCapture = point;
        }
        else
        {
            // Start new polygon
            // Unselect any current one:
            GetDocument()->SetCurrentPolygonIndex(-1);
            _currentPolyInEdit = make_unique<SCIPolygon>();
            _nextPolyPoint = point;
            _currentPolyInEdit->AppendPoint(CPointToPoint(point));
            InvalidateOurselves();
        }
    }
    else
    {
        // We're continuing with the current polygon
        int currentHover = _HitTestCurrentPolyPoint(point);
        if (currentHover == 0)
        {
            // End the polygon, the user clicked on the first point.
            _EndNewPoly();
        }
        else
        {
            // Add another point
            _currentPolyInEdit->AppendPoint(CPointToPoint(point));
            InvalidateOurselves();
        }
    }
}

#define PI 3.1415926535

int CalcTotalAngle(const std::vector<point16> &points)
{
    double totalAngle = 0;
    double curAngle = 0;
    double prevAngle = 0;
    for (size_t i = 0; i < points.size(); i++)
    {
        point16 cur = points[i];
        point16 next = points[(i + 1) % points.size()];
        curAngle = atan2(cur.y - next.y, cur.x - next.x);
        double angleDiff = (curAngle - prevAngle);
        if (angleDiff > PI)
        {
            angleDiff -= (PI * 2.0);
        }
        else if (angleDiff < -PI)
        {
            angleDiff += (PI * 2.0);
        }
        totalAngle += angleDiff;

        prevAngle = curAngle;
    }
    return (int)(totalAngle * 180.0 / PI);
}

void CPicView::_EndPolyDrag()
{
    if (GetDocument() && (_polyDragPointIndex != -1))
    {
        int currentPolygon = GetDocument()->GetCurrentPolygonIndex();
        GetDocument()->ApplyChanges<PolygonComponent>(
            [this, currentPolygon](PolygonComponent &polygonComponent)
        {
            SCIPolygon *polygon = polygonComponent.GetAt(currentPolygon);
            polygon->SetPoint(_polyDragPointIndex, _currentDragPolyPoint);
            FixupPolygon(*polygon);
            return WrapHint(PicChangeHint::PolygonsChanged);
        }
            );

    }
    _polyDragPointIndex = -1;
}

void CPicView::_EndNewPoly()
{
    if (GetDocument() && _currentPolyInEdit)
    {
        bool valid = false;
        // TODO: Remove duplicate points.
        // Validate that there are at least three points.
        valid = (_currentPolyInEdit->Points().size() >= 3);
        if (valid)
        {
            // TODO: SCI validates that polygon angles add up to 360 or -360.
            // REVIEW This doesn't seem to work.
            int totalAngle = CalcTotalAngle(_currentPolyInEdit->Points());
        }
        
        if (valid)
        {
            GetDocument()->ApplyChanges<PolygonComponent>(
                [this](PolygonComponent &polygonComponent)
            {
                FixupPolygon(*_currentPolyInEdit);
                polygonComponent.AppendPolygon(*_currentPolyInEdit);
                return WrapHint(PicChangeHint::PolygonsChanged);
            }
            );

            // Select the polygon we just created
            GetDocument()->SetCurrentPolygonIndex((int)GetDocument()->GetPolygonComponent()->Polygons().size() - 1);
        }
    }
    // No longer in edit:
    _currentPolyInEdit.reset(nullptr);
    _polyDragPointIndex = -1;
    InvalidateOurselves();
}

void CPicView::_OnPolygonRClick(CPoint point)
{
    if (_currentPolyInEdit)
    {
        _EndNewPoly();
    }
    else
    {
        CPoint ptScreen = GameResolutionToScreenResolution(point, appState->GetVersion().DefaultResolution);
        ptScreen = _MapPicPointToClient(ptScreen);
        ClientToScreen(&ptScreen);

        // Hit test a point and offer to delete.
        int pointUnderMouse = _HitTestCurrentPolyPoint(point);
        if (pointUnderMouse != -1)
        {
            // Offer to delete.
            CMenu contextMenu;
            contextMenu.LoadMenu(IDR_MENUDELETEPOINT);
            CMenu* pTracker;
            pTracker = contextMenu.GetSubMenu(0);
            if (pTracker)
            {
                DWORD selection = pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                    ptScreen.x, ptScreen.y, AfxGetMainWnd());
                if (selection == ID_PIC_DELETEPOINT)
                {
                    int currentPolyIndex = GetDocument()->GetCurrentPolygonIndex();
                    GetDocument()->ApplyChanges<PolygonComponent>(
                        [currentPolyIndex, pointUnderMouse](PolygonComponent &polygonComponent)
                    {
                        SCIPolygon *polygon = polygonComponent.GetAt(currentPolyIndex);
                        polygon->DeletePoint(pointUnderMouse);
                        FixupPolygon(*polygon);
                        return WrapHint(PicChangeHint::PolygonsChanged);
                    }
                        );
                    InvalidateOurselves();
                }
            }

        }
        else
        {
            // Offset to split a line
            int lineIndex = _HitTestCurrentPolyEdge(point);
            if (lineIndex != -1)
            {
                CMenu contextMenu;
                contextMenu.LoadMenu(IDR_MENUSPLITEDGE);
                CMenu* pTracker;
                pTracker = contextMenu.GetSubMenu(0);
                if (pTracker)
                {
                    DWORD selection = pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                        ptScreen.x, ptScreen.y, AfxGetMainWnd());
                    if (selection == ID_PIC_SPLITEDGE)
                    {
                        int currentPolyIndex = GetDocument()->GetCurrentPolygonIndex();
                        GetDocument()->ApplyChanges<PolygonComponent>(
                            [currentPolyIndex, lineIndex, point](PolygonComponent &polygonComponent)
                        {
                            SCIPolygon *polygon = polygonComponent.GetAt(currentPolyIndex);
                            polygon->InsertPoint(lineIndex, CPointToPoint(point));
                            FixupPolygon(*polygon);
                            return WrapHint(PicChangeHint::PolygonsChanged);
                        }
                        );
                        InvalidateOurselvesImmediately();
                    }
                }
            }
        }
    }
}

int CPicView::_HitTestCurrentPolyPoint(CPoint point)
{
    // See if we are interesecting an existing polygon
    int currentHover = -1;
    const SCIPolygon *polygon = _GetCurrentPolygon();
    if (polygon)
    {
        int index = 0;
        for (const auto &polyPoint : polygon->Points())
        {
            int distance = max(abs(point.x - polyPoint.x), abs(point.y - polyPoint.y));
            if (distance <= PolygonPointHitTestMargin)
            {
                currentHover = index;
                break;
            }
            index++;
        }
    }
    return currentHover;
}

int SegmentDistToPoint(CPoint segA, CPoint segB, CPoint p)
{
    CPoint p2 = segB - segA;
    //var p2 : Point = new Point(segB.x - segA.x, segB.y - segA.y);
    int something = p2.x * p2.x + p2.y * p2.y;
    //var something : Number = p2.x*p2.x + p2.y*p2.y;
    double u = (double)((p.x - segA.x) * p2.x + (p.y - segA.y) * p2.y) / (double)something;
    //var u : Number = ((p.x - segA.x) * p2.x + (p.y - segA.y) * p2.y) / something;

    if (u > 1.0)
        u = 1.0;
    else if (u < 0.0)
        u = 0.0;

    double x = segA.x + u * p2.x;
    ////var x : Number = segA.x + u * p2.x;
    double y = segA.y + u * p2.y;
    //var y : Number = segA.y + u * p2.y;

    double dx = x - p.x;
    double dy = y - p.y;
    //var dx : Number = x - p.x;
    //var dy : Number = y - p.y;

    double dist =  sqrt(dx*dx + dy*dy);

    return (int)dist;
}

int CPicView::_HitTestCurrentPolyEdge(CPoint point)
{
    int currentEdge = -1;
    const SCIPolygon *polygon = _GetCurrentPolygon();
    if (polygon)
    {
        for (size_t i = 0; i < polygon->Points().size(); i++)
        {
            int nextIndex = (i + 1) % polygon->Points().size();
            int distance = SegmentDistToPoint(PointToCPoint(polygon->Points()[i]), PointToCPoint(polygon->Points()[nextIndex]), point);
            if (distance <= PolygonPointHitTestMargin)
            {
                currentEdge = (int)i;
                break;
            }
        }
    }
    return currentEdge;
}

void CPicView::_OnPolyMouseMove(CPoint point)
{
    bool invalidate = false;
    if (_currentPolyInEdit)
    {
        _nextPolyPoint = point;
        // However, hit test the first point, so the user has a way of closing it with left click.
        int currentHover = _HitTestCurrentPolyPoint(point);
        _currentHoverPolyPointIndex = (currentHover == 0) ? 0 : - 1;
        invalidate = true;
    }
    else
    {
        int currentHover = _HitTestCurrentPolyPoint(point);
        if (currentHover != _currentHoverPolyPointIndex)
        {
            _currentHoverPolyPointIndex = currentHover;
            invalidate = true;
        }

        int currentEdgeHover = _HitTestCurrentPolyEdge(point);
        if (currentEdgeHover != _currentHoverPolyEdgeIndex)
        {
            _currentHoverPolyEdgeIndex = currentEdgeHover;
            invalidate = true;
        }
    }

    if (invalidate)
    {
        InvalidateOurselvesImmediately();
    }
}

void CPicView::_OnFillLClick(CPoint point)
{
    PicCommand command = PicCommand::CreateFill((uint16_t)point.x, (uint16_t)point.y);
    GetDocument()->InsertCommand(&command);
    // FEATURE: Here, we could sample the pixel, and determine that if nothing happened (i.e. the pixel wasn't white)
    // then we don't need to add a command, and we could even put a little message in the status bar.
}

//
// iMultiplier is 1 or -1
//
void CPicView::_OnZoomClick(CPoint *ppt, int iMultiplier)
{
    // Assumes already validated.
    CPoint ptScreenBefore;
    if (ppt)
    {
        // Zoom around where the user clicked!
         ptScreenBefore = _MapPicPointToClient(*ppt);
    }

    _cxPic = _cxPic + (_GetPicSize().cx * iMultiplier);
    _cyPic = _cyPic + (_GetPicSize().cy * iMultiplier);
    _cxPic = max(_cxPic, _GetPicSize().cx);
    _cyPic = max(_cyPic, _GetPicSize().cy);

    CPicDoc* pDoc = GetDocument();
    // Push to the document (it only fires an event when it changes, so we shouldn't get re-entered).
    pDoc->SetZoom(_cxPic / _GetPicSize().cx);

    CPoint ptScreenAfter;
    if (ppt)
    {
        ptScreenAfter = _MapPicPointToClient(*ppt);
    }
    CPoint ptDiff = ptScreenAfter - ptScreenBefore;
    // Now adjust origin by this amount
    _ScrollDelta(SB_HORZ, this->m_hWnd, ptDiff.x);
    _ScrollDelta(SB_VERT, this->m_hWnd, ptDiff.y);
    // Don't need to do this:
    //_InvalidateScrollStuff();
    InvalidateOurselves();
}

//
// Make big
//
void CPicView::_OnZoomLClick(CPoint *ppt)
{
    if (_cxPic < (_GetPicSize().cx * 16))
    {
        _OnZoomClick(ppt, 1);
    }
    // Otherwise, make it no bigger
}

//
// Make small
//
void CPicView::_OnZoomRClick(CPoint *ppt)
{
    if (_cxPic > _GetPicSize().cx)
    {
        _OnZoomClick(ppt, -1);
    }
    // Otherwise, make it no smaller
}



//
// Returns the ID of the current tool or command.
// (-1 if none)
//
UINT CPicView::_GetCurrentToolOrCommand()
{
    int nID = -1;

    if (_currentTool != Command)
    {
        // Do we have a tool
        nID = LookupKeyValue(c_toolToID, ARRAYSIZE(c_toolToID), _currentTool, (UINT)-1);
    }

    if (nID == -1)
    {
        // or a command
        nID = LookupKeyValue(c_commandToID, ARRAYSIZE(c_commandToID), _currentCommand, (UINT)-1);
    }
    
    return nID;
}

BOOL PointIsNear(CPoint &pt1, CPoint &pt2)
{
    // Within 5 pixels
    return ((abs(pt1.x - pt2.x) < 5) && (abs(pt1.y - pt2.y) < 5));
}

bool CPicView::_NearPasteCorners()
{
    bool fRet = false;
    // See if we're near the edges.
    // Near the
    CRect rc(_xPasted, _yPasted, _xPasted + _cxPasted, _yPasted + _cyPasted);
    rc.OffsetRect(_xPasteOffset, _yPasteOffset);
    if (PointIsNear(_ptCurrentHover, rc.TopLeft()) ||
        PointIsNear(_ptCurrentHover, rc.BottomRight()) ||
        PointIsNear(_ptCurrentHover, CPoint(rc.right, rc.top)) ||
        PointIsNear(_ptCurrentHover, CPoint(rc.left, rc.bottom)))
    {
        fRet = true;
    }
    return fRet;
}

void CPicView::_UpdateCursor()
{
    if (_fMouseWithin)
    {
        bool showPasteCursor = ((_currentTool == Pasting) || ((_currentTool == None) && _fShowingEgo)) && _fInPasteRect;
        showPasteCursor = showPasteCursor || ((_currentTool != None) && _transformingCoords);
        if (showPasteCursor)
        {
            // Show a special cursor while the mouse is hovered over the ego (and drawing tools are off) or paste rect
            int iResource = IDC_CURSORPASTE;
            if (_currentTool == Pasting)
            {
                if (_NearPasteCorners())
                {
                    iResource = IDC_CURSORROTATE;
                }
            }
            SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(iResource)));
        }
        else
        {
            if (_fShowPriorityLines && _fPriLineHover && (_priBarMoveIndex != -1))
            {
                SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
            }
            else
            {
                BOOL fUseDefault = TRUE;
                // Otherwise, show the cursor for the current tool.
                UINT nID = _GetCurrentToolOrCommand();
                if (nID != -1)
                {
                    int idResource = _IDToCursor(nID);
                    if (idResource != -1)
                    {
                        fUseDefault = FALSE;
                        SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(idResource)));
                    }
                }

                if (fUseDefault)
                {
                    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
                }
            }
        }
    }
}

//
// This must be called every time a tool or command is changed.
//
void CPicView::_OnCommandChanged()
{
    // Reset x and y
    _xOld = -1;
    _yOld = -1;

    // Special case for lines:
    if ((_currentTool != Command) || (_currentCommand != PicCommand::Line))
    {
        if (_fDrawingLine)
        {
            // To clean up the line drawing tool thing.
            InvalidateOurselves();
            _fDrawingLine = FALSE;
        }
    }
    if ((_currentTool != Command) || (_currentCommand != PicCommand::Circle))
    {
        if (_fDrawingCircle)
        {
            // To clean up the line drawing tool thing.
            InvalidateOurselves();
            _fDrawingCircle = FALSE;
        }
    }

    // Special case for pens:
    BOOL fPreviewPen = (_currentTool == Command) && (_currentCommand == PicCommand::Pattern);
    if (fPreviewPen != _fPreviewPen)
    {
        _fPreviewPen = fPreviewPen;
        InvalidateOurselves();
    }

    if ((_currentTool != Pasting) && (!_pastedCommands.empty()))
    {
        _CleanUpPaste();
    }
}

void CPicView::_TransformPoint(bool commit, CPoint pt)
{
    int dx = pt.x - _pointCapture.x;
    int dy = pt.y - _pointCapture.y;
    
    // Forcefully set the command at the position
    if (commit)
    {
        // Apply changes works on a clone of the current resource, while the current resource
        // goes into the undo stack. Since we've been modifying the current resource, we
        // need to restore it before applying our final changes to the clone.
        _transformCommandMod->ApplyDifference(const_cast<PicComponent&>(*GetDocument()->GetPic()), 0, 0);

        GetDocument()->ApplyChanges<PicComponent>(
            [this, dx, dy](PicComponent &pic)
        {
            _transformCommandMod->ApplyDifference(pic, dx, dy);
            return WrapHint(PicChangeHint::EditPicInvalid);
        }
        );
    }
    else
    {
        // We have to go poking around in the resource directly
        _transformCommandMod->ApplyDifference(const_cast<PicComponent&>(*GetDocument()->GetPic()), dx, dy);
        // As a result we need to tell people manually to update
    }
}

void CPicView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Don't do anything if this is off the picture.
    CPoint ptPic = _MapClientPointToPic(point);

    if (_currentTool == Polygons)
    {
        // Make it easy to put polygons up to the image boundaries:
        _ClampPoint(ptPic);
    }

    // The out of bounds check doesn't apply to fake ego.
    if ((_currentTool == None) && _fShowingEgo && _HitTestFakeEgo(ptPic))
    {
        // The ego is showing.  Start capture.
        _fCapturing = TRUE;
        _pointCapture = ptPic;
        _pointEgoOrig = _fakeEgoAttributes.Location;
        _fCanBeHere = _EvaluateCanBeHere(_pointEgoOrig);
        SetCapture();
        InvalidateOurselves();
    }

    if (_fShowPriorityLines && _HitTestPriorityBar(ptPic, &_priBarMoveIndex) && (_priBarMoveIndex != -1))
    {
        _originalPriValueCommand = _EnsurePriorityBarCommand();
        _originalPriValue = _originalPriValueCommand.setPriorityBars.pPriorityLines[_priBarMoveIndex];
        _fCapturing = TRUE;
        _pointCapture = ptPic;
        SetCapture();
    }
    else
    {
        // Ensure things are in-bounds. SCI doesn't like negative lines and such..
        _ClampPoint(ptPic);
        switch (_currentTool)
        {
            case History:
            {
                _OnHistoryLClick(ptPic);
                break;
            }

            case Zoom:
            {
                _OnZoomLClick(&ptPic);
                break;
            }

            case Command:
            {
                if (_transformingCoords && (_currentTool != None))
                {
                    // Hit test and see if we found one
                    int bestDistance = INT_MAX;
                    int bestIndex = -1;
                    bool firstSet = false;
                    _EnsurateCoordinates(_currentCommand,
                        [this, &bestIndex, &bestDistance, &firstSet](ptrdiff_t index, int x, int y, bool first)
                    {
                        int hitTestDistanceSq = _GetTransformHitTestDistance(x, y);
                        if ((hitTestDistanceSq < HitTestDistanceSqThreshold) && (hitTestDistanceSq < bestDistance))
                        {
                            bestDistance = hitTestDistanceSq;
                            bestIndex = index;
                            firstSet = first;
                        }
                        return true;
                    }
                    );
                    if (bestIndex != -1)
                    {
                        _transformCommandMod->Begin(*_GetEditPic(), _currentCommand, bestIndex, firstSet);
                        InvalidateOurselves();
                        _fCapturing = true;
                        _pointCapture = ptPic;
                        SetCapture();
                    }
                }
                else
                {
                    switch (_currentCommand)
                    {
                        case PicCommand::Pattern:
                        {
                            _OnPatternLClick(ptPic);
                            break;
                        }
                        case PicCommand::Line:
                        {
                            _OnLineLClick(ptPic);
                            break;
                        }
                        case PicCommand::Fill:
                        {
                            _OnFillLClick(ptPic);
                            break;
                        }
                        case PicCommand::Circle:
                        {
                            _OnCircleLClick(ptPic);
                            break;
                        }
                    }
                }
                break;
            }

            case Pasting:
            {
                CRect rectPasted;
                _GetPasteRect(rectPasted);
                if (!_pastedCommands.empty() && PtInRect(&rectPasted, CPoint(ptPic.x, ptPic.y)))
                {
                    _fCapturing = TRUE;
                    _pointCapture = ptPic;

                    _xPasteOffsetFromCapture = _xPasteOffset;
                    _yPasteOffsetFromCapture = _yPasteOffset;

                    // Angle where user clicked (in case they're rotating)
                    CRect rc(_xPasted, _yPasted, _xPasted + _cxPasted, _yPasted + _cyPasted);
                    rc.OffsetRect(_xPasteOffset, _yPasteOffset);
                    _iAngleCapture = _iAngle;

                    _fRotating = _NearPasteCorners();

                    SetCapture();
                    InvalidateOurselves();
                }
                break;
            }

            case Polygons:
            {
                _OnPolygonLClick(ScreenResolutionToGameResolution(ptPic));
                break;
            }
        }
    }
}

struct pointf
{
    float x;
    float y;
}; 

bool pnpoly(int nvert, const vector<pointf> &points, float testx, float testy)
{
    bool c = false;
    int i, j;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((points[i].y > testy) != (points[j].y > testy)) &&
            (testx < (points[j].x - points[i].x) * (testy - points[i].y) / (points[j].y - points[i].y) + points[i].x))
            c = !c;
    }
    return c;
}

bool CPicView::_CanBeInPolygons(CPoint pt)
{
    bool canBe = true;
    CPicDoc *pDoc = GetDocument();
    if (pDoc && pDoc->GetShowPolygons())
    {
        const PolygonComponent *polygonComponent = pDoc->GetPolygonComponent();
        if (polygonComponent)
        {
            float xTest = (float)pt.x;
            float yTest = (float)pt.y;
            for (const auto &poly : polygonComponent->Polygons())
            {
                // Turn them into floating point:
                vector<pointf> points;
                transform(poly.Points().begin(), poly.Points().end(), back_inserter(points),
                    [](point16 pointIn) {  pointf temp = { (float)pointIn.x, (float)pointIn.y }; return temp; }
                    );

                canBe = !pnpoly((int)points.size(), points, xTest, yTest);
                if (poly.Type == PolygonType::ContainedAccess)
                {
                    canBe = !canBe;
                }

                if (!canBe)
                {
                    break;
                }
            }
        }
    }
    return canBe;
}

bool CPicView::_EvaluateCanBeHere(CPoint pt)
{
    const BYTE *pdataControl = _GetDrawManager().GetPicBits(PicScreen::Control, PicPosition::PrePlugin, _GetPicSize());
    bool canBe = true;
    if (appState->_fObserveControlLines)
    {
        canBe = CanBeHere(_GetPicSize(), pdataControl, GetViewBoundsRect((uint16_t)pt.x, (uint16_t)pt.y, _GetFakeEgo(), _fakeEgoAttributes.Loop, _fakeEgoAttributes.Cel));
    }

    if (canBe && appState->_fObservePolygons)
    {
        canBe = _CanBeInPolygons(pt);
    }

    return canBe;
}

bool CPicView::_HitTestFakeEgo(CPoint ptPic)
{
    bool fHitTest = false;
    if (!appState->_fUseBoxEgo && _GetFakeEgo())
    {
        fHitTest = HitTestView((uint16_t)ptPic.x, (uint16_t)ptPic.y, (uint16_t)_fakeEgoAttributes.Location.x, (uint16_t)_fakeEgoAttributes.Location.y, _GetFakeEgo(), _fakeEgoAttributes.Loop, _fakeEgoAttributes.Cel);
    }
    else
    {
        fHitTest = HitTestEgoBox((uint16_t)ptPic.x, (uint16_t)ptPic.y, (uint16_t)_fakeEgoAttributes.Location.x, (uint16_t)_fakeEgoAttributes.Location.y, appState->_cxFakeEgo, appState->_cyFakeEgo);
    }
    return fHitTest;
}

//
// Center of ego in pic coords.
//
CPoint CPicView::_FindCenterOfFakeEgo()
{
    if (appState->_fUseBoxEgo)
    {
        return CPoint(_fakeEgoAttributes.Location.x, _fakeEgoAttributes.Location.y - appState->_cyFakeEgo / 2);
    }
    else
    {
        return FindCenterOfView((uint16_t)_fakeEgoAttributes.Location.x, (uint16_t)_fakeEgoAttributes.Location.y, _GetFakeEgo(), _fakeEgoAttributes.Loop, _fakeEgoAttributes.Cel);
    }
}

void CPicView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_fCapturing)
    {
        assert((_fShowingEgo && (_currentTool == None)) || (_currentTool == Pasting) || _transformingCoords || (_originalPriValue != -1) || (_polyDragPointIndex != -1));
        _fCapturing = FALSE;
        ReleaseCapture();

        CPoint ptPic = _MapClientPointToPic(point);
        if (_polyDragPointIndex != -1)
        {
            // Commit this point to the polygon
            _EndPolyDrag();
        }
        else if (_transformingCoords && (_currentTool != None))
        {
            // Commit the change
            _TransformPoint(true, ptPic);
            _transformCommandMod->Reset();
        }
        else if (_fShowPriorityLines && (_originalPriValue != -1))
        {
            _MovePriorityBar(true, ptPic.y - _pointCapture.y);
        }
        _originalPriValue = -1;
    }
}

void CPicView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CPoint ptPic = _MapClientPointToPic(point);

    // Don't do anything if this is off the picture.
    if ((ptPic.x >= _GetPicSize().cx) || (ptPic.y >= _GetPicSize().cy))
    {
        return;
    }

    switch (_currentTool)
    {
    case Zoom:
        {
            // Zoom in on the 2nd click too
            _OnZoomLClick(&ptPic);
            break;
        }

    case Pasting:
        {
            _InsertPastedCommands();
            // Get out of pasting mode:
            _currentTool = None;
            _UpdateCursor();
            _OnCommandChanged();
            _GetDrawManager().InvalidatePlugins();
            InvalidateOurselves();
        }

    case Command:
        {
            // Perform pattern and line commands on the 2nd click too
            switch(_currentCommand)
            {
            case PicCommand::Pattern:
                {
                    _OnPatternLClick(ptPic);
                    break;
                }
            case PicCommand::Line:
                {
                    _OnLineLClick(ptPic);
                    break;
                }
            }
            break;
        }

    case None:
        {
            if (_fShowingEgo)
            {
                // The ego is showing - double click centers it around the mouse again.
                _fakeEgoAttributes.Location = ptPic;
                _fakeEgoAttributes.Location.y += appState->_cyFakeEgo / 2;
                _fCanBeHere = _EvaluateCanBeHere(_fakeEgoAttributes.Location);
                appState->_ptFakeEgo = _fakeEgoAttributes.Location;
                InvalidateOurselves();
            }
            break;
        }

    }
}


void CPicView::OnRButtonDown(UINT nFlags, CPoint point)
{
    CPoint ptPic = _MapClientPointToPic(point);

    // Don't do anything if this is off the picture.
    if ((ptPic.x >= _GetPicSize().cx) || (ptPic.y >= _GetPicSize().cy) || (ptPic.x < 0) || (ptPic.y < 0))
    {
        return;
    }

    switch (_currentTool)
    {
    case History:
        {
            _OnHistoryRClick(ptPic);
            break;
        }
    case Zoom:
        {
            _OnZoomRClick(&ptPic);
            break;
        }

    case Command:
        {
            if (_transformingCoords && (_currentTool != None))
            {
                // If we're on point, offer the option of deleting it
                // Hit test and see if we found one
                int bestDistance = INT_MAX;
                int bestIndex = -1;
                bool firstSet = false;
                _EnsurateCoordinates(_currentCommand,
                    [this, &bestIndex, &bestDistance, &firstSet](ptrdiff_t index, int x, int y, bool first)
                {
                    int hitTestDistanceSq = _GetTransformHitTestDistance(x, y);
                    if ((hitTestDistanceSq < HitTestDistanceSqThreshold) && (hitTestDistanceSq < bestDistance))
                    {
                        bestDistance = hitTestDistanceSq;
                        bestIndex = index;
                        firstSet = first;
                    }
                    return true;
                }
                );
                if (bestIndex != -1)
                {
                    _transformCommandMod->Begin(*_GetEditPic(), _currentCommand, bestIndex, firstSet);

                    CMenu contextMenu;
                    contextMenu.LoadMenu(IDR_MENUDELETEPOINT);
                    CMenu* pTracker;
                    pTracker = contextMenu.GetSubMenu(0);
                    CPoint ptScreen = point;
                    ClientToScreen(&ptScreen);
                    if (pTracker)
                    {
                        BOOL fRet = pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
                            ptScreen.x, ptScreen.y, AfxGetMainWnd());
                        InvalidateOurselves();
                    }
                    // We return from the menu before the command is issued. If we specify TPM_RETURNCMD though, then the command
                    // is not issued (we'd need to to it ourselves). Is it ok to not reset?
                    //_transformCommandMod.Reset();
                }
            }
            else
            {
                switch (_currentCommand)
                {
                    case PicCommand::Line:
                    {
                        _OnLineRClick(ptPic);
                        break;
                    }
                    case PicCommand::Pattern:
                    {
                        _OnPatternRClick();
                        break;
                    }
                    case PicCommand::Circle:
                    {
                        _OnCircleRClick(ptPic);
                        break;
                    }
                }
            }
            break;
        }

    case Pasting:
        {
            // On right click, stop pasting.
            _CleanUpPaste();
            _currentTool = None;
            _UpdateCursor();
            _OnCommandChanged();
            InvalidateOurselves();
            break;
        }

    case Polygons:
    {
        _OnPolygonRClick(ScreenResolutionToGameResolution(ptPic));
        break;
    }

    case None:
        {
            if (_fShowingEgo && _HitTestFakeEgo(ptPic))
            {
                // Bring up a menu that let's us choose the ego's priority.
                CMenu contextMenu; 
                contextMenu.LoadMenu(IDR_MENUFAKEEGO); 
                CMenu* pTracker; 
                pTracker = contextMenu.GetSubMenu(0); 
                CPoint ptScreen = point;
                ClientToScreen(&ptScreen);
                if (pTracker)
                {
                    BOOL fRet = pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
                                                         ptScreen.x, ptScreen.y, AfxGetMainWnd()); 
                    InvalidateOurselves();
                }
            }
            else
            {
                // Bring up some generic menu things.
                CMenu contextMenu;
                contextMenu.LoadMenu(IDR_MENUPICBG);
                CMenu* pTracker;
                pTracker = contextMenu.GetSubMenu(0);
                CPoint ptScreen = point;
                ClientToScreen(&ptScreen);
                if (pTracker)
                {
                    BOOL fRet = pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
                        ptScreen.x, ptScreen.y, AfxGetMainWnd());
                }
            }
            break;
        }
    }
}


void CPicView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    CPoint ptPic = _MapClientPointToPic(point);

    // Don't do anything if this is off the picture.
    if ((ptPic.x >= _GetPicSize().cx) || (ptPic.y >= _GetPicSize().cy))
    {
        return;
    }

    switch (_currentTool)
    {
    case Zoom:
        {
            // Zoom in on the 2nd click too
            _OnZoomRClick(&ptPic);
            break;
        }

    case Command:
        {
            switch(_currentCommand)
            {
            case PicCommand::Pattern:
                {
                    // Do the same as R-click
                    _OnPatternRClick();
                }
                break;
            }
            break;
        }
    }
}


void CPicView::_OnMouseWheel(UINT nFlags, BOOL fForward, CPoint pt, short nNotches)
{
    // This is cool.
    if (nFlags & MK_CONTROL)
    {
        // Shift held down.  Do a zoom:
        if (fForward)
        {
            _OnZoomLClick();
        }
        else
        {
            _OnZoomRClick();
        }
    }

    // REVIEW: Disabled horizontal for now.
    if (nFlags == 0)
    {
        // No keys held down.  Do a scroll by 5 pics coordinates.
        int iAmount = 5 * (_cxPic / _GetPicSize().cx) * nNotches;

        // Which scroll bar?  Which one are we closest to?
        RECT rc;
        GetClientRect(&rc);
        _ScrollDelta(SB_VERT, m_hWnd, fForward ? -iAmount : iAmount);
    }
}

//
// This just controls the cel/loop of the fake ego.
//
void CPicView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL fHandled = TRUE;
    if (_currentTool == Pasting)
    {
        // TODO: temporary, until there is UI
        switch (nChar)
        {
        case VK_RIGHT:
            {
                _cxPastedScale++;
                break;
            }
        case VK_LEFT:
            {
                _cxPastedScale--;
                break;
            }
        case VK_UP:
            {
                _cyPastedScale--;
                break;
            }
        case VK_DOWN:
            {
                _cyPastedScale++;
                break;
            }
        case VK_NUMPAD4:
        case VK_NUMPAD6:
            _fVFlip = !_fVFlip;
            break;
        case VK_NUMPAD8:
        case VK_NUMPAD2:
            _fHFlip = !_fHFlip;
            break;

        case VK_NUMPAD7:
            _iAngle -= 5;
            break;
        case VK_NUMPAD9:
            _iAngle += 5;
            break;

        default:
            {
                fHandled = FALSE;
                break;
            }
        }
        if (fHandled)
        {
            _GetDrawManager().InvalidatePlugins();
            InvalidateOurselves();
        }
    }
    else
    {
        switch (nChar)
        {
        case VK_RIGHT:
            {
                _fakeEgoAttributes.Cel++;
                break;
            }
        case VK_LEFT:
            {
                _fakeEgoAttributes.Cel--;
                break;
            }
        case VK_UP:
            {
                _fakeEgoAttributes.Loop--;
                break;
            }
        case VK_DOWN:
            {
                _fakeEgoAttributes.Loop++;
                break;
            }
        default:
            {
                fHandled = FALSE;
                break;
            }
        }
        if (fHandled && _fShowingEgo)
        {
            // We changed cel/loop
            _GetDrawManager().InvalidatePlugins();
            InvalidateOurselves();
        }
    }

    if (!fHandled)
    {
        __super::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}


// CPicView printing

BOOL CPicView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CPicView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CPicView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

size16 CPicView::_GetPicSize() const
{
    size16 size = size16(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT);
    const PicComponent *pic = _GetEditPic();
    if (pic)
    {
        size = pic->Size;
    }
    return size;
}

const PicComponent *CPicView::_GetEditPic() const
{
    const PicComponent *ppic = nullptr;
    CPicDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (pDoc)
    {
        ppic = pDoc->GetPic();
    }
    return ppic;
}

const PaletteComponent *CPicView::_GetPalette()
{
    const PaletteComponent *ppalette = nullptr;
    CPicDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (pDoc)
    {
        ppalette = pDoc->GetCurrentPaletteComponent();
    }
    return ppalette;
}

// CPicView diagnostics

#ifdef _DEBUG
void CPicView::AssertValid() const
{
    __super::AssertValid();
}

void CPicView::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}

CPicDoc* CPicView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPicDoc)));
    return (CPicDoc*)m_pDocument;
}
#endif //_DEBUG





// CPicView message handlers
