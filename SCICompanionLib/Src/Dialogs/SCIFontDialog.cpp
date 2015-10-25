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
// SCIFontDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SCIFontDialog.h"


// CSCIFontDialog

IMPLEMENT_DYNAMIC(CSCIFontDialog, CFontDialog)
CSCIFontDialog::CSCIFontDialog(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
}

CSCIFontDialog::~CSCIFontDialog()
{
}


BEGIN_MESSAGE_MAP(CSCIFontDialog, CFontDialog)
END_MESSAGE_MAP()



// CSCIFontDialog message handlers

