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
    { None, ID_DRAWOFF },
    // Add more here...
};

CExtAlphaSlider *g_pPicAlphaSlider = NULL;

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
    ON_COMMAND(IDC_TOGGLECONTROL, CPicView::OnToggleControl)
    ON_COMMAND(ID_SHOWVISUALSCREEN, CPicView::OnShowVisual)
    ON_COMMAND(ID_SHOWPRIORITYSCREEN, CPicView::OnShowPriority)
    ON_COMMAND(ID_SHOWCONTROLSCREEN, CPicView::OnShowControl)
    ON_COMMAND(ID_SHOWPALETTE0, CPicView::OnShowPalette0)
    ON_COMMAND(ID_SHOWPALETTE1, CPicView::OnShowPalette1)
    ON_COMMAND(ID_SHOWPALETTE2, CPicView::OnShowPalette2)
    ON_COMMAND(ID_SHOWPALETTE3, CPicView::OnShowPalette3)
    ON_COMMAND(ID_TOGGLEPRIORITYLINES, CPicView::OnTogglePriorityLines)
    ON_COMMAND(ID_TOGGLEEGO, CPicView::OnToggleEgo)
    ON_COMMAND(ID_LIGHTUPCOORDS, CPicView::OnLightUpCoords)
    ON_COMMAND(ID_DEFINEPALETTES, CPicView::OnSetPalette)
    ON_COMMAND(ID_EDIT_COPY, CPicView::OnCopyPic)
    ON_COMMAND(ID_EDIT_PASTE, CPicView::OnPaste)
    ON_COMMAND(ID_EDIT_PASTEINTOPIC, CPicView::OnPasteIntoPic)
    ON_COMMAND(ID_VIEW_ZOOMIN, CPicView::OnZoomIn)
    ON_COMMAND(ID_VIEW_ZOOMOUT, CPicView::OnZoomOut)
    ON_COMMAND(ID_OBSERVECONTROLLINES, CPicView::OnObserveControlLines)
    ON_COMMAND(ID_PIC_EXPORT8, CPicView::OnExportPalettizedBitmap)
    ON_COMMAND(ID_PIC_EDITPALETTE, CPicView::EditVGAPalette)
    ON_COMMAND(ID_PIC_DELETEPOINT, CPicView::OnDeletePoint)
    
    ON_COMMAND_RANGE(ID_DEFAULTPRIORITY, ID_MAIN_PRI15, CPicView::OnSetEgoPriority)
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
    ON_UPDATE_COMMAND_UI(ID_TOGGLEPRIORITYLINES, CPicView::OnUpdateTogglePriorityLines)
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
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEINTOPIC, CPicView::OnUpdateIsGDIPAvailable)
    ON_UPDATE_COMMAND_UI(ID_DEFAULTPRIORITY, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI_RANGE(ID_DEFAULTPRIORITY, ID_MAIN_PRI15, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_ZOOMSLIDER, CPicView::OnCommandUIAlwaysValid)
    ON_UPDATE_COMMAND_UI(ID_OBSERVECONTROLLINES, CPicView::OnUpdateObserveControlLines)
    ON_UPDATE_COMMAND_UI(ID_PIC_EXPORT8, CPicView::OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_PIC_EDITPALETTE, CPicView::OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_PIC_DELETEPOINT, CPicView::OnCommandUIAlwaysValid)  // Since it's in a context menu we only bring up when it's available.
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
    _priBarMoveIndex = -1;
    memset(&_originalPriValueCommand, 0, sizeof(_originalPriValueCommand));

    _isDrawUpToDate = PicScreenFlags::None;

    _attachedPlugin = false;

    _currentCommand = PicCommand::Pattern; // just something
    _currentTool = None;
    _fShowTraceImage = false;
    _fHaveTraceImage = false;
    _fShowPriorityLines = FALSE;
    _fShowingEgo = FALSE;
    _nFakeCel = 0;
    _nFakeLoop = 0;
    _nFakePri = -1;
    _fCapturing = FALSE;
    _pointCapture.x = 0;
    _pointCapture.y = 0;
    _pointEgo.x = 140; // Good starting values.
    _pointEgo.y = 80;
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
    _cyPic = sPIC_HEIGHT * 2;
    _cxPic = sPIC_WIDTH * 2;

    // Start with the visual screen.
    _mainViewScreen = PicScreen::Visual;

    _fRegenerateTrace = false;
    _bRandomNR = 0;
    _transformingCoords = false;
    _iTraceAlpha = 50;
}

PicDrawManager &CPicView::_GetDrawManager()
{
    if (_pPreview)
    {
        return _pPreview->GetDrawManager();
    }
    else
    {
        return GetDocument()->GetDrawManager();
    }
}

CPicView::~CPicView()
{
    _CleanUpPaste();
}

//
// Given a screen coordinate, returns the coordinate in the pic.
// It assumes origins have been taken into account, so ptScreen
// is not the physical viewport point, but the point within the whole thing.
//
CPoint CPicView::_MapScreenPointToPic(CPoint ptScreen)
{
    return CPoint(ptScreen.x * sPIC_WIDTH / _cxPic,
                  ptScreen.y * sPIC_HEIGHT / _cyPic);
}

