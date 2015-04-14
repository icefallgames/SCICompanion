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

#if (!defined __EXT_LABEL_H)
#define __EXT_LABEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#include <vfw.h>

class CExtPopupMenuTipWnd;

/////////////////////////////////////////////////////////////////////////////
// CExtLabel window

class __PROF_UIS_API CExtLabel
	: public CStatic
	, public CExtPmBridge
{
public:
	DECLARE_DYNCREATE( CExtLabel );
	DECLARE_CExtPmBridge_MEMBERS( CExtLabel );
	CExtLabel();
	virtual ~CExtLabel();

	CRect m_rcLabelTextMargins;

	void SetFontBold( bool bSet = true );
	void SetFontItalic( bool bSet = true );
	void SetFontUnderline( bool bSet = true );
	void SetFontStrikeOut( bool bSet = true );
	void SetBkColor( COLORREF clrBk );
	void SetTextColor(
		bool bEnabled,
		COLORREF clrText = COLORREF(-1L)
		);
	bool GetFontBold();
	bool GetFontItalic();
	bool GetFontUnderline();
	bool GetFontStrikeOut();
	COLORREF GetBkColor() const;
	COLORREF GetTextColor( bool bEnabled ) const;
	
	enum e_ImageMode_t
	{
		eAlign = 0,			// The image is aligned according to the text aligning styles (the SS_LEFT, SS_CENTER and SS_RIGHT styles)
		eTile = 1,			// The image is repeated until the entire available area is filled. 
		eStretch = 2,		// The image is stretched to fit all the available area. 
		eTouchInside = 3,	// Stretch the image proportionally and touch inside.
		eTouchOutside = 4,	// Stretch the image proportionally and touch outside.
	};

	e_ImageMode_t SetImageMode( e_ImageMode_t eImageMode );
	e_ImageMode_t GetImageMode() const;
	const CExtBitmap * GetBitmapEx() const;
	CExtBitmap * GetBitmapEx();
	bool SetBitmapEx( CExtBitmap * pBmp  );
	void SetTooltipText( int nId );
	void SetTooltipText( __EXT_MFC_SAFE_LPCTSTR sText );
	INT m_nAdvancedTipStyle; // CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON
	bool m_bHelper_AutoAdjust_SS_NOTIFY_style:1;
	static bool g_bHelper_AutoAdjust_SS_NOTIFY_style;
protected:
	CExtSafeString m_strToolTipText;
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea
		) const;

	//{{AFX_VIRTUAL(CExtLabel)
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual void OnEraseBackground(
		CDC & dc,
		const CRect & rcClient
		);
	virtual void OnDrawLabelText(
		CDC & dc,
		const RECT & rcText,
		__EXT_MFC_SAFE_LPCTSTR strText,
		DWORD dwDrawTextFlags,
		bool bEnabled
		);
	virtual COLORREF OnQueryTextColor(
		bool bEnabled
		) const;

protected:
	bool m_bFontBold:1, m_bFontItalic:1, m_bFontUnderline:1, m_bFontStrikeOut:1, m_bInitText:1;
	COLORREF m_clrTextNormal, m_clrTextDisabled, m_clrBackground;
	CExtSafeString m_sText;
	CExtBitmap m_bmp;
	e_ImageMode_t m_eImageMode;

	virtual HFONT OnQueryFont() const;
	virtual void DoPaint( CDC * pDC, CRect & rcClient );

protected:
	//{{AFX_MSG(CExtLabel)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; /// class CExtLabel

/////////////////////////////////////////////////////////////////////////////
// CExtAviLabel window

#define __EXT_AVI_PLAY_AUTOMATICALLY			0x00000010
#define __EXT_AVI_TRANSPARENT_VIDEO				0x00000020
#define __EXT_AVI_EMPTY_BACKGROUND_ON_STOP		0x00000040
#define __EXT_AVI_USER_DEFINED_28				0x00000080
#define __EXT_AVI_USER_DEFINED_27				0x00000010
#define __EXT_AVI_USER_DEFINED_26				0x00000020
#define __EXT_AVI_USER_DEFINED_25				0x00000040
#define __EXT_AVI_USER_DEFINED_24				0x00000080
#define __EXT_AVI_USER_DEFINED_23				0x00000100
#define __EXT_AVI_USER_DEFINED_22				0x00000200
#define __EXT_AVI_USER_DEFINED_21				0x00000400
#define __EXT_AVI_USER_DEFINED_20				0x00000800
#define __EXT_AVI_USER_DEFINED_19				0x00001000
#define __EXT_AVI_USER_DEFINED_18				0x00002000
#define __EXT_AVI_USER_DEFINED_17				0x00004000
#define __EXT_AVI_USER_DEFINED_16				0x00008000
#define __EXT_AVI_USER_DEFINED_15				0x00010000
#define __EXT_AVI_USER_DEFINED_14				0x00020000
#define __EXT_AVI_USER_DEFINED_13				0x00040000
#define __EXT_AVI_USER_DEFINED_12				0x00080000
#define __EXT_AVI_USER_DEFINED_11				0x00100000
#define __EXT_AVI_USER_DEFINED_10				0x00200000
#define __EXT_AVI_USER_DEFINED_09				0x00400000
#define __EXT_AVI_USER_DEFINED_08				0x00800000
#define __EXT_AVI_USER_DEFINED_07				0x01000000
#define __EXT_AVI_USER_DEFINED_06				0x02000000
#define __EXT_AVI_USER_DEFINED_05				0x04000000
#define __EXT_AVI_USER_DEFINED_04				0x08000000
#define __EXT_AVI_USER_DEFINED_03				0x10000000
#define __EXT_AVI_USER_DEFINED_02				0x20000000
#define __EXT_AVI_USER_DEFINED_01				0x40000000
#define __EXT_AVI_USER_DEFINED_00				0x80000000
#define __EXT_AVI_DEFAULT_STYLES				0

