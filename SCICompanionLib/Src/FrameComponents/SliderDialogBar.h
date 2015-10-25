/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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

