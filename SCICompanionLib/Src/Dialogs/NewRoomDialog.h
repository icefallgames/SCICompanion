#pragma once

#include "NewScriptDialog.h"

// fwd decl
class ScriptId;

//
// CNewRoomDialog dialog
//
// This dialog will return the new scriptnumber, the text to use in this script,
// and the script name (script id).  It will also write the appropriate entry in
// the game.ini
//
class CNewRoomDialog : public CNewScriptDialog
{
	DECLARE_DYNAMIC(CNewRoomDialog)

public:
	CNewRoomDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewRoomDialog();

// Dialog Data
	enum { IDD = IDD_NEWROOM };

protected:
    virtual void OnOK();
    virtual void _PrepareDialog();
    virtual void _DiscoveredScriptName(PCTSTR pszName);
    virtual void _PrepareBuffer();
    int _GetMinSuggestedScriptNumber() override;
    void _PrepareBufferOld();
    virtual void _AttachControls(CDataExchange *pDX);
	DECLARE_MESSAGE_MAP()
    afx_msg void OnRadioSameAsRoom();
    afx_msg void OnRadioOther();

    int _nPicScript;

    CExtRadioButton m_wndRadioUseRoom;
    CExtRadioButton m_wndRadioUseOther;
    CListBox m_wndListBox;
    CExtEdit m_wndEditPicNumber;
    CExtCheckBox m_wndCheckMessage;

    // Visuals
    CExtEdit m_wndRoomNumber;
    CExtLabel m_wndRoomNumberLabel;
    CExtLabel m_wndUses;
    CExtGroupBox m_wndPicNumberGroup;
};
