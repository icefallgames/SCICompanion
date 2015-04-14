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

#if (!defined __EXT_EDIT_H)
#define __EXT_EDIT_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#include <AfxRich.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExtPopupMenuWnd;
class CExtPopupMenuTipWnd;

/////////////////////////////////////////////////////////////////////////////
// CExtEditBase window

class __PROF_UIS_API CExtEditBase
	: public CEdit
	, public CExtPmBridge
{
public:
	struct __PROF_UIS_API CExtNotImplRichEditOleCallBack : public IRichEditOleCallback
	{
		STDMETHOD( QueryInterface ) ( REFIID riid, LPVOID FAR * lplpObj )
		{
			if( lplpObj == NULL )
				return E_INVALIDARG;
			(*lplpObj) = NULL;
			if( riid == IID_IUnknown || riid == IID_IRichEditOleCallback )
			{
				(*lplpObj) = this;
				return S_OK;
			}
			return E_NOTIMPL;
		}
		STDMETHOD_( ULONG, AddRef ) ()
		{ return 1; }
		STDMETHOD_( ULONG, Release ) ()
		{ return 1; }
		STDMETHOD( GetNewStorage ) ( LPSTORAGE FAR * lplpstg )
		{ lplpstg; return E_NOTIMPL; }
		STDMETHOD( GetInPlaceContext ) (LPOLEINPLACEFRAME FAR * lplpFrame, LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo )
		{ lplpFrame; lplpDoc; lpFrameInfo; return E_NOTIMPL; }
		STDMETHOD( ShowContainerUI ) ( BOOL fShow )
		{ fShow; return E_NOTIMPL; }
		STDMETHOD( QueryInsertObject ) (LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp )
		{ lpclsid; lpstg; cp; return E_NOTIMPL; }
		STDMETHOD( DeleteObject ) ( LPOLEOBJECT lpoleobj )
		{ lpoleobj; return E_NOTIMPL; }
		STDMETHOD( QueryAcceptData ) ( LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict )
		{ lpdataobj; lpcfFormat; reco; fReally; hMetaPict; return E_NOTIMPL; }
		STDMETHOD( ContextSensitiveHelp ) ( BOOL fEnterMode )
		{ fEnterMode; return E_NOTIMPL; }
		STDMETHOD( GetClipboardData ) ( CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj )
		{ lpchrg; reco; lplpdataobj; return E_NOTIMPL; }
		STDMETHOD( GetDragDropEffect ) ( BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect )
		{ fDrag; grfKeyState; (*pdwEffect) = DROPEFFECT_NONE; return S_OK; }
		STDMETHOD( GetContextMenu ) ( WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu )
		{ seltype; lpoleobj; lpchrg; lphmenu; return E_NOTIMPL; }
	};
	DECLARE_DYNCREATE( CExtEditBase );
	DECLARE_CExtPmBridge_MEMBERS( CExtEditBase );
	DECLARE_PROF_UIS_WINDOW_METHODS;
	CExtEditBase();
	virtual ~CExtEditBase();
public:
	bool m_bRichMode:1, m_bHandleCtxMenus:1, m_bHandleAltKeyCodes:1;
	static HWND g_hWndEditInAltKeyCodeMode;
protected:
	bool m_bAltKeyCodeMode:1;
	LONG m_nTrackedAltKeyCode;
	CToolTipCtrl m_wndToolTip;
	void InitToolTip();
	COLORREF m_clrBack;
	COLORREF m_clrBackPrev;
	CBrush m_brBack;
	COLORREF m_clrText;
protected:
    CExtSafeString m_strCueBanner;
public:
    bool SetCueBanner( __EXT_MFC_SAFE_LPCTSTR lpcText );
    bool GetCueBanner( __EXT_MFC_SAFE_LPTSTR lpText, int cchText ) const;
    CExtSafeString GetCueBanner() const;

	//{{AFX_VIRTUAL(CExtEditBase)
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	CExtSafeString m_strToolTipText;
	bool m_bToolTipActive:1;
public:
	INT m_nAdvancedTipStyle; // CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea
		) const;

	void SetTooltipText(
		int nId,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		CExtSafeString * spText,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		CExtSafeString & sText,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		__EXT_MFC_SAFE_LPCTSTR sText,
		BOOL bActivate = TRUE
		);
	void ActivateTooltip(
		BOOL bEnable = TRUE
		);
	void SetBkColor( COLORREF clrBk );
	COLORREF GetBkColor() const;
	void SetTextColor( COLORREF clrText );
	COLORREF GetTextColor() const;
protected:
	virtual __EXT_MFC_INT_PTR OnToolHitTest( CPoint point, TOOLINFO * pTI ) const;
	virtual COLORREF OnQueryBackColor() const;
	virtual COLORREF OnQueryTextColor() const;
	virtual int OnQueryMaxTipWidth( __EXT_MFC_SAFE_LPCTSTR lpszText );
public:
	static void stat_DoPaintNc(
		bool bEnabled,
		bool bMouseOver,
		CWnd * pWnd,
		CExtPmBridge * pPmBridge,
		CDC * pDC
		);
	static void stat_DrawEditImpl(
		bool bEnabled,
		bool bMouseOver,
		CWnd * pWnd,
		CExtPmBridge * pPmBridge,
		CRect rectClient,
		CDC * pDC
		);
	static void stat_PrintClient(
		bool bEnabled,
		bool bMouseOver,
		UINT message,
		LPARAM lParam,
		CWnd * pWnd,
		CExtPmBridge * pPmBridge,
		CDC * pDC
		);

