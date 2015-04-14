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

#if (!defined __EXT_MEMORY_DC_H)
#define __EXT_MEMORY_DC_H

//
// The enhanced version of Keith Rule's memory DC class
//

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

class __PROF_UIS_API CExtMemoryDC : public CDC 
{
protected:	
	CBitmap m_bitmapSuface, * m_pBitmapPrevSurface;
	CDC * m_pExternalDC;
	CRect m_rc;
	bool m_bMemoryMode:1;
	COLORREF * m_pHelperDibSurface;
public:
	enum eOptFlags
	{
		MDCOPT_TO_MEMORY				= 0x0001L,
		MDCOPT_FILL_SURFACE				= 0x0002L,
		MDCOPT_FORCE_BLACK				= 0x0004L,
		MDCOPT_FORCE_DIB				= 0x0008L,
		MDCOPT_NO_COPY_OPT				= 0x0010L,
		MDCOPT_FILL_BITS				= 0x0020L,
		MDCOPT_RTL_COMPATIBILITY		= 0, // MDCOPT_NO_COPY_OPT
		MDCOPT_DIB_NEGATIVE_WIDTH		= 0x0040L,
		MDCOPT_DIB_NEGATIVE_HEIGHT		= 0x0080L,
		MDCOPT_DESKTOP_COMPATIBILITY	= 0x0100L,
		MDCOPT_DEFAULT					= MDCOPT_TO_MEMORY|MDCOPT_FILL_SURFACE|MDCOPT_RTL_COMPATIBILITY
	}; // enum eOptFlags
	void __InitMemoryDC( CDC * pDC, const RECT * pRect = NULL, DWORD dwOptFlags = MDCOPT_DEFAULT )
	{
		ASSERT( m_pExternalDC == NULL ); 
		ASSERT( pDC != NULL ); 
		ASSERT( pDC->GetSafeHdc() != NULL ); 
		m_pExternalDC = pDC;
		m_pBitmapPrevSurface = NULL;
		if( ( dwOptFlags & MDCOPT_TO_MEMORY ) != 0 )
		{
			if( ( dwOptFlags & MDCOPT_DESKTOP_COMPATIBILITY ) != 0 )
				m_bMemoryMode = true;
			else
				m_bMemoryMode = pDC->IsPrinting() ? false : true;
		}
		else
			m_bMemoryMode = false;
		if( pRect == NULL )
			pDC->GetClipBox( &m_rc );
		else 
			m_rc = *pRect;
		if( m_bMemoryMode ) 
		{
			if( ( dwOptFlags & MDCOPT_DESKTOP_COMPATIBILITY ) != 0 )
				CreateCompatibleDC( NULL );
			else
				CreateCompatibleDC( pDC );
			if( GetSafeHdc() == NULL )
				return;
			DWORD dwLayout = pDC->GetLayout();
			if( dwLayout == GDI_ERROR )
				dwLayout = LAYOUT_LTR;
			if( (dwOptFlags & MDCOPT_NO_COPY_OPT) == 0 )
			{
				SetLayout( dwLayout );
				if( (dwLayout&LAYOUT_RTL) != 0 )
					__EXT_MFC_SwapLeftRight( m_rc );
				pDC->LPtoDP( &m_rc );
			}
			if( dwOptFlags & MDCOPT_FORCE_BLACK )
				m_bitmapSuface.CreateBitmap( m_rc.Width(), m_rc.Height(), 1, 1, NULL );
			else if( dwOptFlags & MDCOPT_FORCE_DIB )
			{
				BITMAPINFOHEADER bih;
				::memset( LPVOID(&bih), 0, sizeof(BITMAPINFOHEADER) );
				bih.biSize = sizeof(BITMAPINFOHEADER);
				bih.biWidth = ( ( dwOptFlags & MDCOPT_DIB_NEGATIVE_WIDTH ) != 0 ) ? (-m_rc.Width()) : m_rc.Width();
				bih.biHeight = ( ( dwOptFlags & MDCOPT_DIB_NEGATIVE_HEIGHT ) != 0 ) ? (-m_rc.Height()) : m_rc.Height();
				bih.biPlanes = 1;
				bih.biBitCount = 32;
				bih.biCompression = BI_RGB;
				bih.biSizeImage = m_rc.Width() * m_rc.Height();
				ASSERT( m_pHelperDibSurface == NULL );
				HBITMAP hDIB = ::CreateDIBSection( pDC->GetSafeHdc(), (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_pHelperDibSurface, NULL, NULL );
				if( hDIB == NULL || m_pHelperDibSurface == NULL )
				{
					ASSERT( FALSE );
					return;
				}
				m_bitmapSuface.Attach( hDIB );
			}
			else
				m_bitmapSuface.CreateCompatibleBitmap( pDC, m_rc.Width(), m_rc.Height() );
			m_pBitmapPrevSurface = SelectObject( &m_bitmapSuface );
			if( (dwOptFlags & MDCOPT_NO_COPY_OPT) == 0 )
			{
				SetMapMode( pDC->GetMapMode() );
				if( ( dwLayout & LAYOUT_RTL ) != 0 )
					__EXT_MFC_SwapLeftRight( m_rc );
				pDC->DPtoLP( &m_rc );
				SetWindowOrg( m_rc.left, m_rc.top );
			}
		} 
		else 
		{
			if( pDC->IsPrinting() )
				m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
		if( ( dwOptFlags & MDCOPT_FILL_BITS ) != 0 )
			BitBlt( m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), pDC, m_rc.left, m_rc.top, SRCCOPY );
		else if( ( dwOptFlags & MDCOPT_FILL_SURFACE ) != 0 )
			FillSolidRect( m_rc, pDC->GetBkColor() );
	}
	void __Flush( BOOL bMoveImage = TRUE )
	{
		if( m_pExternalDC == NULL )
			return;
		if( m_bMemoryMode ) 
		{
			if( bMoveImage )
				m_pExternalDC->BitBlt( m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), this, m_rc.left, m_rc.top, SRCCOPY );			
			SelectObject( m_pBitmapPrevSurface );
		} 
		else 
			m_hDC = m_hAttribDC = NULL;
		m_pExternalDC = NULL;
		m_pBitmapPrevSurface = NULL;
		m_rc.SetRectEmpty();
		m_bMemoryMode = false;
		m_pHelperDibSurface = NULL;
	}
	CExtMemoryDC( CDC * pDC = NULL, const RECT * pRect = NULL, DWORD dwOptFlags = MDCOPT_DEFAULT )
		: m_bMemoryMode( false )
		, m_pBitmapPrevSurface( NULL )
		, m_pExternalDC( NULL )
		, m_rc( 0, 0, 0, 0 )
		, m_pHelperDibSurface( NULL )
	{
		if( pDC != NULL && pDC->GetSafeHdc() != NULL )
			__InitMemoryDC( pDC, pRect, dwOptFlags );
	}
	CExtMemoryDC( CDC & dc, const RECT * pRect = NULL, DWORD dwOptFlags = MDCOPT_DEFAULT )
		: m_bMemoryMode( false )
		, m_pBitmapPrevSurface( NULL )
		, m_pExternalDC( NULL )
		, m_rc( 0, 0, 0, 0 )
		, m_pHelperDibSurface( NULL )
	{
		if( dc.GetSafeHdc() != NULL )
			__InitMemoryDC( &dc, pRect, dwOptFlags );
	}
	CExtMemoryDC( CDC & dc, const RECT & aRect, DWORD dwOptFlags = MDCOPT_DEFAULT )
		: m_bMemoryMode( false )
		, m_pBitmapPrevSurface( NULL )
		, m_pExternalDC( NULL )
		, m_rc( 0, 0, 0, 0 )
		, m_pHelperDibSurface( NULL )
	{
		if( dc.GetSafeHdc() != NULL )
			__InitMemoryDC( &dc, &aRect, dwOptFlags );
	}
	virtual ~CExtMemoryDC() { __Flush(); }
	CRect & __GetDrawRect() { return m_rc; }
	const CRect & __GetDrawRect() const { return m_rc; }
	BOOL __IsMemDC() const { return m_bMemoryMode ? TRUE : FALSE; }
	CExtMemoryDC * operator -> ()  { return this; }
	operator CExtMemoryDC * () { return this; }
	COLORREF * GetDibSurface() { ASSERT( m_pHelperDibSurface != NULL ); return m_pHelperDibSurface; }
	const COLORREF * GetDibSurface() const { ASSERT( m_pHelperDibSurface != NULL ); return m_pHelperDibSurface; }
}; /// class CExtMemoryDC

#endif // !defined(__EXT_MEMORY_DC_H)

