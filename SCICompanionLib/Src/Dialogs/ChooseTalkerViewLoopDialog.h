#pragma once


// SaveResourceDialog dialog
#include "resource.h"

class ChooseTalkerViewLoopDialog : public CExtResizableDialog
{
public:
    ChooseTalkerViewLoopDialog(uint16_t talker, int view, int loop, CWnd* pParent = nullptr);   // standard constructor
    virtual ~ChooseTalkerViewLoopDialog();
    int GetView() { return _nView; }
    int GetLoop() { return _nLoop; }

    virtual void OnOK();

    // Dialog Data
    enum { IDD = IDD_VIEWLOOP};

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    int _nView;
    int _nLoop;
    uint16_t _talker;

    CExtEdit m_wndEditView;
    CExtEdit m_wndEditLoop;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
};
