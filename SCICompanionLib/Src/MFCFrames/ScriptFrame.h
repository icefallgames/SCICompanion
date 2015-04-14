#pragma once


// CScriptFrame frame

#include "MDITabChildWnd.h"

class CScriptFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CScriptFrame)

public:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CScriptFrame();           // protected constructor used by dynamic creation
	virtual ~CScriptFrame();
    virtual MDITabType GetTabType() { return TAB_SCRIPT; }

protected:
	DECLARE_MESSAGE_MAP()
};