protected:
	//{{AFX_MSG(CExtEditBase)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd,CPoint pos );	
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	INT Rich_GetLine( INT nIndex, __EXT_MFC_SAFE_LPTSTR lpszBuffer ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (INT) ::SendMessage( m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer );
	}
	INT Rich_LineIndex( INT nLine = -1 ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (int)::SendMessage( m_hWnd, EM_LINEINDEX, nLine, 0 );
	}
	INT Rich_LineLength( INT nLine = -1 ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (INT)::SendMessage( m_hWnd, EM_LINELENGTH, nLine, 0 );
	}
	void Rich_LineScroll( INT nLines, INT nChars = 0 )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		::SendMessage( m_hWnd, EM_LINESCROLL, nChars, nLines );
	}
	void Rich_SetSel( LONG nStartChar, LONG nEndChar )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARRANGE cr; cr.cpMin = nStartChar; cr.cpMax = nEndChar;
		::SendMessage( m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr );
	}
	void Rich_SetSel( CHARRANGE & cr )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		::SendMessage( m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr );
	}
	BOOL Rich_CanPaste( UINT nFormat ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		COleMessageFilter * pFilter = AfxOleGetMessageFilter();
		if( pFilter != NULL )
			pFilter->BeginBusyState();
		BOOL b = (BOOL)::SendMessage( m_hWnd, EM_CANPASTE, nFormat, 0L );
		if( pFilter != NULL )
			pFilter->EndBusyState();
		return b;
	}
	void Rich_PasteSpecial( UINT nClipFormat, DWORD dvAspect, HMETAFILE hMF )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		REPASTESPECIAL reps; reps.dwAspect = dvAspect; reps.dwParam = (__EXT_MFC_DWORD_PTR)hMF;
		::SendMessage( m_hWnd, EM_PASTESPECIAL, nClipFormat, (LPARAM)&reps );
	}
	INT Rich_GetLine( INT nIndex, __EXT_MFC_SAFE_LPTSTR lpszBuffer, INT nMaxLength ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		if( nMaxLength < sizeof(INT) )
		{
			ASSERT( FALSE );
			return 0;
		}
		(*(LPINT(LPVOID(LPCTSTR(lpszBuffer))))) = nMaxLength;
		return (INT)::SendMessage( m_hWnd, EM_GETLINE, nIndex, (LPARAM)LPCTSTR(lpszBuffer) );
	}
	CPoint Rich_GetCharPos( LONG lChar ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CPoint pt; ::SendMessage( m_hWnd, EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar ); return pt;
	}
	void Rich_GetSel( LONG & nStartChar, LONG & nEndChar ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARRANGE cr; ::SendMessage( m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr ); nStartChar = cr.cpMin; nEndChar = cr.cpMax;
	}
	void Rich_GetSel( CHARRANGE & cr ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
 		::SendMessage( m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr );
	}
	void Rich_LimitText( LONG nChars )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		::SendMessage( m_hWnd, EM_EXLIMITTEXT, 0, nChars );
	}
	LONG Rich_LineFromChar( LONG nIndex ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, EM_EXLINEFROMCHAR, 0, nIndex );
	}
	LONG Rich_FindText( DWORD dwFlags, FINDTEXTEX * pFindText ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, EM_FINDTEXTEX, dwFlags, (LPARAM)pFindText );
	}
	LONG Rich_FormatRange( FORMATRANGE * pfr, BOOL bDisplay )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, EM_FORMATRANGE, (WPARAM)bDisplay, (LPARAM)pfr );
	}
	LONG Rich_GetEventMask() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, EM_GETEVENTMASK, 0, 0L );
	}
	LONG Rich_GetLimitText() const
	{
		ASSERT_VALID( this );
		ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, EM_GETLIMITTEXT, 0, 0L );
	}
	LONG Rich_GetSelText( __EXT_MFC_SAFE_LPTSTR lpBuf ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpBuf );
	}
	void Rich_HideSelection( bool bHide, bool bPerm )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		::SendMessage( m_hWnd, EM_HIDESELECTION, WPARAM( bHide ? TRUE : FALSE ), LPARAM( bPerm ? TRUE : FALSE ) );
	}
	void Rich_RequestResize()
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		::SendMessage( m_hWnd, EM_REQUESTRESIZE, 0, 0L );
	}
	WORD Rich_GetSelectionType() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (WORD)::SendMessage( m_hWnd, EM_SELECTIONTYPE, 0, 0L );
	}
	COLORREF Rich_SetBackgroundColor(BOOL bSysColor, COLORREF cr)
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (COLORREF)::SendMessage( m_hWnd, EM_SETBKGNDCOLOR, bSysColor, cr );
	}
	DWORD Rich_SetEventMask(DWORD dwEventMask)
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (DWORD)::SendMessage( m_hWnd, EM_SETEVENTMASK, 0, dwEventMask );
	}
	BOOL Rich_SetOLECallback(IRichEditOleCallback* pCallback)
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (BOOL)::SendMessage( m_hWnd, EM_SETOLECALLBACK, 0, (LPARAM)pCallback );
	}
	BOOL Rich_SetTargetDevice(HDC hDC, LONG lLineWidth)
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (BOOL)::SendMessage( m_hWnd, EM_SETTARGETDEVICE, (WPARAM)hDC, lLineWidth );
	}
	BOOL Rich_SetTargetDevice(CDC &dc, LONG lLineWidth)
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (BOOL)::SendMessage( m_hWnd, EM_SETTARGETDEVICE, (WPARAM)dc.m_hDC, lLineWidth );
	}
	LONG Rich_GetTextLength() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		return (LONG)::SendMessage( m_hWnd, WM_GETTEXTLENGTH, NULL, NULL );
	}

	CExtSafeString Rich_GetSelText() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARRANGE cr; cr.cpMin = cr.cpMax = 0;
		::SendMessage( m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr );
		LPTSTR lpsz = (LPTSTR)_alloca((cr.cpMax - cr.cpMin + 1)*2);
		lpsz[0] = NULL;
		::SendMessage( m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpsz );
		return lpsz;
	}
	IRichEditOle * Rich_GetIRichEditOle() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		IRichEditOle * pRichItem = NULL;
		::SendMessage( m_hWnd, EM_GETOLEINTERFACE, 0, (LPARAM)&pRichItem );
		return pRichItem;
	}
	PARAFORMAT Rich_GetParagraphFormat() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf; ::memset( &_pf, 0, sizeof(PARAFORMAT) ); _pf.cbSize = sizeof(PARAFORMAT);
		_pf.dwMask = PFM_ALIGNMENT | PFM_NUMBERING;
		::SendMessage( m_hWnd, EM_GETPARAFORMAT, 0, (LPARAM)&_pf );
		return _pf;
	}
	bool Rich_Paragraph_Bulleted_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf; ::memset( &_pf, 0, sizeof(PARAFORMAT) ); _pf.cbSize = sizeof(PARAFORMAT);
		bool bRetVal = ( _pf.wNumbering == PFN_BULLET ) ? true : false;
		return bRetVal;
	}
	void Rich_Paragraph_Bulleted_Set()
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf = Rich_GetParagraphFormat();
		if ( (_pf.dwMask & PFM_NUMBERING) && (_pf.wNumbering == PFN_BULLET) )
		{
			_pf.wNumbering = 0;
			_pf.dxOffset = 0;
			_pf.dxStartIndent = 0;
			_pf.dwMask = PFM_NUMBERING|PFM_STARTINDENT|PFM_OFFSET;
		}
		else
		{
			_pf.wNumbering = PFN_BULLET;
			_pf.dwMask = PFM_NUMBERING;
			if (_pf.dxOffset == 0)
			{
				_pf.dxOffset = 4;
				_pf.dwMask = PFM_NUMBERING|PFM_STARTINDENT|PFM_OFFSET;
			}
		}
		::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&_pf );
	}
	bool Rich_Paragraph_Right_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf = Rich_GetParagraphFormat();
		bool bRetVal = ( _pf.wAlignment == PFA_RIGHT ) ? true : false;
		return bRetVal;
	}
	bool Rich_Paragraph_Left_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf = Rich_GetParagraphFormat();
		bool bRetVal = ( _pf.wAlignment == PFA_LEFT ) ? true : false;
		return bRetVal;
	}
	bool Rich_Paragraph_Centered_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf = Rich_GetParagraphFormat();
		bool bRetVal = ( _pf.wAlignment == PFA_CENTER ) ? true : false;
		return bRetVal;
	}
	void Rich_SetParagraphRight()
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf; ::memset( &_pf, 0, sizeof(PARAFORMAT) ); _pf.cbSize = sizeof(PARAFORMAT);
		_pf.dwMask = PFM_ALIGNMENT; _pf.wAlignment = PFA_RIGHT;
		::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&_pf );
	}
	void Rich_SetParagraphLeft()
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf; ::memset( &_pf, 0, sizeof(PARAFORMAT) ); _pf.cbSize = sizeof(PARAFORMAT);
		_pf.dwMask = PFM_ALIGNMENT; _pf.wAlignment = PFA_LEFT;
		::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&_pf );
	}
	void Rich_SetParagraphCenter()
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		PARAFORMAT _pf; ::memset( &_pf, 0, sizeof(PARAFORMAT) ); _pf.cbSize = sizeof(PARAFORMAT);
		_pf.dwMask = PFM_ALIGNMENT; _pf.wAlignment = PFA_CENTER;
		::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&_pf );
	}
	CHARFORMAT Rich_GetCharFormat( DWORD dwMask = CFM_COLOR|CFM_FACE|CFM_SIZE|CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE ) const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARFORMAT _cf; ::memset( &_cf, 0, sizeof(CHARFORMAT) ); _cf.cbSize = sizeof(CHARFORMAT); _cf.dwMask = dwMask;
		::SendMessage( m_hWnd, EM_GETCHARFORMAT, 1, (LPARAM)&_cf );
		return _cf;
	}
	void Rich_SetCharStyle( INT nMask, INT nStyle /*, INT nStart, INT nEnd*/ )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARFORMAT _cf; ::memset( &_cf, 0, sizeof(CHARFORMAT) ); _cf.cbSize = sizeof(CHARFORMAT);
		//cf.dwMask = nMask;
		::SendMessage( m_hWnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&_cf );
		if( _cf.dwMask & nMask )
			_cf.dwEffects ^= nStyle; 
		else
			_cf.dwEffects |= nStyle;
		_cf.dwMask = nMask;
		::SendMessage( m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&_cf );
	}
	bool Rich_Selection_Bold_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARFORMAT _cf = Rich_GetCharFormat();
		bool bRetVal = ( ( _cf.dwEffects & CFM_BOLD ) != 0 ) ? true : false;
		return bRetVal;
	}
	bool Rich_Selection_Italic_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARFORMAT _cf = Rich_GetCharFormat();
		bool bRetVal = ( ( _cf.dwEffects & CFM_ITALIC ) != 0 ) ? true : false;
		return bRetVal;
	}
	bool Rich_Selection_Underlined_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARFORMAT _cf = Rich_GetCharFormat();
		bool bRetVal = ( ( _cf.dwEffects & CFM_UNDERLINE ) != 0 ) ? true : false;
		return bRetVal;
	}
	bool Rich_Selection_StrikeOut_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		CHARFORMAT _cf = Rich_GetCharFormat();
		bool bRetVal = ( ( _cf.dwEffects & CFM_STRIKEOUT ) != 0 ) ? true : false;
		return bRetVal;
	}
	void Rich_Selection_Bold_Set( bool bSet = true )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		Rich_SetCharStyle( CFM_BOLD, bSet ? CFE_BOLD : 0 );
	}
	void Rich_Selection_Italic_Set( bool bSet = true )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		Rich_SetCharStyle( CFM_ITALIC, bSet ? CFE_ITALIC : 0 );
	}
	void Rich_Selection_Underlined_Set( bool bSet = true )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		Rich_SetCharStyle( CFM_UNDERLINE, bSet ? CFE_UNDERLINE : 0 );
	}
	void Rich_Selection_StrikeOut_Set( bool bSet = true )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		Rich_SetCharStyle( CFM_STRIKEOUT, bSet ? CFE_STRIKEOUT : 0 );
	}
	void Rich_SetSelectionColor( COLORREF clr )
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		INT nStart = 0, nEnd = 0; GetSel( nStart, nEnd );
		CHARFORMAT _cf; ::memset( &_cf, 0, sizeof(CHARFORMAT) ); _cf.cbSize = sizeof(CHARFORMAT); _cf.crTextColor = clr; _cf.dwMask = CFM_COLOR;
		::SendMessage( m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&_cf );
	}
	LONG Rich_StreamIn( INT nFormat, EDITSTREAM & _es ) { ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode ); return (LONG)::SendMessage( m_hWnd, EM_STREAMIN,  nFormat, (LPARAM)&_es ); }
	LONG Rich_StreamOut( INT nFormat, EDITSTREAM & _es ) const { ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode ); return (long)::SendMessage( m_hWnd, EM_STREAMOUT, nFormat, (LPARAM)&_es ); }
	static DWORD CALLBACK stat_Rich_CallBack_StreamIn( __EXT_MFC_DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb );
	static DWORD CALLBACK stat_Rich_CallBack_StreamOut( __EXT_MFC_DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb );
	CExtSafeString Rich_PlainText_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		EDITSTREAM _es; ::memset( &_es, 0, sizeof(EDITSTREAM) );
		_es.dwError = 0; _es.pfnCallback = stat_Rich_CallBack_StreamOut;
		CExtSafeString str = _T(""); _es.dwCookie = (__EXT_MFC_DWORD_PTR)&str;
		Rich_StreamOut( SF_TEXT, _es );
		return str;
	}
	CExtSafeString Rich_RTF_Get() const
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		EDITSTREAM _es; ::memset( &_es, 0, sizeof(EDITSTREAM) );
		_es.dwError = 0; _es.pfnCallback = stat_Rich_CallBack_StreamOut;
		CExtSafeString str = _T(""); _es.dwCookie = (__EXT_MFC_DWORD_PTR)&str;
		Rich_StreamOut( SF_RTF, _es );
		return str;
	}
	void Rich_PlainText_Set( const CExtSafeString & str ) 
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		EDITSTREAM _es; ::memset( &_es, 0, sizeof(EDITSTREAM) );
		_es.dwError = 0; _es.pfnCallback = stat_Rich_CallBack_StreamIn; _es.dwCookie = (__EXT_MFC_DWORD_PTR)&str;
		Rich_StreamIn( SF_TEXT, _es );
	}
	void Rich_RTF_Set( const CExtSafeString & str ) 
	{
		ASSERT_VALID( this ); ASSERT( ::IsWindow( m_hWnd ) ); ASSERT( m_bRichMode );
		EDITSTREAM _es; ::memset( &_es, 0, sizeof(EDITSTREAM) );
		_es.dwError = 0; _es.pfnCallback = stat_Rich_CallBack_StreamIn; _es.dwCookie = (__EXT_MFC_DWORD_PTR)&str;
		Rich_StreamIn( SF_RTF, _es );
	}
}; // class CExtEditBase

