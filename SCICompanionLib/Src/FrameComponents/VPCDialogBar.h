#pragma once


#include "NonViewClient.h"
#include "ColorButton.h"
#include "ExtDialogFwdCmd.h"
#include "NonViewClient.h"

class CPicDoc;

// CVPCDialogBar

class CVPCDialogBar : public CExtDialogFwdCmd, public INonViewClient
{
public:
    CVPCDialogBar(CWnd* pParent = NULL);
	virtual ~CVPCDialogBar();

    void SetDocument(CDocument *pDoc);

    // INonViewClient
    virtual void UpdateNonView(CObject *pObject) override;

protected:
    void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

private:
    CPicDoc *_pDoc;

    CExtCheckBox m_wndVToggle;
    CExtCheckBox m_wndPToggle;
    CExtCheckBox m_wndCToggle;
    CColorPickerButton m_wndVSet;
    CColorPickerButton m_wndPSet;
    CColorPickerButton m_wndCSet;

    CBrushButton m_wndPenSet;

    // Visuals
    CExtLabel m_wndLabel;
};


