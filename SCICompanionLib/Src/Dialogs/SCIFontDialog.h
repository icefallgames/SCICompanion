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


// CSCIFontDialog

class CSCIFontDialog : public CFontDialog
{
	DECLARE_DYNAMIC(CSCIFontDialog)

public:
	CSCIFontDialog(LPLOGFONT lplfInitial = NULL,
			DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
			CDC* pdcPrinter = NULL,
			CWnd* pParentWnd = NULL);
#ifndef _AFX_NO_RICHEDIT_SUPPORT
	CSCIFontDialog(const CHARFORMAT& charformat,
			DWORD dwFlags = CF_SCREENFONTS,
			CDC* pdcPrinter = NULL,
			CWnd* pParentWnd = NULL);
#endif
	virtual ~CSCIFontDialog();

protected:
	DECLARE_MESSAGE_MAP()
};


