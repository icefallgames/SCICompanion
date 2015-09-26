#pragma once

#include "ResourceDocument.h"
#include "UndoResource.h"
#include "Components.h"
#include "ResourceEntity.h"
#include "ResourceEntityDocument.h"
#include "RasterOperations.h"
#include "ResourceMapEvents.h"
#include "DocumentWithPaletteChoices.h"

// CNewRasterResourceDocument document

// A sort of workaround
CHintWithObject<CelIndex> WrapRasterChange(RasterChange change);

struct ImageSequenceItem;

class CNewRasterResourceDocument : public ResourceEntityDocument, public IResourceMapEvents, public DocumentWithPaletteChoices
{
    DECLARE_DYNCREATE(CNewRasterResourceDocument)

public:
    CNewRasterResourceDocument();

    void SetResource(std::unique_ptr<ResourceEntity> pResource, int id = -1);
    void SetNewResource(std::unique_ptr<ResourceEntity> pResource);

    void LockResource(bool fLock) { _fLocked = fLock; }
    PenStyle GetPenStyle() { return _penStyle; }
    void SetPenStyle(PenStyle penStyle);
    void SetApplyToAllCels(BOOL fApply) { _fApplyToAllCels = !!fApply; UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::ApplyToAll)); }
    BOOL GetApplyToAllCels() const { return _fApplyToAllCels; }

    // THESE ARE TODO
    CelIndex GetSelectedIndex() { _ValidateCelIndex(); return CelIndex(_nLoop, _nCel); }
    int GetSelectedGroup(CelIndex *rgGroups, size_t ceGroup);
    void MoveSelectedCel(CPoint point);

    uint8_t GetViewColor() { return _color; }
    uint8_t GetAlternateViewColor() { return _alternateColor; }
    void SetViewColor(uint8_t color);
    void SetAlternateViewColor(uint8_t color);
    COLORREF SCIColorToCOLORREF(uint8_t color);

    const RGBQUAD *GetPaletteVGA() const;
    int GetDefaultZoom() const;
    void SetDefaultZoom(int iZoom) const;
    BOOL CanDeleteCels() const { return TRUE; } // REVIEW: Not true for fonts?

    int GetSelectedCel() const { return _nCel; }
    int GetSelectedLoop() const { return _nLoop; }
    void SetSelectedLoop(int nLoop);
    void SetSelectedCel(int nCel);
    void GetLabelString(PTSTR  pszLabel, size_t cch, int nCel) const { StringCchPrintf(pszLabel, cch, TEXT("%d"), nCel); }

    void SetPreviewLetters(std::string &previewLetters)
    {
        _previewLetters = previewLetters;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::SampleText));
    }
    std::string GetPreviewLetters() { return _previewLetters;  }
    void MakeFont();
    void ExportAsGif();

    bool v_PreventUndos() { return _fLocked; }
    void v_OnUndoRedo();

    template<typename _T>
    const _T &GetComponent()
    {
        return GetResource()->GetComponent<_T>();
    }

    void PostApplyChanges(CObject *pObj) override;

    // IResourceMapEvents
    void OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior);
    void OnResourceDeleted(const ResourceBlob *pData) {}
    void OnResourceMapReloaded(bool isInitialLoad) {}
    void OnResourceTypeReloaded(ResourceType iType) {}

    bool v_IsVGA() override;
    void v_OnUpdatePaletteOptions() override;
    const ResourceEntity *v_GetResource() override;
    void v_OnUpdatePaletteChoice() override;

    void OnCloseDocument() override;

private:
    void _OnExportAsImageWorker(CelIndex celIndex);

    virtual ResourceType _GetType() const
    {
        const ResourceEntity *pResource = static_cast<const ResourceEntity*>(GetResource());
        return pResource->GetType();
    }

    // Ensure the currently selected cel index is correct.
    void _SetInitialPalette();
    void _ValidateCelIndex();
    void _UpdateHelper(RasterChange change);
    void _TrimUndoStack();
    void _InsertFiles(const std::vector<std::string> &files);
    void _ApplyImageSequenceNew(uint8_t transparentColor, const PaletteComponent *optionalNewPalette, std::vector<ImageSequenceItem> &items, bool fixedPalette, int paletteSize);
    std::list<ResourceEntity*>::iterator _GetLastUndoFrame();
    bool _GetColors(const RasterComponent &raster, const PaletteComponent *optionalNewPalette,
        const uint8_t **paletteMapping,
        int *colorCount,
        const RGBQUAD **colors
        );

    template<typename _T>
    const _T &GetComponent() const
    {
        return GetResource()->GetComponent<_T>();
    }
    
    DECLARE_MESSAGE_MAP()
    afx_msg void OnAnimate();
    afx_msg void OnImportImageSequence();
    afx_msg void OnExportCelAsImage();
    afx_msg void OnExportLoopAsImage();
    afx_msg void OnExportViewAsImage();
    afx_msg void OnUpdateImportImage(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEGA(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAnimate(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFont(CCmdUI *pCmdUI);

    bool _fLocked;
    PenStyle _penStyle;
    bool _fApplyToAllCels;
    int _nCel;
    int _nLoop;

    uint8_t _color;
    uint8_t _alternateColor;

    // These should go away
    std::string _previewLetters = "!@#$%^&*()_+0123456789-=~`ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz{}|[]\\:\";'<>?,./";
};
