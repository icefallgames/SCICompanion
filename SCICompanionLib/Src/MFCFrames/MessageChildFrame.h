#pragma once

#include "MDITabChildWnd.h"

class CMessageChildFrame : public CMDITabChildWnd
{
    DECLARE_DYNCREATE(CMessageChildFrame)
protected:
    CMessageChildFrame();           // protected constructor used by dynamic creation
    virtual ~CMessageChildFrame();
    virtual MDITabType GetTabType() { return TAB_MESSAGE; }

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};


