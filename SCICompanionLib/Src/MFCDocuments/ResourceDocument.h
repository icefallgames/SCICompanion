#pragma once

#include "ResourceUtil.h"
#include "NonViewHostDocument.h"

class ResourceEntity;

//
// CResourceDocument document
// 
// Documents that represent a particular SCI resource should inherit from this.
// It provides saving/export functionality that is common across resource types.
//
class CResourceDocument : public CNonViewHostDocument, public IResourceIdentifier
{
	DECLARE_DYNAMIC(CResourceDocument)

public:
    CResourceDocument() { _checksum = -1; _fMostRecent = true; }
    BOOL CanCloseFrame(CFrameWnd* pFrameArg);
	virtual BOOL SaveModified(); // return TRUE if ok to continue
    void OnFileSave();
    void OnFileSaveAs();

    // This returns a const pointer - we should never manipulate the resource via this.
    virtual const ResourceEntity *GetResource() const = 0;

    // IResourceIdentifier
    int GetPackageHint() const override;
    int GetNumber() const override;
    uint32_t GetBase36() const override;
    ResourceType GetType() const override;
    int GetChecksum() const override { return _checksum; }

    bool IsMostRecent() const;

private:
    afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI) { pCmdUI->Enable(IsModified()) ; }
    afx_msg void OnUpdateResSize(CCmdUI *pCmdUI);
    
    BOOL DoPreResourceSave(BOOL fSaveAs);
    void OnExportAsResource();

protected:
    void OnExportAsBitmap();

    BOOL _DoResourceSave(int iPackageNumber, int iResourceNumber);
    void _UpdateTitle();
    afx_msg void OnUpdateAlwaysOn(CCmdUI *pCmdUI) { pCmdUI->Enable() ; }

    // Override to prevent saving (by returning false)
    virtual bool v_DoPreResourceSave() { return true; }
    virtual void _OnSuccessfulSave(const ResourceEntity *pResource) {};
    virtual ResourceType _GetType() const = 0;

    // Disabled until we see what people are doing with this
    // virtual PCTSTR _GetFileNameDefault() { return g_resourceInfo[(int)_GetType()].pszFileNameDefault; }
    virtual PCTSTR _GetTitleDefault() { return g_resourceInfo[(int)_GetType()].pszTitleDefault; }
    virtual std::string _GetFileDialogFilter();

    DECLARE_MESSAGE_MAP()

    int _checksum;
    bool _fMostRecent;
};
