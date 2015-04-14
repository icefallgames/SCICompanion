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