/////////////////////////////////////////////////////////////////////////////
// CExtED (for CExtEdit and CExtEditHotKey classes)

#define __EXT_EDIT_UPDATE_TIMER			1
#define __EXT_EDIT_UPDATE_TIMER_PERIOD	100

template < class _BED >
class CExtED : public _BED
{
public:
	CExtED()
		: m_bMouseOver( false )
	{
	}
protected:
	virtual void _DoPaintNC( CDC * pDC )
	{
		ASSERT_VALID( this );
		ASSERT_VALID( pDC );
		ASSERT( pDC->GetSafeHdc() != NULL );
		CExtEditBase::stat_DoPaintNc( OnQueryWindowEnabledState(), m_bMouseOver, this, this, pDC );
	}
	virtual void _PostRedraw()
	{
		if( m_hWnd != NULL && ::IsWindow( m_hWnd ) )
		{
			VERIFY( RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME|RDW_ALLCHILDREN ) );
		}
	}
	virtual void _DrawEditImpl( CRect rectClient, CDC * pDC = NULL )
	{
		CExtEditBase::stat_DrawEditImpl( OnQueryWindowEnabledState(), m_bMouseOver, this, this, rectClient, pDC );
	}
	bool m_bMouseOver:1;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch( message )
		{
		case WM_PRINT:
		case WM_PRINTCLIENT:
			{
				_BED::WindowProc(message, wParam, (lParam&(~(PRF_NONCLIENT|PRF_CHECKVISIBLE))|PRF_CLIENT|PRF_ERASEBKGND) );
				CDC * pDC = CDC::FromHandle( (HDC) wParam );
				CExtEditBase::stat_PrintClient( OnQueryWindowEnabledState(), m_bMouseOver, message, lParam, this, this, pDC );
				return (!0L);
			}
			break;
		case WM_TIMER:
			{
				if( wParam == __EXT_EDIT_UPDATE_TIMER )
				{
					POINT pt;
					if(		IsWindowVisible()
						&&	::GetCursorPos( &pt ) 
						)
					{
						bool bOldMouseOver = m_bMouseOver;
						CRect rectItem;
						GetWindowRect( &rectItem );
						if( !rectItem.PtInRect(pt) )
						{
							KillTimer( __EXT_EDIT_UPDATE_TIMER );
							m_bMouseOver = false;
						}
						else 
							if(	::WindowFromPoint( pt ) == m_hWnd )
								m_bMouseOver = true;
							if( bOldMouseOver != m_bMouseOver )
								_PostRedraw();
					}
					return 0L;
				}
			}
			break;
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			{
				LRESULT lRes = _BED::WindowProc(message, wParam, lParam);
				_PostRedraw();
				return lRes;
			}
			break;
		case WM_SETCURSOR:
			{
				if( HIWORD( lParam ) == WM_MOUSEMOVE )
				{
					CRect rcWnd;
					GetWindowRect( &rcWnd );
					if( rcWnd.PtInRect( GetCurrentMessage()->pt ) )
					{
						SetTimer( __EXT_EDIT_UPDATE_TIMER, __EXT_EDIT_UPDATE_TIMER_PERIOD, NULL );
						SendMessage( WM_TIMER, __EXT_EDIT_UPDATE_TIMER, 0L );
					}
				}
			}
			break;
		case WM_NCPAINT:
			{
				CWindowDC dc( this );
				_DoPaintNC( &dc );
				return 0L;
			}
			break;
		}
		return _BED::WindowProc(message, wParam, lParam);
	}
}; // class CExtED

