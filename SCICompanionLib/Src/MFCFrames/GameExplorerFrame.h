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
#pragma once


// CResourceListChildFrame frame

#include "MDITabChildWnd.h"
#include "BarContainerDialog.h"

class ResourcePreviewer;
class ResourceBlob;

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
    afx_msg void OnSysCommand(UINT nID, LPARAM param);


private:
    ResourcePreviewer *_GetPreviewer(ResourceType iType);

    CExtControlBar m_wndResizableBarPreview;
    CBarContainerDialog m_dlgEmpty;

	typedef std::unordered_map<ResourceType, ResourcePreviewer*> previewer_map;
    previewer_map _previewers;
};


