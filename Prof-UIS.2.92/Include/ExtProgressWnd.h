// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2011 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// mailto:support@prof-uis.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#if (!defined __EXT_PROGRESS_WND_H)
#define __EXT_PROGRESS_WND_H

#if (!defined __EXT_MFC_NO_PROGRESS_WND)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class __PROF_UIS_API CExtProgressWnd
	: public CProgressCtrl
	, public CExtPmBridge
{
public:
	DECLARE_SERIAL( CExtProgressWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtProgressWnd );
    CExtProgressWnd();
    virtual ~CExtProgressWnd();
	COLORREF m_clrProgressBarFrom, m_clrProgressBarTo,
		m_clrBackgroundArea, m_clrBorderLT, m_clrBorderRB;
	double m_lfEnlightClrProgressBarFrom, m_lfEnlightClrProgressBarTo;
	CSize m_sizeBorderSpace, m_sizeDistanceBetweenBlocks,
		m_sizeBlockScaleFactorH, m_sizeBlockScaleFactorV;
	bool m_bHasBorder:1, m_bHorizontalGradientBars:1,
		m_bImmediateUpdating:1, m_bUsePaintManager:1,
		m_bEndlessMode:1;
	LONG m_nEndlessNominator, m_nEndlessDenominator;
	virtual void OnPaintProgress(
		CDC & dc,
		CRect rcClient
		);
protected:
    //{{AFX_VIRTUAL(CExtProgressWnd)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

    //{{AFX_MSG(CExtProgressWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC * pDC );
	afx_msg void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR * lpncsp );
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
}; // class CExtProgressWnd

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_MFC_NO_PROGRESS_WND)

#endif // (!defined __EXT_PROGRESS_WND_H)
