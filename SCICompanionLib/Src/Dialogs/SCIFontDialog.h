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


