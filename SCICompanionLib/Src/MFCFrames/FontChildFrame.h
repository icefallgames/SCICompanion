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

#include "MdiTabChildWnd.h"
#include "RasterSidePane.h"

// fwd decl
class CRasterView;
class CFontPreviewView;

// CEditFontChildFrame frame with splitter

class CEditFontChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CEditFontChildFrame)
protected:
	CEditFontChildFrame();           // protected constructor used by dynamic creation
	virtual ~CEditFontChildFrame();
    virtual MDITabType GetTabType() { return TAB_FONT; }

public:
    void HookUpNonViews(CDocument *pDoc);
    BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

	CExtSplitterWnd m_wndSplitter;
    CExtSplitterWnd m_wndSplitterTop;
    
    CRasterView *_pViewMainView;
    CFontPreviewView *_pFontPreviewView;
};