/////////////////////////////////////////////////////////////////////////////
// CExtEdit window

#if _MFC_VER >= 0x700
	#ifndef __EXT_PROFUIS_STATIC_LINK
		template class __PROF_UIS_API CExtED < CExtEditBase >;
	#endif  // #ifndef __EXT_PROFUIS_STATIC_LINK
#endif // _MFC_VER >= 0x700

class __PROF_UIS_API CExtEdit : public CExtED < CExtEditBase >
{
public:
	DECLARE_DYNCREATE( CExtEdit );
	CExtEdit();
	virtual ~CExtEdit();

	//{{AFX_VIRTUAL(CExtEdit)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CExtEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
}; // class CExtEdit

/////////////////////////////////////////////////////////////////////////////
// CExtIPAddressCtrl window

class __PROF_UIS_API CExtIPAddressCtrl 
	: public CIPAddressCtrl
	, public CExtPmBridge
{
public:
	DECLARE_DYNCREATE( CExtIPAddressCtrl );
	DECLARE_CExtPmBridge_MEMBERS( CExtIPAddressCtrl );
	CExtIPAddressCtrl();
	virtual ~CExtIPAddressCtrl();
	BOOL Create(
		DWORD dwStyle, 
		const RECT & rect,
		CWnd * pParentWnd, 
		UINT nID
		);

	//{{AFX_VIRTUAL(CExtIPAddressCtrl)
	public:
	virtual BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext * pContext = NULL );
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
protected:
	virtual bool _CreateHelper();
	virtual void _DoPaintNC( CDC * pDC );
	virtual void _PostRedraw();
	virtual void _DrawEditImpl( CRect rectClient, CDC * pDC = NULL );
	bool m_bDirectCreateCall:1;
	bool m_bInitialized:1;
	bool m_bMouseOver:1;
	WNDPROC m_pWNDPROC[4];
	HWND m_hWndHooked[4];
	static CTypedPtrList < CPtrList, CExtIPAddressCtrl * > g_listTargets;
	static LRESULT CALLBACK g_HookWndProc( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam );
