#pragma once


// CResourceListChildFrame frame

#include "MDITabChildWnd.h"
#include "BarContainerDialog.h"

class ResourcePreviewer;

class CGameExplorerFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CGameExplorerFrame)
protected:
	CGameExplorerFrame();           // protected constructor used by dynamic creation
	virtual ~CGameExplorerFrame();
    virtual MDITabType GetTabType() { return TAB_GAME; }

public:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    void OnResourceSelected(const ResourceBlob &blob);
    void OnResourceTypeShown(ResourceType type);

protected:
	DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnShowResource(UINT nId);
    afx_msg void OnUpdateResourceTypeCommand(CCmdUI *pCmdUI);

private:
    ResourcePreviewer *_GetPreviewer(ResourceType iType);

    CExtControlBar m_wndResizableBarPreview;
    CBarContainerDialog m_dlgEmpty;

	typedef std::unordered_map<ResourceType, ResourcePreviewer*> previewer_map;
    previewer_map _previewers;
};


