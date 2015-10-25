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

#include "DialogFactory.h"
#include "PicCommandsCommon.h"

// CColorPickerButton

class CColorPickerButton : public CButton
{
	DECLARE_DYNAMIC(CColorPickerButton)

public:
	CColorPickerButton();
	virtual ~CColorPickerButton();
    void SetColorAndState(RGBQUAD color, BOOL bOn);
    void SetColorAndState(EGACOLOR color, BOOL bOn);
    void SetDialogFactory(IDialogFactory *pFactory) { _pFactory = pFactory; }

protected:
    virtual void _OnDraw(HDC hdc, RECT *prc);
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void DrawItem(LPDRAWITEMSTRUCT pdis);

	DECLARE_MESSAGE_MAP()

    IDialogFactory *_pFactory;
    EGACOLOR _color;
    RGBQUAD _color1;
    RGBQUAD _color2;
    BOOL _bOn;
};


//
// CBrushButton
// (subclassed from CColorPickerButton due to laziness)
//
class CBrushButton : public CColorPickerButton
{
    DECLARE_DYNAMIC(CBrushButton)

public:
	CBrushButton();
	virtual ~CBrushButton();
    void SetPenStyle(const PenStyle &penStyle);

protected:
    virtual void _OnDraw(HDC hdc, RECT *prc);
    
private:
    PenStyle _penStyle;
};
