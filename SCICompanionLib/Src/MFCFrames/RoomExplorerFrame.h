#pragma once


// CRoomExplorerFrame frame

#include "MDITabChildWnd.h"

class CRoomExplorerFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CRoomExplorerFrame)
protected:
	CRoomExplorerFrame();           // protected constructor used by dynamic creation
	virtual ~CRoomExplorerFrame();
    virtual MDITabType GetTabType() { return TAB_ROOMEXPLORER; }

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
    
    CToolBar m_wndToolBar;
};


