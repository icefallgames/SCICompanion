#pragma once
// CSliderDialogBar

#include "NonViewClient.h"

class CPicDoc; // fwd decl

class CPositionSlider : public CExtSliderWnd, public INonViewClient
{
public:
	CPositionSlider();
	virtual ~CPositionSlider();

    void SetDocument(CDocument *pDoc);
    virtual void UpdateNonView(CObject *pObject) override;
    void OnThumbPosition(NMHDR *pnmhdr, LRESULT *plres);
    virtual void HScroll(UINT i, UINT j);

protected:
	DECLARE_MESSAGE_MAP()
    LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);

private:
    CPicDoc *_pDoc;
    UINT _lastPos;
};

