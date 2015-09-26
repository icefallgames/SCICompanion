#pragma once


// ResourceListDoc document

// FWD decl
class ResourceBlob;

#include "ResourceMap.h"
#include "NonViewHostDocument.h"
#include "SyncResourceMap.h"

enum class ResourceMapChangeHint
{
    // A resource was added or deleted from the resource map.  The Object parameter empty
    Change      = 0x00000040,
    // A particular resource was added.  The Object parameter is a ResourceBlob
    Added = 0x00000100,
    Replaced = 0x00000800,
    // A particular resource was deleted.  The Object parameter is a ResourceBlob
    Deleted = 0x00000200,
    // A particular resource type was changed (outside our control), tell that type to reload. The Object is a ResourceType
    Type = 0x00000400,
    // Show a resource type. The Object is a ResourceType
    ShowType = 0x00001000
};

DEFINE_ENUM_FLAGS(ResourceMapChangeHint, uint32_t)


//
// Manages a bunch of ResourceBlob's.
//
class CResourceListDoc : public CNonViewHostDocument, public IResourceMapEvents
{
	DECLARE_DYNCREATE(CResourceListDoc)

public:
	CResourceListDoc();
	virtual ~CResourceListDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // IResourceMapEvents
    void OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior);
    void OnResourceDeleted(const ResourceBlob *pData);
    void OnResourceMapReloaded(bool isInitialLoad);
    void OnResourceTypeReloaded(ResourceType iType);

    void ShowResourceType(ResourceType iType);    
    ResourceType GetShownResourceType() { return _shownResourceType; }

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    void OnDisableCommand(CCmdUI *pCmdUI);

	virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual void OnCloseDocument();

	DECLARE_MESSAGE_MAP()

private:
    ResourceType _shownResourceType;
};
