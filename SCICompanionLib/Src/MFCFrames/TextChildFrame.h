#pragma once


// CTextChildFrame frame

#include "MDITabChildWnd.h"

class CTextChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CTextChildFrame)
protected:
	CTextChildFrame();           // protected constructor used by dynamic creation
	virtual ~CTextChildFrame();
    virtual MDITabType GetTabType() { return TAB_TEXT; }

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


