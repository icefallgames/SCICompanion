#pragma once

#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
// CBaseColorDialog dialog

class CBaseColorDialog : public CExtResizableDialog
{
public:
    CBaseColorDialog(UINT nID, CWnd* pParent);
    void SetTrackRect(RECT *prc) { _rcTrack = *prc; _fUseTrack = TRUE; }

    virtual ~CBaseColorDialog();

protected:
    virtual void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

    void _SetPosition(); // Determine the right position to start this in.

    RECT _rcTrack;
    BOOL _fUseTrack;
    BOOL _fEnded;

    CWnd m_wndDummyOwner;
};