//
// This one takes into account origins
//
CPoint CPicView::_MapClientPointToPic(CPoint ptScreen)
{
    return CPoint((ptScreen.x + _xOrigin) * sPIC_WIDTH / _cxPic,
                  (ptScreen.y + _yOrigin) * sPIC_HEIGHT / _cyPic);
}

//
// From pic coordinates to window client coordinates
// (taking into account origins)
//
CPoint CPicView::_MapPicPointToClient(CPoint ptPic)
{
    return CPoint((ptPic.x * _cxPic / sPIC_WIDTH) - _xOrigin,
                  (ptPic.y * _cyPic / sPIC_HEIGHT) - _yOrigin);
}

void CPicView::_MapPicRectToScreen(RECT *prcPic, RECT *prcScreen)
{
    int cxScreen = RECTWIDTH(*prcPic) * _cxPic / sPIC_WIDTH;
    int cyScreen = RECTHEIGHT(*prcPic) * _cyPic / sPIC_HEIGHT;
    int x = prcPic->left *  _cxPic / sPIC_WIDTH - _xOrigin;
    int y = prcPic->top *  _cyPic / sPIC_HEIGHT - _yOrigin;
    prcScreen->left = x;
    prcScreen->top = y;
    prcScreen->right = x + cxScreen;
    prcScreen->bottom = y + cyScreen;
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

void CPicView::OnSetEgoPriority(UINT nID)
{
    if (nID == ID_DEFAULTPRIORITY)
    {
        _nFakePri = -1;
    }
    else
    {
        _nFakePri = (nID - ID_MAIN_PRI0);
    }
}

void CPicView::OnObserveControlLines()
{
    appState->_fObserveControlLines = !appState->_fObserveControlLines;
}

void CPicView::OnDeletePoint()
{
    GetDocument()->ApplyChanges<PicComponent>(
        [this](PicComponent &pic)
    {
        _transformCommandMod.Delete(pic);
        return WrapHint(PicChangeHint::EditPicInvalid);
    }
    );
    _transformCommandMod.Reset();
}

void CPicView::EditVGAPalette()
{
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

        PaletteComponent paletteCopy = *palette; // Since we'll be changing it, and this is const
        PaletteEditorDialog paletteEditor(this, paletteCopy, cels, true);

        GetDocument()->SetPreviewPalette(&paletteCopy);

        paletteEditor.DoModal();
        PaletteComponent paletteResult = paletteEditor.GetPalette();
        if (paletteResult != *palette)
        {
            GetDocument()->ApplyChanges<PicComponent, PaletteComponent>(
                [&paletteResult](PicComponent &pic, PaletteComponent &palette)
            {
                // Replace the palette with the new one.
                palette = paletteResult;
                return WrapHint(PicChangeHint::Palette);
            }
            );
        }

        GetDocument()->SetPreviewPalette(nullptr);
    }
}

// Default to bmp files by default, since those are the only ones that have consistent behavior end-to-end.
extern char g_szGdiplus8BitSaveFilter[] = "Bitmap files(*.bmp)|*.bmp|GIF files(*.gif)|*.gif|PNG files(*.png)|*.png|TIFF files(*.tif;*.tiff)|*.tif;*.tiff|All Files|*.*|";

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
                CFileDialog fileDialog(FALSE, ".bmp", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, g_szGdiplus8BitSaveFilter);
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

void CPicView::OnUpdateIsVGA(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(_GetEditPic() && _GetEditPic()->Traits.IsVGA);
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
        if ((_pgdiplusTrace->GetWidth() != sPIC_WIDTH) || (_pgdiplusTrace->GetHeight() != sPIC_HEIGHT))
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
    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY, g_szGdiplusFilter, this);
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

