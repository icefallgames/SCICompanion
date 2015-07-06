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
#include "SyncResourceMap.h"
#include "Polygon.h"
#include "DocumentWithPaletteChoices.h"

using namespace std;

// FWD declaration.
struct PicComponent;
struct PaletteComponent;

extern const char *c_rgControlColourNames[16];

class IBitmapEditor
{
public:
    virtual CPoint GetCursorPos() = 0;
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
};


DEFINE_ENUM_FLAGS(PicChangeHint, uint32_t)

class CPicDoc : public ResourceEntityDocument, public IDialogFactory, public ISyncResourceMap, public DocumentWithPaletteChoices
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
    void SetPalette(BYTE b);

    void SeekToPos(INT_PTR iPos, bool fNotify = true);
    void InsertCommand(PicCommand *pCommand);
    void InsertCommands(INT_PTR iStart, INT_PTR cCount, PicCommand *pCommands);
    void InsertCommands(INT_PTR cCount, PicCommand *pCommands);
    void RemoveCommand(INT_PTR iCommandIndex);
    void RemoveCommandRange(INT_PTR iStart, INT_PTR iEnd);
    void ExplicitNotify(PicChangeHint hint);

    const PolygonComponent *GetPolygonComponent();
    int GetCurrentPolygonIndex() const { return _currentPolyIndex; }
    void SetCurrentPolygonIndex(int index);
    bool GetShowPolygons() const { return _showPolygons; }
    void SetShowPolygons(bool showPolygons);

    void v_OnUndoRedo();

    void SetPreviewPalette(const PaletteComponent *palette);

    // ISyncResourceMap
    void OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior);
    void OnResourceDeleted(const ResourceBlob *pData) {}
    void OnResourceMapReloaded(bool isInitialLoad) {}
    void OnResourceTypeReloaded(ResourceType iType) {}

// Overrides
public:
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

    // This transfers ownership of pic resource to this class.
    void SetEditPic(std::unique_ptr<ResourceEntity> pEditPic, int id = -1);
    
    PicComponent *GetPic() const { return _GetPic(); }

    void InformBitmapEditor(PicChangeHint hint, IBitmapEditor *pObj);

    void PostSuccessfulSave(const ResourceEntity *pResource) override;

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
    PicComponent *_GetPic() const;
    void _SetInitialPalette();

    const PaletteComponent *_previewPalette;

    void PostApplyChanges(CObject *pObj) override;

    // Pattern state.  We do not use the pattern state
    // from the current position in the pic.  Instead, we store our
    // own pattern state in the document.
    PenStyle _penStyle;

    // Zoom level of pic.
    int _iZoom;

    // For resources with PolygonComponent
    int _currentPolyIndex;
    bool _showPolygons;
};

template<class _T>
BOOL InsertPaletteCommands(typename _T *pepic, INT_PTR iPos, const EGACOLOR *pPaletteOrig, EGACOLOR *pPaletteNew, BOOL fWriteEntire)
{
    BOOL fSomethingChanged = FALSE;
    if (fWriteEntire)
    {
        // Write the entire palette, for any palette that changed.
        for (int i = 0; i < 4; i++)
        {
            if (0 != memcmp(&pPaletteOrig[40 * i], &pPaletteNew[40 * i], sizeof(EGACOLOR) * 40))
            {
                // This palette changed.  Write it.
                PicCommand command = PicCommand::CreateSetPalette(i, &pPaletteNew[40 * i]);
                InsertCommands(*pepic, iPos, 1, &command);
                fSomethingChanged = TRUE;
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
            InsertCommands(*pepic, iPos, commands.size(), &commands[0]);
            fSomethingChanged = TRUE;
        }
    }
    return fSomethingChanged;
}

