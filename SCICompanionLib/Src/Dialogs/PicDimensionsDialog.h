#pragma once

#include "resource.h"

class PicDimensionsDialog : public CExtResizableDialog
{
public:
    PicDimensionsDialog(size16 size, CWnd* pParent = NULL);   // standard constructor

    size16 GetSize() { return _size; }
    virtual void OnOK();

// Dialog Data
	enum { IDD = IDD_PICSIZE };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    size16 _size;
    CExtEdit m_wndEditWidth;
    CExtEdit m_wndEditHeight;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
};