protected:
	//{{AFX_MSG(CExtIPAddressCtrl)
 	afx_msg void OnDestroy();
	afx_msg void OnTimer(__EXT_MFC_UINT_PTR nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
}; // class CExtIPAddressCtrl

/////////////////////////////////////////////////////////////////////////////
// CExtEditHotKeyBase window

class __PROF_UIS_API CExtEditHotKeyBase : public CExtEditBase
{
// Construction
public:
	DECLARE_DYNCREATE(CExtEditHotKeyBase);
	CExtEditHotKeyBase();
	virtual ~CExtEditHotKeyBase();
protected:
	bool m_bAllowTabKeyInput:1;
	BYTE m_nSimpleKeyModifiers;
	ACCEL m_dataAccel;
	void _SyncEditText();
	void _Empty();
public:
	static CExtSafeString stat_GetSingleKeyName(
		HKL hKeyboardLayout,
		WORD nKey,
		__EXT_MFC_SAFE_LPCTSTR strAppendIfNonEmpty = NULL,
		bool bMakeOnlyFirstLettersUpper = true
		);
	static CExtSafeString stat_FormatAccelText( const ACCEL & _dataAccel );
	bool IsAccelDefined() const;
	bool IsEmpty() const;
	void Empty();
	const ACCEL & AccelGet() const;
	void AccelSet( const ACCEL & _dataAccel );
	WORD KeyGet() const;
	void KeySet( WORD nKey );
	BYTE ModifiersGet() const;
	void ModifiersSet( BYTE nModifiers );
	BYTE SimpleKeyModifiersGet() const;
	void SimpleKeyModifiersSet( BYTE nModifiers );
	bool AllowTabKeyInputGet() const;
	void AllowTabKeyInputSet( bool bAllowTabKeyInput );
	virtual CExtSafeString OnGetEmptyText();
	virtual CExtSafeString OnGetAccelText();
	virtual bool OnAccelChanging( const ACCEL & _dataAccelOld, const ACCEL & _dataAccelNew );
	virtual void OnAccelChanged();
	virtual void OnAccelDefined();
	virtual bool OnTestEraseAccel( const ACCEL & _dataAccel );
	virtual bool OnTestSimpleKey( WORD nKey );
	virtual bool OnTestDisabledKey( WORD nKey );
public:
    //{{AFX_VIRTUAL(CExtEditHotKeyBase)
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CExtEditHotKeyBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

}; /// class CExtEditHotKeyBase

/////////////////////////////////////////////////////////////////////////////
// CExtEditHotKey window

#if _MFC_VER >= 0x700
	#ifndef __EXT_PROFUIS_STATIC_LINK
		template class __PROF_UIS_API CExtED < CExtEditHotKeyBase >;
	#endif  // #ifndef __EXT_PROFUIS_STATIC_LINK
#endif // _MFC_VER >= 0x700

class __PROF_UIS_API CExtEditHotKey : public CExtED < CExtEditHotKeyBase >
{
public:
	DECLARE_DYNCREATE( CExtEditHotKey );
	CExtEditHotKey();
	virtual ~CExtEditHotKey();
	
	//{{AFX_VIRTUAL(CExtEditHotKey)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CExtEditHotKey)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
}; /// class CExtEditHotKey

__PROF_UIS_API void AFXAPI __EXT_MFC_DDX_HotKey( CDataExchange * pDX, INT nIDC, ACCEL & value );

/////////////////////////////////////////////////////////////////////////////
// CExtEditWithBehavior

