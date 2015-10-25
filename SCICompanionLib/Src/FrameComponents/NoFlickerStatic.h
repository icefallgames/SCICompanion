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

//
// CExtNoFlickerStatic
// Helper class for drawing images.  The regular CStatic's SetBitmap
// behaves differently on different OS's, and is prone to leaks as a result.
// NOTE: We may not need this anymore... it's possible prof-uis's CExtLabel would suffice?
//
class CExtNoFlickerStatic : public CExtLabel
{
public:
    CExtNoFlickerStatic() {}
    virtual ~CExtNoFlickerStatic() {}
    // This does *not* take ownership of the HBITMAP
    bool FromBitmap(HBITMAP hbmp, int cx, int cy, bool correctForAspectRatio);

protected:
	DECLARE_MESSAGE_MAP()
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    CExtBitmap _bitmap;
    int _cx;
    int _cy;
    bool _correctForAspectRatio;
};