void CPicView::OnToggleEgo()
{
    _fShowingEgo = !_fShowingEgo;
    if (_fShowingEgo)
    {
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

void CPicView::OnSomethingChanged(BOOL fWriteEntire, EGACOLOR *pPalettes, int iPalette)
{
    _pPreview.reset(); // Reset to NULL before making a new one, since we rely on
                       // getting the "real" draw manager and pic resource.
    INT_PTR iRealPos = _GetDrawManager().GetPos();
    _pPreview.reset(new PaletteChangePreview(_GetEditPic(), _GetPalette(), _GetDrawManager()));
    _pPreview->GetDrawManager().SetPalette(static_cast<BYTE>(iPalette));
    // Insert the commands at the current spot:
    _pPreview->GetDrawManager().SeekToPos(iRealPos);
    // Grab the palette state from the actual pic:
    const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
    InsertPaletteCommands(&_pPreview->GetPicResource(), _pPreview->GetDrawManager().GetPos(), pstate->pPalettes, pPalettes, fWriteEntire);
    _pPreview->GetDrawManager().SeekToPos(-1); // Move to the end for the preview...
    InvalidateOurselves();
}

void CPicView::OnPreviewOff()
{
    _pPreview.reset();
    InvalidateOurselves();
}

void CPicView::OnSetPalette()
{
    if (_GetEditPic()->Traits.IsVGA)
    {
        EditVGAPalette();
    }
    else
    {
        PicDrawManager &pdm = _GetDrawManager();
        CPaletteDefinitionDialog dialog;
        const ViewPort *pstate = pdm.GetViewPort(PicPosition::PostPlugin);

        // For now, just do palette 0
        dialog.SetCallback(this);
        dialog.InitPalettes(pstate->pPalettes);
        INT_PTR iDialogResult = dialog.DoModal();
        _pPreview.reset();
        if (IDOK == iDialogResult)
        {
            EGACOLOR paletteNew[160];
            dialog.RetrievePalettes(paletteNew);
            InsertPaletteCommands(GetDocument()->GetPic(), pdm.GetPos(), pstate->pPalettes, paletteNew, dialog.GetWriteEntirePalette());
        }
        InvalidateOurselves(); // In case we didn't insert anything (to refresh from possible preview)
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
        PastedCommands_Adjust(_pastedCommands, &adjust);

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
    rect.right = min(sPIC_WIDTH, rect.right);
    rect.top = _yPasted + _yPasteOffset;
    rect.top = max(0, rect.top);
    rect.bottom = _yPasted + _yPasteOffset + _cyPastedScale;
    rect.bottom = min(sPIC_HEIGHT, rect.bottom);
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
            // Create a byte stream with this data.
            sci::ostream stream;
            stream.WriteBytes(pBits, (int)cb);
            sci::istream reader = istream_from_ostream(stream);
            bool fOk = true;
            while (fOk)
            {
                PicCommand command;
                fOk = command.Initialize(reader);
                if (fOk)
                {
                    _pastedCommands.push_back(command);
                }
            }

            sRECT rcPasted;
            PastedCommands_GetBounds(&_pastedCommands[0], _pastedCommands.size(), &rcPasted);
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
    if (_GetEditPic()->Traits.IsVGA)
    {
        CBitmapToVGADialog dialog(_GetPalette(), _GetEditPic()->Traits.AllowMultipleBitmaps);
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
                            void *pDIBBits24 = nullptr;;
                            CBitmap bitmap;
                            bitmap.Attach(BitmapToRGB24Bitmap(pbmi, 320, 190, &pDIBBits24));
                            globalLock.Unlock();    // Unlock and close ASAP, before we do the expensive quantization
                            openClipboard.Close();


                            if (_GetEditPic()->Traits.IsVGA)
                            {
                                const PaletteComponent *globalPalette = appState->GetResourceMap().GetPalette999();

                                GetDocument()->ApplyChanges<PicComponent, PaletteComponent>(
                                    [globalPalette, pDIBBits24](PicComponent &pic, PaletteComponent &palette)
                                {
                                    std::unique_ptr<uint8_t[]> sciBits = QuantizeImage(pDIBBits24, 320, 320 * 3, 190, globalPalette->Colors, palette.Colors, 255);
                                    // We just directly modified the palette.
                                    PicCommand command;
                                    Cel cel;
                                    cel.placement = point16(0, 0);
                                    cel.size = size16(320, 190);
                                    cel.TransparentColor = 255;
                                    cel.Data.allocate(PaddedSize(cel.size));
                                    cel.Data.assign(&sciBits.get()[0], &sciBits.get()[cel.Data.size()]);
                                    command.CreateDrawVisualBitmap(cel, true);
                                    _ReplaceDrawVisual(pic, cel);
                                    return WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::Palette);
                                });

                            }
                            else
                            {
                                void *pDIBBits = _GetBitsPtrFromBITMAPINFO(pbmi);
                                if (pDIBBits)
                                {
                                    _MakeNewMasterTraceImage(NULL, pbmi, pDIBBits);
                                }
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
    TCHAR szText[50];
    szText[0] = 0;
    if (pCmdUI->m_nID == ID_INDICATOR_COORDS)
    {
        int x = min(_ptCurrentHover.x, 319);
        int y = min(_ptCurrentHover.y, 189);
        pCmdUI->Enable(); 
        StringCchPrintf(szText, ARRAYSIZE(szText), "%3d,%3d", x, y);
        pCmdUI->SetText(szText);
    }
    else if (pCmdUI->m_nID == ID_INDICATOR_PRI)
    {
        int y = min(_ptCurrentHover.y, 189);
        pCmdUI->Enable();
        const ViewPort *pstate = _GetDrawManager().GetViewPort(PicPosition::PostPlugin);
        StringCchPrintf(szText, ARRAYSIZE(szText), "Pri bar: %2d", PriorityFromY((uint16_t)y, *pstate));
        pCmdUI->SetText(szText);
    }
    else if ((_ptCurrentHover.x < sPIC_WIDTH) && (_ptCurrentHover.y < sPIC_HEIGHT))
    {
        const uint8_t *picBits = _GetDrawManager().GetPicBits(_mainViewScreen, PicPosition::PrePlugin);
        uint8_t bColor = *(picBits + BUFFEROFFSET(_ptCurrentHover.x, _ptCurrentHover.y));
        if ((pCmdUI->m_nID == ID_INDICATOR_CONTROLCOLOR) && (_mainViewScreen == PicScreen::Control))
        {
            StringCchCopy(szText, ARRAYSIZE(szText), c_rgControlColourNames[bColor % 16]);
        }
        else if ((pCmdUI->m_nID == ID_INDICATOR_PRICOLOR) && (_mainViewScreen == PicScreen::Priority))
        {
            StringCchPrintf(szText, ARRAYSIZE(szText), "Priority: %3d", bColor);
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

    // Always enabled
    pCmdUI->Enable();
}

void CPicView::OnUpdateIsEGA(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!_GetEditPic()->Traits.IsVGA);
}

void CPicView::OnUpdateShowPaletteControl(CCmdUI *pCmdUI)
{
    if (_GetEditPic()->Traits.IsVGA)
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
    pCmdUI->Enable();
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
            pt.y = _yOld + sgn(dy) * d;
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

    if ((_currentTool == Command) && (appState->_fGridLines))
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
                _pointEgo = ptNew;
                appState->_ptFakeEgo = _pointEgo;
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
        enabled = _GetEditPic()->Traits.SupportsPenCommands;
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
    int iZoom = min(rect.Width() / sPIC_WIDTH, rect.Height() / sPIC_HEIGHT);
    iZoom = max(iZoom, 2);
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

void CPicView::_EnsureDoubleBuffer(PicScreen screen)
{
    CBitmap &bitmap = _bitmapDoubleBuf;
    if ((HBITMAP)bitmap == nullptr)
    {
        CDC *pDC = this->GetDC();
        bitmap.CreateCompatibleBitmap(pDC, sPIC_WIDTH, sPIC_HEIGHT);
        this->ReleaseDC(pDC);
    }
}

void CPicView::_GenerateTraceImage(CDC *pDC)
{
    _bitmapTrace.DeleteObject();
    
    // Take our master copy, and generate a 320x190 version of it.
    if (_pgdiplusTrace)
    {
        _bitmapTrace.CreateCompatibleBitmap(pDC, sPIC_WIDTH, sPIC_HEIGHT);

        // Now copy the trace image into it.
        CDC dcDest;
        if (dcDest.CreateCompatibleDC(pDC))
        {
            HGDIOBJ hgdiObj = dcDest.SelectObject(_bitmapTrace);
            Graphics graphics((HDC)dcDest);
            if (appState->_fScaleTracingImages)
            {
                graphics.DrawImage(_pgdiplusTrace.get(), 0, 0, sPIC_WIDTH, sPIC_HEIGHT);
            }
            else
            {
                int cx = min(sPIC_WIDTH, _pgdiplusTrace->GetWidth());
                int cy = min(sPIC_HEIGHT, _pgdiplusTrace->GetHeight());
                graphics.DrawImage(_pgdiplusTrace.get(), 0, 0, 0, 0, cx, cy, Gdiplus::UnitPixel);
            }
            dcDest.SelectObject(hgdiObj);

            _fRegenerateTrace = false;
        }
    }
}

void CPicView::_DrawEgoCoordinates(CDC *pDC)
{
    if ((appState->_fUseBoxEgo && (_nFakePri != -1)) || (!appState->_fUseBoxEgo && appState->GetSelectedViewResource()))
    {
        // The cursor is over the ego.  Draw the coordinates.
        // First we need to convert the ego coordinates to "client" coordinates
        CPoint ptEgoClient = _MapPicPointToClient(_pointEgo);
        TCHAR szCoords[20];
        if (appState->_fUseBoxEgo)
        {
            ASSERT(_nFakePri != -1);
            // The only time we draw anything for "box" egos is if the user set a pri.
            StringCchPrintf(szCoords, ARRAYSIZE(szCoords), TEXT("pri %d"), _nFakePri);
        }
        else
        {
            if (_nFakePri == -1)
            {
                StringCchPrintf(szCoords, ARRAYSIZE(szCoords), TEXT("(%d,%d)"), _pointEgo.x, _pointEgo.y);
            }
            else
            {
                StringCchPrintf(szCoords, ARRAYSIZE(szCoords), TEXT("(%d,%d) - pri %d"), _pointEgo.x, _pointEgo.y, _nFakePri);
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
            pDC->AlphaBlend(0, yPriLine, sPIC_WIDTH, 2, &dcMem2, 0, i, 1, 2, blendFunc);
        }
        dcMem2.SelectObject(hgdiObj2);
    }
}

PicCommand CPicView::_EnsurePriorityBarCommand()
{
    assert(_GetEditPic()->Traits.CanChangePriorityLines);
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
        setPriBarsCommand.CreateSetPriorityBars(g_defaultPriBands16Bit, pic.Traits.SixteenBitPri, pic.Traits.IsVGA);
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
    int maxBound = (_priBarMoveIndex < (priLineCount - 1)) ? priLines[_priBarMoveIndex + 1] : (sPIC_HEIGHT - 1);
    newPriValue = max(minBound, min(maxBound, newPriValue));

    PicCommand newPriBarsCommand = _originalPriValueCommand;
    newPriBarsCommand.setPriorityBars.pPriorityLines[_priBarMoveIndex] = newPriValue;

    // Find the pri bar command
    vector<PicCommand> &commands = GetDocument()->GetPic()->commands;
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
    CRect rect(sPIC_WIDTH, 0, sPIC_WIDTH + PriorityBarWidth, sPIC_HEIGHT);
    bool hit = !!rect.PtInRect(pt);
    if (hit && _GetEditPic()->Traits.CanChangePriorityLines)
    {
        const uint16_t *priLines = _GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines;
        size_t priLineCount = ARRAYSIZE(_GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines);
        // Hit on current line and previous
        for (size_t i = 0; i < priLineCount; i++)
        {
            int priLine = (i < priLineCount) ? priLines[i] : sPIC_HEIGHT;
            if ((pt.y >= (priLine - 1)) && (pt.y <= priLine))
            {
                *barIndex = (int)i;
                break;
            }
        }
    }
    return hit;
}

void CPicView::_DrawPriorityBar(CDC *pDC)
{
    RECT rcClient;
    GetClientRect(&rcClient);

    const uint16_t *priLines = _GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines;
    size_t priLineCount = ARRAYSIZE(_GetDrawManager().GetViewPort(PicPosition::PostPlugin)->bPriorityLines);
    int zoom = _cxPic / sPIC_WIDTH;

    size_t i = 0;
    int previousPriorityY = -1;
    for (; i <= priLineCount; i++, previousPriorityY++)
    {
        int priLine = (i < priLineCount) ? priLines[i] : sPIC_HEIGHT;
        priLine--;
        CBrush brush(g_egaColorsCR[i]);
        CRect rect(CPoint(_cxPic - _xOrigin, previousPriorityY * zoom - _yOrigin), CSize(PriorityBarWidth, zoom * (priLine - previousPriorityY + 1)));
        pDC->FillRect(&rect, &brush);
        previousPriorityY = priLine;
    }
}

CPoint CPicView::GetCursorPos()
{
    return _ptCurrentHover;
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
    _EnsureDoubleBuffer(screen);

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

int g_testLeft = 0;

void CPicView::OnDraw(CDC *pDC)
{
    _AttachPicPlugin();

    RECT rcClient;
    GetClientRect(&rcClient);
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        int cxVisible = min(_cxPic, RECTWIDTH(rcClient));
        int cyVisible = min(_cyPic, RECTHEIGHT(rcClient));

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
                    dcMem.AlphaBlend(0, 0, sPIC_WIDTH, sPIC_HEIGHT, &dcMem2, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, blendFunc);
                    dcMem2.SelectObject(hgdiObj2);
                }
            }

            if (_fMouseWithin &&  appState->_fGridLines && (_currentTool == Command))
            {
                // The mouse is within the window, and we are currently drawing.
                // If the user wants guide lines, give them to him.
                _DrawGuideLines(dcMem);
            }

            if (_fShowingEgo && _GetEditPic()->Traits.IsVGA)
            {
                _DrawShowingEgoVGA(dcMem, _GetDrawManager());
            }

            if (_fShowPriorityLines)
            {
                _DrawPriorityLines(&dcMem);
            }
        }

        // Now blt back to the real DC.
        // Now we want to copy it back to the real dc.
        pDC->StretchBlt(-_xOrigin, -_yOrigin, _cxPic, _cyPic, &dcMem, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, SRCCOPY); 

        dcMem.SelectObject(hgdiObj);
    }

    // TODO: There is a painting bug where sometimes switching between the view/priority screens (or control)
    // doesn't redraw the view (until we mouse over it and redraw). Paint is called, the clip region seems fine,
    // but nothing we draw here appears onscreen. It isn't a problem with the underlying v/p/c buffers, because
    // not even the following DrawText will show up.
    /*
    g_testLeft += 10;
    g_testLeft %= 100;
    CRect testText = { g_testLeft, 70, 200, 30 };
    pDC->SetTextColor(RGB(0, 0, 0));
    int result = pDC->DrawText("TEST", -1, &testText, DT_LEFT | DT_NOCLIP | DT_SINGLELINE);
    OutputDebugString(fmt::format("DT result:{0}\n", (int)result).c_str());
    */

    int picRightEdge = _cxPic;
    if (_fShowPriorityLines)
    {
        _DrawPriorityBar(pDC);
        picRightEdge += PriorityBarWidth;
    }

    // Finish off by drawing a navy background around the pic.
    CBrush brush(RGB(0, 0, 128));
    int cyBottom = RECTHEIGHT(rcClient) - _cyPic;
    if (cyBottom > 0)
    {
        CRect rect1(CPoint(0, _cyPic), CSize(RECTWIDTH(rcClient), cyBottom));
        pDC->FillRect(&rect1, &brush);
    }
    int cxLeft = RECTWIDTH(rcClient) - picRightEdge;
    if (cxLeft > 0)
    {
        CRect rect2(CPoint(picRightEdge, 0), CSize(cxLeft, RECTHEIGHT(rcClient)));
        pDC->FillRect(&rect2, &brush);
    }

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
                _transformCommandMod.GetTransformCoord(*_GetEditPic(), xBest, yBest);
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

void CPicView::_DrawShowingEgoWorker(const ViewPort &viewPort, uint8_t *pdataVisual, const uint8_t *pdataPriority, PicScreenFlags flags)
{
    BYTE bEgoPriority = (_nFakePri) == -1 ? PriorityFromY((uint16_t)_pointEgo.y, viewPort) : (BYTE)_nFakePri;
    // Now we need to draw a box (or whatever), but only where
    // the bEgoPriority is equal to or greater than the priority
    // of the priority screen.
    if (pdataPriority)
    {
        // Alright, we're part way there.
        if (!appState->_fUseBoxEgo && appState->GetSelectedViewResource())
        {
            // Draw a view.
            DrawViewWithPriority(pdataVisual, pdataPriority, bEgoPriority, (uint16_t)_pointEgo.x, (uint16_t)_pointEgo.y, appState->GetSelectedViewResource(), _nFakeLoop, _nFakeCel, _HitTestFakeEgo(_ptCurrentHover));
        }
        else
        {
            // Just draw a box
            DrawBoxWithPriority(pdataVisual, pdataPriority, bEgoPriority, (uint16_t)_pointEgo.x, (uint16_t)_pointEgo.y, appState->_cxFakeEgo, appState->_cyFakeEgo);
        }
    }
}

void CPicView::_DrawShowingEgoEGA(ViewPort &viewPort, PicData &picData, PicScreenFlags flags)
{
    // EGA uses the same palette for display, control and priority. Easy.
    _DrawShowingEgoWorker(viewPort, picData.pdataVisual, picData.pdataPriority, flags);
}

void CPicView::_DrawShowingEgoVGA(CDC &dc, PicDrawManager &pdm)
{
    const PaletteComponent *palette = _GetPalette();
    ResourceEntity *fakeEgo = appState->GetSelectedViewResource();
    if (fakeEgo && palette)
    {
        CDC dcMem;
        if (dcMem.CreateCompatibleDC(&dc))
        {
            // Prepare a pic-sized buffer to draw the ego into 
            const Cel &cel = GetCel(fakeEgo, _nFakeLoop, _nFakeCel);

            // VGA display screen uses a different palette than priority or control.
            // When drawing the ego, we don't want to be limited by the palette of the screen
            // we're currently showing.
            
            SCIBitmapInfo bmi(sPIC_WIDTH, sPIC_HEIGHT, (appState->_fUseBoxEgo ? g_egaColorsPlusOne : palette->Colors), appState->_fUseBoxEgo ? ARRAYSIZE(g_egaColorsPlusOne) : ARRAYSIZE(palette->Colors));
            uint8_t transparentColor = cel.TransparentColor;
            RGBQUAD transparentRGB = palette->Colors[cel.TransparentColor];
            if (appState->_fUseBoxEgo)
            {
                transparentColor = 16;
                transparentRGB = g_egaColorsPlusOne[transparentColor];
            }

            uint8_t *pBitsDest;
            CBitmap bitmap;
            bitmap.Attach(CreateDIBSection((HDC)dcMem, &bmi, DIB_RGB_COLORS, (void**)&pBitsDest, nullptr, 0));
            if ((HBITMAP)bitmap)
            {
                memset(pBitsDest, transparentColor, sPIC_WIDTH * sPIC_HEIGHT);
                _DrawShowingEgoWorker(*_GetDrawManager().GetViewPort(PicPosition::PostPlugin), pBitsDest, _GetDrawManager().GetPicBits(PicScreen::Priority, PicPosition::PostPlugin), PicScreenFlags::Visual);

                // Now blt it
                HGDIOBJ hold = dcMem.SelectObject(bitmap);
                COLORREF transparent = RGB(transparentRGB.rgbRed, transparentRGB.rgbGreen, transparentRGB.rgbBlue);
                TransparentBlt(dc, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, dcMem, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, transparent);
                dcMem.SelectObject(hold);
            }
        }
    }
}

void CPicView::_DrawLineDraw(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags)
{
    // The user is drawing a line.  We'll draw it right into the bits, by asking
    // a line command to draw.
    assert(_xOld != -1);
    assert(_yOld != -1);

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
    if (rcAroundThis.right < sPIC_WIDTH)
    {
        dcMem.MoveTo(rcAroundThis.right, _ptCurrentHover.y);
        dcMem.LineTo(sPIC_WIDTH, _ptCurrentHover.y);
    }
    if (rcAroundThis.top >= 0)
    {
        dcMem.MoveTo(_ptCurrentHover.x, 0);
        dcMem.LineTo(_ptCurrentHover.x, rcAroundThis.top);
    }
    if (rcAroundThis.bottom < sPIC_HEIGHT)
    {
        dcMem.MoveTo(_ptCurrentHover.x, rcAroundThis.bottom);
        dcMem.LineTo(_ptCurrentHover.x, sPIC_HEIGHT);
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
        if (_fPreviewPen || (appState->_fGridLines && (_currentTool == Command)))
        {
            return true;
        }
    }
    return !_pastedCommands.empty();
}

void CPicView::DrawOnPic(ViewPort &viewPort, PicData &picData, PicScreenFlags flags)
{
    if (_fShowingEgo && !_GetEditPic()->Traits.IsVGA)
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

    const uint8_t *displayBits = _GetDrawManager().GetPicBits(screen, PicPosition::PostPlugin);
    _GetDrawManager().GetBitmapInfo(screen, &pbmi);

    // FEATURE: this could be optimized (when zoomed, by limiting how much we copy)
    // - it involves palette conversion, so it could be expensive.
    if (pbmi)
    {
        StretchDIBits((HDC)*pDC, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, displayBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
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
    int iZoomOffset = (ppev->_cyPic / sPIC_HEIGHT) / 2;

    CPoint ptClient1 = ppev->_MapPicPointToClient(CPoint(x1, y1));
    ptClient1.Offset(iZoomOffset, iZoomOffset);
    pDC->MoveTo(ptClient1.x - 3, ptClient1.y - 3);
    pDC->LineTo(ptClient1.x + 4, ptClient1.y - 3);
    pDC->LineTo(ptClient1.x + 4, ptClient1.y + 4);
    pDC->LineTo(ptClient1.x - 3, ptClient1.y + 4);
    pDC->LineTo(ptClient1.x - 3, ptClient1.y - 3);

    CPoint ptClient2 = ppev->_MapPicPointToClient(CPoint(x2, y2));
    ptClient2.Offset(iZoomOffset, iZoomOffset);
    pDC->MoveTo(ptClient2.x - 3, ptClient2.y - 3);
    pDC->LineTo(ptClient2.x + 4, ptClient2.y + 4);
    pDC->MoveTo(ptClient2.x + 3, ptClient2.y - 3);
    pDC->LineTo(ptClient2.x - 4, ptClient2.y + 4);

    pDC->MoveTo(ptClient2.x, ptClient2.y);
    pDC->LineTo(ptClient1.x, ptClient1.y);
}

void CPicView::_DrawCoord(CDC *pDC, int x, int y, int additionalSize)
{
    int iZoomOffset = (_cyPic / sPIC_HEIGHT) / 2;
    CPoint ptClient1 = _MapPicPointToClient(CPoint(x, y));
    ptClient1.Offset(iZoomOffset, iZoomOffset);
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
            int iOldZoom = _cxPic / sPIC_WIDTH;
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
                    ptZoom.SetPoint(sPIC_WIDTH / 2, sPIC_HEIGHT / 2);
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

    if (IsFlagSet(hint, PicChangeHint::NewPic | PicChangeHint::EditPicPos | PicChangeHint::Palette | PicChangeHint::EditPicInvalid | PicChangeHint::PreviewPalette))
    {
        // Need to redraw the pic, yup
        InvalidateOurselvesImmediately();
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

    _cxPic = _cxPic + (sPIC_WIDTH * iMultiplier);
    _cyPic = _cyPic + (sPIC_HEIGHT * iMultiplier);
    _cxPic = max(_cxPic, sPIC_WIDTH);
    _cyPic = max(_cyPic, sPIC_HEIGHT);

    CPicDoc* pDoc = GetDocument();
    // Push to the document (it only fires an event when it changes, so we shouldn't get re-entered).
    pDoc->SetZoom(_cxPic / sPIC_WIDTH);

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
    if (_cxPic < (sPIC_WIDTH * 16))
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
    if (_cxPic > sPIC_WIDTH)
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
        _transformCommandMod.ApplyDifference(*GetDocument()->GetPic(), 0, 0);

        GetDocument()->ApplyChanges<PicComponent>(
            [this, dx, dy](PicComponent &pic)
        {
            _transformCommandMod.ApplyDifference(pic, dx, dy);
            return WrapHint(PicChangeHint::EditPicInvalid);
        }
        );
    }
    else
    {
        // We have to go poking around in the resource directly
        _transformCommandMod.ApplyDifference(*GetDocument()->GetPic(), dx, dy);
        // As a result we need to tell people manually to update
    }
}

void CPicView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Don't do anything if this is off the picture.
    CPoint ptPic = _MapClientPointToPic(point);

    // The out of bounds check doesn't apply to fake ego.
    if ((_currentTool == None) && _fShowingEgo && _HitTestFakeEgo(ptPic))
    {
        // The ego is showing.  Start capture.
        _fCapturing = TRUE;
        _pointCapture = ptPic;
        _pointEgoOrig = _pointEgo;
        _fCanBeHere = _EvaluateCanBeHere(_pointEgoOrig);
        SetCapture();
        InvalidateOurselves();
    }

    if ((ptPic.x >= sPIC_WIDTH) || (ptPic.y >= sPIC_HEIGHT))
    {
        if (_fShowPriorityLines && _HitTestPriorityBar(ptPic, &_priBarMoveIndex) && (_priBarMoveIndex != -1))
        {
            _originalPriValueCommand = _EnsurePriorityBarCommand();
            _originalPriValue = _originalPriValueCommand.setPriorityBars.pPriorityLines[_priBarMoveIndex];
            _fCapturing = TRUE;
            _pointCapture = ptPic;
            SetCapture();
        }
    }
    else
    {
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
                        _transformCommandMod.Begin(*_GetEditPic(), _currentCommand, bestIndex, firstSet);
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
        }
    }
}

bool CPicView::_EvaluateCanBeHere(CPoint pt)
{
    const BYTE *pdataControl = _GetDrawManager().GetPicBits(PicScreen::Control, PicPosition::PrePlugin);
    if (appState->_fObserveControlLines)
    {
        return CanBeHere(pdataControl, GetViewBoundsRect((uint16_t)pt.x, (uint16_t)pt.y, appState->GetSelectedViewResource(), _nFakeLoop, _nFakeCel));
    }
    else
    {
        return true;
    }
}

bool CPicView::_HitTestFakeEgo(CPoint ptPic)
{
    bool fHitTest = false;
    if (!appState->_fUseBoxEgo && appState->GetSelectedViewResource())
    {
        fHitTest = HitTestView((uint16_t)ptPic.x, (uint16_t)ptPic.y, (uint16_t)_pointEgo.x, (uint16_t)_pointEgo.y, appState->GetSelectedViewResource(), _nFakeLoop, _nFakeCel);
    }
    else
    {
        fHitTest = HitTestEgoBox((uint16_t)ptPic.x, (uint16_t)ptPic.y, (uint16_t)_pointEgo.x, (uint16_t)_pointEgo.y, appState->_cxFakeEgo, appState->_cyFakeEgo);
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
        return CPoint(_pointEgo.x, _pointEgo.y - appState->_cyFakeEgo / 2);
    }
    else
    {
        return FindCenterOfView((uint16_t)_pointEgo.x, (uint16_t)_pointEgo.y, appState->GetSelectedViewResource(), _nFakeLoop, _nFakeCel);
    }
}

void CPicView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_fCapturing)
    {
        assert((_fShowingEgo && (_currentTool == None)) || (_currentTool == Pasting) || _transformingCoords || (_originalPriValue != -1));
        _fCapturing = FALSE;
        ReleaseCapture();

        CPoint ptPic = _MapClientPointToPic(point);
        if (_transformingCoords && (_currentTool != None))
        {
            // Commit the change
            _TransformPoint(true, ptPic);
            _transformCommandMod.Reset();
        }
        else if (_fShowPriorityLines && (_originalPriValue != -1))
        {
            _MovePriorityBar(true, ptPic.y - _pointCapture.y);
        }
    }
}

void CPicView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CPoint ptPic = _MapClientPointToPic(point);

    // Don't do anything if this is off the picture.
    if ((ptPic.x >= sPIC_WIDTH) || (ptPic.y >= sPIC_HEIGHT))
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
                _pointEgo = ptPic;
                _pointEgo.y += appState->_cyFakeEgo / 2;
                _fCanBeHere = _EvaluateCanBeHere(_pointEgo);
                appState->_ptFakeEgo = _pointEgo;
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
    if ((ptPic.x >= sPIC_WIDTH) || (ptPic.y >= sPIC_HEIGHT))
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
                    _transformCommandMod.Begin(*_GetEditPic(), _currentCommand, bestIndex, firstSet);

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
    if ((ptPic.x >= sPIC_WIDTH) || (ptPic.y >= sPIC_HEIGHT))
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
        int iAmount = 5 * (_cxPic / sPIC_WIDTH) * nNotches;

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
                _nFakeCel++;
                break;
            }
        case VK_LEFT:
            {
                _nFakeCel--;
                break;
            }
        case VK_UP:
            {
                _nFakeLoop--;
                break;
            }
        case VK_DOWN:
            {
                _nFakeLoop++;
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

const PicComponent *CPicView::_GetEditPic()
{
    const PicComponent *ppic = nullptr;
    if (_pPreview)
    {
        ppic = &_pPreview->GetPicResource();
    }
    else
    {
        CPicDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);
        if (pDoc)
        {
            ppic = pDoc->GetPic();
        }
    }
    return ppic;
}

const PaletteComponent *CPicView::_GetPalette()
{
    const PaletteComponent *ppalette = nullptr;
    if (_pPreview)
    {
        ppalette = _pPreview->GetPaletteResource();
    }
    else
    {
        CPicDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);
        if (pDoc)
        {
            ppalette = pDoc->GetPalette();
        }
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
