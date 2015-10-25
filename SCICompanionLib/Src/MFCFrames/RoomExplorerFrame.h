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


