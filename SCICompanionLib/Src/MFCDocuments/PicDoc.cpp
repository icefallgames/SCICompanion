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

// EditPicDoc.cpp : implementation of the CPicDoc class
//

#include "stdafx.h"
#include "AppState.h"
#include "PicDoc.h"
#include "ChooseColorDialog.h"
#include "ChooseColorDialogVGA.h"
#include "ChoosePriConDialog.h"
#include "ChoosePenStyleDialog.h"
#include "ResourceMap.h"
#include "CObjectWrap.h"
#include "PaletteOperations.h"
#include "AppState.h"
#include "Polygon.h"
#include "ClassBrowser.h"
#include "ResourceBlob.h"
#include "PicCommands.h"
#include "DependencyTracker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

__int64 g_Ticks = 0;
__int64 g_TicksIncAdd = 0;

// CPicDoc

IMPLEMENT_DYNCREATE(CPicDoc, CResourceDocument)

BEGIN_MESSAGE_MAP(CPicDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
END_MESSAGE_MAP()

// CPicDoc construction/destruction

CPicDoc::CPicDoc() : _previewPalette(nullptr), _showPolygons(false), _currentPolyIndex(-1), _showNamedPositions(false), _currentNamedPositionIndex(-1), _fakeEgoResourceNumber(-1), _dependencyTracker(nullptr), _isUndithered(false)
{
    // Add ourselves as a sync
    CResourceMap &map = appState->GetResourceMap();
    map.AddSync(this);
}

CPicDoc::~CPicDoc()
{
}

void CPicDoc::SetPreviewPalette(const PaletteComponent *palette)
{
    _previewPalette = palette;
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PreviewPalette));
}

void CPicDoc::_CloneCurrentAndAdd()
{
    AddNewResourceToUndo(move(GetResource()->Clone()));
}

void CPicDoc::_NotifyNewResource(PicChangeHint hint)
{
    SetModifiedFlag(TRUE);
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(hint));
}

const char *c_rgControlColourNames[16] =
{
    "ctlBLACK",
    "ctlNAVY",
    "ctlGREEN",
    "ctlTEAL",
    "ctlMAROON",
    "ctlPURPLE",
    "ctlBROWN",
    "ctlSILVER",
    "ctlGREY",
    "ctlBLUE",
    "ctlLIME",
    "ctlCYAN",
    "ctlRED",
    "ctlFUCHSIA",
    "ctlYELLOW",
    "ctlWHITE",
};

void CPicDoc::InvokeDialog(UINT nID, RECT *prcButton)
{
    _pdm.SetPic(_GetPic(), GetCurrentPaletteComponent(), _isUndithered, appState->GetVersion().NewSCI);
    switch (nID)
    {
    case IDC_SETVISUAL:
        {
            if (_GetPic()->Traits->IsVGA)
            {
                CChooseColorDialogVGA dialog(*GetCurrentPaletteComponent());
                dialog.SetTrackRect(prcButton);
                if (IDOK == dialog.DoModal())
                {
                    BYTE bIndex = dialog.GetColor();
                    PicCommand command = PicCommand::CreateSetVisual(bIndex);
                    InsertCommand(&command);
                }
            }
            else
            {
                CChooseColorDialog dialog;
                const ViewPort *pViewPort = _pdm.GetViewPort(PicPosition::PostPlugin);
                BYTE bPalette = _pdm.GetPalette();
                ASSERT((bPalette >= 0) && (bPalette < 4));
                dialog.Init(&pViewPort->pPalettes[bPalette * 40]);
                dialog.SetColor(pViewPort->bPaletteOffset);
                dialog.SetTrackRect(prcButton);
                if (IDOK == dialog.DoModal())
                {
                    BYTE bIndex = dialog.GetColor();
                    PicCommand command = PicCommand::CreateSetVisual(0, bIndex);
                    InsertCommand(&command);
                }
            }
        }
        break;

    case IDC_SETPRIORITY:
        {
            CChoosePriConDialog dialog(nullptr);
            const ViewPort *pViewPort = _pdm.GetViewPort(PicPosition::PostPlugin);
            dialog.SetCaptionID(IDS_CHOOSEAPRIORITY);
            dialog.SetColor(pViewPort->bPriorityValue);
            dialog.SetTrackRect(prcButton);
            if (IDOK == dialog.DoModal())
            {
                BYTE bIndex = dialog.GetColor();
                // Time to create a command.
                PicCommand command = PicCommand::CreateSetPriority(bIndex);
                InsertCommand(&command);
            }
        }
        break;

    case IDC_SETCONTROL:
        {
            CChoosePriConDialog dialog(nullptr);
            const ViewPort *pViewPort = _pdm.GetViewPort(PicPosition::PostPlugin);
            dialog.SetCaptionID(IDS_CHOOSEACONTROL);
            dialog.SetColor(pViewPort->bControlValue);
            dialog.SetTrackRect(prcButton);
            dialog.SetStatusText(c_rgControlColourNames);
            if (IDOK == dialog.DoModal())
            {
                BYTE bIndex = dialog.GetColor();
                // Time to create a command.
                PicCommand command = PicCommand::CreateSetControl(bIndex);
                InsertCommand(&command);
            }
        }
        break;

    case IDC_SETPENSTYLE:
        {
            CChoosePenStyleDialog dialog;
            dialog.SetPenStyle(&_penStyle);
            dialog.SetTrackRect(prcButton);
            if (IDOK == dialog.DoModal())
            {
                dialog.GetPenStyle(&_penStyle);
                UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PenStyle));
                // Just set our state, issue no commands.
            }
        }
        break;
    }
}

