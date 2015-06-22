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

CPicDoc::CPicDoc() : _previewPalette(nullptr)
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
    _pdm.SetPic(_GetPic(), _GetPalette());
    switch (nID)
    {
    case IDC_SETVISUAL:
        {
            if (_GetPic()->Traits.IsVGA)
            {
                CChooseColorDialogVGA dialog(*_GetPalette());
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
    map.RemoveSync((ISyncResourceMap*)this);
    __super::OnCloseDocument();
}

void CPicDoc::OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior)
{
    if ((pData->GetType() == ResourceType::Palette) && (pData->GetNumber() == 999))
    {
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::Palette));
    }
}

void CPicDoc::SetEditPic(std::unique_ptr<ResourceEntity> pEditPic, int id)
{
    _checksum = id;

    if (pEditPic && pEditPic->GetComponent<PicComponent>().Traits.IsVGA)
    {
        // Add a polygon component
        pEditPic->AddComponent<PolygonComponent>(CreatePolygonComponent(appState->GetResourceMap().Helper().GetPolyFolder(), pEditPic->ResourceNumber));
    }

    AddFirstResource(move(pEditPic));

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

void CPicDoc::SetPalette(BYTE b)
{
    _pdm.SetPalette(b);
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::Palette));
}

void CPicDoc::SeekToPos(INT_PTR iPos, bool fNotify)
{
    if (_pdm.SeekToPos(iPos))
    {
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

void CPicDoc::InsertCommand(PicCommand *pCommand)
{
    _CloneCurrentAndAdd();
    ::InsertCommands(*_GetPic(), _pdm.GetPos(), 1, pCommand);
    SeekToPos(_NextPicPos(_pdm.GetPos(), 1), false);
    _pdm.Invalidate();
    _NotifyNewResource(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    SetModifiedFlag(TRUE);
}
void CPicDoc::InsertCommands(INT_PTR iStart, INT_PTR cCount, PicCommand *pCommands)
{
    _CloneCurrentAndAdd();
    ::InsertCommands(*_GetPic(), iStart, cCount, pCommands);
    INT_PTR iPos = _pdm.GetPos();
    if (iStart <= iPos)
    {
        SeekToPos(_NextPicPos(iPos, cCount), false);
    }
    _pdm.Invalidate();
    _NotifyNewResource(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    SetModifiedFlag(TRUE);
}
void CPicDoc::InsertCommands(INT_PTR cCount, PicCommand *pCommands)
{
    _CloneCurrentAndAdd();
    ::InsertCommands(*_GetPic(), _pdm.GetPos(), cCount, pCommands);
    SeekToPos(_NextPicPos(_pdm.GetPos(), cCount), false);
    _pdm.Invalidate();
    _NotifyNewResource(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    SetModifiedFlag(TRUE);
}
void CPicDoc::RemoveCommand(INT_PTR iCommandIndex)
{
    _CloneCurrentAndAdd();
    ::RemoveCommand(*_GetPic(), iCommandIndex);
    INT_PTR iPos = _pdm.GetPos();
    if (iCommandIndex < iPos)
    {
        SeekToPos(_PrevPicPos(iPos, 1), false);
    }
    _pdm.Invalidate();
    _NotifyNewResource(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    SetModifiedFlag(TRUE);
}
void CPicDoc::RemoveCommandRange(INT_PTR iStart, INT_PTR iEnd)
{
    if (iEnd == -1)
    {
        iEnd = (int)_GetPic()->commands.size() - 1;
    }
    int delta = (iEnd - iStart + 1);
    _CloneCurrentAndAdd();
    ::RemoveCommandRange(*_GetPic(), iStart, iEnd);
    INT_PTR iPos = _pdm.GetPos();
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
    _pdm.Invalidate();
    _NotifyNewResource(PicChangeHint::EditPicInvalid | PicChangeHint::EditPicPos);
    SetModifiedFlag(TRUE);
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
        // TODO: If the user was in the process of making a polygon, we need to finalize it.

        _currentPolyIndex = index;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::PolygonChoice));
    }
}

void CPicDoc::v_OnUndoRedo()
{
    _pdm.Invalidate();
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(PicChangeHint::EditPicInvalid));
}

void CPicDoc::PostApplyChanges(CObject *pObj)
{
    // Invalidate our pic before we update views..
    _pdm.Invalidate();
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

PicComponent *CPicDoc::_GetPic() const
{
    const ResourceEntity *pEntity = static_cast<const ResourceEntity*>(GetResource());
    return pEntity ? &pEntity->GetComponent<PicComponent>() : nullptr;
}

PaletteComponent *CPicDoc::_GetPalette() const
{
    const ResourceEntity *pEntity = static_cast<const ResourceEntity*>(GetResource());
    return pEntity ? pEntity->TryGetComponent<PaletteComponent>() : nullptr;
}

PicDrawManager &CPicDoc::GetDrawManager()
{
    _pdm.SetPic(_GetPic(), _previewPalette ? _previewPalette : _GetPalette());
    return _pdm;
};

void CPicDoc::PostSuccessfulSave(const ResourceEntity *pResource)
{
    // I was saving polygons here, but it makes no sense.
}

// CPicDoc commands