#define __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME		_T("AVI")
#define __EXT_MFC_AVI_SPEED_RATE_MIN					0.0
#define __EXT_MFC_AVI_SPEED_RATE_MAX					10.0

class __PROF_UIS_API CExtAviLabel : public CExtLabel
{
public:
	DECLARE_DYNCREATE( CExtAviLabel );
	CExtAviLabel();
	virtual ~CExtAviLabel();
public:
	virtual bool AviPlay(
		UINT nFrom = 0,
		UINT nTo   = UINT(-1),
		UINT nRep  = UINT(-1)
		);
	virtual bool AviStop(
		bool bResetToFirst = false
		);
	virtual bool AviSeek( UINT nTo );
	virtual CSize AviFrameGetSize() const;
	virtual UINT AviFrameGetFrom() const;
	virtual UINT AviFrameGetTo() const;
	virtual UINT AviFrameGetCurrent() const;
	virtual UINT AviFrameGetCount() const;
	virtual bool AviIsPlaying() const;
	virtual bool AviOpen(
		UINT nAviResourceID,
		__EXT_MFC_SAFE_LPCTSTR strAviResourceSection = NULL // NULL means __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME
		);
	virtual bool AviOpen(
		UINT nAviResourceID,
		__EXT_MFC_SAFE_LPCTSTR strAviResourceSection, // NULL means __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME
		WORD wLangIdDesired,
		WORD wLangIdNeutral
		);
	virtual bool AviOpen(
		LPCVOID pAviBytes,
		DWORD dwAviSize
		);
	virtual bool AviOpen(
		__EXT_MFC_SAFE_LPCTSTR strAviFilePath
		);
	virtual DWORD AviStyleGet() const;
	virtual bool AviStyleSet( DWORD dwAviStyle );
	virtual bool AviIsEmpty() const;
	virtual void AviEmpty();
	virtual COLORREF AviTransparentColorGet() const;
	virtual void AviTransparentColorSet(
		COLORREF clrTransparent = COLORREF(-1L) // COLORREF(-1L) - means no transparent color
		);

	virtual void OnAviPaintCurrentFrame( CDC & dc, const CRect & rcDrawDst, const CSize & sizeAvi );
	virtual COLORREF OnAviCalcTransparentColor();
	virtual CExtSafeString OnAviGenTempFileName();

	virtual void DoPaint( CDC * pDC, CRect & rcClient );
	virtual bool AviSizeFromContent(
		CSize & sizeBefore,
		CSize & sizeAfter,
		bool bMoveWindow = true
		);
	bool AviSizeFromContent(
		bool bMoveWindow = true
		);

	void AviSpeedRateSet( DOUBLE lfAviSpeedRate );
	DOUBLE AviSpeedRateGet();
	virtual CExtSafeString AviTempFileNameGet();


	//{{AFX_VIRTUAL(CExtAviLabel)
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	//static void CALLBACK stat_PlayTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime );
	//{{AFX_MSG(CExtAviLabel)
	protected:
	afx_msg void OnSize( UINT nType, int cx, int cy );
	//}}AFX_MSG
	afx_msg void OnTimer( __EXT_MFC_UINT_PTR nIDEvent );
	DECLARE_MESSAGE_MAP()

protected:
	virtual void _Impl_Init();
	virtual void _Impl_PostLoadSync();
	virtual void _Impl_CalcTime();
	virtual void _Impl_CalcRate();
	DWORD m_dwAviStyle;
	CExtSafeString m_strTempFileName;
	CPoint m_ptAviOffset;
	CSize m_sizeAvi;
	bool m_bPlaying:1, m_bSeek:1;
	COLORREF m_clrTransparent, m_clrTransparentEffective;
	HDRAWDIB m_hDrawDib;
	UINT m_nAviTimerID, m_nAviTimerEllapse, m_nAviFrameCount, m_nAviFrameTotalCount, m_nAviFrameCurrent, m_nAviFrameFrom, m_nAviFrameTo, m_nRep;
	PGETFRAME m_pGF;
	PAVISTREAM m_pAS;
	DOUBLE m_lfAviSpeedRate;
}; /// class CExtAviLabel

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_LABEL_H)