void CPicDoc::InformBitmapEditor(PicChangeHint hint, IBitmapEditor *pObj)
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapObject(hint, pObj));
}

void CPicDoc::OnToolbarDropDown(NMTOOLBAR *pnmtb)
{
    if (pnmtb->iItem == ID_SHOWTRACEIMAGE)
    {
        // Bring up the alpha slider
        // Need to figure out the screen location of the button.
        RECT rcButton;
        // Get its rect in the toolbar:
        ::SendMessage(pnmtb->hdr.hwndFrom, TB_GETRECT, pnmtb->iItem, (LPARAM)&rcButton);

        RECT rcToolbar;
        // Then the toolbar's rect:
        ::GetWindowRect(pnmtb->hdr.hwndFrom, &rcToolbar);

        // Adjust, and invoke the dialog.
        OffsetRect(&rcButton, rcToolbar.left, rcToolbar.top);
        InvokeDialog(ID_SHOWTRACEIMAGE, &rcButton);
    }
}

// So that we store the pic pos in the undo stack:
ptrdiff_t CPicDoc::v_GetExtra()
{
    return _pdm.GetPos();
}

BOOL CPicDoc::OnNewDocument()
{
    if (!__super::OnNewDocument())
        return FALSE;

    return TRUE;
}

void CPicDoc::OnCloseDocument()
{
    // Remove ourselves as a sync
    CResourceMap &map = appState->GetResourceMap();
    map.RemoveSync((IResourceMapEvents*)this);
    __super::OnCloseDocument();
}

void CPicDoc::OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior)
{
    if (pData->GetType() == ResourceType::Palette)
    {
        RefreshPaletteOptions();
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::Palette));
    }
    if (pData->GetType() == ResourceType::View)
    {
        // Update for all views...
        //if (pData->GetNumber() == _fakeEgoResourceNumber)
        {
            UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::FakeEgo));
        }
    }
}

void CPicDoc::SetEditPic(DependencyTracker &tracker, std::unique_ptr<ResourceEntity> pEditPic, int id)
{
	_isUndithered = appState->GetResourceMap().Helper().GetUndither();

    _dependencyTracker = &tracker;
    _checksum = id;

    if (pEditPic && appState->GetVersion().UsesPolygons)
    {
        // Add a polygon component
        pEditPic->AddComponent<PolygonComponent>(CreatePolygonComponent(appState->GetResourceMap().Helper().GetPolyFolder(), pEditPic->ResourceNumber));
        _lastPoly = std::make_unique<PolygonComponent>(pEditPic->GetComponent<PolygonComponent>());
    }

    AddFirstResource(move(pEditPic), -1);
    RefreshPaletteOptions();

    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::NewPic));
    _UpdateTitle();
}

void CPicDoc::SetZoom(int iZoom)
{
    iZoom = max(1, iZoom);
    iZoom = min(15, iZoom);
    if (iZoom != _iZoom)
    {
        _iZoom = iZoom;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::ZoomLevel));
    }
}

void CPicDoc::SetFakeEgo(int resourceNumber)
{
    if (resourceNumber != _fakeEgoResourceNumber)
    {
        _fakeEgoResourceNumber = resourceNumber;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::FakeEgo));
    }
}

void CPicDoc::EnsureFakeEgoNumber()
{
    if (_fakeEgoResourceNumber == -1)
    {
        _fakeEgoResourceNumber = appState->GetSelectedViewResourceNumber();
    }
}

