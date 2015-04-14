#pragma once


// CSoundChildFrame frame

#include "MDITabChildWnd.h"

class CSoundChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CSoundChildFrame)
protected:
	CSoundChildFrame();           // protected constructor used by dynamic creation
	virtual ~CSoundChildFrame();
    virtual MDITabType GetTabType() { return TAB_SOUND; }

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


