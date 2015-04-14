#pragma once


// CVocabChildFrame frame

#include "MDITabChildWnd.h"

class CVocabChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CVocabChildFrame)
protected:
	CVocabChildFrame();           // protected constructor used by dynamic creation
	virtual ~CVocabChildFrame();
    virtual MDITabType GetTabType() { return TAB_VOCAB; }

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