void CPicDoc::SetPalette(BYTE b)
{
    _pdm.SetPalette(b);
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::Palette));
}

void CPicDoc::SeekToPos(INT_PTR iPos, bool fNotify)
{
    if (_pdm.SeekToPos(iPos))
    {
        SetExtra(_pdm.GetPos());
        if (fNotify)
        {
            UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::EditPicPos));
        }
    }
}

INT_PTR _NextPicPos(INT_PTR iPos, INT_PTR delta)
{
    if (iPos != -1)
    {
        iPos += delta;
    }
    return iPos;
}

INT_PTR _PrevPicPos(INT_PTR iPos, INT_PTR delta)
{
    if (iPos != -1)
    {
        iPos -= delta;
        iPos = max(0, iPos);
    }
    return iPos;
}

ptrdiff_t CPicDoc::GetPosition()
{
    return _pdm.GetPos();
}

void CPicDoc::InsertCommand(PicCommand *pCommand)
{
    ptrdiff_t delta = 0;
    ptrdiff_t pos = _pdm.GetPos();
    ApplyChangesWithPost<PicComponent>(
        [pCommand, &delta, pos](PicComponent &pic)
    {
        delta = ::InsertCommands(pic, pos, 1, pCommand);
        return WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    },
        [this, &delta](ResourceEntity &resource)
    {
        SeekToPos(_NextPicPos(_pdm.GetPos(), delta), false);
    }
        );
}

void CPicDoc::InsertCommands(INT_PTR iStart, INT_PTR cCount, PicCommand *pCommands)
{
    INT_PTR iPos = _pdm.GetPos();

    ApplyChangesWithPost<PicComponent>(
        [iStart, cCount, pCommands](PicComponent &pic)
    {
        ::InsertCommands(pic, iStart, cCount, pCommands);
        return WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    },
        [iPos, iStart, this, cCount](ResourceEntity &resource)
    {
        if (iStart <= iPos)
        {
            SeekToPos(_NextPicPos(iPos, cCount), false);
        }
    }
        );
}

void CPicDoc::InsertCommands(INT_PTR cCount, PicCommand *pCommands)
{
    InsertCommands(_pdm.GetPos(), cCount, pCommands);
}

void CPicDoc::RemoveCommand(INT_PTR iCommandIndex)
{
    INT_PTR iPos = _pdm.GetPos();
    ApplyChangesWithPost<PicComponent>(
        [iCommandIndex](PicComponent &pic)
    {
        ::RemoveCommand(pic, iCommandIndex);
        return WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    },
        [this, iPos, iCommandIndex](ResourceEntity &entity)
    {
        if (iCommandIndex < iPos)
        {
            SeekToPos(_PrevPicPos(iPos, 1), false);
        }
    }
        );
}

void CPicDoc::RemoveCommandRange(INT_PTR iStart, INT_PTR iEnd)
{
    if (iEnd == -1)
    {
        iEnd = (int)_GetPic()->commands.size() - 1;
    }
    int delta = (iEnd - iStart + 1);
    INT_PTR iPos = _pdm.GetPos();

    ApplyChangesWithPost<PicComponent>(
        [iStart, iEnd](PicComponent &pic)
    {
        ::RemoveCommandRange(pic, iStart, iEnd);
        return WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    },
        [this, iEnd, iPos, iStart, delta](ResourceEntity &entity)
    {
        if (iEnd < iPos)
        {
            // Adjust selection down
            SeekToPos(_PrevPicPos(iPos, delta), false);
        }
        else if (iStart < iPos)
        {
            // It was in the selection
            SeekToPos(iStart, false);
        }
    }
    );
}

void CPicDoc::ExplicitNotify(PicChangeHint hint)
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(hint));
}

const PolygonComponent *CPicDoc::GetPolygonComponent()
{
    const PolygonComponent *poly = nullptr;
    const ResourceEntity *resource = GetResource();
    if (resource)
    {
        poly = resource->TryGetComponent<PolygonComponent>();
    }
    return poly;
}

void CPicDoc::SetCurrentPolygonIndex(int index)
{
    if (index != _currentPolyIndex)
    {
        _currentPolyIndex = index;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PolygonChoice));
    }
}

void CPicDoc::SetShowPolygons(bool showPolygons)
{
    if (showPolygons != _showPolygons)
    {
        _showPolygons = showPolygons;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PolygonChoice));
    }
}


void CPicDoc::SetCurrentNamedPositionIndex(int index)
{
    if (index != _currentNamedPositionIndex)
    {
        _currentNamedPositionIndex = index;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::NamedPositionChoice));
    }
}

