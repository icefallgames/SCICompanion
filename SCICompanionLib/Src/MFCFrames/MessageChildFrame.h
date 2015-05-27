#pragma once

#include "MDITabChildWnd.h"
#include "MessageNounConditionPane.h"
#include "MessageEditPane.h"

class CMessageChildFrame : public CMDITabChildWnd
{
    DECLARE_DYNCREATE(CMessageChildFrame)
protected:
    CMessageChildFrame();           // protected constructor used by dynamic creation
    virtual ~CMessageChildFrame();
    virtual MDITabType GetTabType() { return TAB_MESSAGE; }

    void HookUpNonViews(CDocument *pDoc) override;

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

    MessageNounConditionPane m_wndNounConditionPane;
    CExtControlBar m_wndResizableBarNounCondition;
    MessageEditPane m_wndMessageEditPane;
    CExtControlBar m_wndResizableBarMessageEdit;
};