class __PROF_UIS_API CExtEditWithBehavior : public CExtEdit
{
public:
	DECLARE_DYNAMIC( CExtEditWithBehavior );
	CExtEditWithBehavior();
	virtual ~CExtEditWithBehavior();
	virtual void TextSet( __EXT_MFC_SAFE_LPCTSTR strText );
	virtual CExtSafeString TextGet() const;
	virtual CExtSafeString TextGetTrimmed() const;
	COLORREF m_clrRichTextNormal, m_clrRichTextReadOnly, m_clrRichTextDisabled;
	COLORREF m_clrRichBackNormal, m_clrRichBackReadOnly, m_clrRichBackDisabled;
	virtual void _Rich_OnDraw();
	virtual BOOL _Rich_OnProtected( ENPROTECTED * pEP );
protected:
	bool m_bRichPainting:1, m_bHandlingWmCut:1;
	virtual void _UpdateContent();
public:
	virtual CExtSafeString TextGetValid() const;
	virtual bool Behavior_OnQueryCharEnabledTyping( __EXT_MFC_SAFE_TCHAR _tchr ) const;
private:
	enum e_InternalStateFlags_t
	{
		__EISF_CLEAN = 0x0000,
		__EISF_TCWAS = 0x0001, // text color was already set
	};
	UINT m_nInternalFlags;
public:
	class __PROF_UIS_API CBehaviorBase
	{
	public:
		CBehaviorBase();
		virtual ~CBehaviorBase();
		bool _FlagsModify( UINT uAdd, UINT uRemove );
		UINT _FlagsGet() const;
		virtual CExtSafeString Behavior_GetValidText( bool bSetSel = false ) const = 0;
		virtual void Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags ) = 0;
		virtual void Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags );
		virtual void Behavior_OnKillFocus( CWnd * pNewWnd );
		virtual LRESULT Behavior_OnPaste( WPARAM wParam, LPARAM lParam );
	protected:
		virtual LRESULT _Default();
		virtual void _Redraw();
	public:
		virtual bool Behavior_OnQueryCharEnabledTyping(__EXT_MFC_SAFE_TCHAR _tchr ) const;
		CExtEditWithBehavior * m_pEditWithBehaviorWnd;
	protected:
		UINT m_nBehaviorFlags;
	}; /// class CBehaviorBase
	friend class CBehaviorBase;
	class __PROF_UIS_API CBehaviorMasked : public CBehaviorBase
	{
	public:
		CBehaviorMasked(
			__EXT_MFC_SAFE_LPCTSTR strMask = _T("")
			);
		void MaskSet( __EXT_MFC_SAFE_LPCTSTR strMask );
		__EXT_MFC_SAFE_LPCTSTR MaskGet() const;
		CExtSafeString _GetNumericText() const;
		void InsertModeSet( bool bInsertMode );
		bool InsertModeGet() const;
		void NullableModeSet( bool bNullableMode );
		bool NullableModeGet() const;
		void NullCharSet( __EXT_MFC_SAFE_TCHAR _tchrNullChar );
		__EXT_MFC_SAFE_TCHAR NullCharGet() const;
		void FieldSelectionModeSet( bool bFieldSelectionMode );
		bool FieldSelectionModeGet() const;
		class __PROF_UIS_API CharRule
		{
		public:
			#ifndef _UNICODE
				typedef int   (*ValidationFunction)(UINT);	// similar to _istdigit, _istalpha
				typedef UINT  (*ConversionFunction)(UINT);	// similar to _totupper, _totlower
			#else
				typedef int   (*ValidationFunction)(__EXT_MFC_SAFE_TCHAR);
				typedef __EXT_MFC_SAFE_TCHAR (*ConversionFunction)(__EXT_MFC_SAFE_TCHAR);
			#endif
			CharRule();
			CharRule( __EXT_MFC_SAFE_TCHAR cSymbol, ValidationFunction fnValidation, ConversionFunction fnConversion = NULL );
			CharRule( COLORREF clrSymbol, __EXT_MFC_SAFE_TCHAR cSymbol, ValidationFunction fnValidation, ConversionFunction fnConversion = NULL );
			virtual ~CharRule();
			virtual bool Validate( __EXT_MFC_SAFE_TCHAR _tchr ) const;
			virtual __EXT_MFC_SAFE_TCHAR Convert( __EXT_MFC_SAFE_TCHAR _tchr ) const;
			void Set( __EXT_MFC_SAFE_TCHAR cSymbol );
			__EXT_MFC_SAFE_TCHAR Get() const;
			operator __EXT_MFC_SAFE_TCHAR() const;
			__EXT_MFC_SAFE_TCHAR m_cSymbol;
			COLORREF m_clrSymbol;
			ValidationFunction m_fnValidation;
			ConversionFunction m_fnConversion;
			virtual void Serialize( CArchive & ar );
		}; /// class CharRule
		class __PROF_UIS_API CharRuleArray : public CArray < CharRule, CharRule & >
		{
		public:
			INT FindPos( __EXT_MFC_SAFE_TCHAR tchrMask ) const
			{
				INT nIndex, nCount = INT( GetSize() );
				for( nIndex = 0; nIndex < nCount; nIndex ++ )
				{
					const CharRule & _rule = (*this)[ nIndex ];
					__EXT_MFC_SAFE_TCHAR tchrMaskRule = _rule.Get();
					if( tchrMask == tchrMaskRule )
						return nIndex;
				}
				return -1;
			}
			bool CheckRule(
				__EXT_MFC_SAFE_TCHAR tchrMask,
				__EXT_MFC_SAFE_TCHAR tchrInput,
				bool * p_bRuleWasFound = NULL,
				bool bConvert = false
				) const
			{
				INT nIndex = FindPos( tchrMask );
				if( nIndex >= 0 )
				{
					if( p_bRuleWasFound != NULL )
						(*p_bRuleWasFound) = true;
					const CharRule & _rule = (*this)[ nIndex ];
					if( bConvert )
						tchrInput = _rule.Convert( tchrInput );
					return _rule.Validate( tchrInput );
				}
				else
				{
					if( p_bRuleWasFound != NULL )
						(*p_bRuleWasFound) = false;
					if( tchrMask == tchrInput )
						return true;
					else
						return false;
				}
			}
			virtual void Serialize( CArchive & ar );
		}; /// class CharRuleArray
		CharRuleArray & GetSymbolArray();
	protected:
		virtual CExtSafeString Behavior_GetValidText( bool bSetSel = false ) const;
		virtual void Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags );
		virtual void Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags );
		void _GetMaskBasedGroupArrays(
			CExtSafeStringArray & arrMaskGroups,
			CExtSafeStringArray * p_arrEnteredValues = NULL,
			bool bJoinSimilarRules = true,
			bool bJoinDifferentRules = false,
			bool bPutNullableCharIntoValueArray = false
			);
	public:
		virtual void OnCheckText( bool bAppendEndChars );
		virtual void OnInputChar( CExtSafeString & str );
	protected:
		bool _IsNullChar( int & _pos ) const;
		void _ShowNullableMask();
		bool _GetFieldSelection( int _currentPosition );
		void _DefineNewPosition( int & _nStart, int & _nEnd );
		void _MaskSymbolDelete( CExtSafeString & _str );
		void _MaskSymbolInsert( CExtSafeString _str );
		void _SymbolInsert( CExtSafeString _str );
		bool _NullCharBefore( int _nPos );
		bool _AddSymbolMaskToEnd();
		INT _GetCountMaskSymbolsFromEnd();
	public:
		CharRuleArray m_arrRules;
	protected:
		CExtSafeString m_strMask;
		bool m_bInsertMode:1;
		bool m_bNullableMode:1;
		bool m_bFieldSelectionMode:1;
		__EXT_MFC_SAFE_TCHAR m_tchrNullChar;
		bool m_bBack:1;
	}; /// class CBehaviorMasked
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	//{{AFX_MSG(CExtEditWithBehavior)
//	afx_msg void OnChange();
//	afx_msg void OnUpdate();
	//}}AFX_MSG
	afx_msg LRESULT OnCut( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPaste( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnClear( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetText( WPARAM wParam, LPARAM lParam );
	afx_msg void OnProtected( NMHDR * pNMHDR, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()
}; /// class CExtEditWithBehavior

/////////////////////////////////////////////////////////////////////////////
// CExtEditMasked window

class __PROF_UIS_API CExtEditMasked
	: public CExtEditWithBehavior
	, public CExtEditWithBehavior::CBehaviorMasked
{
public:
	DECLARE_DYNCREATE( CExtEditMasked );
	COLORREF m_clrDefault;
	CExtEditMasked(
		__EXT_MFC_SAFE_LPCTSTR strMask = _T("")
		);
	virtual void _Rich_OnDraw();
	virtual CExtSafeString TextGetValid() const;
#ifndef _UNICODE
	static int stat_IsCharDigit( UINT c ); // similar to _istdigit()
	static int stat_IsCharAlpha( UINT c ); // similar to _istalpha()
	static int stat_IsCharAlphaOrDigit( UINT c ); // similar to _istalnum()
#else
	static int stat_IsCharDigit( __EXT_MFC_SAFE_TCHAR c ); // similar to _istdigit()
	static int stat_IsCharAlpha( __EXT_MFC_SAFE_TCHAR c ); // similar to _istalpha()
	static int stat_IsCharAlphaOrDigit( __EXT_MFC_SAFE_TCHAR c ); // similar to _istalnum()
#endif
public:
	//{{AFX_VIRTUAL(CExtEditMasked)
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CExtEditMasked)
	afx_msg void OnChar( UINT uChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus( CWnd * pNewWnd );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; /// CExtEditMasked

/////////////////////////////////////////////////////////////////////////////
// CExtEditSystemNumberCurrencyBase window

#define __EXT_MFC_SYSTEM_NUMBER_MIN							-1.7976931348623158e+308
#define __EXT_MFC_SYSTEM_NUMBER_MAX							 1.7976931348623158e+308

#define __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS					9
#define __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS				2

#define __ESE_KF_PAD_WITH_ZEROS_BEFORE_DECIMAL				0x00000010
#define __ESE_KF_PAD_WITH_ZEROS_AFTER_DECIMAL				0x00000020
#define __ESE_KF_DONT_PAD_WITH_ZEROS_IF_EMPTY				0x00000040
#define __ESE_KF_REMOVE_EXTRA_LEADING_ZEROS					0x00000080
#define __ESE_KF_MASK										0x000000FF
#define __ESE_DISABLE_NEGATIVE								0x00001000
#define __ESE_ADD_DECIMAL_AFTER_MAX_WHOLE_DIGITS			0x00002000

class __PROF_UIS_API CExtEditSystemNumberCurrencyBase
	: public CExtEditWithBehavior
	, public CExtEditWithBehavior::CBehaviorBase
{
public:
	DECLARE_DYNAMIC( CExtEditSystemNumberCurrencyBase );
	bool m_bEmptyGroupSeparatorMode:1, m_bUseScientificNumberMode:1;

	CExtEditSystemNumberCurrencyBase(
		INT nMaxWholeDigits = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS,
		INT nMaxFractionDigits = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS
		);
	virtual ~CExtEditSystemNumberCurrencyBase();

	virtual void CurrencySet( const CURRENCY & _cy );
	virtual COleCurrency CurrencyGet() const;

	virtual void LongDoubleSet( long double lfValue, bool bTrimTrailingZeros = true );
	virtual long double LongDoubleGet() const = 0;

	virtual void IntSet( INT nVal ) = 0;
	virtual INT IntGet() const = 0;
	
	virtual void MaxWholeDigitsSet( INT nMaxWholeDigits );
	virtual INT MaxWholeDigitsGet() const;
	
	virtual void MaxFractionDigitsSet( INT nMaxFractionDigits );
	virtual INT MaxFractionDigitsGet() const;
	
	virtual void AllowNegativeSet( bool bAllowNegative = true );
	virtual bool AllowNegativeGet() const;
	
	virtual void RangeSet( long double lfMin, long double lfMax );
	virtual void RangeGet( long double * p_lfMin, long double * p_lfMax ) const;

	virtual void NumberColorSet( COLORREF clrNumber = COLORREF(-1L) );
	virtual COLORREF NumberColorGet() const;

	virtual void FractionColorSet( COLORREF clrFraction = COLORREF(-1L) );
	virtual COLORREF FractionColorGet() const;

	virtual void GroupSeparatorColorSet( COLORREF clrGroupSeparator = COLORREF(-1L) );
	virtual COLORREF GroupSeparatorColorGet() const;

	virtual void DecimalSeparatorColorSet( COLORREF clrDecimalSeparator = COLORREF(-1L) );
	virtual COLORREF DecimalSeparatorColorGet() const;

	virtual void SignColorSet( COLORREF clrSign = COLORREF(-1L) );
	virtual COLORREF SignColorGet() const;

	virtual void BracketsColorSet( COLORREF clrBrackets = COLORREF(-1L) );
	virtual COLORREF BracketsColorGet() const;

protected:
	virtual bool _IsValid() const;
	virtual bool _IsNegative( __EXT_MFC_SAFE_LPCTSTR strText ) const = 0;
	virtual CExtSafeString TextGetValid() const;
	virtual bool _DeleteSymbols( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bBackspace = false, LPARAM lParam = 0L ) = 0;
	virtual INT _GetGroupSeparatorCount( const CExtSafeString & strText ) const;
	virtual CExtSafeString _GetNumericText( const CExtSafeString & strText, bool bUseMathSymbols = false ) const = 0;
	virtual CExtSafeString _GetDoubleText( long double lfValue, bool bTrimTrailingZeros = true ) const = 0;
	virtual void _GetTextWithoutMarks( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bCutDecimalPoint = false ) const = 0;
	virtual void _GetTextWithMarks( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bInsertNegSign = false ) const = 0;
	virtual void _AdjustSeparators( INT nCurrentSeparatorCount, bool bDecimalPoint = false );
	static void stat_InsertZeros( CExtSafeString * pStrText, INT nPos, INT nCount );
	virtual void _AdjustWithinRange() = 0;
	virtual void _CheckFlags();
	virtual void Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags );
	virtual void Behavior_OnKillFocus( CWnd * pNewWnd );
	virtual LRESULT Behavior_OnPaste( WPARAM wParam, LPARAM lParam );
	INT m_nMaxWholeDigits, m_nMaxFractionDigits,
		m_nDisplayLeadingZeros, m_nNegativeFormatIndex, m_nPositiveFormatIndex,
		m_nNegSignMatch, m_nStartSel, m_nEndSel;
	CExtSafeString m_strNegativeSign, m_strCurrencySymbol, m_strDecimalPoint, m_strGroupSeparator, m_strScientificSymbol;
	long double m_lfMin, m_lfMax;
	bool m_bBackspace:1, m_bAdjustingSeparators:1;
	CList < INT, INT > m_listDigitGrouping;
	COLORREF m_clrNumber, m_clrFraction, m_clrGroupSeparator, m_clrDecimalSeparator, m_clrSign, m_clrBrackets;
public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CExtEditWithBehavior)
	afx_msg void OnChar( UINT uChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus( CWnd * pNewWnd );
	afx_msg LRESULT OnPaste( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; /// class CExtEditSystemNumberCurrencyBase

/////////////////////////////////////////////////////////////////////////////
// CExtEditSystemNumber window

class __PROF_UIS_API CExtEditSystemNumber : public CExtEditSystemNumberCurrencyBase
{
public:
	DECLARE_DYNCREATE( CExtEditSystemNumber );
	CExtEditSystemNumber(
		INT nMaxWholeDigits = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS,
		INT nMaxFractionDigits = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS,
		bool bUseScientificNumberMode = false
		);
	virtual ~CExtEditSystemNumber();
	virtual long double LongDoubleGet() const;
	virtual long double LongDoubleGetBase( LPCTSTR strNumeric ) const;
	virtual long double LongDoubleGetMantissa() const;
	virtual long double LongDoubleGetPower() const;
	virtual void LongDoubleSetMantissa( long double lfValue, bool bTrimTrailingZeros = true );
	virtual void LongDoubleSetPower( long double lfValue, bool bTrimTrailingZeros = true );
	virtual void IntSet( INT nVal );
	virtual INT IntGet() const;
protected:
	void _Init();
	virtual bool _IsNegative( __EXT_MFC_SAFE_LPCTSTR strText ) const;
	virtual bool _DeleteSymbols( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bBackspace = false, LPARAM lParam = 0L );
	virtual void _GetTextWithoutMarks( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bCutDecimalPoint = false ) const;
	virtual void _GetTextWithoutMarksBase( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bCutDecimalPoint = false ) const;
	virtual void _GetTextWithMarks( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bInsertNegSign = false ) const;
	virtual void _GetTextWithMarksBase( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bInsertNegSign = false, bool bPower = false ) const;
	virtual CExtSafeString Behavior_GetValidText( bool bSetSel = false ) const;
	virtual CExtSafeString Behavior_GetValidText_Base( LPCTSTR strText, INT & nStartSel, INT & nEndSel, bool bPower = false ) const;
	virtual void Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags );
	virtual void Behavior_Base_Mode_OnChar( CExtSafeString & strTextBase, INT & nStartSel, INT & nEndSel, UINT uChar, bool bPower = false );
	virtual void Behavior_Scientific_Mode_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags );
	virtual void Behavior_OnKeyDown( UINT uChar, UINT nRepCnt, UINT nFlags );
	virtual LRESULT Behavior_OnPaste( WPARAM wParam, LPARAM lParam );
	virtual void _OnPaste( CExtSafeString & strText, CExtSafeString strClipboardText, INT & nStart, INT & nEnd, bool bPower = false, bool bDelScientificSignFromClipboard = true ) const;
	virtual CExtSafeString _GetNumericText( const CExtSafeString & strText, bool bUseMathSymbols = false ) const;
	virtual CExtSafeString _GetNumericTextBase( const CExtSafeString & strText, bool bUseMathSymbols = false ) const;
	virtual CExtSafeString _GetDoubleText( long double lfValue, bool bTrimTrailingZeros = true ) const;
	virtual void _AdjustWithinRange();
	virtual bool _IsValid() const;
	COLORREF m_clrPower, m_clrScientificSign;
public:
	virtual void _Rich_OnDraw();
protected:
	//{{AFX_MSG(CExtEditSystemNumber)
	//}}AFX_MSG
	afx_msg void OnSettingChange( UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection );
	DECLARE_MESSAGE_MAP()
}; /// class CExtEditSystemNumber

/////////////////////////////////////////////////////////////////////////////
// CExtEditSystemCurrency window

class __PROF_UIS_API CExtEditSystemCurrency : public CExtEditSystemNumberCurrencyBase
{
public:
	DECLARE_DYNCREATE( CExtEditSystemCurrency );
	CExtEditSystemCurrency(
		INT nMaxWholeDigits = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS,
		INT nMaxFractionDigits = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS
		);
	virtual ~CExtEditSystemCurrency();
	virtual long double LongDoubleGet() const;
	virtual void IntSet( INT nVal );
	virtual INT IntGet() const;
protected:
	virtual INT _GetSignLengthBeforeCurrency( bool bNegative = false ) const;
	virtual void _GetSignStrings( CExtSafeString & strBeforeCurrency, CExtSafeString & strAfterCurrency, bool bNegative = true ) const;
	void _Init();
	virtual bool _IsNegative( __EXT_MFC_SAFE_LPCTSTR strText ) const;
	virtual bool _DeleteSymbols( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bBackspace = false, LPARAM lParam = 0L );
	virtual void _GetTextWithoutMarks( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bCutDecimalPoint = false ) const;
	virtual void _GetTextWithMarks( CExtSafeString & strText, INT & nStart, INT & nEnd, bool bInsertNegSign = false ) const;
	virtual CExtSafeString Behavior_GetValidText( bool bSetSel = false ) const;
	virtual void Behavior_OnChar( UINT uChar, UINT nRepCnt, UINT nFlags );
	virtual CExtSafeString _GetNumericText( const CExtSafeString & strText, bool bUseMathSymbols = false ) const;
	virtual CExtSafeString _GetDoubleText( long double lfValue, bool bTrimTrailingZeros = true ) const;
	virtual void _AdjustWithinRange();
public:
	virtual void _Rich_OnDraw();
protected:
	//{{AFX_MSG(CExtEditSystemCurrency)
	//}}AFX_MSG
	afx_msg void OnSettingChange( UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection );
	DECLARE_MESSAGE_MAP()
}; /// class CExtEditSystemCurrency

/////////////////////////////////////////////////////////////////////////////
// CExtEditScientificNumber window

class __PROF_UIS_API CExtEditScientificNumber : public CExtEditSystemNumber
{
public:
	DECLARE_DYNCREATE( CExtEditScientificNumber );
	CExtEditScientificNumber(
		INT nMaxWholeDigits = __EXT_MFC_DEFAULT_MAX_WHOLE_DIGITS,
		INT nMaxFractionDigits = __EXT_MFC_DEFAULT_FRACTION_WHOLE_DIGITS
		);
	virtual void PowerColorSet( COLORREF clrPower = COLORREF(-1L) );
	virtual COLORREF PowerColorGet() const;
	virtual void ScientificSignColorSet( COLORREF clrScientifcSign = COLORREF(-1L) );
	virtual COLORREF ScientificSignColorGet() const;
	//{{AFX_MSG(CExtEditScientificNumber)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; /// class CExtEditScientificNumber

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __EXT_EDIT_H
