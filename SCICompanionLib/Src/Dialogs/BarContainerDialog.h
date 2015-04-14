#pragma once

#include "Resource.h"
// CBarContainerDialog dialog

class CBarContainerDialog : public CExtResizableDialog
{
public:
	CBarContainerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBarContainerDialog();

// Dialog Data
	enum { IDD = IDD_DIALOGEMPTY };

    // iType can be any unique type (e.g. MDITabType, or ResourceType)
    // REVIEW: we could templatize this.
    void SelectChild(CWnd *pWnd, int iType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	typedef std::unordered_map<int, CWnd *> windowmap;
    windowmap _childWindows;
};
