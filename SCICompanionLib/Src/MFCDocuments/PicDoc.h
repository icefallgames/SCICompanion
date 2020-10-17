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

// PicDoc.h : interface of the CPicDoc class
//
#pragma once

#include "DialogFactory.h"
#include "ResourceDocument.h"
#include "PicDrawManager.h"
#include "UndoResource.h"
#include "Pic.h"
#include "ResourceEntity.h"
#include "PicOperations.h"
#include "ResourceEntityDocument.h"
#include "ResourceMapEvents.h"
#include "Polygon.h"
#include "DocumentWithPaletteChoices.h"

using namespace std;

// FWD declaration.
struct PicComponent;
struct PaletteComponent;
class ResourceBlob;
class DependencyTracker;
class PolygonComponent;

extern const char *c_rgControlColourNames[16];

class IBitmapEditor
{
public:
    virtual CPoint GetCursorPos() = 0;
    virtual CSize GetBitmapSize() = 0;
    virtual PicScreen GetPicScreen() = 0;
};

enum class PicChangeHint
{
    None = 0x00000000,
    // The selected/edited pic changed
    NewPic = 0x00000001,
    // Something about the content of the edited pic was invalidated.
    EditPicInvalid = 0x00000002,
    // The current position of the edited pic was changed
    EditPicPos = 0x00000004,
    // The pen style changed
    PenStyle = 0x00000008,
    // Some preferences changed (such as drawgridlines, or fake ego size)
    Preferences = 0x00000010,
    // The palette with which the picture is drawn just changed.
    Palette = 0x00000020,
    // Zoom level of the pic changed.
    ZoomLevel = 0x00000080,
    // Cursor pos
    CursorPosition = 0x00000200,
    ViewScreenChange = 0x00000400,
    PreviewPalette = 0x00000800,
    PolygonChoice = 0x00001000,
    PolygonsChanged = 0x00002000,
    FakeEgo = 0x00004000,
    NamedPositionChoice = 0x00008000,
    NamedPositionsChanged = 0x00010000
};


DEFINE_ENUM_FLAGS(PicChangeHint, uint32_t)

class CPicDoc : public ResourceEntityDocument, public IDialogFactory, public IResourceMapEvents, public DocumentWithPaletteChoices
{
protected: // create from serialization only
    CPicDoc();
    DECLARE_DYNCREATE(CPicDoc)

// Attributes
public:
    // IDialogFacotry
    void InvokeDialog(UINT nID, RECT *prcTarget);

    void OnToolbarDropDown(NMTOOLBAR *pnmtb);
    PenStyle GetPenStyle() { return _penStyle; }
    int GetZoom() { return _iZoom; }
    void SetZoom(int iZoom);
    int GetFakeEgo() { return _fakeEgoResourceNumber; }
    void SetFakeEgo(int resourceNumber);
    void EnsureFakeEgoNumber();
    void SetPalette(BYTE b);

    void SeekToPos(INT_PTR iPos, bool fNotify = true);
    void InsertCommand(PicCommand *pCommand);
    void InsertCommands(INT_PTR iStart, INT_PTR cCount, PicCommand *pCommands);
    void InsertCommands(INT_PTR cCount, PicCommand *pCommands);
    void RemoveCommand(INT_PTR iCommandIndex);
    void RemoveCommandRange(INT_PTR iStart, INT_PTR iEnd);
    void ExplicitNotify(PicChangeHint hint);
    ptrdiff_t GetPosition();

    const PolygonComponent *GetPolygonComponent();
    int GetCurrentPolygonIndex() const { return _currentPolyIndex; }
    void SetCurrentPolygonIndex(int index);
    bool GetShowPolygons() const { return _showPolygons; }
    void SetShowPolygons(bool showPolygons);
    int GetCurrentNamedPositionIndex() const { return _currentNamedPositionIndex; }
    void SetCurrentNamedPositionIndex(int index);
    bool GetShowNamedPositions() const { return _showNamedPositions; }
    void SetShowNamedPositions(bool showNamedPosition);

    void v_OnUndoRedo();

    void SetPreviewPalette(const PaletteComponent *palette);

    // IResourceMapEvents
    void OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior);
    void OnResourceDeleted(const ResourceBlob *pData) {}
    void OnResourceMapReloaded(bool isInitialLoad) {}
    void OnResourceTypeReloaded(ResourceType iType) {}
    void OnImagesInvalidated() {}

// Overrides
public:
    ptrdiff_t v_GetExtra() override;
    BOOL OnNewDocument() override;
    void OnCloseDocument() override;

// Implementation
public:
    virtual ~CPicDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    PicDrawManager &GetDrawManager();
    std::unique_ptr<PicDrawManager> GetDrawManagerCopy();

    // This transfers ownership of pic resource to this class.
    void SetEditPic(DependencyTracker &tracker, std::unique_ptr<ResourceEntity> pEditPic, int id = -1);
    
    const PicComponent *GetPic() const { return _GetPic(); }

    void InformBitmapEditor(PicChangeHint hint, IBitmapEditor *pObj);

    void PostSuccessfulSave(const ResourceEntity *pResource) override;

	bool IsUndithered() const { return _isUndithered; }
	void SetIsUndithered(bool undithered) { _isUndithered = undithered; }


protected:
    virtual ResourceType _GetType() const { return ResourceType::Pic; }

    bool v_IsVGA() override;
    void v_OnUpdatePaletteOptions() override;
    const ResourceEntity *v_GetResource() override;
    void v_OnUpdatePaletteChoice() override;

    PicDrawManager _pdm;

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

private:
    void _CloneCurrentAndAdd();
    void _NotifyNewResource(PicChangeHint hint);
    const PicComponent *_GetPic() const;
    void _SetInitialPalette();

    const PaletteComponent *_previewPalette;

    void PostApplyChanges(CObject *pObj) override;

    // Pattern state.  We do not use the pattern state
    // from the current position in the pic.  Instead, we store our
    // own pattern state in the document.
    PenStyle _penStyle;

    // Zoom level of pic.
    int _iZoom;

    int _fakeEgoResourceNumber;

    // For resources with PolygonComponent
    int _currentPolyIndex;
    bool _showPolygons;
    int _currentNamedPositionIndex;
    bool _showNamedPositions;

	bool _isUndithered;

    DependencyTracker *_dependencyTracker;
    std::unique_ptr<PolygonComponent> _lastPoly;
};

bool InsertPaletteCommands(PicComponent &pepic, INT_PTR iPos, const EGACOLOR *pPaletteOrig, const EGACOLOR *pPaletteNew, BOOL fWriteEntire);