void CPicDoc::SetShowNamedPositions(bool showNamedPositions)
{
    if (showNamedPositions != _showNamedPositions)
    {
        _showNamedPositions = showNamedPositions;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::NamedPositionChoice));
    }
}

void CPicDoc::v_OnUndoRedo()
{
    PostApplyChanges(&WrapHint(PicChangeHint::NewPic));
    SeekToPos(GetExtra(), false); // Restore the position associated with this thing.
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos));
}

void CPicDoc::PostApplyChanges(CObject *pObj)
{
    // If palette choice changed or view changed, then we need to update palette choice (if it's currently an embedded palette)
    // This has to be done before we calls views to update them, since they will ask us for the current palette to render with.
    PicChangeHint hint = GetHint<PicChangeHint>(pObj);
    if (IsFlagSet(hint, PicChangeHint::NewPic | PicChangeHint::Palette))
    {
        RefreshPaletteOptions();
    }

    // Invalidate our pic before we update views..
    _pdm.Invalidate();
}

bool CPicDoc::v_IsVGA()
{
    return _GetPic() && _GetPic()->Traits->IsVGA;
}
void CPicDoc::v_OnUpdatePaletteOptions()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::NewPic));
}
const ResourceEntity *CPicDoc::v_GetResource()
{
    return GetResource();
}
void CPicDoc::v_OnUpdatePaletteChoice()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PreviewPalette));
}


// CPicDoc diagnostics

#ifdef _DEBUG
void CPicDoc::AssertValid() const
{
    __super::AssertValid();
}

void CPicDoc::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}
#endif //_DEBUG

const PicComponent *CPicDoc::_GetPic() const
{
    const ResourceEntity *pEntity = static_cast<const ResourceEntity*>(GetResource());
    return pEntity ? &pEntity->GetComponent<PicComponent>() : nullptr;
}

PicDrawManager &CPicDoc::GetDrawManager()
{
    _pdm.SetPic(_GetPic(), _previewPalette ? _previewPalette : GetCurrentPaletteComponent(), _isUndithered, appState->GetVersion().NewSCI);
    return _pdm;
};

std::unique_ptr<PicDrawManager> CPicDoc::GetDrawManagerCopy()
{
    return std::make_unique<PicDrawManager>(_GetPic(), _previewPalette ? _previewPalette : GetCurrentPaletteComponent(), _isUndithered, appState->GetVersion().NewSCI);
}


void CPicDoc::PostSuccessfulSave(const ResourceEntity *pResource)
{
    // Ideally we only want to do this if the polygons change.
    const PolygonComponent *poly = pResource->TryGetComponent<PolygonComponent>();
    if (_dependencyTracker && poly)
    {
        if (!_lastPoly || (*poly != *_lastPoly))
        {
            _lastPoly = std::make_unique<PolygonComponent>(*poly);
            std::string polyFile = poly->GetPolyFilename();
            _dependencyTracker->NotifyHeaderFileChanged(polyFile);
        }
    }
}

// CPicDoc commands

bool InsertPaletteCommands(PicComponent &pepic, INT_PTR iPos, const EGACOLOR *pPaletteOrig, const EGACOLOR *pPaletteNew, BOOL fWriteEntire)
{
    bool fSomethingChanged = false;
    if (fWriteEntire)
    {
        // Write the entire palette, for any palette that changed.
        for (int i = 0; i < 4; i++)
        {
            if (0 != memcmp(&pPaletteOrig[40 * i], &pPaletteNew[40 * i], sizeof(EGACOLOR) * 40))
            {
                // This palette changed.  Write it.
                PicCommand command = PicCommand::CreateSetPalette(i, &pPaletteNew[40 * i]);
                InsertCommands(pepic, iPos, 1, &command);
                fSomethingChanged = true;
            }
        }
    }
    else
    {
        // Cycle through each color, and see if it changed.
        vector<PicCommand> commands;
        for (int i = 0; i < 160; i++)
        {
            EGACOLOR oldColor = pPaletteOrig[i];
            EGACOLOR newColor = pPaletteNew[i];
            if ((oldColor.color1 != newColor.color1) || (oldColor.color2 != newColor.color2))
            {
                // This entry changed. Write it.
                commands.push_back(PicCommand::CreateSetPaletteEntry(i / 40, i % 40, newColor));
            }
        }
        if (commands.size() > 0)
        {
            InsertCommands(pepic, iPos, commands.size(), &commands[0]);
            fSomethingChanged = true;
        }
    }
    return fSomethingChanged;
}
