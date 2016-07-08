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

#include "StdAfx.h"

#if (!defined __EXT_CMD_ICON_H)
	#include <ExtCmdIcon.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtBitmap

CExtBitmap::CExtBitmap()
	: m_pImageBuffer( NULL )
{
}

CExtBitmap::CExtBitmap(
	const CExtBitmap & other
	)
	: m_pImageBuffer( NULL )
{
	VERIFY( AssignFromOther( other ) );
}

CExtBitmap::~CExtBitmap()
{
	Empty();
}

CExtBitmap & CExtBitmap::operator = (
	const CExtBitmap & other
	)
{
	VERIFY( AssignFromOther( other ) );
	return (*this);
}

void CExtBitmap::Empty()
{
	if( m_pImageBuffer != NULL )
	{
		stat_FreeHL( m_pImageBuffer );
		m_pImageBuffer = NULL;
	}
	OnContentChanged();
}

bool CExtBitmap::IsEmpty() const
{
	return ( ( m_pImageBuffer != NULL ) ? false : true );
}

void CExtBitmap::SerializeUnpacked( CArchive & ar )
{
DWORD dwFlags = 0;
	if( ar.IsStoring() )
	{
		bool bEmpty = IsEmpty();
		if( bEmpty )
			dwFlags |= 0x00000001;
		else
		{
			if( PreMultipliedRGBChannelsGet() )
			{
				PreMultiplyRGBChannels( false );
				PreMultipliedRGBChannelsSet( false );
			}
		}
		ar << dwFlags;
		if( bEmpty )
			return;
		ASSERT( m_pImageBuffer != NULL );
		VERIFY( stat_BmpSave( m_pImageBuffer, ar, true, true ) );
	} // if( ar.IsStoring() )
	else
	{
		Empty();
		ar >> dwFlags;
		if( ( dwFlags & 0x00000001 ) != 0 )
			return;
		m_pImageBuffer = stat_BmpLoad( ar, true, false );
		ASSERT( m_pImageBuffer != NULL );
		OnContentChanged();
	} // else from if( ar.IsStoring() )
}

bool CExtBitmap::Clipboard_Copy(
	HWND hWnd, // = NULL
	bool bProvideWinFormat, // = true
	bool bProvideExtFormat  // = true
	) const
{
	if( ! ( bProvideWinFormat || bProvideExtFormat ) )
		return false;
	if( PreMultipliedRGBChannelsGet() )
	{
		CExtBitmap _bmp2;
		_bmp2 = *this;
		if( _bmp2.IsEmpty() )
			return false;
		_bmp2.PreMultiplyRGBChannels( false );
		_bmp2.PreMultipliedRGBChannelsSet( false );
		return _bmp2.Clipboard_Copy( hWnd, bProvideWinFormat, bProvideExtFormat );
	}
	if( hWnd == NULL )
		hWnd = ::GetDesktopWindow();
	if( ! ::OpenClipboard( hWnd ) )
		return false;
	if( ! ::EmptyClipboard() )
	{
		::CloseClipboard();
		return false;
	}
bool bSuccess = false;
	if( bProvideExtFormat )
	{
		CLIPFORMAT cf = Clipboard_GetExtFormat();
		if( cf != 0 )
		{
			try
			{
				CSharedFile _file;
				CArchive ar( &_file, CArchive::store );
				( const_cast < CExtBitmap * > ( this ) ) -> SerializeUnpacked( ar );
				ar.Flush();
				ar.Close();
				HGLOBAL hGlobal = (HGLOBAL) _file.Detach();
				ASSERT( hGlobal != NULL );
				::SetClipboardData( cf, hGlobal );
				bSuccess = true;
			}
			catch( CException * pException )
			{
				pException->Delete();
			}
		}
	}
	if( bProvideWinFormat )
	{
		HBITMAP hBmpCopy = NULL;
		COLORREF clrTransparent = RGB( 0, 0, 0 ), * pClrSurface = NULL;
		if( ExtractEditableBitmap(
				hBmpCopy,
				clrTransparent,
				NULL,
				&pClrSurface
				)
			)
		{
			HGLOBAL	hGlobal = CExtBitmap::stat_HBITMAPtoHGLOBAL( hBmpCopy );
			if( hGlobal != NULL )
			{
				if( ::SetClipboardData( CF_DIB, hGlobal ) == NULL )
				{
					ASSERT( FALSE );
				}
				else
					bSuccess = true;
			} // if( hGlobal != NULL )
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			} // else from if( hGlobal != NULL )
#endif // _DEBUG
			::DeleteObject( hBmpCopy );
		}
	}
	::CloseClipboard();
	return bSuccess;
}

bool CExtBitmap::Clipboard_Paste(
	HWND hWnd, // = NULL
	bool bProvideWinFormat, // = true
	bool bProvideExtFormat  // = true
	)
{
	Empty();
	if( ! ( bProvideWinFormat || bProvideExtFormat ) )
		return false;
	if( hWnd == NULL )
		hWnd = ::GetDesktopWindow();
	if( ! ::OpenClipboard( hWnd ) )
		return false;
bool bSuccess = false;
	if( bProvideExtFormat )
	{
		CLIPFORMAT cf = Clipboard_GetExtFormat();
		if(		cf != 0
			&&	::IsClipboardFormatAvailable( cf )
			)
		{
			HGLOBAL hGlobal = (HGLOBAL) ::GetClipboardData( cf );
			if( hGlobal != NULL )
			{
				CSharedFile _file;
				try
				{
					_file.SetHandle( hGlobal, FALSE );
					_file.Seek( 0, CFile::begin );
					CArchive ar( &_file, CArchive::load );
					SerializeUnpacked( ar );
					if( ! IsEmpty() )
						bSuccess = true;
				}
				catch( CException * pException )
				{
					pException->Delete();
					Empty();
				}
				_file.Detach();
			}
		}
	}
	if(		( ! bSuccess )
		&&	bProvideWinFormat
		&&	(	::IsClipboardFormatAvailable( CF_BITMAP )
			||	::IsClipboardFormatAvailable( CF_DIB )
			)
		)
	{
		HBITMAP hBmpClipboard = (HBITMAP) ::GetClipboardData( CF_BITMAP );
		if( hBmpClipboard != NULL )
		{
			FromBitmap( hBmpClipboard );
			if( ! IsEmpty() )
				bSuccess = true;
		}
	}
	::CloseClipboard();
	return bSuccess;
}

bool CExtBitmap::Clipboard_CanCopy(
	bool bProvideWinFormat, // = true
	bool bProvideExtFormat  // = true
	) const
{
	if( ! ( bProvideWinFormat || bProvideExtFormat ) )
		return false;
	if( IsEmpty() )
		return false;
bool bCanCopy = false;
	if( bProvideExtFormat )
	{
		CLIPFORMAT cf = Clipboard_GetExtFormat();
		if( cf != 0 )
			bCanCopy = true;
	}
	if(		( ! bCanCopy )
		&&	bProvideWinFormat
		)
		bCanCopy = true;
	return bCanCopy;
}

bool CExtBitmap::Clipboard_CanPaste(
	HWND hWnd, // = NULL
	bool bProvideWinFormat, // = true
	bool bProvideExtFormat  // = true
	) const
{
	if( ! ( bProvideWinFormat || bProvideExtFormat ) )
		return false;
	if( hWnd == NULL )
		hWnd = ::GetDesktopWindow();
	if( ! ::OpenClipboard( hWnd ) )
		return false;
bool bCanPaste = false;
	if( bProvideExtFormat )
	{
		CLIPFORMAT cf = Clipboard_GetExtFormat();
		if(		cf != 0
			&&	::IsClipboardFormatAvailable( cf )
			)
			bCanPaste = true;
	}
	if(		( ! bCanPaste )
		&&	bProvideWinFormat
		&&	(	::IsClipboardFormatAvailable( CF_BITMAP )
			||	::IsClipboardFormatAvailable( CF_DIB )
			)
		)
		bCanPaste = true;
	::CloseClipboard();
	return bCanPaste;
}

__EXT_MFC_SAFE_LPCTSTR CExtBitmap::g_strExtBitmapClipboardFormatName = _T("ProfUIS-ExtBitmap");

CLIPFORMAT CExtBitmap::Clipboard_GetExtFormat() const
{
static CLIPFORMAT g_cf;
	if( g_cf != 0 )
		return g_cf;
CExtSafeString str = Clipboard_GetExtFormatName();
	if( str.IsEmpty() )
		return 0;
	g_cf = (CLIPFORMAT) ::RegisterClipboardFormat( LPCTSTR(str) );
	return g_cf;
}

CExtSafeString CExtBitmap::Clipboard_GetExtFormatName() const
{
CExtSafeString str;
	str = g_strExtBitmapClipboardFormatName;
	return str;
}

bool CExtBitmap::Process(
	LONG * pMatrix,
	LONG nMatrixSize,
	LONG nFactor,
	LONG nOffset,
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return false;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CExtBitmap _bmpBuffer = (*this);
	if( _bmpBuffer.IsEmpty() )
		return false;
RGBQUAD _pixel;
LONG nX, nY, nRed, nGreen, nBlue,
	nMatrixValue, nMatrixX, nMatrixY,
	nHalfMatrixSize = nMatrixSize/2;
LONG nMaxMatrixReview = nMatrixSize - nHalfMatrixSize;
CRect rcReview( 0, 0, 0, 0 );
	if( pRect != NULL )
		rcReview = (*pRect);
	else
	{
		CSize _size = GetSize();
		rcReview.right = _size.cx;
		rcReview.bottom = _size.cy;
	}
	for( nY = rcReview.top; nY < rcReview.bottom; nY++ )
	{
		for( nX = rcReview.left; nX < rcReview.right; nX++ )
		{
			if( ! rcReview.PtInRect( CPoint( nX, nY ) ) )
				continue;
			nRed = nBlue = nGreen = 0L;
			for( nMatrixX = -nHalfMatrixSize; nMatrixX < nMaxMatrixReview; nMatrixX++ )
			{
				for( nMatrixY = -nHalfMatrixSize; nMatrixY < nMaxMatrixReview; nMatrixY++ )
				{
					if( ! GetPixel(
							nX + nMatrixX,
							nY + nMatrixY,
							_pixel
							)
						)
						continue;
					nMatrixValue =
						pMatrix[
							( nMatrixX + nHalfMatrixSize )
							+ nMatrixSize * ( nMatrixY + nHalfMatrixSize )
							];
					nRed   += _pixel.rgbRed   * nMatrixValue;
					nGreen += _pixel.rgbGreen * nMatrixValue;
					nBlue  += _pixel.rgbBlue  * nMatrixValue;
				} // for( nMatrixY = -nHalfMatrixSize; nMatrixY < nMaxMatrixReview; nMatrixY++ )
			} // for( nMatrixX = -nHalfMatrixSize; nMatrixX < nMaxMatrixReview; nMatrixX++ )
			if( nFactor == 0 )
			{
				_pixel.rgbRed   = (BYTE) min( 255, max( 0, (LONG)( nRed   + nOffset ) ) );
				_pixel.rgbGreen = (BYTE) min( 255, max( 0, (LONG)( nGreen + nOffset ) ) );
				_pixel.rgbBlue  = (BYTE) min( 255, max( 0, (LONG)( nBlue  + nOffset ) ) );
			}
			else
			{
				_pixel.rgbRed   = (BYTE) min( 255, max( 0, (LONG)( nRed   / nFactor + nOffset ) ) );
				_pixel.rgbGreen = (BYTE) min( 255, max( 0, (LONG)( nGreen / nFactor + nOffset ) ) );
				_pixel.rgbBlue  = (BYTE) min( 255, max( 0, (LONG)( nBlue  / nFactor + nOffset ) ) );
			}
			_bmpBuffer.SetPixel( nX, nY, _pixel );
		} // for( nX = rcReview.left; nX < rcReview.right; nX++ )
	} // for( nY = rcReview.top; nY < rcReview.bottom; nY++ )
	(*this) = _bmpBuffer;
	OnContentChanged();
	if( IsEmpty() )
		return false;
	return true;
}

CExtBitmap::Filter::Filter(
	CExtBitmap::Filter::e_filter_type eFT, // = ft_default
	double * pArrParms, // = NULL
	INT nParmCount // = 0
	)
	: m_eFT( eFT )
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	FilterWidthSetDefault();
INT nParmIndex;
	for(	nParmIndex = 0;
			nParmIndex < (sizeof(m_arrParmBuffer)/(sizeof(m_arrParmBuffer[0])));
			nParmIndex ++
		)
		m_arrParmBuffer[ nParmIndex ] = 0.0;
	if( pArrParms != NULL && nParmCount > 0 )
	{
		nParmCount = min( nParmCount, (sizeof(m_arrParmBuffer)/(sizeof(m_arrParmBuffer[0]))) );
		for(	nParmIndex = 0;
				nParmIndex < nParmCount;
				nParmIndex ++
			)
			m_arrParmBuffer[ nParmIndex ] = pArrParms[ nParmIndex ];
	}
	if( m_eFT == bicubic )
	{
		if( pArrParms == NULL || nParmCount < 2 )
			m_arrParmBuffer[0] = m_arrParmBuffer[1] = 1.0 / 3.0;
		m_arrParmBuffer[2] = ( 6.0 - 2.0 * m_arrParmBuffer[0] ) / 6.0;
		m_arrParmBuffer[3] = ( -18.0 + 12.0 * m_arrParmBuffer[0] + 6.0 * m_arrParmBuffer[1] ) / 6.0;
		m_arrParmBuffer[4] = ( 12.0 - 9.0 * m_arrParmBuffer[0] - 6.0 * m_arrParmBuffer[1] ) / 6.0;
		m_arrParmBuffer[5] = ( 8.0 * m_arrParmBuffer[0] + 24.0 * m_arrParmBuffer[1] ) / 6.0;
		m_arrParmBuffer[6] = ( -12.0 * m_arrParmBuffer[0] - 48.0 * m_arrParmBuffer[1] ) / 6.0;
		m_arrParmBuffer[7] = ( 6.0 * m_arrParmBuffer[0] + 30.0 * m_arrParmBuffer[1] ) / 6.0;
		m_arrParmBuffer[8] = ( -m_arrParmBuffer[0] - 6.0 * m_arrParmBuffer[1] ) / 6.0;
	}
}

CExtBitmap::Filter::~Filter()
{
}

double CExtBitmap::Filter::stat_rCalc( double lfV )
{
	if( lfV == 0.0 )
		return 1.0;
	lfV *= double(3.1415926535897932384626433832795);
	lfV = ::sin( lfV ) / lfV;
	return lfV;
}

double CExtBitmap::Filter::Process(
	double lfV
	)
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	switch( m_eFT )
	{
	case box:
		return ( ( ::fabs( lfV ) <= m_lfWidth ) ? 1.0 : 0.0 );
	case linear:
		if( lfV < -1.0 )
			return 0.0;
		else if( lfV < 0.0 )
			return 1.0 + lfV;
		else if( lfV < 1.0 )
			return 1.0 - lfV;
		else
			return 0.0;
	case bilinear:
		lfV = ::fabs( lfV ); 
		return ( ( lfV < m_lfWidth ) ? m_lfWidth - lfV : 0.0 );
	case hermite:
		if( lfV < -1.0 )
			return 0.0;
		else if( lfV < 0.0 )
			return ( -2.0 * lfV - 3.0 ) * lfV * lfV + 1.0;
		else if( lfV < 1.0 )
			return ( 2.0 * lfV - 3.0 ) * lfV * lfV + 1.0;
		else
			return 0.0;
	case sinc:
		if( lfV == 0.0 )
			return 1.0;
		else
		{
			double lfTemp = 3.1415926535897932384626433832795 * lfV;
			return ::sin( lfTemp ) / ( lfTemp );
		}
	case catmull_rom:
		if( lfV < -2.0 )
			return 0.0;
		else if( lfV < -1.0 )
			return ( 0.5 * ( 4.0 + lfV * ( 8.0 + lfV * ( 5.0 + lfV ) ) ) );
		else if( lfV < 0.0 )
			return ( 0.5 * ( 2.0 + lfV * lfV * ( -5.0 - 3.0 * lfV ) ) );
		else if( lfV < 1.0 )
			return ( 0.5 * ( 2.0 + lfV * lfV * ( -5.0 + 3.0 * lfV ) ) );
		else if( lfV < 2.0 )
			return ( 0.5 * ( 4.0 + lfV * ( -8.0 + lfV * ( 5.0 - lfV ) ) ) );
		else
			return 0.0;
	case b_spline:
		lfV = ::fabs( lfV );
		if( lfV < 1.0 )
			return ( 4.0 + lfV * lfV * ( -6.0 + 3.0 * lfV ) ) / 6.0;
		else if( lfV < 2.0 )
		{
			double lfTemp = 2.0 - lfV;
			return ( lfTemp * lfTemp * lfTemp / 6.0 );
		}
		else
			return 0.0;
	case blackman:
		if( ::fabs( lfV ) > m_lfWidth )
			return 0.0; 
		else
		{
			double lfTemp = 2 * m_lfWidth + 1; 
			lfV /= ( lfTemp - 1 );
			return
				0.42
				+ 0.5  * ::cos( 2.0 * 3.1415926535897932384626433832795 * lfV )
				+ 0.08 * ::cos( 4.0 * 3.1415926535897932384626433832795 * lfV )
				; 
		}
	case lanczos:
		lfV = ::fabs( lfV ); 
		if( lfV >= m_lfWidth )
			return 0.0;
		lfV = stat_rCalc( lfV ) * stat_rCalc( lfV / m_lfWidth );
		return lfV;
	case quadratic:
		if( lfV < -1.5 )
			return 0.0;
		else if( lfV < -0.5 )
			return ( 0.5 * ( lfV + 1.5 ) * ( lfV + 1.5 ) );
		else if (lfV < 0.5 )
			return ( 0.75 - lfV * lfV );
		else if( lfV < 1.5 )
			return ( 0.5 * ( lfV - 1.5 ) * ( lfV - 1.5 ) );
		else
			return 0.0;
	case gaussian:
		if( ::fabs( lfV ) > m_lfWidth ) 
			return 0.0;
		else
		{
			static const double lfSq2PI = ::sqrt( 2.0 * 3.1415926535897932384626433832795 );
			return ( ::exp( -lfV * lfV / 2.0 ) / lfSq2PI ); 
		}
	case hamming:
		if( ::fabs( lfV ) > m_lfWidth ) 
			return 0.0;
		else
		{
			double lfWin =
				0.54 +
				0.46 * ::cos( 2.0 * 3.1415926535897932384626433832795 * lfV ); 
			double lfS = ( lfV == 0 )
				? 1.0
				: ( ::sin(
						3.1415926535897932384626433832795 * lfV)
						/ ( 3.1415926535897932384626433832795 * lfV
						)
					)
				; 
			return lfWin * lfS;
		}
	case bicubic:
		lfV = ::fabs( lfV );
		if( lfV < 1.0 )
			return ( m_arrParmBuffer[2] + lfV * lfV * ( m_arrParmBuffer[3] + lfV * m_arrParmBuffer[4] ) );
		else if( lfV < 2.0 )
			return ( m_arrParmBuffer[5] + lfV * ( m_arrParmBuffer[6] + lfV * ( m_arrParmBuffer[7] + lfV * m_arrParmBuffer[8] ) ) );
		else
			return 0.0;
	default:
		ASSERT( FALSE );
		return 0.0;
	} // switch( m_eFT )
}

double CExtBitmap::Filter::static_DefaultWidth(
	CExtBitmap::Filter::e_filter_type eFT
	)
{
	ASSERT( ft_min_number <= eFT && eFT <= ft_max_number );
	switch( eFT )
	{
	case box:
		return 0.5;
	case linear:
		return 1.0;
	case bilinear:
		return 1.0;
	case hermite:
		return 1.0;
	case sinc:
		return 1.0;
	case catmull_rom:
		return 2.0;
	case b_spline:
		return 2.0;
	case blackman:
		return 0.5;
	case lanczos:
		return 3.0;
	case quadratic:
		return 1.0;
	case gaussian:
		return 3.0;
	case hamming:
		return 0.5;
	case bicubic:
		return 2.0;
	default:
		ASSERT( FALSE );
		return 1.0;
	} // switch( eFT )
}

CExtBitmap::Filter::e_filter_type CExtBitmap::Filter::FilterTypeGet() const
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	return m_eFT;
}

void CExtBitmap::Filter::FilterTypeSet(
	CExtBitmap::Filter::e_filter_type eFT
	)
{
	ASSERT( ft_min_number <= eFT && eFT <= ft_max_number );
	m_eFT = eFT;
}

double CExtBitmap::Filter::FilterWidthGet() const
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	return m_lfWidth;
}

void CExtBitmap::Filter::FilterWidthSet(
	double lfWidth
	)
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	m_lfWidth = lfWidth;
}

void CExtBitmap::Filter::FilterWidthSetDefault()
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
double lfWidth = static_DefaultWidth( m_eFT );
	FilterWidthSet( lfWidth );
}

bool CExtBitmap::Filter::stat_FilterWidthIsUsed(
	CExtBitmap::Filter::e_filter_type eFT
	)
{
	ASSERT( ft_min_number <= eFT && eFT <= ft_max_number );
	switch( eFT )
	{
	case linear:
	case hermite:
	case sinc:
	case catmull_rom:
	case b_spline:
	case quadratic:
	case bicubic:
		return false;
	} // switch( eFT )
	return true;
}

bool CExtBitmap::Filter::FilterWidthIsUsed() const
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	return stat_FilterWidthIsUsed( m_eFT );
}

INT CExtBitmap::Filter::FilterParameterGetCount()
{
	return (sizeof(m_arrParmBuffer)/(sizeof(m_arrParmBuffer[0])));
}

double CExtBitmap::Filter::FilterParameterGetAt(
	INT nPos
	) const
{
	if( ! ( 0 <= nPos && nPos < (sizeof(m_arrParmBuffer)/(sizeof(m_arrParmBuffer[0]))) ) )
	{
		ASSERT( FALSE );
		return 0.0;
	}
	return m_arrParmBuffer[ nPos ];
}

void CExtBitmap::Filter::FilterParameterSetAt(
	INT nPos,
	double lfV
	)
{
	if( ! ( 0 <= nPos && nPos < (sizeof(m_arrParmBuffer)/(sizeof(m_arrParmBuffer[0]))) ) )
	{
		ASSERT( FALSE );
		return;
	}
	m_arrParmBuffer[ nPos ] = lfV;
}

__EXT_MFC_SAFE_LPCTSTR CExtBitmap::Filter::stat_FilterNameGet(
	CExtBitmap::Filter::e_filter_type eFT
	)
{
	ASSERT( ft_min_number <= eFT && eFT <= ft_max_number );
	switch( eFT )
	{
	case box:
		return _T("Box");
	case linear:
		return _T("Linear");
	case bilinear:
		return _T("Bilinear");
	case hermite:
		return _T("Hermite");
	case sinc:
		return _T("Sinc");
	case catmull_rom:
		return _T("Catmull-Rom");
	case b_spline:
		return _T("B-Spline");
	case blackman:
		return _T("Blackman");
	case lanczos:
		return _T("Lanczos");
	case quadratic:
		return _T("Quadratic");
	case gaussian:
		return _T("Gaussian");
	case hamming:
		return _T("Hamming");
	case bicubic:
		return _T("Bicubic");
	default:
		ASSERT( FALSE );
		return _T("");
	} // switch( eFT )
}

__EXT_MFC_SAFE_LPCTSTR CExtBitmap::Filter::FilterNameGet() const
{
	ASSERT( ft_min_number <= m_eFT && m_eFT <= ft_max_number );
	return stat_FilterNameGet( m_eFT );

}


CExtBitmap::WT::WT(
	Filter & _f,
	UINT nLineSizeDst,
	UINT nLineSizeSrc
	)
	: m_pWT( NULL )
{
	try
	{
		double lfTarget, lfScaleA = 1.0, lfScale = double(nLineSizeDst) / double(nLineSizeSrc);
		if( lfScale < 1.0 )
		{
			lfTarget = _f.FilterWidthGet() / lfScale; 
			lfScaleA = lfScale; 
		}
		else
			lfTarget = _f.FilterWidthGet(); 
		m_nReviewSize = 2 * (INT)::ceil( lfTarget ) + 1; 
		m_nImageLineSize = nLineSizeDst; 
		m_pWT = new WE [ m_nImageLineSize ];
		UINT nIndex;
		for( nIndex = 0; nIndex < m_nImageLineSize; nIndex++ )
			m_pWT[nIndex].m_pWV = new double[ m_nReviewSize ];
		double dOffset = (0.5 / lfScale) - 0.5;
		for( nIndex = 0; nIndex < m_nImageLineSize; nIndex++ )
		{
			double dCenter = (double)nIndex / lfScale + dOffset;
			INT nPosMin = max(0, (INT) ::floor( dCenter - lfTarget ) ); 
			INT nPosMax = min( (INT)ceil( dCenter + lfTarget ), int(nLineSizeSrc) - 1 ); 
			if( ( nPosMax - nPosMin + 1 ) > INT( m_nReviewSize ) )
			{
				if( nPosMin < ( INT(nLineSizeSrc) - 1 / 2 ) )
					nPosMin ++;
				else
					nPosMax --;
			}
			m_pWT[nIndex].m_nMin = nPosMin; 
			m_pWT[nIndex].m_nMax = nPosMax;
			int iSrc = 0;
			double dTotalWeight = 0;
			for( iSrc = nPosMin; iSrc <= nPosMax; iSrc++ )
			{
				double lfW =
					lfScaleA *
						_f.Process( lfScaleA * (dCenter - (double)iSrc) );
				m_pWT[nIndex].m_pWV[ iSrc - nPosMin ] = lfW;
				dTotalWeight += lfW;
			}
			if( (dTotalWeight > 0) && (dTotalWeight != 1) )
			{
				for( iSrc = nPosMin; iSrc <= nPosMax; iSrc++ )
					m_pWT[nIndex].m_pWV[iSrc-nPosMin] /= dTotalWeight; 
				iSrc = nPosMax - nPosMin;
				while( m_pWT[nIndex].m_pWV[iSrc] == 0 )
				{
					m_pWT[nIndex].m_nMax--;
					iSrc--;
					if( m_pWT[nIndex].m_nMax == m_pWT[nIndex].m_nMin )
						break;
				}
			}
		} 
		return;
	}
	catch( CException * pException )
	{
		pException->Delete();
		ASSERT( FALSE );
	}
	catch( ... )
	{
	}
	if( m_pWT != NULL )
	{
		delete [ ] m_pWT;
		m_pWT = NULL;
	}
}

CExtBitmap::WT::~WT()
{
	if( m_pWT != NULL )
		delete [ ] m_pWT;
}

LPBYTE CExtBitmap::stat_rcResize(
	LPBYTE pImageBuffer,
	ULONG nWidth,
	ULONG nHeight,
	Filter & _f,
	bool bEnableAlphaMarginAdjustment
	)
{
LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
ULONG nMaskRed		= stat_MaskRed( pImageBuffer );
ULONG nMaskGreen	= stat_MaskGreen( pImageBuffer );
ULONG nMaskBlue		= stat_MaskBlue( pImageBuffer );
ULONG nMaskAlpha	= stat_MaskAlpha( pImageBuffer );
ULONG nFlags		= stat_RunTimeFlagsGet( pImageBuffer );
LPBYTE pImageBufferDst =
		stat_AllocHL(
			ULONG(nWidth),
			ULONG(nHeight),
			ULONG(pBIH->biBitCount),
			nMaskRed,
			nMaskGreen,
			nMaskBlue,
			nMaskAlpha,
			nFlags
			);
	if( pImageBufferDst == NULL )
		return NULL;
bool bHorzFirst = false;
LONG nWeightLeft = LONG(nWidth) * pBIH->biHeight;
LONG nWeightRight = LONG(nHeight) * pBIH->biWidth;
	if( nWeightLeft <= nWeightRight )
		bHorzFirst = true;
	if( bHorzFirst )
	{
		LPBYTE pImageBufferTmp =
			stat_AllocHL(
				ULONG(nWidth),
				ULONG(pBIH->biHeight),
				ULONG(pBIH->biBitCount),
				nMaskRed,
				nMaskGreen,
				nMaskBlue,
				nMaskAlpha,
				nFlags
				);
		if( pImageBufferTmp == NULL )
		{
			stat_FreeHL( pImageBufferDst );
			return NULL;
		}
		stat_rcResizeH(
			pImageBuffer,
			pBIH->biWidth,
			pImageBufferTmp,
			nWidth,
			pBIH->biHeight,
			_f,
			bEnableAlphaMarginAdjustment
			);
		stat_rcResizeV(
			pImageBufferTmp,
			pBIH->biHeight,
			pImageBufferDst,
			nWidth,
			nHeight,
			_f,
			bEnableAlphaMarginAdjustment
			);
		stat_FreeHL( pImageBufferTmp );
	} // if( bHorzFirst )
	else
	{
		LPBYTE pImageBufferTmp =
			stat_AllocHL(
				ULONG(pBIH->biWidth),
				ULONG(nHeight),
				ULONG(pBIH->biBitCount),
				nMaskRed,
				nMaskGreen,
				nMaskBlue,
				nMaskAlpha,
				nFlags
				);
		if( pImageBufferTmp == NULL )
		{
			stat_FreeHL( pImageBufferDst );
			return NULL;
		}
		stat_rcResizeV(
			pImageBuffer,
			pBIH->biHeight,
			pImageBufferTmp,
			pBIH->biWidth,
			nHeight,
			_f,
			bEnableAlphaMarginAdjustment
			);
		stat_rcResizeH(
			pImageBufferTmp,
			pBIH->biWidth,
			pImageBufferDst,
			nWidth,
			nHeight,
			_f,
			bEnableAlphaMarginAdjustment
			);
		stat_FreeHL( pImageBufferTmp );
	} // if( bHorzFirst )
	return pImageBufferDst;
}

void CExtBitmap::stat_rcResizeH(
	LPBYTE pImageBufferSrc,
	ULONG nWidthSrc,
	LPBYTE pImageBufferDst,
	ULONG nWidthDst,
	ULONG nHeightDst,
	Filter & _f,
	bool bEnableAlphaMarginAdjustment
	)
{
LPBITMAPINFOHEADER pSrcBIH = stat_RcInfoHeader( pImageBufferSrc );
LPBITMAPINFOHEADER pDstBIH = stat_RcInfoHeader( pImageBufferDst );
	if( nWidthDst == nWidthSrc )
	{
		LPBYTE pSurfaceSrc = stat_RcSurface( pImageBufferSrc );
		LPBYTE pSurfaceDst = stat_RcSurface( pImageBufferDst );
		UINT nSizeInBytes = 
			nHeightDst *
				stat_RcScanLineSize(
					pDstBIH->biWidth,
					pDstBIH->biBitCount
					);
		__EXT_MFC_MEMCPY(
			pSurfaceDst,
			nSizeInBytes,
			pSurfaceSrc,
			nSizeInBytes
			);
	}
	else
	{
		UINT nWalkIdx = 0;
		WT _wt( _f, nWidthDst, nWidthSrc ); 
		switch( pSrcBIH->biBitCount )
		{
			case 8:
			case 24:
			case 32:
			{
				if( bEnableAlphaMarginAdjustment )
				{
					if(		pSrcBIH->biBitCount != 32
						||	stat_MaskAlpha( pImageBufferSrc ) == 0
						)
						bEnableAlphaMarginAdjustment = false;
				}
				UINT nPixelBytes =
					stat_RcLine(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						)
					/ pSrcBIH->biWidth;
				ULONG nScanLineSizeSrc =
					stat_RcScanLineSize(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						);
				ULONG nScanLineSizeDst =
					stat_RcScanLineSize(
						pDstBIH->biWidth,
						pDstBIH->biBitCount
						);
				for( UINT nY = 0; nY < nHeightDst; nY++ )
				{
					LPBYTE pSurfaceSrc =
						stat_RcScanLine(
							pImageBufferSrc,
							nY,
							nScanLineSizeSrc
							);
					LPBYTE pSurfaceDst =
						stat_RcScanLine(
							pImageBufferDst,
							nY,
							nScanLineSizeDst
							);
					for( UINT nX = 0; nX < nWidthDst; nX++ )
					{
						double arrW [ 4 ] = { 0.0, 0.0, 0.0, 0.0 };
						INT nPosMin = _wt.GetMin( nX );
						INT nPosMax = _wt.GetMax( nX );
						for( INT nPos = nPosMin; nPos <= nPosMax; nPos++ )
						{
							double lfW = _wt.Weight( nX, nPos - nPosMin );
							nWalkIdx = nPos * nPixelBytes;
							if(		bEnableAlphaMarginAdjustment
								&&	nPixelBytes == 4
								//&&	pSurfaceSrc[ nWalkIdx + 3 ] != 0
								)
							{
								arrW [ 0 ] +=
									lfW * double( pSurfaceSrc[ nWalkIdx + 0 ] ) * double( pSurfaceSrc[ nWalkIdx + 3 ] ) / 255.0;
								arrW [ 1 ] +=
									lfW * double( pSurfaceSrc[ nWalkIdx + 1 ] ) * double( pSurfaceSrc[ nWalkIdx + 3 ] ) / 255.0;
								arrW [ 2 ] +=
									lfW * double( pSurfaceSrc[ nWalkIdx + 2 ] ) * double( pSurfaceSrc[ nWalkIdx + 3 ] ) / 255.0;
								arrW [ 3 ] +=
									lfW * double( pSurfaceSrc[ nWalkIdx + 3 ] );
								nWalkIdx += 4;
							}
							else
							{
								for( UINT nPart = 0; nPart < nPixelBytes; nPart++ )
									arrW [ nPart ] +=
										lfW * double( pSurfaceSrc[ nWalkIdx ++ ] );
							}
						} 
						for( UINT nPart = 0; nPart < nPixelBytes; nPart++ )
							pSurfaceDst [ nPart ] = (BYTE)
								min(
									max(
										(int)0,
										(int)( arrW [ nPart ] + 0.5 )
										),
									(int)255
									);
						pSurfaceDst += nPixelBytes;
					} 
				}
			}
			break;
			case 16:
			{
				UINT nPixelWords =
					( stat_RcLine(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						)
					/ pSrcBIH->biWidth )
						/ sizeof(WORD);
				ULONG nScanLineSizeSrc =
					stat_RcScanLineSize(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						);
				ULONG nScanLineSizeDst =
					stat_RcScanLineSize(
						pDstBIH->biWidth,
						pDstBIH->biBitCount
						);
				for( UINT nY = 0; nY < nHeightDst; nY++ )
				{
					LPWORD pSurfaceSrc = (LPWORD)
						stat_RcScanLine(
							pImageBufferSrc,
							nY,
							nScanLineSizeSrc
							);
					LPWORD pSurfaceDst = (LPWORD)
						stat_RcScanLine(
							pImageBufferDst,
							nY,
							nScanLineSizeDst
							);
					for( UINT nX = 0; nX < nWidthDst; nX++ )
					{
						double arrW [ 4 ] = { 0.0, 0.0, 0.0, 0.0 };
						INT nPosMin = _wt.GetMin( nX );
						INT nPosMax = _wt.GetMax( nX );
						for( int nPos = nPosMin; nPos <= nPosMax; nPos++ )
						{
							double lfW = _wt.Weight( nX, nPos - nPosMin );
							nWalkIdx = nPos * nPixelWords;
							for( UINT nPart = 0; nPart < nPixelWords; nPart++ )
								arrW[ nPart ] +=
									lfW * double( pSurfaceSrc[ nWalkIdx ++ ] ); 
						} 
						for( UINT nPart = 0; nPart < nPixelWords; nPart++ )
							pSurfaceDst [ nPart ] = (WORD)
								min(
									max(
										(int)0,
										(int)( arrW [ nPart ] + 0.5 )
									),
									(int)0xFFFF
									);
						pSurfaceDst += nPixelWords;
					} 
				}
			}
			break;
		} // switch( pSrcBIH->biBitCount )
	}
}

void CExtBitmap::stat_rcResizeV(
	LPBYTE pImageBufferSrc,
	ULONG nHeightSrc,
	LPBYTE pImageBufferDst,
	ULONG nWidthDst,
	ULONG nHeightDst,
	Filter & _f,
	bool bEnableAlphaMarginAdjustment
	)
{
LPBITMAPINFOHEADER pSrcBIH = stat_RcInfoHeader( pImageBufferSrc );
LPBITMAPINFOHEADER pDstBIH = stat_RcInfoHeader( pImageBufferDst );
	if( nHeightSrc == nHeightDst )
	{
		LPBYTE pSurfaceSrc = stat_RcSurface( pImageBufferSrc );
		LPBYTE pSurfaceDst = stat_RcSurface( pImageBufferDst );
		UINT nSizeInBytes = 
			nHeightDst *
				stat_RcScanLineSize(
					pDstBIH->biWidth,
					pDstBIH->biBitCount
					);
		__EXT_MFC_MEMCPY(
			pSurfaceDst,
			nSizeInBytes,
			pSurfaceSrc,
			nSizeInBytes
			);
	}
	else
	{
		UINT nWalkIdx = 0;
		WT _wt( _f, nHeightDst, nHeightSrc ); 
		switch( pSrcBIH->biBitCount )
		{
			case 8:
			case 24:
			case 32:
			{
				if( bEnableAlphaMarginAdjustment )
				{
					if(		pSrcBIH->biBitCount != 32
						||	stat_MaskAlpha( pImageBufferSrc ) == 0
						)
						bEnableAlphaMarginAdjustment = false;
				}
				UINT nPixelBytes =
					stat_RcLine(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						)
					/ pSrcBIH->biWidth;
				UINT nScanLineSizeSrc =
					stat_RcScanLineSize(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						);
				UINT nScanLineSizeDst =
					stat_RcScanLineSize(
						pDstBIH->biWidth,
						pDstBIH->biBitCount
						);
				for( UINT nX = 0; nX < nWidthDst; nX++ )
				{
					nWalkIdx = nX * nPixelBytes;
					LPBYTE pSurfaceDst =
						stat_RcSurface( pImageBufferDst )
						+ nWalkIdx;
					for( UINT nY = 0; nY < nHeightDst; nY++ )
					{
						double arrW [ 4 ] = { 0.0, 0.0, 0.0, 0.0 };
						INT nPosMin = _wt.GetMin( nY );
						INT nPosMax = _wt.GetMax( nY );
						LPBYTE pSurfaceSrc =
							stat_RcScanLine(
								pImageBufferSrc,
								nPosMin,
								nScanLineSizeSrc
								)
							+ nWalkIdx;
						for( INT nPos = nPosMin; nPos <= nPosMax; nPos++ )
						{
							double lfW = _wt.Weight( nY, nPos - nPosMin );							
							if(		bEnableAlphaMarginAdjustment
								&&	nPixelBytes == 4
								//&&	pSurfaceSrc[ 3 ] != 0
								)
							{
								arrW [ 0 ] +=
									lfW * double( pSurfaceSrc[ 0 ] ) * double( pSurfaceSrc[ 3 ] ) / 255.0;
								arrW [ 1 ] +=
									lfW * double( pSurfaceSrc[ 1 ] ) * double( pSurfaceSrc[ 3 ] ) / 255.0;
								arrW [ 2 ] +=
									lfW * double( pSurfaceSrc[ 2 ] ) * double( pSurfaceSrc[ 3 ] ) / 255.0;
								arrW [ 3 ] +=
									lfW * double( pSurfaceSrc[ 3 ] );
							}
							else
							{
								for( UINT nPart = 0; nPart < nPixelBytes; nPart++ )
									arrW [ nPart ] +=
										lfW * double( pSurfaceSrc [ nPart ] );
							}
							pSurfaceSrc += nScanLineSizeSrc;
						}
						for( UINT nPart = 0; nPart < nPixelBytes; nPart++ )
						{
							pSurfaceDst [ nPart ] =
								(BYTE)min(
									max(
										(int)0,
										(int)( arrW [ nPart ] + 0.5 )
										),
									(int)255
									);
						}
						pSurfaceDst += nScanLineSizeDst;
					}
				}
			}
			break;
			case 16:
			{
				UINT nPixelWords =
					( stat_RcLine(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						)
					/ pSrcBIH->biWidth )
						/ sizeof(WORD);
				UINT nScanLineSizeSrc =
					stat_RcScanLineSize(
						pSrcBIH->biWidth,
						pSrcBIH->biBitCount
						) / sizeof(WORD);
				UINT nScanLineSizeDst =
					stat_RcScanLineSize(
						pDstBIH->biWidth,
						pDstBIH->biBitCount
						) / sizeof(WORD);
				for( UINT nX = 0; nX < nWidthDst; nX++ )
				{
					nWalkIdx = nX * nPixelWords;
					LPWORD pSurfaceDst = (LPWORD)
						stat_RcSurface( pImageBufferDst ) + nWalkIdx;
					for( UINT nY = 0; nY < nHeightDst; nY++ )
					{
						double arrW [ 4 ] = { 0.0, 0.0, 0.0, 0.0 };
						INT nPosMin = _wt.GetMin( nY );
						INT nPosMax = _wt.GetMax( nY );
						LPWORD pSurfaceSrc = (LPWORD)
							stat_RcScanLine(
								pImageBufferSrc,
								nPosMin,
								nScanLineSizeSrc
								)
							+ nWalkIdx;
						for( INT nPos = nPosMin; nPos <= nPosMax; nPos++ )
						{
							double lfW = _wt.Weight( nY, nPos - nPosMin );							
							for( UINT nPart = 0; nPart < nPixelWords; nPart++ )
								arrW [ nPart ] +=
									lfW * double( pSurfaceSrc [ nPart ] );
							pSurfaceSrc += nScanLineSizeSrc;
						}
						for( UINT nPart = 0; nPart < nPixelWords; nPart++ )
							pSurfaceDst [ nPart ] = (WORD)
								min(
									max(
										(int)0,
										(int)( arrW [ nPart ] + 0.5 )
										),
									(int)0xFFFF
									);
						pSurfaceDst += nScanLineSizeDst;
					}
				}
			}
			break;
		} // switch( pSrcBIH->biBitCount )
	}
}

bool CExtBitmap::Flip(
	bool bHorizontal,
	bool bVertVertical,
	LPCRECT pRectFlip // = NULL
	)
{
	if(		IsEmpty()
		||	( (!bHorizontal) && (!bVertVertical) )
		)
		return false;
CSize _size = GetSize();
CRect rcFlip( 0, 0, _size.cx, _size.cy );
	if( pRectFlip != NULL )
	{
		rcFlip.left = max( rcFlip.left, pRectFlip->left );
		rcFlip.top = max( rcFlip.top, pRectFlip->top );
		rcFlip.right = min( rcFlip.right, pRectFlip->right );
		rcFlip.bottom = min( rcFlip.bottom, pRectFlip->bottom );
	}
	if(		rcFlip.left >= rcFlip.right
		||	rcFlip.top >= rcFlip.bottom
		)
		return false;
CSize _sizeFlip = rcFlip.Size();
INT nX, nY, nSizeWalk;
RGBQUAD _pixel1, _pixel2;
	if( bHorizontal )
	{
		nSizeWalk = _sizeFlip.cx / 2;
		ASSERT( nSizeWalk >= 1 );
		for( nX = 0; nX < nSizeWalk; nX ++ )
		{
			for( nY = 0; nY < _sizeFlip.cy; nY ++ )
			{
				VERIFY(
					GetPixel(
						nX + rcFlip.left,
						nY + rcFlip.top,
						_pixel1
						)
					);
				VERIFY(
					GetPixel(
						rcFlip.right - nX - 1,
						nY + rcFlip.top,
						_pixel2
						)
					);
				VERIFY(
					SetPixel(
						nX + rcFlip.left,
						nY + rcFlip.top,
						_pixel2
						)
					);
				VERIFY(
					SetPixel(
						rcFlip.right - nX - 1,
						nY + rcFlip.top,
						_pixel1
						)
					);
			} // for( nY = 0; nY < _sizeFlip.cy; nY ++ )
		} // for( nX = 0; nX < nSizeWalk; nX ++ )
	} // if( bHorizontal )
	if( bVertVertical )
	{
		nSizeWalk = _sizeFlip.cy / 2;
		ASSERT( nSizeWalk >= 1 );
		for( nY = 0; nY < nSizeWalk; nY ++ )
		{
			for( nX = 0; nX < _sizeFlip.cx; nX ++ )
			{
				VERIFY(
					GetPixel(
						nX + rcFlip.left,
						nY + rcFlip.top,
						_pixel1
						)
					);
				VERIFY(
					GetPixel(
						nX + rcFlip.left,
						rcFlip.bottom - nY - 1,
						_pixel2
						)
					);
				VERIFY(
					SetPixel(
						nX + rcFlip.left,
						nY + rcFlip.top,
						_pixel2
						)
					);
				VERIFY(
					SetPixel(
						nX + rcFlip.left,
						rcFlip.bottom - nY - 1,
						_pixel1
						)
					);
			} // for( nX = 0; nX < _sizeFlip.cx; nX ++ )
		} // for( nY = 0; nY < nSizeWalk; nY ++ )
	} // if( bVertVertical )
	OnContentChanged();
	return true;
}

bool CExtBitmap::FlipHorizontal(
	LPCRECT pRectFlip // = NULL
	)
{
	return Flip( true, false, pRectFlip );
}

bool CExtBitmap::FlipVertical(
	LPCRECT pRectFlip // = NULL
	)
{
	return Flip( false, true, pRectFlip );
}

bool CExtBitmap::Scale(
	INT nWidth,
	INT nHeight,
	CExtBitmap::Filter & _f
	)
{
	if(		IsEmpty()
		||	nWidth <= 0
		||	nHeight <= 0
		||	GetBPP() < 16
		)
		return false;
CSize _size = GetSize();
	if(		INT(_size.cx) == nWidth
		&&	INT(_size.cy) == nHeight
		)
		return true;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
LPBYTE pImageBuffer =
		stat_rcResize(
			m_pImageBuffer,
			ULONG(nWidth),
			ULONG(nHeight),
			_f,
			true
			);
	if( pImageBuffer == NULL )
		return false;
	Empty();
	m_pImageBuffer = pImageBuffer;
	OnContentChanged();
	return true;
}

bool CExtBitmap::Scale(
	INT nWidth,
	INT nHeight
	)
{
CExtBitmap::Filter _f; //( CExtBitmap::Filter::bilinear );
	return Scale( nWidth, nHeight, _f );
}

bool CExtBitmap::LoadBMP_Resource(
	__EXT_MFC_SAFE_LPCTSTR sResourceID,
	__EXT_MFC_SAFE_LPCTSTR sResourceType, // = RT_BITMAP // default bitmap section
	HINSTANCE hInst, // = NULL // find it automatically
	bool bMake32, //= false
	bool bEnableThrowException // = false
	)
{
	if( hInst == NULL )
	{
		hInst =
			g_ResourceManager->FindResourceHandle(
				sResourceType,
				(UINT)(__EXT_MFC_UINT_PTR)(LPCTSTR)sResourceID
				);
		if( hInst == NULL )
		{
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		} // if( hInst == NULL )
	} // if( hInst == NULL )
HRSRC hRsrc =
		::FindResource(
			hInst,
			sResourceID,
			sResourceType
			);
	if( hRsrc == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return NULL;
	} // if( hRsrc == NULL )
	return
		LoadBMP_Resource(
			hInst,
			hRsrc,
			bMake32,
			bEnableThrowException
			);
}

bool CExtBitmap::LoadBMP_Resource(
	HINSTANCE hInst,
	HRSRC hRsrc,
	bool bMake32, //= false
	bool bEnableThrowException // = false
	)
{
HGLOBAL hGlobal = ::LoadResource( hInst, hRsrc );
	if( hGlobal == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( hGlobal == NULL )
LPBYTE pData = (LPBYTE)::LockResource( hGlobal );
DWORD dwResourceSize = ::SizeofResource( hInst, hRsrc );
	ASSERT( dwResourceSize > 0 );
	if( pData == NULL )
	{
		::FreeResource( hGlobal );
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( pData == NULL )
	try
	{
		CMemFile _file;
		_file.Attach( pData, dwResourceSize );
		_file.Seek( 0, CFile::begin );
		CArchive ar( &_file, CArchive::load );
		bool bRetVal =
			LoadBMP(
				ar,
				bMake32,
				bEnableThrowException,
				true
				);
		::UnlockResource( hGlobal );
		::FreeResource( hGlobal );
		return bRetVal;
	} // try
	catch( CException * pException )
	{
		::UnlockResource( hGlobal );
		::FreeResource( hGlobal );
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		::UnlockResource( hGlobal );
		::FreeResource( hGlobal );
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return false;
}

bool CExtBitmap::LoadBMP_Buffer(
	LPCVOID pBuffer,
	UINT nByteCount,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bResourceFormat // = false
	)
{
	ASSERT( pBuffer != NULL );
	try
	{
		Empty();
		CMemFile _file(
			(LPBYTE)pBuffer,
			nByteCount
			);
		if( ! LoadBMP_File(
				_file,
				bMake32,
				bEnableThrowException,
				bResourceFormat
				)
			)
			return false;
		return true;
	} // try
	catch( CException * pException )
	{
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return false;
}

bool CExtBitmap::LoadBMP_File(
	__EXT_MFC_SAFE_LPCTSTR strFilePath,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bResourceFormat // = false
	)
{
	try
	{
		CFile _file(
			strFilePath,
			CFile::modeRead
			| CFile::typeBinary
			);
		return
			LoadBMP_File(
				_file,
				bMake32,
				bEnableThrowException,
				bResourceFormat
				);
	} // try
	catch( CException * pException )
	{
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return false;
}

bool CExtBitmap::LoadBMP_File(
	CFile & _file,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bResourceFormat // = false
	)
{
CArchive ar(
		&_file,
		CArchive::load
		);
	return
		LoadBMP(
			ar,
			bMake32,
			bEnableThrowException,
			bResourceFormat
			);
}

bool CExtBitmap::LoadBMP(
	CArchive & ar,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bResourceFormat // = false
	)
{
	ASSERT( ar.IsLoading() );
	Empty();
	m_pImageBuffer =
		stat_BmpLoad(
			ar,
			bEnableThrowException,
			bResourceFormat
			);
	if( m_pImageBuffer == NULL )
		return false;
	OnLoad();
	if( bMake32 )
		return Make32( bEnableThrowException );
	return true;
}

#ifndef _AFX_NO_OLE_SUPPORT

bool CExtBitmap::stat_OlePictureLoad(
	IStream * pStream, DWORD dwImageSize, IPicture ** ppPicture,
	CSize * pSizeImage, OLE_XSIZE_HIMETRIC * p_nWidthHM, OLE_YSIZE_HIMETRIC * p_nHeightHM,
	bool bShowMsgBoxes // = false
	)
{
	ASSERT( ppPicture != NULL && pStream != NULL );
	if( (*ppPicture) != NULL )
	{
		(*ppPicture)->Release();
		(*ppPicture) = NULL;
	}
	if( pSizeImage != NULL )
		pSizeImage->cx = pSizeImage->cy = 0;
	if( p_nWidthHM != NULL )
		(*p_nWidthHM) = 0L;
	if( p_nHeightHM != NULL )
		(*p_nHeightHM) = 0L;
HRESULT hr = ::OleLoadPicture( pStream, dwImageSize, FALSE, IID_IPicture, (LPVOID*)ppPicture );
	if( ( !( SUCCEEDED(hr) ) ) || (*ppPicture) == NULL )
	{
		if( bShowMsgBoxes )
			::AfxMessageBox( _T("Image format error(s) detected while reading picture file."), MB_OK|MB_ICONERROR );
		return false;
	}
	if( pSizeImage != NULL || p_nWidthHM != NULL || p_nHeightHM != NULL )
	{
		OLE_XSIZE_HIMETRIC nWidthHM = 0L;
		OLE_YSIZE_HIMETRIC nHeightHM = 0L;
		(*ppPicture)->get_Width ( &nWidthHM );
		(*ppPicture)->get_Height( &nHeightHM );
		if( pSizeImage != NULL )
		{
			pSizeImage->cx = ::MulDiv( nWidthHM,  g_PaintManager.m_nLPX, 2540 );
			pSizeImage->cy = ::MulDiv( nHeightHM, g_PaintManager.m_nLPY, 2540 );
		}
		if( p_nWidthHM != NULL )
			(*p_nWidthHM) = nWidthHM;
		if( p_nHeightHM != NULL )
			(*p_nHeightHM) = nHeightHM;
	}
	return true;
}

bool CExtBitmap::stat_OlePictureLoad(
	LPCTSTR strPictureFilePath, IPicture ** ppPicture,
	CSize * pSizeImage, OLE_XSIZE_HIMETRIC * p_nWidthHM, OLE_YSIZE_HIMETRIC * p_nHeightHM,
	bool bShowMsgBoxes // = false
	)
{
	ASSERT( ppPicture != NULL && strPictureFilePath != NULL && _tcslen(strPictureFilePath) > 0 );
	if( (*ppPicture) != NULL )
	{
		(*ppPicture)->Release();
		(*ppPicture) = NULL;
	}
	if( pSizeImage != NULL )
		pSizeImage->cx = pSizeImage->cy = 0;
	if( p_nWidthHM != NULL )
		(*p_nWidthHM) = 0L;
	if( p_nHeightHM != NULL )
		(*p_nHeightHM) = 0L;
HANDLE hFile = ::CreateFile( strPictureFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		if( bShowMsgBoxes )
		{
			CExtSafeString strMsgBox;
			strMsgBox.Format( _T("Failed not read the \"%s\" picture file."), strPictureFilePath );
			::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
		}
		return false;
	}
DWORD dwFileSize = ::GetFileSize( hFile, NULL );

	if( dwFileSize == (DWORD)-1 )
	{
		::CloseHandle( hFile );
		if( bShowMsgBoxes )
		{
			CExtSafeString strMsgBox;
			strMsgBox.Format( _T("The \"%s\" picture file is invalid."), strPictureFilePath );
			::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
		}
		return false;
	}
LPVOID pvData = NULL;
HGLOBAL hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, dwFileSize );
	if( hGlobal == NULL)
	{
		::CloseHandle( hFile );
		if( bShowMsgBoxes )
		{
			CExtSafeString strMsgBox;
			strMsgBox.Format( _T("Not enough memory for reading the \"%s\" picture file."), strPictureFilePath );
			::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
		}
		return false;
	}
	pvData = GlobalLock(hGlobal);
	if( pvData == NULL )
	{
		::GlobalUnlock( hGlobal );
		::CloseHandle( hFile );
		if( bShowMsgBoxes )
		{
			CExtSafeString strMsgBox;
			strMsgBox.Format( _T("Memory locking error occured while reading the \"%s\" picture file."), strPictureFilePath );
			::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
		}
		return false;
	}
DWORD dwBytesRead = 0;
BOOL bRead = ::ReadFile( hFile, pvData, dwFileSize, &dwBytesRead, NULL );
	::GlobalUnlock( hGlobal );
	::CloseHandle( hFile );
	if( ! bRead )
	{
		if( bShowMsgBoxes )
		{
			CExtSafeString strMsgBox;
			strMsgBox.Format( _T("File reading error occured while accessing the \"%s\" picture file."), strPictureFilePath );
			::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
		}
		return false;
	}
LPSTREAM pStream = NULL;
HRESULT hr = ::CreateStreamOnHGlobal( hGlobal, TRUE, &pStream );
	if( ( ! ( SUCCEEDED(hr) ) ) || pStream == NULL )
	{
		if( bShowMsgBoxes )
		{
			CExtSafeString strMsgBox;
			strMsgBox.Format( _T("Not enough memory for reading the \"%s\" picture file."), strPictureFilePath );
			::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
		}
		if( pStream != NULL )
			pStream->Release();
		return false;
	}

bool bRetVal = stat_OlePictureLoad( pStream, dwFileSize, ppPicture, pSizeImage, p_nWidthHM, p_nHeightHM, false );
	pStream->Release();
	if( (!bRetVal) && bShowMsgBoxes )
	{
		CExtSafeString strMsgBox;
		strMsgBox.Format( _T("Image format error(s) detected while reading the \"%s\" picture file."), strPictureFilePath );
		::AfxMessageBox( LPCTSTR(strMsgBox), MB_OK|MB_ICONERROR );
	}
	return bRetVal;
}

bool CExtBitmap::OlePictureLoad(
	__EXT_MFC_SAFE_LPCTSTR sResourceID,
	__EXT_MFC_SAFE_LPCTSTR sResourceType, // = RT_BITMAP // default bitmap section
	HINSTANCE hInst, // = NULL // find it automatically
	bool bMake32, // = false
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
	if( hInst == NULL )
	{
		hInst =
			g_ResourceManager->FindResourceHandle(
				sResourceType,
				(UINT)(__EXT_MFC_UINT_PTR)(LPCTSTR)sResourceID
				);
		if( hInst == NULL )
		{
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		} // if( hInst == NULL )
	} // if( hInst == NULL )
HRSRC hRsrc =
		::FindResource(
			hInst,
			sResourceID,
			sResourceType
			);
	if( hRsrc == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return NULL;
	} // if( hRsrc == NULL )
	return
		OlePictureLoad(
			hInst,
			hRsrc,
			bMake32,
			bEnableThrowException,
			bShowMsgBoxes
			);
}

bool CExtBitmap::OlePictureLoad(
	HINSTANCE hInst,
	HRSRC hRsrc,
	bool bMake32, // = false
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
HGLOBAL hGlobal = ::LoadResource( hInst, hRsrc );
	if( hGlobal == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( hGlobal == NULL )
LPBYTE pData = (LPBYTE)::LockResource( hGlobal );
DWORD dwResourceSize = ::SizeofResource( hInst, hRsrc );
	ASSERT( dwResourceSize > 0 );
	if( pData == NULL )
	{
		::FreeResource( hGlobal );
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( pData == NULL )
	try
	{
		CMemFile _file;
		_file.Attach( pData, dwResourceSize );
		_file.Seek( 0, CFile::begin );
		CArchive ar( &_file, CArchive::load );
		bool bRetVal =
			OlePictureLoad(
				ar,
				bMake32,
				bEnableThrowException,
				bShowMsgBoxes
				);
		::UnlockResource( hGlobal );
		::FreeResource( hGlobal );
		return bRetVal;
	} // try
	catch( CException * pException )
	{
		::UnlockResource( hGlobal );
		::FreeResource( hGlobal );
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		::UnlockResource( hGlobal );
		::FreeResource( hGlobal );
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return false;
}

bool CExtBitmap::OlePictureLoad(
	CFile & _file,
	bool bMake32, // = false,
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
CArchive ar( &_file, CArchive::load );
	return OlePictureLoad( ar, bMake32, bEnableThrowException, bShowMsgBoxes );
}

bool CExtBitmap::OlePictureLoad(
	CArchive & ar,
	bool bMake32, // = false,
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
CArchiveStream _stream( &ar );
	return OlePictureLoad( &_stream, bMake32, bEnableThrowException, bShowMsgBoxes );
}

bool CExtBitmap::OlePictureLoad(
	IStream * pStream,
	bool bMake32, // = false
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
	return OlePictureLoad( pStream, 0, bMake32, bEnableThrowException, bShowMsgBoxes );
}

bool CExtBitmap::OlePictureLoad(
	IStream * pStream,
	DWORD dwImageSize,
	bool bMake32, // = false
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
	if( pStream == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
	Empty();
LPPICTURE pPicture = NULL;
CSize sizeBmp( 0, 0 );
OLE_XSIZE_HIMETRIC nWidthHM = 0L;
OLE_YSIZE_HIMETRIC nHeightHM = 0L;
	if( ! stat_OlePictureLoad( pStream, dwImageSize, &pPicture, &sizeBmp, &nWidthHM, &nHeightHM, bShowMsgBoxes ) )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
CWindowDC dcDesktop(NULL);
CDC dcBuffer;
	if( dcBuffer.CreateCompatibleDC( &dcDesktop ) )
	{
		BITMAPINFOHEADER bih; bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = sizeBmp.cx; bih.biHeight = sizeBmp.cy; bih.biPlanes = 1; bih.biBitCount = 32; bih.biCompression = BI_RGB;
		bih.biSizeImage = bih.biWidth*bih.biHeight; bih.biXPelsPerMeter = 0; bih.biYPelsPerMeter = 0; bih.biClrUsed = 0; bih.biClrImportant = 0;
		COLORREF * pClrSurface = NULL;
		HBITMAP hDIB = ::CreateDIBSection( dcDesktop.GetSafeHdc(), (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&pClrSurface, NULL, NULL );
		if( hDIB != NULL )
		{
			ASSERT( pClrSurface != NULL );
			CBitmap bmpBuffer; // works as destructor for hDIB
			bmpBuffer.Attach( hDIB );
			CBitmap * pOldBmp = dcBuffer.SelectObject( &bmpBuffer );
			HRESULT hr = pPicture->Render( dcBuffer.m_hDC, 0, 0, sizeBmp.cx, sizeBmp.cy, 0, nHeightHM, nWidthHM, -nHeightHM, NULL );
			if( SUCCEEDED( hr ) )
			{
				CRect rcBuffer( 0, 0, sizeBmp.cx, sizeBmp.cy );
				FromSurface( dcBuffer.m_hDC, rcBuffer );
			}
			dcBuffer.SelectObject( pOldBmp );
		}
	}
	pPicture->Release();
bool bRetVal = IsEmpty() ? false : true;
	if( bRetVal )
	{
		if( bMake32 )
			bRetVal = Make32( bEnableThrowException );
		if( bRetVal )
			OnLoad();
	}
	else if( bEnableThrowException )
		::AfxThrowUserException();
	return bRetVal;
}

bool CExtBitmap::OlePictureLoad(
	LPCTSTR strPictureFilePath,
	bool bMake32,
	bool bEnableThrowException, // = false
	bool bShowMsgBoxes // = false
	)
{
	if( strPictureFilePath == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
	Empty();
LPPICTURE pPicture = NULL;
CSize sizeBmp( 0, 0 );
OLE_XSIZE_HIMETRIC nWidthHM = 0L;
OLE_YSIZE_HIMETRIC nHeightHM = 0L;
	if( ! stat_OlePictureLoad( strPictureFilePath, &pPicture, &sizeBmp, &nWidthHM, &nHeightHM, bShowMsgBoxes ) )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
CWindowDC dcDesktop(NULL);
CDC dcBuffer;
	if( dcBuffer.CreateCompatibleDC( &dcDesktop ) )
	{
		BITMAPINFOHEADER bih; bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = sizeBmp.cx; bih.biHeight = sizeBmp.cy; bih.biPlanes = 1; bih.biBitCount = 32; bih.biCompression = BI_RGB;
		bih.biSizeImage = bih.biWidth*bih.biHeight; bih.biXPelsPerMeter = 0; bih.biYPelsPerMeter = 0; bih.biClrUsed = 0; bih.biClrImportant = 0;
		COLORREF * pClrSurface = NULL;
		HBITMAP hDIB = ::CreateDIBSection( dcDesktop.GetSafeHdc(), (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&pClrSurface, NULL, NULL );
		if( hDIB != NULL )
		{
			ASSERT( pClrSurface != NULL );
			CBitmap bmpBuffer; // works as destructor for hDIB
			bmpBuffer.Attach( hDIB );
			CBitmap * pOldBmp = dcBuffer.SelectObject( &bmpBuffer );
			HRESULT hr = pPicture->Render( dcBuffer.m_hDC, 0, 0, sizeBmp.cx, sizeBmp.cy, 0, nHeightHM, nWidthHM, -nHeightHM, NULL );
			if( SUCCEEDED( hr ) )
			{
				CRect rcBuffer( 0, 0, sizeBmp.cx, sizeBmp.cy );
				FromSurface( dcBuffer.m_hDC, rcBuffer );
			}
			dcBuffer.SelectObject( pOldBmp );
		}
	}
	pPicture->Release();
bool bRetVal = IsEmpty() ? false : true;
	if( bRetVal )
	{
		if( bMake32 )
			bRetVal = Make32( bEnableThrowException );
		if( bRetVal )
			OnLoad();
	}
	else if( bEnableThrowException )
		::AfxThrowUserException();
	return bRetVal;
}

#endif // _AFX_NO_OLE_SUPPORT

bool CExtBitmap::Make32(
	bool bEnableThrowException // = false
	)
{
	if( IsEmpty() )
		return false;
LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( m_pImageBuffer );
	if( pBIH->biBitCount == 32 )
	{
		LPBYTE pImageBuffer = m_pImageBuffer;
		ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
		pImageBuffer -= nMaskBufferSize;
		ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
		if(		pMaskBuffer[0] == 0x00FF0000
			&&	pMaskBuffer[1] == 0x0000FF00
			&&	pMaskBuffer[2] == 0x000000FF
			&&	pMaskBuffer[3] != 0xFF000000
			)
		{
			pMaskBuffer[3] = 0xFF000000;
			OnContentChanged();
		}
		return true;
	}
LPBYTE pImageBuffer32 = stat_RcTo32( m_pImageBuffer, bEnableThrowException );
	if( pImageBuffer32 == NULL )
		return false;
	stat_FreeHL( m_pImageBuffer );
	m_pImageBuffer = pImageBuffer32;
	OnContentChanged();
	return true;
}

bool CExtBitmap::SaveBMP_File(
	__EXT_MFC_SAFE_LPCTSTR strFilePath,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bUseIndexedRLE // = true
	)
{
	try
	{
		CFile _file(
			strFilePath,
			CFile::modeCreate
				| CFile::modeWrite
				| CFile::typeBinary
			);
		return
			SaveBMP_File(
				_file,
				bMake32,
				bEnableThrowException,
				bUseIndexedRLE
				);
	} // try
	catch( CException * pException )
	{
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return false;
}

bool CExtBitmap::SaveBMP_File(
	CFile & _file,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bUseIndexedRLE // = true
	)
{
CArchive ar(
		&_file,
		CArchive::store
		);
	return
		SaveBMP(
			ar,
			bMake32,
			bEnableThrowException,
			bUseIndexedRLE
			);
}

bool CExtBitmap::SaveBMP(
	CArchive & ar,
	bool bMake32, //= false
	bool bEnableThrowException, // = false
	bool bUseIndexedRLE // = true
	)
{
	ASSERT( ar.IsStoring() );
	if( m_pImageBuffer == NULL )
	{
		ASSERT( FALSE );
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( m_pImageBuffer == NULL )
	if( bMake32 )
	{
		if( ! Make32( bEnableThrowException ) )
			return false;
	} // if( bMake32 )
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
	return
		stat_BmpSave(
			m_pImageBuffer,
			ar,
			bEnableThrowException,
			bUseIndexedRLE
			);
}

bool CExtBitmap::FromColor(
	COLORREF clr,
	const SIZE & _dimension,
	BYTE nAlpha, // = BYTE(255)
	bool bEnableThrowException // = false
	)
{
	if( _dimension.cx <= 0 || _dimension.cy <= 0 )
		return false;
	Empty();
	m_pImageBuffer =
		stat_AllocHL(
			_dimension.cx,
			_dimension.cy,
			32,
			0x00FF0000,
			0x0000FF00,
			0x000000FF,
			0xFF000000,
			0 // run-time flags
			);
	if( m_pImageBuffer == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( m_pImageBuffer == NULL )
CRect rcFill( 0, 0, _dimension.cx, _dimension.cy );
	ColorRect( clr, rcFill, nAlpha );
	OnLoad();
	return true;
}

void CExtBitmap::ColorRect(
	COLORREF clr,
	const RECT & rcFill,
	BYTE nAlpha // = BYTE(255)
	)
{
	if( IsEmpty() )
		return;
CSize sizeBmp = GetSize();
CRect rcDraw(
		max( rcFill.left, 0 ),
		max( rcFill.top, 0 ),
		min( rcFill.right, sizeBmp.cx ),
		min( rcFill.bottom, sizeBmp.cy )
		);
	if( rcDraw.left >= rcDraw.right || rcDraw.top >= rcDraw.bottom )
		return;
LONG nX, nY;
	for( nY = rcDraw.top; nY < rcDraw.bottom; nY ++ )
	{
		for( nX = rcDraw.left; nX < rcDraw.right; nX ++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			_pixel.rgbRed   = GetRValue(clr);
			_pixel.rgbGreen = GetGValue(clr);
			_pixel.rgbBlue  = GetBValue(clr);
			_pixel.rgbReserved = nAlpha;
			VERIFY( SetPixel( nX, nY, _pixel ) );
		}
	}
}

bool CExtBitmap::FromGradient(
	COLORREF clrLeft,
	COLORREF clrRight,
	bool bHorz,
	const SIZE & _dimension,
	UINT nCountOfSteps, // = 256
	BYTE nAlphaLeft, // = BYTE(255)
	BYTE nAlphaRight, // = BYTE(255)
	bool bEnableThrowException // = false
	)
{
	if( _dimension.cx <= 0 || _dimension.cy <= 0 || nCountOfSteps <= 0 )
		return false;
	nCountOfSteps = min( nCountOfSteps, 256 );
	Empty();
	m_pImageBuffer =
		stat_AllocHL(
			_dimension.cx,
			_dimension.cy,
			32,
			0x00FF0000,
			0x0000FF00,
			0x000000FF,
			0xFF000000,
			0 // run-time flags
			);
	if( m_pImageBuffer == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( m_pImageBuffer == NULL )
CRect rcFill( 0, 0, _dimension.cx, _dimension.cy );
	GradientRect( clrLeft, clrRight, bHorz, rcFill, nCountOfSteps, nAlphaLeft, nAlphaRight );
	OnLoad();
	return true;
}

void CExtBitmap::GradientRect(
	COLORREF clrLeft,
	COLORREF clrRight,
	bool bHorz,
	const RECT & rcFill,
	UINT nCountOfSteps, // = 256
	BYTE nAlphaLeft, // = BYTE(255),
	BYTE nAlphaRight // = BYTE(255)
	)
{
	if( nCountOfSteps == 0 || IsEmpty() )
		return;
	nCountOfSteps = min( nCountOfSteps, 256 );
CSize sizeBmp = GetSize();
CRect rcDraw(
		max( rcFill.left, 0 ),
		max( rcFill.top, 0 ),
		min( rcFill.right, sizeBmp.cx ),
		min( rcFill.bottom, sizeBmp.cy )
		);
	if( rcDraw.left >= rcDraw.right || rcDraw.top >= rcDraw.bottom )
		return;
	ASSERT( nCountOfSteps > 0 && nCountOfSteps <= 256 );
INT nWidth = rcDraw.Width(), nHeight = rcDraw.Height();
	if( bHorz )
	{
		if( nCountOfSteps > UINT(nHeight) )
			nCountOfSteps = UINT(nHeight);
	}
	else
	{
		if( nCountOfSteps > UINT(nWidth) )
			nCountOfSteps = UINT(nWidth);
	}
CRect rcItem = rcDraw;
	for( INT nStep = 0, nBackStep = nCountOfSteps; nStep < INT(nCountOfSteps); nStep++, nBackStep-- )
	{
		COLORREF clrItem =
			RGB(
				( GetRValue(clrLeft)*nBackStep + GetRValue(clrRight)*nStep ) / nCountOfSteps,
				( GetGValue(clrLeft)*nBackStep + GetGValue(clrRight)*nStep ) / nCountOfSteps,
				( GetBValue(clrLeft)*nBackStep + GetBValue(clrRight)*nStep ) / nCountOfSteps
				);
		BYTE nAlpha = BYTE( ( INT(nAlphaLeft)*nBackStep + INT(nAlphaRight)*nStep ) / nCountOfSteps );
		if( bHorz )
		{
			rcItem.top = rcDraw.bottom - ::MulDiv( nStep, nHeight, nCountOfSteps );
			rcItem.top --;
			if( nStep == ( INT(nCountOfSteps) - 1 ) )
			{
				rcItem.top = rcDraw.top;
				if( rcItem.top > rcItem.bottom )
					break;
			}
			ASSERT( rcItem.Height() >= 1 );
			ColorRect( clrItem, rcItem, nAlpha );
			rcItem.bottom = rcItem.top;
		} // if( bHorz )
		else
		{
			rcItem.right = ::MulDiv( nStep, nWidth, nCountOfSteps ) + rcDraw.left;
			rcItem.right ++;
			if( nStep == ( INT(nCountOfSteps) - 1 ) )
			{
				rcItem.right = rcDraw.right;
				if( rcItem.right < rcItem.left )
					break;
			}
			ASSERT( rcItem.Width() >= 1 );
			ColorRect( clrItem, rcItem, nAlpha );
			rcItem.left = rcItem.right;
		} // else from if( bHorz )
	} // for( INT nStep = 0, nBackStep = nCountOfSteps; nStep < INT(nCountOfSteps); nStep++, nBackStep-- )
}

bool CExtBitmap::FromSurface(
	HDC hDC,
	const RECT & rcSrc,
	bool bEnableThrowException // = false
	)
{
	Empty();
	if(		hDC == NULL
		||	rcSrc.left >= rcSrc.right
		||	rcSrc.top >= rcSrc.bottom
		)
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
HDC hDCSrc = ::CreateCompatibleDC( NULL );
	if( hDCSrc == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
CDC dc;
	dc.Attach( hDCSrc );
DWORD biWidth = rcSrc.right - rcSrc.left;
DWORD biHeight = rcSrc.bottom - rcSrc.top;
BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = biWidth;
	bih.biHeight = biHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = biWidth * biHeight;
COLORREF * pClrSurface = NULL;
HBITMAP hBitmap =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)&pClrSurface,
			NULL,
			NULL
			);
	if(		hBitmap == NULL 
		||	pClrSurface == NULL 
		)
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
CBitmap bmp;
	bmp.Attach( hBitmap );
HGDIOBJ hBitmapOld = ::SelectObject( hDCSrc, (HGDIOBJ)hBitmap );
	VERIFY(
		::BitBlt(
			hDCSrc,
			0, 0, biWidth, biHeight,
			hDC,
			rcSrc.left, rcSrc.top,
			SRCCOPY
			)
		);
	::SelectObject( hDCSrc, (HGDIOBJ)hBitmapOld );
	if( ! FromBitmap( hBitmap, bEnableThrowException ) )
		return false;
	bmp.DeleteObject();
	dc.DeleteDC();
	OnLoad();
	return true;
}

bool CExtBitmap::FromBitmap(
	HBITMAP hBitmap,
	bool bEnableThrowException, // = false
	bool bNoAdjustAlpha // = false
	)
{
	return
		FromBitmap(
			hBitmap,
			NULL,
			bEnableThrowException,
			bNoAdjustAlpha
			);
}

bool CExtBitmap::FromBitmap(
	HBITMAP hBitmap,
	LPCRECT pRectBitmapSrc, // NULL - entire bitmap
	bool bEnableThrowException,
	bool bNoAdjustAlpha
	)
{
	Empty();
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	} // if( hBitmap == NULL )
BITMAP _bmpInfo;
	::GetObject( hBitmap, sizeof(BITMAP), &_bmpInfo );
bool bCustomRect = false;
CRect rcSrc( 0, 0, _bmpInfo.bmWidth, _bmpInfo.bmHeight );
	if( pRectBitmapSrc != NULL )
	{
		if(		pRectBitmapSrc->left < rcSrc.left
			||	pRectBitmapSrc->top < rcSrc.top
			||	pRectBitmapSrc->right > rcSrc.right
			||	pRectBitmapSrc->bottom > rcSrc.bottom
			)
		{
			ASSERT( FALSE );
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		}
		if(		pRectBitmapSrc->left != rcSrc.left
			||	pRectBitmapSrc->top != rcSrc.top
			||	pRectBitmapSrc->right != rcSrc.right
			||	pRectBitmapSrc->bottom != rcSrc.bottom
			)
		{
			rcSrc = (*pRectBitmapSrc);
			bCustomRect = true;
		}
	} // if( pRectBitmapSrc != NULL )
	if( bCustomRect )
	{
		CExtBitmap other;
		if( ! other.FromBitmap( hBitmap, bEnableThrowException, bNoAdjustAlpha ) )
			return false;
		other.Make32();
		m_pImageBuffer =
			stat_AllocHL(
				rcSrc.Width(),
				rcSrc.Height(),
				32, //_bmpInfo.bmBitsPixel,
// 				0,
// 				0,
// 				0,
// 				0
				0x00FF0000,
				0x0000FF00,
				0x000000FF,
				0xFF000000,
				0 // run-time flags
				);
		if( m_pImageBuffer == NULL )
		{
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		} // if( m_pImageBuffer == NULL )
		INT xDst, yDst, xSrc, ySrc = rcSrc.top, dx = rcSrc.Width(), dy = rcSrc.Height();
		for( yDst = 0; yDst < dy; yDst ++, ySrc ++ )
		{
			xSrc = rcSrc.left;
			for( xDst = 0; xDst < dx; xDst ++, xSrc ++ )
			{
				RGBQUAD _pixel;
				VERIFY( other.GetPixel( xSrc, ySrc, _pixel ) );
//_pixel.rgbReserved = 0x80;
				VERIFY( SetPixel( xDst, yDst, _pixel ) );
			} // for( xDst = 0; xDst < dx; xDst ++, xSrc ++ )
		} // for( yDst = 0; yDst < dy; yDst ++, ySrc ++ )
		OnLoad();
		return true;
	} // if( bCustomRect )
	else
	{
		if( _bmpInfo.bmBitsPixel == 32 )
			m_pImageBuffer =
				stat_AllocHL(
					_bmpInfo.bmWidth,
					_bmpInfo.bmHeight,
					32,
					0x00FF0000,
					0x0000FF00,
					0x000000FF,
					0xFF000000,
					0 // run-time flags
					);
		else
			m_pImageBuffer =
				stat_AllocHL(
					_bmpInfo.bmWidth,
					_bmpInfo.bmHeight,
					_bmpInfo.bmBitsPixel,
					0,
					0,
					0,
					0,
					0 // run-time flags
					);
		if( m_pImageBuffer == NULL )
		{
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		} // if( m_pImageBuffer == NULL )
		LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( m_pImageBuffer );
		HDC hDC = ::GetDC( NULL );
		int nGetDIBitsResult =
			::GetDIBits(
				hDC,
				hBitmap,
				0,
				pBIH->biHeight, 
				stat_RcSurface( m_pImageBuffer ),
				stat_RcInfo( m_pImageBuffer ),
				DIB_RGB_COLORS
				);
		::ReleaseDC( NULL, hDC );
		if( nGetDIBitsResult == 0 )
		{
			Empty();
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		} // if( nGetDIBitsResult == 0 )
		if( _bmpInfo.bmBitsPixel == 32 )
		{
			CSize _sizeBitmap = GetSize();
			ULONG nScanLineSize = stat_RcScanLineSize( _sizeBitmap.cx, 32 );
			LPBYTE p32bitColorSurface = stat_RcSurface( m_pImageBuffer ); 
			for( INT nY = 0; nY < _sizeBitmap.cy; nY++ )
			{
				for( INT nX = 0; nX < _sizeBitmap.cx; nX++ )
				{
					LPLONG ptrLinePart = (LPLONG)p32bitColorSurface + nX;
					if( ! bNoAdjustAlpha )
						(*ptrLinePart) |= 0xFF000000;
				} // for( INT nX = 0; nX < _sizeBitmap.cx; nX++ )
				p32bitColorSurface += nScanLineSize;
			} // for( INT nY = 0; nY < _sizeBitmap.cy; nY++ )
		} // if( _bmpInfo.bmBitsPixel == 32 )
		OnLoad();
		return true;
	} // else from if( bCustomRect )
}

bool CExtBitmap::FromBitmap(
	const CExtBitmap & _other,
	LPCRECT pRectBitmapSrc, // = NULL - entire bitmap
	bool bEnableThrowException // = false
	)
{
	Empty();
	if( _other.IsEmpty() )
	{
		ASSERT( FALSE );
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
CSize _sizeOther = _other.GetSize();
bool bCustomRect = false;
CRect rcSrc( 0, 0, _sizeOther.cx, _sizeOther.cy );
	if( pRectBitmapSrc != NULL )
	{
		if(		pRectBitmapSrc->left < rcSrc.left
			||	pRectBitmapSrc->top < rcSrc.top
			||	pRectBitmapSrc->right > rcSrc.right
			||	pRectBitmapSrc->bottom > rcSrc.bottom
			)
		{
			ASSERT( FALSE );
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		}
		if(		pRectBitmapSrc->left != rcSrc.left
			||	pRectBitmapSrc->top != rcSrc.top
			||	pRectBitmapSrc->right != rcSrc.right
			||	pRectBitmapSrc->bottom != rcSrc.bottom
			)
		{
			rcSrc = (*pRectBitmapSrc);
			bCustomRect = true;
		}
	} // if( pRectBitmapSrc != NULL )
	if( bCustomRect )
	{
		CExtBitmap other;
		if( ! other.AssignFromOther( _other, bEnableThrowException ) )
			return false;
		other.Make32();
		m_pImageBuffer =
			stat_AllocHL(
				rcSrc.Width(),
				rcSrc.Height(),
				32, //_bmpInfo.bmBitsPixel,
// 				0,
// 				0,
// 				0,
// 				0
				0x00FF0000,
				0x0000FF00,
				0x000000FF,
				0xFF000000,
				0 // run-time flags
				);
		if( m_pImageBuffer == NULL )
		{
			if( bEnableThrowException )
				::AfxThrowUserException();
			return false;
		} // if( m_pImageBuffer == NULL )
		INT xDst, yDst, xSrc, ySrc = rcSrc.top, dx = rcSrc.Width(), dy = rcSrc.Height();
		for( yDst = 0; yDst < dy; yDst ++, ySrc ++ )
		{
			xSrc = rcSrc.left;
			for( xDst = 0; xDst < dx; xDst ++, xSrc ++ )
			{
				RGBQUAD _pixel;
				VERIFY( other.GetPixel( xSrc, ySrc, _pixel ) );
//_pixel.rgbReserved = 0x80;
				VERIFY( SetPixel( xDst, yDst, _pixel ) );
			} // for( xDst = 0; xDst < dx; xDst ++, xSrc ++ )
		} // for( yDst = 0; yDst < dy; yDst ++, ySrc ++ )
		OnLoad();
		return true;
	} // if( bCustomRect )
	else
	{
		if( ! AssignFromOther( _other, bEnableThrowException ) )
			return false;
		return true;
	} // else from if( bCustomRect )
}

bool CExtBitmap::ExtractEditableBitmap(
	HBITMAP & hBitmap,
	COLORREF & clrTransparent,
	CArray <COLORREF,COLORREF> * pArr, // = NULL // recommended palette, item 0 is clrTransparent
	COLORREF ** ppClrSurface // = NULL // if not NULL - force 32 bit
	) const
{
	if( IsEmpty() )
		return false;
static COLORREF arrTransparentColors[] =
{ RGB(255,0,255), RGB(0,255,0), RGB(192,192,192), };

CArray <COLORREF,COLORREF> _arr;
	GetBitmapColorArray( _arr );
	ASSERT( _arr.GetSize() >= 1 );
COLORREF clrFoundTransparent = ((COLORREF)(-1L));
	for( INT i = 0; i < sizeof(arrTransparentColors)/(sizeof(arrTransparentColors[0])); i++ )
	{
		COLORREF clrAnalyze = arrTransparentColors[i];
		bool bAbandone = false;
		for( int j = 0; j < _arr.GetSize(); j++ )
		{
			COLORREF clrPal = _arr[j];
			if( clrAnalyze == clrPal )
			{
				bAbandone = true;
				break;
			} // if( clrAnalyze == clrPal )
		} // for( int j = 0; j < _arr.GetSize(); j++ )
		if( bAbandone )
			continue;
		clrFoundTransparent = clrAnalyze;
		break;
	} // for( int i = 0; i < sizeof(arrTransparentColors)/(sizeof(arrTransparentColors[0])); i++ )
	if( clrFoundTransparent == ((COLORREF)(-1L)) )
	{
		// do not use black/white colors as transparent
		for( DWORD k = 0x00000001; k < 0x00FFFFFE; k++ )
		{
			COLORREF clrAnalyze = ((COLORREF)(k));
			bool bAbandone = false;
			for( int j = 0; j < _arr.GetSize(); j++ )
			{
				COLORREF clrPal = _arr[j];
				if( clrAnalyze == clrPal )
				{
					bAbandone = true;
					break;
				} // if( clrAnalyze == clrPal )
			} // for( int j = 0; j < _arr.GetSize(); j++ )
			if( bAbandone )
				continue;
			clrFoundTransparent = clrAnalyze;
			break;
		} // for( DWORD k = 0x00000001; k < 0x00FFFFFE; k++ )
	} // if( clrFoundTransparent == ((COLORREF)(-1L)) )
	if( clrFoundTransparent == ((COLORREF)(-1L)) )
	{
		ASSERT( false );
		return false;
	}
	hBitmap = CreateBitmap( ppClrSurface, clrFoundTransparent );
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
/*	if( ppClrSurface != NULL )
	{
		COLORREF * pSurf = (*ppClrSurface);
		ASSERT( pSurf != NULL );
		COLORREF clrActualTransparent = m_BitmapCache.GetTransparentColor();
		if( clrActualTransparent != clrFoundTransparent )
		{
			CSize _sizeIcon = GetSize();
			ASSERT( _sizeIcon.cx > 0 && _sizeIcon.cy > 0 );
			LONG nIconPixelCount = _sizeIcon.cx * _sizeIcon.cy;
			COLORREF clrQuadSearch =
				RGB(
					GetBValue( clrActualTransparent ),
					GetGValue( clrActualTransparent ),
					GetRValue( clrActualTransparent )
					);
			COLORREF clrQuadReplace =
				RGB(
					GetBValue( clrFoundTransparent ),
					GetGValue( clrFoundTransparent ),
					GetRValue( clrFoundTransparent )
					);
			for( LONG nPixel = 0; nPixel < nIconPixelCount ; nPixel++, pSurf++ )
			{
				if( (*pSurf) == clrQuadSearch )
					(*pSurf) = clrQuadReplace;
			} // for( LONG nPixel = 0; nPixel < nIconPixelCount ; nPixel++, pSurf++ )
		} // if( clrActualTransparent != clrFoundTransparent )
	} // if( ppClrSurface != NULL )
*/
	clrTransparent = clrFoundTransparent;
	if( pArr != NULL )
	{
		pArr->RemoveAll();
		pArr->Append( _arr );
		if( pArr->GetAt(0) != clrTransparent )
		{
			pArr->InsertAt( 0, clrTransparent, 1 );
			for( INT i = 1; i < pArr->GetSize(); i++ )
			{
				if( pArr->GetAt( i ) == clrTransparent )
				{
					pArr->RemoveAt( i, 1 );
					break;
				} // if( pArr->GetAt(i) == clrTransparent )
			} // for( int i = 1; i < pArr->GetSize(); i++ )
		} // if( pArr->GetAt(0) != clrTransparent )
	} // if( pArr != NULL )
	return true;
}

bool CExtBitmap::CreateRotated9xStack(
	const CExtBitmap & bmpSrcStack,
	int nAngleCw90, // 0, 90, 180, 270, 360 clockwise
	int nStackItemCount, // >= 1
	int nSrcItemWidth,
	int nSrcItemHeight,
	bool bSrcStackIsHorizontal,
	bool bDstStackIsHorizontal
	)
{
	if(		bmpSrcStack.IsEmpty()
		||	bmpSrcStack.GetBPP() < 15
		||	nStackItemCount < 1
		||	nSrcItemWidth < 1
		||	nSrcItemHeight < 1
		||	(! (
					nAngleCw90 ==   0
				||	nAngleCw90 ==  90
				||	nAngleCw90 == 180
				||	nAngleCw90 == 270
				||	nAngleCw90 == 360
				) )
		)
		return false;
	Empty();
int nDstItemWidth  = ( nAngleCw90 == 90 || nAngleCw90 == 270 ) ? nSrcItemHeight : nSrcItemWidth;
int nDstItemHeight = ( nAngleCw90 == 90 || nAngleCw90 == 270 ) ? nSrcItemWidth : nSrcItemHeight;
ULONG nBPP = bmpSrcStack.GetBPP();
	m_pImageBuffer =
		stat_AllocHL(
			nDstItemWidth * ( bDstStackIsHorizontal ? nStackItemCount : 1 ),
			nDstItemHeight * ( bDstStackIsHorizontal ? 1 : nStackItemCount ),
			nBPP,
			stat_MaskRed( LPBYTE( bmpSrcStack.m_pImageBuffer ) ),
			stat_MaskGreen( LPBYTE( bmpSrcStack.m_pImageBuffer ) ),
			stat_MaskBlue( LPBYTE( bmpSrcStack.m_pImageBuffer ) ),
			stat_MaskAlpha( LPBYTE( bmpSrcStack.m_pImageBuffer ) ),
			stat_RunTimeFlagsGet( LPBYTE( bmpSrcStack.m_pImageBuffer ) )
			);
	if( m_pImageBuffer == NULL )
		return false;
CRect rcSrc( 0, 0, nSrcItemWidth, nSrcItemHeight ), rcDst( 0, 0, nDstItemWidth, nDstItemHeight );
int nStackItemIndex, nX, nY;
	for( nStackItemIndex = 0; nStackItemIndex < nStackItemCount; nStackItemIndex++ )
	{
		for( nY = 0; nY < nSrcItemHeight; nY++ )
		{
			for( nX = 0; nX < nSrcItemWidth; nX++ )
			{
				RGBQUAD _pixel;
				VERIFY(
					bmpSrcStack.GetPixel(
						nX + rcSrc.left,
						nY + rcSrc.top,
						_pixel
						)
					);
				switch( nAngleCw90 )
				{
				case 0:
				case 360:
				{
					VERIFY(
						SetPixel(
							nX + rcDst.left,
							nY + rcDst.top,
							_pixel
							)
						);
				}
				break;
				case 90:
				{
					VERIFY(
						SetPixel(
							nDstItemWidth - 1 - nY + rcDst.left,
							nX + rcDst.top,
							_pixel
							)
						);
				}
				break;
				case 180:
				{
					VERIFY(
						SetPixel(
							nDstItemWidth - 1 - nX + rcDst.left,
							nDstItemHeight - 1 - nY + rcDst.top,
							_pixel
							)
						);
				}
				break;
				case 270:
				{
					VERIFY(
						SetPixel(
							nY + rcDst.left,
							nDstItemHeight - 1 - nX + rcDst.top,
							_pixel
							)
						);
				}
				break;
				} // switch( nAngleCw90 )
			} // for( nX = 0; nX < nSrcItemWidth; nX++ )
		} // for( nY = 0; nY < nSrcItemHeight; nY++ )
		rcSrc.OffsetRect(
			bSrcStackIsHorizontal ? nSrcItemWidth : 0,
			bSrcStackIsHorizontal ? 0: nSrcItemHeight
			);
		rcDst.OffsetRect(
			bDstStackIsHorizontal ? nDstItemWidth : 0,
			bDstStackIsHorizontal ? 0: nDstItemHeight
			);
	}
	return true;
}

HICON CExtBitmap::CreateHICON(
	bool bReplaceSysColors, // = true
	COLORREF clrTransparent // = COLORREF(-1L)
	) const
{
	if( IsEmpty() )
		return NULL;
CSize _sizeBitmap = GetSize();
CRect rcClone( 0, 0, _sizeBitmap.cx, _sizeBitmap.cy );
HBITMAP hBmpSrc = CreateBitmap( false );
	if( hBmpSrc == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
CBitmap _bmpDestructor;
	_bmpDestructor.Attach( hBmpSrc );
BITMAP bmpDescription;
	::memset( &bmpDescription, 0, sizeof(BITMAP) );
	::GetObject( hBmpSrc, sizeof(BITMAP), &bmpDescription );
	ASSERT( bmpDescription.bmWidth > 0 && bmpDescription.bmHeight > 0 );
CImageList _il;
	if( !_il.Create(
			bmpDescription.bmWidth,
			bmpDescription.bmHeight,
			ILC_COLOR32|ILC_MASK,
			0,
			1
			)
		)
	{
//		ASSERT( FALSE );
		return NULL;
	}
	if( clrTransparent == COLORREF(-1L) )
		clrTransparent = RGB(255,0,255);
COLORREF * pSurface = NULL;
HBITMAP hBmpExtracted = stat_CloneBitmap( hBmpSrc, &rcClone, &pSurface );
	if( hBmpExtracted == NULL || pSurface == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
CBitmap bmpExtracted;
	bmpExtracted.Attach( hBmpExtracted );
	if( bReplaceSysColors )
	{
		COLORREF quadClrTransparent =
			RGB(
				GetBValue(clrTransparent),
				GetGValue(clrTransparent),
				GetRValue(clrTransparent)
				);
		struct
		{
			COLORREF m_quadClrSrc;
			COLORREF m_quadClrDst;
			INT m_nDstSysColorIdx;
			bool m_bReplace;
		} _repl_arr[] =
		{
			{ RGB(128,128,128), RGB(0,0,0), COLOR_3DSHADOW, true },
			{ RGB(192,192,192), RGB(0,0,0), COLOR_3DFACE, true },
			{ RGB(223,223,223), RGB(0,0,0), COLOR_3DLIGHT, true },
		};
		const int nReplCount =
			sizeof(_repl_arr) / sizeof(_repl_arr[0]);
		int nReplReal = 0;
		for( int n = 0; n < nReplCount; n++ )
		{
			if( _repl_arr[n].m_quadClrSrc == quadClrTransparent )
			{
				_repl_arr[n].m_bReplace = false;
				continue;
			}
			_repl_arr[n].m_quadClrDst =
				::GetSysColor( _repl_arr[n].m_nDstSysColorIdx );
			_repl_arr[n].m_quadClrDst =
				RGB(
					GetBValue(_repl_arr[n].m_quadClrDst),
					GetGValue(_repl_arr[n].m_quadClrDst),
					GetRValue(_repl_arr[n].m_quadClrDst)
					);
			if( _repl_arr[n].m_quadClrDst == _repl_arr[n].m_quadClrSrc )
			{
				_repl_arr[n].m_bReplace = false;
				continue;
			}
			nReplReal++;
		} // for( int n = 0; n < nReplCount; n++ )
		if( nReplReal > 0 )
		{
			LONG nPixelCount =
				bmpDescription.bmWidth * bmpDescription.bmHeight;
			for( LONG nPos = 0; nPos < nPixelCount; nPos++ )
			{
				if( pSurface[nPos] == quadClrTransparent )
					continue;
				for( int n = 0; n < nReplCount; n++ )
				{
					if( ! (_repl_arr[n].m_bReplace) )
						continue;
					if( pSurface[nPos] != _repl_arr[n].m_quadClrSrc )
						continue;
					pSurface[nPos] = _repl_arr[n].m_quadClrDst;
				} // for( int n = 0; n < nReplCount; n++ )
			} // for( LONG nPos = 0; nPos < nPixelCount; nPos++ )
		} // if( nReplReal > 0 )
	} // if( bReplaceSysColors )

	// first method creates icon indirect
	// (this way is needed for older Windows OS versions)
	if( g_PaintManager.m_bIsWin9x || (!g_PaintManager.m_bIsWin2000orLater) )
	{
		ICONINFO ii;
		::memset( &ii, 0, sizeof(ICONINFO) );
		ii.fIcon = TRUE;
		ii.xHotspot = bmpDescription.bmWidth;
		ii.yHotspot = bmpDescription.bmHeight;
		ii.hbmColor = (HBITMAP)bmpExtracted.GetSafeHandle();
		ii.hbmMask =
			::CreateBitmap(
				bmpDescription.bmWidth,
				bmpDescription.bmHeight,
				1,
				1,
				NULL
				);
		ASSERT( ii.hbmMask != NULL );
		if( ii.hbmMask != NULL )
		{
			HICON hIcon = NULL;
			CDC dcMask, dcColor;
			if(		dcMask.CreateCompatibleDC( NULL )
				&&	dcColor.CreateCompatibleDC( NULL )
				)
			{
				HGDIOBJ hBmpMaskOld =
					::SelectObject( dcMask.m_hDC, ii.hbmMask );
				HGDIOBJ hBmpColorOld =
					::SelectObject( dcColor.m_hDC, ii.hbmColor );
				HBRUSH hStockWhiteBrush = (HBRUSH)
					::GetStockObject( WHITE_BRUSH );
				CRect rcSurface(
					0,
					0,
					bmpDescription.bmWidth,
					bmpDescription.bmHeight
					);
				::FillRect( dcMask.m_hDC, &rcSurface, hStockWhiteBrush );
				INT nX, nY;
				for( nX = 0; nX < bmpDescription.bmWidth; nX++ )
				{
					for( nY = 0; nY < bmpDescription.bmHeight; nY++ )
					{
						COLORREF clr = dcColor.GetPixel( nX, nY );
						if( clr != clrTransparent )
							dcMask.SetPixel( nX, nY, RGB(0,0,0) );
						else
							dcColor.SetPixel( nX, nY, RGB(0,0,0) );
					} // for( nY = 0; nY < bmpDescription.bmHeight; nY++ )
				} // for( nX = 0; nX < bmpDescription.bmWidth; nX++ )
				::SelectObject( dcMask.m_hDC, hBmpMaskOld );
				::SelectObject( dcColor.m_hDC, hBmpColorOld );
				hIcon = ::CreateIconIndirect( &ii );
				ASSERT( hIcon != NULL );
			} // if( dcMask.CreateCompatibleDC( NULL ) ...
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			} // else from if( dcMask.CreateCompatibleDC( NULL ) )
#endif // _DEBUG
			::DeleteObject( ii.hbmMask );
			if( hIcon != NULL )
				return hIcon;
		} // if( ii.hbmMask != NULL )
	} // if( g_PaintManager.m_bIsWin9x || (!g_PaintManager.m_bIsWin2000orLater) )

	// second method creates icon via image list
	// (this way is better for Windows 2000 or higher NT OS branch)
int nImgIdx =
		_il.Add( &bmpExtracted, clrTransparent );
	if( nImgIdx < 0 )
	{
//		ASSERT( FALSE );
		return NULL;
	}
HICON hIcon = _il.ExtractIcon( nImgIdx );
//	ASSERT( hIcon != NULL );
	return hIcon;
}

HBITMAP CExtBitmap::CreateBitmap( 
	HDC hDC,
	bool bCreateDDB // = true
	) const
{
	if( IsEmpty() )
		return NULL;
	
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}

HBITMAP hBitmap = NULL;

	if( bCreateDDB )
	{
		hBitmap =
			::CreateDIBitmap(
				hDC,
				stat_RcInfoHeader( m_pImageBuffer ),
				CBM_INIT,
				stat_RcSurface( m_pImageBuffer ),
				stat_RcInfo( m_pImageBuffer ),
				DIB_RGB_COLORS
				);
	}
	else
	{
		HDC hDCSrc = ::CreateCompatibleDC( hDC );
		if( hDCSrc != NULL )
		{
			CDC _dcDestructor;
			_dcDestructor.Attach( hDCSrc );
			LPBITMAPINFOHEADER pBIH = 
				CExtBitmap::stat_RcInfoHeader( m_pImageBuffer );
			DWORD biWidth = pBIH->biWidth;
			DWORD biHeight = pBIH->biHeight;
			WORD biPlanes = 1; //pBIH->biPlanes;
			WORD biBitCount = 32; //pBIH->biBitCount;
			DWORD biCompression = BI_RGB; // pBIH->biCompression;
			DWORD biClrUsed = pBIH->biClrUsed;
			DWORD biClrImportant = pBIH->biClrImportant;
			BITMAPINFOHEADER bih;
			::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
			bih.biSize = sizeof( BITMAPINFOHEADER );
			bih.biWidth = biWidth;
			bih.biHeight = biHeight;
			bih.biPlanes = biPlanes;
			bih.biBitCount = biBitCount;
			bih.biCompression = biCompression;
			bih.biSizeImage = biWidth * biHeight;
			bih.biClrUsed = biClrUsed;
			bih.biClrImportant = biClrImportant;
			COLORREF * pClrSurface = NULL;
			hBitmap =
				::CreateDIBSection(
					hDCSrc,
					(LPBITMAPINFO)&bih,
					DIB_RGB_COLORS,
					(void **)&pClrSurface,
					NULL,
					NULL
					);
			if(		hBitmap != NULL 
				&&	pClrSurface != NULL 
				)
			{
				HGDIOBJ hBitmapOld = ::SelectObject( hDCSrc, (HGDIOBJ)hBitmap );
				Draw(
					hDCSrc,
					0,0,
					biWidth,
					biHeight,
					SRCCOPY
					);
				::SelectObject( hDCSrc, (HGDIOBJ)hBitmapOld );
			}
		} // if( hDCSrc != NULL )	
	}
	return hBitmap;
}

HBITMAP CExtBitmap::CreateBitmap(
	bool bCreateDDB // = true
	) const
{
	if( IsEmpty() )
		return NULL;
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
		return NULL;
CDC dc;
	dc.Attach( hDC );
	return CreateBitmap( dc.m_hDC, bCreateDDB );
}

HBITMAP CExtBitmap::CreateBitmap( 
	HDC hDC, 
	COLORREF ** ppClrSurface,
	COLORREF clrFillBk // = COLORREF(-1L)
	) const
{
	if( ppClrSurface != NULL )
		(*ppClrSurface) = NULL;
	if( IsEmpty() )
		return NULL;
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
HDC hDCSrc = ::CreateCompatibleDC( hDC );
	if( hDCSrc == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
CDC _dcDestructor;
	_dcDestructor.Attach( hDCSrc );
LPBITMAPINFOHEADER pBIH = 
		CExtBitmap::stat_RcInfoHeader( m_pImageBuffer );
DWORD biWidth = pBIH->biWidth;
DWORD biHeight = pBIH->biHeight;
WORD biPlanes = 1;
WORD biBitCount = 32;
DWORD biCompression = BI_RGB;
DWORD biClrUsed = pBIH->biClrUsed;
DWORD biClrImportant = pBIH->biClrImportant;
BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = biWidth;
	bih.biHeight = biHeight;
	bih.biPlanes = biPlanes;
	bih.biBitCount = biBitCount;
	bih.biCompression = biCompression;
	bih.biSizeImage = biWidth * biHeight;
	bih.biClrUsed = biClrUsed;
	bih.biClrImportant = biClrImportant;
COLORREF * pClrSurface = NULL;
HBITMAP hBitmap =
		::CreateDIBSection(
			hDCSrc,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)&pClrSurface,
			NULL,
			NULL
			);
	if( hBitmap != NULL && pClrSurface != NULL )
	{
		HGDIOBJ hBitmapOld = ::SelectObject( hDCSrc, (HGDIOBJ)hBitmap );
		if( clrFillBk != COLORREF(-1L) )
		{
			COLORREF clrBkOld = ::SetBkColor( hDCSrc, clrFillBk );
			CRect rc( 0, 0, biWidth, biHeight );
			::ExtTextOut( hDCSrc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
			::SetBkColor( hDCSrc, clrBkOld );
		} // if( clrFillBk != COLORREF(-1L) )
		AlphaBlend(
			hDCSrc,
			0,
			0,
			biWidth,
			biHeight
			);
		::SelectObject( hDCSrc, (HGDIOBJ)hBitmapOld );
		if( ppClrSurface != NULL )
			(*ppClrSurface) = pClrSurface;
	} // if( hBitmap != NULL && pClrSurface != NULL )
	return hBitmap;
}

HBITMAP CExtBitmap::CreateBitmap(
	COLORREF ** ppClrSurface,
	COLORREF clrFillBk // = COLORREF(-1L)
	) const
{
	if( IsEmpty() )
		return NULL;
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
		return NULL;
CDC dc;
	dc.Attach( hDC );
	return CreateBitmap( dc.m_hDC, ppClrSurface, clrFillBk );
}

bool CExtBitmap::g_bIsAutoPreMultiplyRGBChannels = true;

bool CExtBitmap::IsAutoPreMultiplyRGBChannels() const
{
	return g_bIsAutoPreMultiplyRGBChannels;
}

void CExtBitmap::PreMultiplyRGBChannels(
	bool bForward // = true
	)
{
	if( IsEmpty() )
		return;
ULONG nBPP = GetBPP();
	if( nBPP != 32 )
		return;
CSize _size = GetSize();
LONG nX, nY;
	for( nY = 0; nY < _size.cy; nY++ )
	{
		LPBYTE pScanLine = stat_RcSurface(m_pImageBuffer) + _size.cx * 4 * nY;
		for( nX = 0; nX < _size.cx; nX++ )
		{
			int nAlpha = pScanLine[3];
			if( bForward )
			{
				pScanLine[0] = BYTE( pScanLine[0] * nAlpha / 255 );
				pScanLine[1] = BYTE( pScanLine[1] * nAlpha / 255 );
				pScanLine[2] = BYTE( pScanLine[2] * nAlpha / 255 );
			}
			else
			{
				if( nAlpha != 0 )
				{
					//pScanLine[0] = BYTE( pScanLine[0] * 255 / nAlpha  );
					//pScanLine[1] = BYTE( pScanLine[1] * 255 / nAlpha  );
					//pScanLine[2] = BYTE( pScanLine[2] * 255 / nAlpha );
					INT xArr[3] =
					{
						INT( pScanLine[0] * 255 / nAlpha  ),
						INT( pScanLine[1] * 255 / nAlpha  ),
						INT( pScanLine[2] * 255 / nAlpha ),
					};
					for( INT k = 0; k < 3; k++ )
						pScanLine[ k ] = BYTE( min( 255, xArr[ k ] ) );
				}
			}
			pScanLine += 4;
		} // for( nX = 0; nX < _size.cx; nX++ )
	} // for( nY = 0; nY < _size.cy; nY++ )
	OnContentChanged();
}

bool CExtBitmap::PreMultipliedRGBChannelsGet() const
{
	if( IsEmpty() )
		return false;
	return stat_PreMultipliedRGBChannelsGet( LPBYTE(m_pImageBuffer) );
}

void CExtBitmap::PreMultipliedRGBChannelsSet( bool bPreMultiplied )
{
	if( IsEmpty() )
		return;
	stat_PreMultipliedRGBChannelsSet( LPBYTE(m_pImageBuffer), bPreMultiplied );
}

void CExtBitmap::MakeMono(
	COLORREF clr, // = COLORREF(-1L)
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return;
ULONG nBPP = GetBPP();
	if( nBPP < 15 )
		return;
CSize _size = GetSize();
CRect _rcFrame;
	if( pRect != NULL )
	{
		_rcFrame = (*pRect);
		if( _rcFrame.left < 0 )
			_rcFrame.left = 0;
		if( _rcFrame.top < 0 )
			_rcFrame.top = 0;
		if( _rcFrame.right > _size.cx )
			_rcFrame.right = _size.cx;
		if( _rcFrame.bottom > _size.cy )
			_rcFrame.bottom = _size.cy;
	}
	else
		_rcFrame.SetRect( 0, 0, _size.cx, _size.cy );
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return;
double clrH = 0.0, H = 0.0, clrS = 0.0, S = 0.0, L = 0.0;
	if( clr != COLORREF(-1L) )
		stat_RGBtoHSL( clr, &clrH, &clrS, &L );
LONG nX, nY;
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			double lfVal =
					double( _pixel.rgbBlue  ) * 0.11 // * 0.30
				+	double( _pixel.rgbGreen ) * 0.59 // * 0.50
				+	double( _pixel.rgbRed   ) * 0.30 // * 0.20
				;
			if( lfVal > 255.0 )
				lfVal = 255.0;
			_pixel.rgbBlue = _pixel.rgbGreen = _pixel.rgbRed
				= BYTE( INT( lfVal ) );
			if( clr != COLORREF(-1L) )
			{
				stat_RGBtoHSL(
					RGB( _pixel.rgbRed, _pixel.rgbGreen, _pixel.rgbBlue ),
					&H,
					&S,
					&L
					);
				COLORREF clrTarget = stat_HLStoRGB( clrH, L, clrS );
				_pixel.rgbRed   = GetRValue( clrTarget );
				_pixel.rgbGreen = GetGValue( clrTarget );
				_pixel.rgbBlue  = GetBValue( clrTarget );
			} // if( clr != COLORREF(-1L) )
			VERIFY( SetPixel( nX, nY, _pixel ) );
		} // for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
	} // for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	OnContentChanged();
}

void CExtBitmap::AlphaColor(
	COLORREF clr,
	COLORREF clrTolerance,
	BYTE nAlpha,
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return;
ULONG nBPP = GetBPP();
	if( nBPP != 32 )
		return;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CSize _size = GetSize();
CRect _rcFrame;
	if( pRect != NULL )
	{
		_rcFrame = (*pRect);
		if( _rcFrame.left < 0 )
			_rcFrame.left = 0;
		if( _rcFrame.top < 0 )
			_rcFrame.top = 0;
		if( _rcFrame.right > _size.cx )
			_rcFrame.right = _size.cx;
		if( _rcFrame.bottom > _size.cy )
			_rcFrame.bottom = _size.cy;
	}
	else
		_rcFrame.SetRect( 0, 0, _size.cx, _size.cy );
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return;
BYTE nLowRed	= GetRValue( clr );
BYTE nLowGreen	= GetGValue( clr );
BYTE nLowBlue	= GetBValue( clr );
BYTE nHighRed	= BYTE( min( 0xff, ( nLowRed + GetRValue(clrTolerance) ) ) );
BYTE nHighGreen	= BYTE( min( 0xff, ( nLowGreen + GetGValue(clrTolerance) ) ) );
BYTE nHighBlue	= BYTE( min( 0xff, ( nLowBlue + GetBValue(clrTolerance) ) ) );
LONG nX, nY;
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			if( _pixel.rgbRed >= nLowRed && _pixel.rgbRed <= nHighRed )
			{
				if( _pixel.rgbGreen >= nLowGreen && _pixel.rgbGreen <= nHighGreen )
				{
					if( _pixel.rgbBlue >= nLowBlue && _pixel.rgbBlue <= nHighBlue )
					{
						_pixel.rgbReserved = nAlpha;
						VERIFY( SetPixel( nX, nY, _pixel ) );
					}
				} // if( nColorPart >= nLowGreen && nColorPart <= nHighGreen )
			} // if( nColorPart >= nLowRed && nColorPart <= nHighRed )
		} // for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
	} // for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	OnContentChanged();
}

void CExtBitmap::AlphaColorDistance(
	COLORREF clr,
	INT nDistance,
	BYTE nAlpha,
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return;
ULONG nBPP = GetBPP();
	if( nBPP != 32 )
		return;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CSize _size = GetSize();
CRect _rcFrame;
	if( pRect != NULL )
	{
		_rcFrame = (*pRect);
		if( _rcFrame.left < 0 )
			_rcFrame.left = 0;
		if( _rcFrame.top < 0 )
			_rcFrame.top = 0;
		if( _rcFrame.right > _size.cx )
			_rcFrame.right = _size.cx;
		if( _rcFrame.bottom > _size.cy )
			_rcFrame.bottom = _size.cy;
	}
	else
		_rcFrame.SetRect( 0, 0, _size.cx, _size.cy );
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return;
INT nSrcRed = INT( GetRValue(clr) );
INT nSrcGreen = INT( GetGValue(clr) );
INT nSrcBlue = INT( GetBValue(clr) );
LONG nX, nY;
INT nDistance2 = nDistance*nDistance;
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			INT nTest2 = 
				  INT(_pixel.rgbRed)*INT(nSrcRed)
				+ INT(_pixel.rgbGreen)*INT(nSrcGreen)
				+ INT(_pixel.rgbBlue)*INT(nSrcBlue)
				;
			if( nTest2 <= nDistance2 )
			{
				_pixel.rgbReserved = nAlpha;
				VERIFY( SetPixel( nX, nY, _pixel ) );
			}
		} // for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
	} // for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	OnContentChanged();
}

void CExtBitmap::AlphaRect(
	const RECT & rcFrame,
	BYTE nAlpha,
	COLORREF clrTransparent, // = COLORREF(-1L),
	COLORREF clrOpaque // = COLORREF(-1L)
	)
{
	if( IsEmpty() )
		return;
ULONG nBPP = GetBPP();
	if( nBPP != 32 )
		return;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CRect _rcFrame = rcFrame;
	if( _rcFrame.left < 0 )
		_rcFrame.left = 0;
	if( _rcFrame.top < 0 )
		_rcFrame.top = 0;
CSize _size = GetSize();
	if( _rcFrame.right > _size.cx )
		_rcFrame.right = _size.cx;
	if( _rcFrame.bottom > _size.cy )
		_rcFrame.bottom = _size.cy;
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return;
LONG nX, nY;
RGBQUAD _pixelAlpha = { 0, 0, 0, nAlpha };
RGBQUAD _pixelTransparent = { 0, 0, 0, 0 };
RGBQUAD _pixelOpaque = { 0, 0, 0, 0xFF };
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			if(		clrTransparent != COLORREF(-1L)
				||	clrOpaque != COLORREF(-1L)
				)
			{
				COLORREF clr = GetPixel( nX, nY );
				if( clr == clrTransparent )
				{
					SetPixel( nX, nY, _pixelTransparent, false, false, false, true );
					continue;
				}
				if( clr == clrOpaque )
				{
					SetPixel( nX, nY, _pixelOpaque, false, false, false, true );
					continue;
				}
			}
			SetPixel( nX, nY, _pixelAlpha, false, false, false, true );
		} // for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
	} // for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	OnContentChanged();
}

void CExtBitmap::AlphaFrame(
	const RECT & rcFrame,
	const RECT & rcPadding,
	BYTE nAlpha,
	COLORREF clrTransparent, // = COLORREF(-1L),
	COLORREF clrOpaque // = COLORREF(-1L)
	)
{
	if(	IsEmpty() )
		return;
ULONG nBPP = GetBPP();
	if( nBPP != 32 )
		return;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CRect rcTop(
		rcFrame.left,
		rcFrame.top,
		rcFrame.right,
		rcFrame.top + rcPadding.top
		);
	AlphaRect(
		rcTop,
		nAlpha,
		clrTransparent,
		clrOpaque
		);
CRect rcBottom(
		rcFrame.left,
		rcFrame.bottom - rcPadding.bottom,
		rcFrame.right,
		rcFrame.bottom
		);
	AlphaRect(
		rcBottom,
		nAlpha,
		clrTransparent,
		clrOpaque
		);
CRect rcLeft(
		rcFrame.left,
		rcFrame.top + rcPadding.top,
		rcFrame.left + rcPadding.left,
		rcFrame.bottom - rcPadding.bottom
		);
	AlphaRect(
		rcLeft,
		nAlpha,
		clrTransparent,
		clrOpaque
		);
CRect rcRight(
		rcFrame.right - rcPadding.right,
		rcFrame.top + rcPadding.top,
		rcFrame.right,
		rcFrame.bottom - rcPadding.bottom
		);
	AlphaRect(
		rcRight,
		nAlpha,
		clrTransparent,
		clrOpaque
		);
}

int CExtBitmap::DrawImpl(
	HDC hDC,
	int nDstX,
	int nDstY,
	int nDstWidth,
	int nDstHeight,
	int nSrcX,
	int nSrcY,
	int nSrcWidth,
	int nSrcHeight,
	DWORD dwRop // = SRCCOPY 
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
	return
		::StretchDIBits(
			hDC,
			nDstX,
			nDstY,
			nDstWidth,
			nDstHeight,
			nSrcX,
			nSrcY,
			nSrcWidth,
			nSrcHeight,
			stat_RcSurface( m_pImageBuffer ),
			stat_RcInfo( m_pImageBuffer ),
			DIB_RGB_COLORS,
			dwRop
			);
}

int CExtBitmap::Draw(
	HDC hDC,
	int nDstX,
	int nDstY,
	int nDstWidth,
	int nDstHeight,
	int nSrcX,
	int nSrcY,
	int nSrcWidth,
	int nSrcHeight,
	DWORD dwRop // = SRCCOPY 
	) const
{
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
CSize _size = GetSize();
	::OffsetWindowOrgEx( hDC, - nDstX, - nDstY, NULL );
int nRetVal =
		DrawImpl(
			hDC,
			0,
			nDstHeight - 1,
			nDstWidth,
			- nDstHeight,
			nSrcX,
			_size.cy - nSrcY + 1,
			nSrcWidth,
			- nSrcHeight,
			dwRop
			);
	::OffsetWindowOrgEx( hDC, nDstX, nDstY, NULL );
	return nRetVal;
}

int CExtBitmap::Draw(
	HDC hDC,
	int nDstX,
	int nDstY,
	int nDstWidth,
	int nDstHeight,
	DWORD dwRop // = SRCCOPY
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
CSize _size = GetSize();
	return
		Draw(
			hDC,
			nDstX,
			nDstY,
			nDstWidth,
			nDstHeight,
			0,
			0,
			_size.cx,
			_size.cy,
			dwRop
			);
}

int CExtBitmap::Draw(
	HDC hDC,
	const RECT & rcDst,
	const RECT & rcSrc,
	DWORD dwRop // = SRCCOPY 
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
	return Draw(
		hDC,
		rcDst.left,
		rcDst.top,
		rcDst.right - rcDst.left,
		rcDst.bottom - rcDst.top,
		rcSrc.left,
		rcSrc.top,
		rcSrc.right - rcSrc.left,
		rcSrc.bottom - rcSrc.top,
		dwRop  
		);
}

int CExtBitmap::Draw(
	HDC hDC,
	const RECT & rcDst,
	DWORD dwRop // = SRCCOPY
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
CSize _size = GetSize();
CRect rcSrc( 0, 0, _size.cx, _size.cy );
	return
		Draw(
			hDC,
			rcDst,
			rcSrc,
			dwRop
			);
}

bool CExtBitmap::DrawNcFrame(
	HDC hDC,
	const RECT & rcDstDraw,
	const RECT & rcSrcDraw,
	const RECT & rcPadding,
	INT nCaptionHeightDst, // = 0
	INT nCaptionHeightSrc, // = 0
	bool bDrawMiddlePart, // = false
	bool bSmootherAsPossible, // = true
	DWORD dwRop // = SRCCOPY
	) const
{
	ASSERT( hDC != NULL );
	ASSERT( rcPadding.left >= 0 );
	ASSERT( rcPadding.right >= 0 );
	ASSERT( rcPadding.top >= 0 );
	ASSERT( rcPadding.bottom >= 0 );
	ASSERT( nCaptionHeightDst >= 0 );
	ASSERT( nCaptionHeightSrc >= 0 );
	ASSERT(
			( nCaptionHeightDst == 0 && nCaptionHeightSrc == 0 )
		||	( nCaptionHeightSrc >  0 && nCaptionHeightSrc >  0 )
		);
	if( IsEmpty() || hDC == NULL )
		return false;
int nOldStretchBltMode = ::GetStretchBltMode( hDC );
	::SetStretchBltMode(
		hDC,
		( bSmootherAsPossible && g_PaintManager.m_bIsWinNT )
			? HALFTONE
			: COLORONCOLOR
		);
	if( rcPadding.top > 0 || nCaptionHeightDst > 0 )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top + rcPadding.top + nCaptionHeightDst
			);
		CRect rcSrc(
			rcSrcDraw.left + rcPadding.left,
			rcSrcDraw.top,
			rcSrcDraw.right - rcPadding.right,
			rcSrcDraw.top + rcPadding.top + nCaptionHeightSrc
			);
		if(		rcDst.left < rcDst.right
			&&	rcDst.top < rcDst.bottom
			&&	rcSrc.left < rcSrc.right
			&&	rcSrc.top < rcSrc.bottom
			)
			Draw( hDC, rcDst, rcSrc, dwRop );
		if( rcPadding.left > 0 )
		{
			CRect rcDst(
				rcDstDraw.left,
				rcDstDraw.top,
				rcDstDraw.left + rcPadding.left,
				rcDstDraw.top + rcPadding.top
				);
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.top,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top + rcPadding.top
				);
			if(		rcDst.left < rcDst.right
				&&	rcDst.top < rcDst.bottom
				&&	rcSrc.left < rcSrc.right
				&&	rcSrc.top < rcSrc.bottom
				)
				Draw( hDC, rcDst, rcSrc, dwRop );
			if( nCaptionHeightDst > 0 )
			{
				CRect rcDst(
					rcDstDraw.left,
					rcDstDraw.top + rcPadding.top,
					rcDstDraw.left + rcPadding.left,
					rcDstDraw.top + rcPadding.top + nCaptionHeightDst
					);
				CRect rcSrc(
					rcSrcDraw.left,
					rcSrcDraw.top + rcPadding.top,
					rcSrcDraw.left + rcPadding.left,
					rcSrcDraw.top + rcPadding.top + nCaptionHeightSrc
					);
				if(		rcDst.left < rcDst.right
					&&	rcDst.top < rcDst.bottom
					&&	rcSrc.left < rcSrc.right
					&&	rcSrc.top < rcSrc.bottom
					)
					Draw( hDC, rcDst, rcSrc, dwRop );
			}
		}
		if( rcPadding.right > 0 )
		{
			CRect rcDst(
				rcDstDraw.right - rcPadding.right,
				rcDstDraw.top,
				rcDstDraw.right,
				rcDstDraw.top + rcPadding.top
				);
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top,
				rcSrcDraw.right,
				rcSrcDraw.top + rcPadding.top
				);
			if(		rcDst.left < rcDst.right
				&&	rcDst.top < rcDst.bottom
				&&	rcSrc.left < rcSrc.right
				&&	rcSrc.top < rcSrc.bottom
				)
				Draw( hDC, rcDst, rcSrc, dwRop );
			if( nCaptionHeightDst > 0 )
			{
				CRect rcDst(
					rcDstDraw.right - rcPadding.right,
					rcDstDraw.top + rcPadding.top,
					rcDstDraw.right,
					rcDstDraw.top + rcPadding.top + nCaptionHeightDst
					);
				CRect rcSrc(
					rcSrcDraw.right - rcPadding.right,
					rcSrcDraw.top + rcPadding.top,
					rcSrcDraw.right,
					rcSrcDraw.top + rcPadding.top + nCaptionHeightSrc
					);
				if(		rcDst.left < rcDst.right
					&&	rcDst.top < rcDst.bottom
					&&	rcSrc.left < rcSrc.right
					&&	rcSrc.top < rcSrc.bottom
					)
					Draw( hDC, rcDst, rcSrc, dwRop );
			}
		}
	}
	if( rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom
			);
		CRect rcSrc(
			rcSrcDraw.left + rcPadding.left,
			rcSrcDraw.bottom - rcPadding.bottom,
			rcSrcDraw.right - rcPadding.right,
			rcSrcDraw.bottom
			);
		if(		rcDst.left < rcDst.right
			&&	rcDst.top < rcDst.bottom
			&&	rcSrc.left < rcSrc.right
			&&	rcSrc.top < rcSrc.bottom
			)
			Draw( hDC, rcDst, rcSrc, dwRop );
	}
	if( rcPadding.left > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.top + rcPadding.top + nCaptionHeightDst,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom - rcPadding.bottom
			);
		CRect rcSrc(
			rcSrcDraw.left,
			rcSrcDraw.top + rcPadding.top + nCaptionHeightSrc,
			rcSrcDraw.left + rcPadding.left,
			rcSrcDraw.bottom - rcPadding.bottom
			);
		if(		rcDst.left < rcDst.right
			&&	rcDst.top < rcDst.bottom
			&&	rcSrc.left < rcSrc.right
			&&	rcSrc.top < rcSrc.bottom
			)
			Draw( hDC, rcDst, rcSrc, dwRop );
		if( rcPadding.bottom > 0 )
		{
			CRect rcDst(
				rcDstDraw.left,
				rcDstDraw.bottom - rcPadding.bottom,
				rcDstDraw.left + rcPadding.left,
				rcDstDraw.bottom
				);
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom
				);
			if(		rcDst.left < rcDst.right
				&&	rcDst.top < rcDst.bottom
				&&	rcSrc.left < rcSrc.right
				&&	rcSrc.top < rcSrc.bottom
				)
				Draw( hDC, rcDst, rcSrc, dwRop );
		}
	}
	if( rcPadding.right > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top + rcPadding.top + nCaptionHeightDst,
			rcDstDraw.right,
			rcDstDraw.bottom - rcPadding.bottom
			);
		CRect rcSrc(
			rcSrcDraw.right - rcPadding.right,
			rcSrcDraw.top + rcPadding.top + nCaptionHeightSrc,
			rcSrcDraw.right,
			rcSrcDraw.bottom - rcPadding.bottom
			);
		if(		rcDst.left < rcDst.right
			&&	rcDst.top < rcDst.bottom
			&&	rcSrc.left < rcSrc.right
			&&	rcSrc.top < rcSrc.bottom
			)
			Draw( hDC, rcDst, rcSrc, dwRop );
		if( rcPadding.bottom > 0 )
		{
			CRect rcDst(
				rcDstDraw.right - rcPadding.right,
				rcDstDraw.bottom - rcPadding.bottom,
				rcDstDraw.right,
				rcDstDraw.bottom
				);
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.right,
				rcSrcDraw.bottom
				);
			if(		rcDst.left < rcDst.right
				&&	rcDst.top < rcDst.bottom
				&&	rcSrc.left < rcSrc.right
				&&	rcSrc.top < rcSrc.bottom
				)
				Draw( hDC, rcDst, rcSrc, dwRop );
		}
	}
	if( bDrawMiddlePart )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top + rcPadding.top + nCaptionHeightDst,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom - rcPadding.bottom
			);
		CRect rcSrc(
			rcSrcDraw.left + rcPadding.left,
			rcSrcDraw.top + rcPadding.top + nCaptionHeightSrc,
			rcSrcDraw.right - rcPadding.right,
			rcSrcDraw.bottom - rcPadding.bottom
			);
		if(		rcDst.left < rcDst.right
			&&	rcDst.top < rcDst.bottom
			&&	rcSrc.left < rcSrc.right
			&&	rcSrc.top < rcSrc.bottom
			)
			Draw( hDC, rcDst, rcSrc, dwRop );
	}
	::SetStretchBltMode(
		hDC,
		nOldStretchBltMode
		);
	return true;
}

bool CExtBitmap::DrawNcFrame(
	HDC hDC,
	const RECT & rcDstDraw,
	const RECT & rcPadding,
	INT nCaptionHeightDst, // = 0
	INT nCaptionHeightSrc, // = 0
	bool bDrawMiddlePart, // = false
	bool bSmootherAsPossible, // = true
	DWORD dwRop // = SRCCOPY
	) const
{
	ASSERT( hDC != NULL );
	ASSERT( rcPadding.left >= 0 );
	ASSERT( rcPadding.right >= 0 );
	ASSERT( rcPadding.top >= 0 );
	ASSERT( rcPadding.bottom >= 0 );
	ASSERT( nCaptionHeightDst >= 0 );
	ASSERT( nCaptionHeightSrc >= 0 );
	ASSERT(
			( nCaptionHeightDst == 0 && nCaptionHeightSrc == 0 )
		||	( nCaptionHeightSrc >  0 && nCaptionHeightSrc >  0 )
		);
	if( IsEmpty() || hDC == NULL )
		return false;
CSize _size = GetSize();
CRect rcSrcDraw( 0, 0, _size.cx, _size.cy );
	return
		DrawNcFrame(
			hDC,
			rcDstDraw,
			rcSrcDraw,
			rcPadding,
			nCaptionHeightDst,
			nCaptionHeightSrc,
			bDrawMiddlePart,
			bSmootherAsPossible,
			dwRop
			);
}

bool CExtBitmap::DrawSkinParts(
	HDC hDC,
	const RECT & rcDstDraw,
	const RECT & rcSrcDraw,
	const RECT & rcPadding,
	e_DrawMethod_t eDM,
	bool bDrawMiddlePart, // = true
	bool bSmootherAsPossible, // = false
	DWORD dwRop // = SRCCOPY
	) const
{
	if( eDM == __EDM_HOLLOW )
		return true;
	ASSERT( hDC != NULL );
	ASSERT( rcPadding.left >= 0 );
	ASSERT( rcPadding.right >= 0 );
	ASSERT( rcPadding.top >= 0 );
	ASSERT( rcPadding.bottom >= 0 );
	if( IsEmpty() || hDC == NULL )
		return false;
int nOldStretchBltMode = ::GetStretchBltMode( hDC );
	::SetStretchBltMode(
		hDC,
		( bSmootherAsPossible && g_PaintManager.m_bIsWinNT )
			? HALFTONE
			: COLORONCOLOR
		);
	if( eDM == __EDM_CENTER )
	{
		//CSize _size = GetSize();
		CSize _size( rcSrcDraw.right - rcSrcDraw.left, rcSrcDraw.bottom - rcSrcDraw.top );
		CRect rcDst(
			CPoint(
				rcDstDraw.left + ( rcDstDraw.right - rcDstDraw.left - _size.cx ) / 2, 
				rcDstDraw.top + ( rcDstDraw.bottom - rcDstDraw.top - _size.cy ) / 2
				),
			_size
			);
		if( ::RectVisible( hDC, &rcDst ) )
			Draw(
				hDC,
				rcDst,
				rcSrcDraw,
				dwRop
				);
		return true;
	} // if( eDM == __EDM_CENTER )
	// left - top
	if( rcPadding.left > 0 && rcPadding.top > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.top,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top + rcPadding.top
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.top,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top + rcPadding.top
				);
			Draw(
				hDC,
				rcDst,
				rcSrc,
				dwRop
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.left > 0 && rcPadding.top > 0 )
	// middle - top
	if( rcPadding.top > 0 )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top + rcPadding.top
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top,
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top + rcPadding.top
				);
			if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstWidth % nSrcWidth;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstWidth / nSrcWidth + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.right > rcDst.right
							)
						{
							INT nDiff = rcDstStep.right - rcDst.right;
							rcDstStep.right -= nDiff;
							rcSrcA.right -= nDiff;
						}
						Draw(
							hDC,
							rcDstStep,
							rcSrcA,
							dwRop
							);
						rcDstStep.OffsetRect( nSrcWidth, 0 );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			else
				Draw(
					hDC,
					rcDst,
					rcSrc,
					dwRop
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.top > 0 )
	// right - top
	if( rcPadding.right > 0 && rcPadding.top > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top,
			rcDstDraw.right,
			rcDstDraw.top + rcPadding.top
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top,
				rcSrcDraw.right,
				rcSrcDraw.top + rcPadding.top
				);
			Draw(
				hDC,
				rcDst,
				rcSrc,
				dwRop
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.right > 0 && rcPadding.top > 0 )
	// left - middle
	if( rcPadding.left > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.top + rcPadding.top,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom - rcPadding.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.top + rcPadding.top,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom - rcPadding.bottom
				);
			if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				INT nLastStepPx = nDstHeight % nSrcHeight;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstHeight / nSrcHeight + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStep.bottom - rcDst.bottom;
							rcDstStep.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						Draw(
							hDC,
							rcDstStep,
							rcSrcA,
							dwRop
							);
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			else
				Draw(
					hDC,
					rcDst,
					rcSrc,
					dwRop
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.left > 0 )
	// right - middle
	if( rcPadding.right > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top + rcPadding.top,
			rcDstDraw.right,
			rcDstDraw.bottom - rcPadding.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top + rcPadding.top,
				rcSrcDraw.right,
				rcSrcDraw.bottom - rcPadding.bottom
				);
			if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstHeight % nSrcHeight;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstHeight / nSrcHeight + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStep.bottom - rcDst.bottom;
							rcDstStep.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						Draw(
							hDC,
							rcDstStep,
							rcSrcA,
							dwRop
							);
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			else
				Draw(
					hDC,
					rcDst,
					rcSrc,
					dwRop
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.right > 0 )
	// left - bottom
	if( rcPadding.left > 0 && rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom
				);
			Draw(
				hDC,
				rcDst,
				rcSrc,
				dwRop
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.left > 0 && rcPadding.bottom > 0 )
	// middle - bottom
	if( rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom
				);
			if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstWidth % nSrcWidth;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstWidth / nSrcWidth + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.right > rcDst.right
							)
						{
							INT nDiff = rcDstStep.right - rcDst.right;
							rcDstStep.right -= nDiff;
							rcSrcA.right -= nDiff;
						}
						Draw(
							hDC,
							rcDstStep,
							rcSrcA,
							dwRop
							);
						rcDstStep.OffsetRect( nSrcWidth, 0 );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			else
				Draw(
					hDC,
					rcDst,
					rcSrc,
					dwRop
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.bottom > 0 )
	// right - bottom
	if( rcPadding.right > 0 && rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.right,
			rcDstDraw.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.right,
				rcSrcDraw.bottom
				);
			Draw(
				hDC,
				rcDst,
				rcSrc,
				dwRop
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.right > 0 && rcPadding.bottom > 0 )
	// central part
	if( bDrawMiddlePart )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top + rcPadding.top,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom - rcPadding.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top + rcPadding.top,
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom - rcPadding.bottom
				);
			if( eDM == __EDM_TILE_H )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				INT nLastStepPx = nDstWidth % nSrcWidth;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstWidth / nSrcWidth + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nDstHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.right > rcDst.right
							)
						{
							INT nDiff = rcDstStep.right - rcDst.right;
							rcDstStep.right -= nDiff;
							rcSrcA.right -= nDiff;
						}
						Draw(
							hDC,
							rcDstStep,
							rcSrcA,
							dwRop
							);
						rcDstStep.OffsetRect( nSrcWidth, 0 );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_H )
			else if( eDM == __EDM_TILE_V )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstHeight % nSrcHeight;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstHeight / nSrcHeight + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nDstWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStep.bottom - rcDst.bottom;
							rcDstStep.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						Draw(
							hDC,
							rcDstStep,
							rcSrcA,
							dwRop
							);
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // else if( eDM == __EDM_TILE_V )
			else if( eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPxH = nDstWidth % nSrcWidth;
					INT nLastStepPxV = nDstHeight % nSrcHeight;
					INT nLastStepH = ( nLastStepPxH != 0 ) ? 1 : 0;
					INT nLastStepV = ( nLastStepPxV != 0 ) ? 1 : 0;
					INT nStepCountH = nDstWidth / nSrcWidth + nLastStepH;
					INT nStepCountV = nDstHeight / nSrcHeight + nLastStepV;
					INT nStepIndexH, nStepIndexV;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					for( nStepIndexV = 0; nStepIndexV < nStepCountV; nStepIndexV ++ )
					{
						CRect rcDstStepA = rcDstStep;
						CRect rcSrcA = rcSrc;
						if(		nLastStepV != 0
							&&	nStepIndexV == ( nStepCountV - 1 )
							&&	rcDstStepA.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStepA.bottom - rcDst.bottom;
							rcDstStepA.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						for( nStepIndexH = 0; nStepIndexH < nStepCountH; nStepIndexH ++ )
						{
							if(		nLastStepH != 0
								&&	nStepIndexH == ( nStepCountH - 1 )
								&&	rcDstStepA.right > rcDst.right
								)
							{
								INT nDiff = rcDstStepA.right - rcDst.right;
								rcDstStepA.right -= nDiff;
								rcSrcA.right -= nDiff;
							}
							Draw(
								hDC,
								rcDstStepA,
								rcSrcA,
								dwRop
								);
							rcDstStepA.OffsetRect( nSrcWidth, 0 );
						} // for( nStepIndexH = 0; nStepIndexH < nStepCountH; nStepIndexH ++ )
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndexV = 0; nStepIndexV < nStepCountV; nStepIndexV ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // else if( eDM == __EDM_TILE )
			else
				Draw(
					hDC,
					rcDst,
					rcSrc,
					dwRop
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( bDrawMiddlePart )
	::SetStretchBltMode(
		hDC,
		nOldStretchBltMode
		);
	return true;
}

bool CExtBitmap::DrawSkinParts(
	HDC hDC,
	const RECT & rcDstDraw,
	const RECT & rcPadding,
	e_DrawMethod_t eDM,
	bool bDrawMiddlePart, // = true
	bool bSmootherAsPossible, // = false
	DWORD dwRop // = SRCCOPY
	) const
{
	ASSERT( hDC != NULL );
	ASSERT( rcPadding.left >= 0 );
	ASSERT( rcPadding.right >= 0 );
	ASSERT( rcPadding.top >= 0 );
	ASSERT( rcPadding.bottom >= 0 );
	if( IsEmpty() || hDC == NULL )
		return false;
CSize _size = GetSize();
RECT rcSrcDraw = { 0, 0, _size.cx, _size.cy };
	return
		DrawSkinParts(
			hDC,
			rcDstDraw,
			rcSrcDraw,
			rcPadding,
			eDM,
			bDrawMiddlePart,
			bSmootherAsPossible,
			dwRop
			);
}

int CExtBitmap::AlphaBlend(
	HDC hDC,
	const RECT & rcDst,
	const RECT & rcSrc,
	BYTE nSCA // = 0xFF 
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
	return
		AlphaBlend(
			hDC,
			rcDst.left,
			rcDst.top,
			rcDst.right - rcDst.left,
			rcDst.bottom - rcDst.top,
			rcSrc.left,
			rcSrc.top,
			rcSrc.right - rcSrc.left,
			rcSrc.bottom - rcSrc.top,
			nSCA
			);
}

int CExtBitmap::AlphaBlend(
	HDC hDC,
	const RECT & rcDst,
	BYTE nSCA // = 0xFF
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;
CSize _size = GetSize();
CRect rcSrc( 0, 0, _size.cx, _size.cy );
	return
		AlphaBlend(
			hDC,
			rcDst,
			rcSrc,
			nSCA
			);
}

int CExtBitmap::AlphaBlend(
	HDC hDC,
	int nDstX,
	int nDstY,
	int nDstWidth,
	int nDstHeight,
	int nSrcX,
	int nSrcY,
	int nSrcWidth,
	int nSrcHeight,
	BYTE nSCA // = 0xFF
	) const
{
	ASSERT( hDC != NULL );
	if(		IsEmpty()
		||	hDC == NULL
		)
		return GDI_ERROR;
ULONG nBPP = GetBPP(), nMaskAlpha = stat_MaskAlpha( m_pImageBuffer );
	if(		nBPP != 32
		||	nMaskAlpha == 0
		)
	{
		CSize _size = GetSize();
		return
			Draw(
				hDC,
				nDstX,
				nDstY,
				nDstWidth,
				nDstHeight,
				nSrcX,
				nSrcY,
				nSrcWidth,
				nSrcHeight
				);
	}

//	if( g_PaintManager.m_bIsWin2000orLater )
//	{
//		int nDeviceShadeBlendCaps = ::GetDeviceCaps( hDC, __EXT_MFC_SHADEBLENDCAPS );
//		const int nRequiredCaps = __EXT_MFC_SB_PIXEL_ALPHA|__EXT_MFC_SB_CONST_ALPHA;
//		if( (nDeviceShadeBlendCaps&nRequiredCaps) != nRequiredCaps )
//		{
//			CRect rcSurfaceCache( nDstX, nDstY, nDstX + nDstWidth, nDstY + nDstHeight );
//			CDC * pDC = CDC::FromHandle( hDC );
//			CExtMemoryDC dc(
//				pDC,
//				&rcSurfaceCache,
//				CExtMemoryDC::MDCOPT_DEFAULT
//					| CExtMemoryDC::MDCOPT_DESKTOP_COMPATIBILITY
//					| CExtMemoryDC::MDCOPT_FORCE_DIB
//				);
//			if( dc.GetSafeHdc() == NULL )
//				return GDI_ERROR;
//			ASSERT( ! dc.IsPrinting() );
//#if (defined _DEBUG)
//			int nDebugCheckShadeBlendCaps = ::GetDeviceCaps( dc.m_hDC, __EXT_MFC_SHADEBLENDCAPS );
//			ASSERT( (nDebugCheckShadeBlendCaps&nRequiredCaps) == nRequiredCaps );
//#endif // (defined _DEBUG)
//			return
//				AlphaBlend(
//					dc.GetSafeHdc(),
//					nDstX,
//					nDstY,
//					nDstWidth,
//					nDstHeight,
//					nSrcX,
//					nSrcY,
//					nSrcWidth,
//					nSrcHeight,
//					nSCA
//					);
//		} // if( (nDeviceShadeBlendCaps&nRequiredCaps) != nRequiredCaps )
//	} // if( g_PaintManager.m_bIsWin2000orLater )

bool bAutoPreMultiplyEnabled = IsAutoPreMultiplyRGBChannels();
bool bPreMultiplied = PreMultipliedRGBChannelsGet();

	if( g_PaintManager.m_pfnMsImgAlphaBlend != NULL )
	{
		bool bRTL = false;
		if( (RunTimeFlagsGet()&__EXT_BMP_FLAG_NO_RTL_DETECTION) == 0 )
		{
			if( CDC::FromHandle(hDC)->GetLayout() == LAYOUT_RTL )
 				bRTL = true;
		}
		if( ! bRTL )
		{
			const CExtBitmap * pBmpRenderSurfaceCreator = this;
			CExtBitmap _bmpCrt;
			if( ! bPreMultiplied )
			{
				if( bAutoPreMultiplyEnabled )
				{
					( const_cast < CExtBitmap * > ( this ) ) -> PreMultipliedRGBChannelsSet( true );
					( const_cast < CExtBitmap * > ( this ) ) -> PreMultiplyRGBChannels();
				} // if( bAutoPreMultiplyEnabled )
				else
				{
					_bmpCrt = (*this);
					if(		_bmpCrt.IsEmpty()
						||	(! _bmpCrt.Make32() )
						)
						return GDI_ERROR;
					_bmpCrt.PreMultiplyRGBChannels();
					pBmpRenderSurfaceCreator = &_bmpCrt;
				} // else from if( bAutoPreMultiplyEnabled )
			} // if( ! bPreMultiplied )
			int nRetVal = GDI_ERROR;
			HDC hSrcDC = ::CreateCompatibleDC( hDC );
			if( hSrcDC != NULL )
			{
				HBITMAP hBmp = pBmpRenderSurfaceCreator->CreateBitmap( hDC, false );
				if( hBmp != NULL )
				{
					HGDIOBJ hOld = ::SelectObject( hSrcDC, hBmp );
					CExtPaintManager::CExtPaintManagerAutoPtr::__EXT_MFC_BLENDFUNCTION
						_bf = { AC_SRC_OVER, 0, nSCA, __EXT_MFC_AC_SRC_ALPHA };
					if( g_PaintManager.m_pfnMsImgAlphaBlend(
							hDC,
							nDstX,
							nDstY,
							nDstWidth,
							nDstHeight,
							hSrcDC,
							nSrcX,
							nSrcY,
							nSrcWidth,
							nSrcHeight,
							_bf
							)
						)
						nRetVal = 0;
					::SelectObject( hSrcDC, hOld );
					::DeleteObject( hBmp );
				} // if( hBmp != NULL )
				::DeleteDC( hSrcDC );
			} // if( hSrcDC != NULL )
			if( nRetVal == 0 )
				return nRetVal;
			_bmpCrt;
		} // if( ! bRTL )
	} // else if( g_PaintManager.m_pfnMsImgAlphaBlend != NULL )

HDC hSrcDC = ::CreateCompatibleDC( NULL );
	if( hSrcDC == NULL )
	{
		//ASSERT( FALSE );
		return GDI_ERROR;
	}
HDC hDstDC = ::CreateCompatibleDC( NULL );
	if( hDstDC == NULL )
	{
		//ASSERT( FALSE );
		::DeleteDC( hSrcDC );
		return GDI_ERROR;
	}
LONG nIconPixelCount = nDstWidth * nDstHeight;
BITMAPINFOHEADER _bihSrc;
	::memset( &_bihSrc, 0, sizeof( BITMAPINFOHEADER ) );
	_bihSrc.biSize = sizeof( BITMAPINFOHEADER );
	_bihSrc.biWidth = nDstWidth;
	_bihSrc.biHeight = nDstHeight;
	_bihSrc.biPlanes = 1;
	_bihSrc.biBitCount = 32;
	_bihSrc.biCompression = BI_RGB;
	_bihSrc.biSizeImage = nIconPixelCount;
BITMAPINFOHEADER _bihDst;
	__EXT_MFC_MEMCPY(
		&_bihDst, 
		sizeof( BITMAPINFOHEADER ),
		&_bihSrc, 
		sizeof( BITMAPINFOHEADER )
		);
COLORREF * pOutSurfaceSrc = NULL, * pOutSurfaceDst = NULL;
HBITMAP hSrcDIB =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&_bihSrc,
			DIB_RGB_COLORS,
			(void**)&pOutSurfaceSrc,
			NULL,
			NULL
			);
	if( hSrcDIB == NULL )
	{
		//ASSERT( FALSE );
		::DeleteDC( hSrcDC );
		::DeleteDC( hDstDC );
		return GDI_ERROR;
	}
	ASSERT( pOutSurfaceSrc != NULL );
HBITMAP hDstDIB =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&_bihDst,
			DIB_RGB_COLORS,
			(void**)&pOutSurfaceDst,
			NULL,
			NULL
			);
	if( hDstDIB == NULL )
	{
		//ASSERT( FALSE );
		::DeleteDC( hSrcDC );
		::DeleteDC( hDstDC );
		::DeleteObject( hSrcDIB );
		return GDI_ERROR;
	}
	ASSERT( pOutSurfaceDst != NULL );
CSize _size = GetSize();
HGDIOBJ hBmpOldSrc = ::SelectObject( hSrcDC, hSrcDIB );
HGDIOBJ hBmpOldDst = ::SelectObject( hDstDC, hDstDIB );

int nOldStretchBltMode = ::GetStretchBltMode( hDC );
	::SetStretchBltMode( hDC, COLORONCOLOR );
int nOldStretchBltModeSrc = ::GetStretchBltMode( hSrcDC );
	::SetStretchBltMode( hSrcDC, COLORONCOLOR );
int nOldStretchBltModeDst = ::GetStretchBltMode( hDstDC );
	::SetStretchBltMode( hDstDC, COLORONCOLOR );

	::StretchBlt(
		hDstDC,
		0,
		0,
		nDstWidth,
		nDstHeight,
		hDC,
		nDstX,
		nDstY,
		nDstWidth,
		nDstHeight,
		SRCCOPY
		);
	Draw(
		hSrcDC,
		0,
		0,
		nDstWidth,
		nDstHeight,
		nSrcX,
		nSrcY,
		nSrcWidth,
		nSrcHeight 
		);
INT nX, nY;
COLORREF * ptrSrcLine = pOutSurfaceSrc + nDstWidth * nDstHeight;
COLORREF * ptrDstLine = pOutSurfaceDst + nDstWidth * nDstHeight;
	for( nY = 0; nY < nDstHeight; nY++ )
	{
		ptrSrcLine -= nDstWidth;
		ptrDstLine -= nDstWidth;
		COLORREF * ptrSrc = ptrSrcLine;
		COLORREF * ptrDst = ptrDstLine;
		for( nX = 0; nX < nDstWidth; nX++ )
		{
			BYTE nDstR = GetRValue( (*ptrDst) );
			BYTE nDstG = GetGValue( (*ptrDst) );
			BYTE nDstB = GetBValue( (*ptrDst) );
			BYTE nSrcR = GetRValue( (*ptrSrc) );
			BYTE nSrcG = GetGValue( (*ptrSrc) );
			BYTE nSrcB = GetBValue( (*ptrSrc) );
			BYTE nSrcA = __EXT_MFC_GetAValue( (*ptrSrc) );
			if( ! bPreMultiplied )
			{
 				if( nSCA != 0x0FF )
					nSrcA = (BYTE) ::MulDiv( nSrcA, nSCA, 255 );
				nSrcR = (BYTE) ::MulDiv( nSrcR, nSrcA, 255 );
				nSrcG = (BYTE) ::MulDiv( nSrcG, nSrcA, 255 );
				nSrcB = (BYTE) ::MulDiv( nSrcB, nSrcA, 255 );
			}
			else if( nSCA != 0x0FF )
			{
				nSrcR = (BYTE) ::MulDiv( nSrcR, nSCA, 255 );
				nSrcG = (BYTE) ::MulDiv( nSrcG, nSCA, 255 );
				nSrcB = (BYTE) ::MulDiv( nSrcB, nSCA, 255 );
				nSrcA = (BYTE) ::MulDiv( nSrcA, nSCA, 255 );
			}
			BYTE _255_nSrcA = (BYTE) ( 255 - nSrcA );
			nDstR = (BYTE) ( nSrcR + ( (BYTE) ::MulDiv( _255_nSrcA, nDstR, 255 ) ) );
			nDstG = (BYTE) ( nSrcG + ( (BYTE) ::MulDiv( _255_nSrcA, nDstG, 255 ) ) );
			nDstB = (BYTE) ( nSrcB + ( (BYTE) ::MulDiv( _255_nSrcA, nDstB, 255 ) ) );
			(*ptrDst) = RGB( nDstR, nDstG, nDstB );
			ptrDst++;
			ptrSrc++;
		} // for( nX = 0; nX < nDstWidth; nX++ )
	} // for( nY = 0; nY < nDstHeight; nY++ )
	::StretchBlt(
		hDC,
		nDstX,
		nDstY,
		nDstWidth,
		nDstHeight,
		hDstDC,
		0,
		0,
		nDstWidth,
		nDstHeight,
		SRCCOPY
		);

	::SetStretchBltMode( hDC, nOldStretchBltMode );
	::SetStretchBltMode( hSrcDC, nOldStretchBltModeSrc );
	::SetStretchBltMode( hDstDC, nOldStretchBltModeDst );
	
	::SelectObject( hSrcDC, hBmpOldSrc );
	::SelectObject( hDstDC, hBmpOldDst );
	::DeleteObject( hSrcDIB );
	::DeleteObject( hDstDIB );
	::DeleteDC( hSrcDC );
	::DeleteDC( hDstDC );
	return 0;
}

int CExtBitmap::AlphaBlend(
	HDC hDC,
	int nDstX,
	int nDstY,
	int nDstWidth,
	int nDstHeight,
	BYTE nSCA // = 0xFF
	) const
{
	return
		AlphaBlend(
			hDC,
			nDstX,
			nDstY,
			nDstWidth,
			nDstHeight,
			0,
			0,
			GetSize().cx,
			GetSize().cy,
			nSCA
			);
}

bool CExtBitmap::AlphaBlendSkinParts(
	HDC hDC,
	const RECT & rcDstDraw,
	const RECT & rcSrcDraw,
	const RECT & rcPadding,
	e_DrawMethod_t eDM,
	bool bDrawMiddlePart, // = true
	bool bSmootherAsPossible, // = false
	BYTE nSCA // = 0xFF
	) const
{
	if( eDM == __EDM_HOLLOW )
		return true;
	ASSERT( hDC != NULL );
	ASSERT( rcPadding.left >= 0 );
	ASSERT( rcPadding.right >= 0 );
	ASSERT( rcPadding.top >= 0 );
	ASSERT( rcPadding.bottom >= 0 );
	if( IsEmpty() || hDC == NULL )
		return false;
	if( eDM == __EDM_CENTER )
	{
		//CSize _size = GetSize();
		CSize _size( rcSrcDraw.right - rcSrcDraw.left, rcSrcDraw.bottom - rcSrcDraw.top );
		CRect rcDst(
			CPoint(
				rcDstDraw.left + ( rcDstDraw.right - rcDstDraw.left - _size.cx ) / 2, 
				rcDstDraw.top + ( rcDstDraw.bottom - rcDstDraw.top - _size.cy ) / 2
				),
			_size
			);
		if( ::RectVisible( hDC, &rcDst ) )
			AlphaBlend(
				hDC,
				rcDst,
				rcSrcDraw,
				nSCA
				);
		return true;
	} // if( eDM == __EDM_CENTER )
int nOldStretchBltMode = ::GetStretchBltMode( hDC );
	::SetStretchBltMode(
		hDC,
		( bSmootherAsPossible && g_PaintManager.m_bIsWinNT )
			? HALFTONE
			: COLORONCOLOR
		);
	// left - top
	if( rcPadding.left > 0 && rcPadding.top > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.top,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top + rcPadding.top
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.top,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top + rcPadding.top
				);
			AlphaBlend(
				hDC,
				rcDst,
				rcSrc,
				nSCA
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.left > 0 && rcPadding.top > 0 )
	// middle - top
	if( rcPadding.top > 0 )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top + rcPadding.top
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top,
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top + rcPadding.top
				);
			if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstWidth % nSrcWidth;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstWidth / nSrcWidth + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.right > rcDst.right
							)
						{
							INT nDiff = rcDstStep.right - rcDst.right;
							rcDstStep.right -= nDiff;
							rcSrcA.right -= nDiff;
						}
						AlphaBlend(
							hDC,
							rcDstStep,
							rcSrcA,
							nSCA
							);
						rcDstStep.OffsetRect( nSrcWidth, 0 );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			else
				AlphaBlend(
					hDC,
					rcDst,
					rcSrc,
					nSCA
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.top > 0 )
	// right - top
	if( rcPadding.right > 0 && rcPadding.top > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top,
			rcDstDraw.right,
			rcDstDraw.top + rcPadding.top
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top,
				rcSrcDraw.right,
				rcSrcDraw.top + rcPadding.top
				);
			AlphaBlend(
				hDC,
				rcDst,
				rcSrc,
				nSCA
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.right > 0 && rcPadding.top > 0 )
	// left - middle
	if( rcPadding.left > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.top + rcPadding.top,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom - rcPadding.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.top + rcPadding.top,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom - rcPadding.bottom
				);
			if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstHeight % nSrcHeight;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstHeight / nSrcHeight + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStep.bottom - rcDst.bottom;
							rcDstStep.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						AlphaBlend(
							hDC,
							rcDstStep,
							rcSrcA,
							nSCA
							);
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			else
				AlphaBlend(
					hDC,
					rcDst,
					rcSrc,
					nSCA
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.left > 0 )
	// right - middle
	if( rcPadding.right > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.top + rcPadding.top,
			rcDstDraw.right,
			rcDstDraw.bottom - rcPadding.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.top + rcPadding.top,
				rcSrcDraw.right,
				rcSrcDraw.bottom - rcPadding.bottom
				);
			if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstHeight % nSrcHeight;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstHeight / nSrcHeight + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStep.bottom - rcDst.bottom;
							rcDstStep.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						AlphaBlend(
							hDC,
							rcDstStep,
							rcSrcA,
							nSCA
							);
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_V || eDM == __EDM_TILE )
			else
				AlphaBlend(
					hDC,
					rcDst,
					rcSrc,
					nSCA
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.right > 0 )
	// left - bottom
	if( rcPadding.left > 0 && rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.left,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom
				);
			AlphaBlend(
				hDC,
				rcDst,
				rcSrc,
				nSCA
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.left > 0 && rcPadding.bottom > 0 )
	// middle - bottom
	if( rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom
				);
			if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstWidth % nSrcWidth;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstWidth / nSrcWidth + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.right > rcDst.right
							)
						{
							INT nDiff = rcDstStep.right - rcDst.right;
							rcDstStep.right -= nDiff;
							rcSrcA.right -= nDiff;
						}
						AlphaBlend(
							hDC,
							rcDstStep,
							rcSrcA,
							nSCA
							);
						rcDstStep.OffsetRect( nSrcWidth, 0 );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_H || eDM == __EDM_TILE )
			else
				AlphaBlend(
					hDC,
					rcDst,
					rcSrc,
					nSCA
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.bottom > 0 )
	// right - bottom
	if( rcPadding.right > 0 && rcPadding.bottom > 0 )
	{
		CRect rcDst(
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom - rcPadding.bottom,
			rcDstDraw.right,
			rcDstDraw.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom - rcPadding.bottom,
				rcSrcDraw.right,
				rcSrcDraw.bottom
				);
			AlphaBlend(
				hDC,
				rcDst,
				rcSrc,
				nSCA
				);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( rcPadding.right > 0 && rcPadding.bottom > 0 )
	// central part
	if( bDrawMiddlePart )
	{
		CRect rcDst(
			rcDstDraw.left + rcPadding.left,
			rcDstDraw.top + rcPadding.top,
			rcDstDraw.right - rcPadding.right,
			rcDstDraw.bottom - rcPadding.bottom
			);
		if( ::RectVisible( hDC, &rcDst ) )
		{
			CRect rcSrc(
				rcSrcDraw.left + rcPadding.left,
				rcSrcDraw.top + rcPadding.top,
				rcSrcDraw.right - rcPadding.right,
				rcSrcDraw.bottom - rcPadding.bottom
				);
			if( eDM == __EDM_TILE_H )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstWidth % nSrcWidth;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstWidth / nSrcWidth + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nDstHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.right > rcDst.right
							)
						{
							INT nDiff = rcDstStep.right - rcDst.right;
							rcDstStep.right -= nDiff;
							rcSrcA.right -= nDiff;
						}
						AlphaBlend(
							hDC,
							rcDstStep,
							rcSrcA,
							nSCA
							);
						rcDstStep.OffsetRect( nSrcWidth, 0 );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // if( eDM == __EDM_TILE_H )
			else if( eDM == __EDM_TILE_V )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPx = nDstHeight % nSrcHeight;
					INT nLastStep = ( nLastStepPx != 0 ) ? 1 : 0;
					INT nStepCount = nDstHeight / nSrcHeight + nLastStep;
					INT nStepIndex;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nDstWidth,
						rcDst.top + nSrcHeight
						);
					CRect rcSrcA = rcSrc;
					for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
					{
						if(		nLastStep != 0
							&&	nStepIndex == ( nStepCount - 1 )
							&&	rcDstStep.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStep.bottom - rcDst.bottom;
							rcDstStep.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						AlphaBlend(
							hDC,
							rcDstStep,
							rcSrcA,
							nSCA
							);
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndex = 0; nStepIndex < nStepCount; nStepIndex ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // else if( eDM == __EDM_TILE_V )
			else if( eDM == __EDM_TILE )
			{
				INT nSrcWidth = rcSrc.right - rcSrc.left;
				INT nSrcHeight = rcSrc.bottom - rcSrc.top;
				INT nDstWidth = rcDst.right - rcDst.left;
				INT nDstHeight = rcDst.bottom - rcDst.top;
				if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
				{
					INT nLastStepPxH = nDstWidth % nSrcWidth;
					INT nLastStepPxV = nDstHeight % nSrcHeight;
					INT nLastStepH = ( nLastStepPxH != 0 ) ? 1 : 0;
					INT nLastStepV = ( nLastStepPxV != 0 ) ? 1 : 0;
					INT nStepCountH = nDstWidth / nSrcWidth + nLastStepH;
					INT nStepCountV = nDstHeight / nSrcHeight + nLastStepV;
					INT nStepIndexH, nStepIndexV;
					CRect rcDstStep(
						rcDst.left,
						rcDst.top,
						rcDst.left + nSrcWidth,
						rcDst.top + nSrcHeight
						);
					for( nStepIndexV = 0; nStepIndexV < nStepCountV; nStepIndexV ++ )
					{
						CRect rcDstStepA = rcDstStep;
						CRect rcSrcA = rcSrc;
						if(		nLastStepV != 0
							&&	nStepIndexV == ( nStepCountV - 1 )
							&&	rcDstStepA.bottom > rcDst.bottom
							)
						{
							INT nDiff = rcDstStepA.bottom - rcDst.bottom;
							rcDstStepA.bottom -= nDiff;
							rcSrcA.bottom -= nDiff;
						}
						for( nStepIndexH = 0; nStepIndexH < nStepCountH; nStepIndexH ++ )
						{
							if(		nLastStepH != 0
								&&	nStepIndexH == ( nStepCountH - 1 )
								&&	rcDstStepA.right > rcDst.right
								)
							{
								INT nDiff = rcDstStepA.right - rcDst.right;
								rcDstStepA.right -= nDiff;
								rcSrcA.right -= nDiff;
							}
							AlphaBlend(
								hDC,
								rcDstStepA,
								rcSrcA,
								nSCA
								);
							rcDstStepA.OffsetRect( nSrcWidth, 0 );
						} // for( nStepIndexH = 0; nStepIndexH < nStepCountH; nStepIndexH ++ )
						rcDstStep.OffsetRect( 0, nSrcHeight );
					} // for( nStepIndexV = 0; nStepIndexV < nStepCountV; nStepIndexV ++ )
				} // if( nSrcWidth != 0 && nSrcHeight != 0 && nDstWidth != 0 && nDstHeight != 0 )
			} // else if( eDM == __EDM_TILE )
			else
				AlphaBlend(
					hDC,
					rcDst,
					rcSrc,
					nSCA
					);
		} // if( ::RectVisible( hDC, &rcDst ) )
	} // if( bDrawMiddlePart )
	::SetStretchBltMode(
		hDC,
		nOldStretchBltMode
		);
	return true;
}

bool CExtBitmap::AlphaBlendSkinParts(
	HDC hDC,
	const RECT & rcDstDraw,
	const RECT & rcPadding,
	e_DrawMethod_t eDM,
	bool bDrawMiddlePart, // = true
	bool bSmootherAsPossible, // = false
	BYTE nSCA // = 0xFF
	) const
{
	ASSERT( hDC != NULL );
	ASSERT( rcPadding.left >= 0 );
	ASSERT( rcPadding.right >= 0 );
	ASSERT( rcPadding.top >= 0 );
	ASSERT( rcPadding.bottom >= 0 );
	if( IsEmpty() || hDC == NULL )
		return false;
CSize _size = GetSize();
RECT rcSrcDraw = { 0, 0, _size.cx, _size.cy };
	return
		AlphaBlendSkinParts(
			hDC,
			rcDstDraw,
			rcSrcDraw,
			rcPadding,
			eDM,
			bDrawMiddlePart,
			bSmootherAsPossible,
			nSCA
			);
}

bool CExtBitmap::GetPixel(
	INT nX,
	INT nY,
	RGBQUAD & _pixel
	) const
{
	if( IsEmpty() )
		return false;
	if( nX < 0 || nY < 0 )
		return false;
CSize _size = GetSize();
	if( nX >= _size.cx || nY >= _size.cy )
		return false;
	nY = _size.cy - nY - 1;
ULONG nBPP = GetBPP();
ULONG nScanLineSize = stat_RcScanLineSize( _size.cx, nBPP );
LPBYTE pScanLine = stat_RcScanLine( m_pImageBuffer, ULONG(nY), nScanLineSize );
	switch( nBPP )
	{
	case 32:
		pScanLine += nX * 4;
		_pixel.rgbRed = pScanLine[2];
		_pixel.rgbGreen = pScanLine[1];
		_pixel.rgbBlue = pScanLine[0];
		_pixel.rgbReserved = pScanLine[3];
		if( PreMultipliedRGBChannelsGet() && _pixel.rgbReserved != 0 )
		{
			INT nRed   = INT( _pixel.rgbRed   * 255 / INT(_pixel.rgbReserved) );
			INT nGreen = INT( _pixel.rgbGreen * 255 / INT(_pixel.rgbReserved) );
			INT nBlue  = INT( _pixel.rgbBlue  * 255 / INT(_pixel.rgbReserved) );
			_pixel.rgbRed   = BYTE( min( 255, nRed   ) );
			_pixel.rgbGreen = BYTE( min( 255, nGreen ) );
			_pixel.rgbBlue  = BYTE( min( 255, nBlue  ) );
		}
		return true;
	case 24:
		pScanLine += nX * 3;
		_pixel.rgbRed = pScanLine[2];
		_pixel.rgbGreen = pScanLine[1];
		_pixel.rgbBlue = pScanLine[0];
		_pixel.rgbReserved	= 0xFF;
		return true;
	case 16:
	case 15:
		pScanLine += nX * 2;
		WORD * pWordPixel16 = ( WORD * ) pScanLine;
		if(		( stat_MaskRed( m_pImageBuffer ) == 0xF800 )
			&&	( stat_MaskGreen( m_pImageBuffer ) == 0x07E0 )
			&&	( stat_MaskBlue( m_pImageBuffer ) == 0x001F )
			)
		{
			_pixel.rgbRed = BYTE( ( ( ( (*pWordPixel16) & 0xF800 ) >> 11 ) * 0xFF ) / 0x1F );
			_pixel.rgbGreen = BYTE( ( ( ( (*pWordPixel16) & 0x07E0 ) >> 5 ) * 0xFF ) / 0x3F );
			_pixel.rgbBlue = BYTE( ( ( (*pWordPixel16) & 0x001F ) * 0xFF ) / 0x1F );
		}
		else
		{
			_pixel.rgbRed = BYTE( ( ( ( (*pWordPixel16) & 0x7C00 ) >> 10 ) * 0xFF ) / 0x1F );
			_pixel.rgbGreen = BYTE( ( ( ( (*pWordPixel16) & 0x03E0 ) >> 5 ) * 0xFF ) / 0x1F );
			_pixel.rgbBlue = BYTE( ( ( (*pWordPixel16) & 0x001F ) * 0xFF) / 0x1F );
		}
		_pixel.rgbReserved = 0xFF;
		return true;
	} // switch( nBPP )
	return false;
}

COLORREF CExtBitmap::GetPixel(
	INT nX,
	INT nY,
	BYTE * p_nAlpha // = NULL
	) const
{
	if( IsEmpty() )
		return COLORREF(-1L);
RGBQUAD _pixel;
	if( ! GetPixel( nX, nY, _pixel ) )
		return COLORREF(-1L);
	if( p_nAlpha != NULL )
		(*p_nAlpha) = _pixel.rgbReserved;
	return RGB( _pixel.rgbRed, _pixel.rgbGreen, _pixel.rgbBlue );
}

bool CExtBitmap::SetPixel(
	INT nX,
	INT nY,
	const RGBQUAD & _pixel,
	bool bSetRed, // = true
	bool bSetGreen, // = true
	bool bSetBlue, // = true
	bool bSetAlpha // = true
	)
{
	if( IsEmpty() )
		return false;
	if( nX < 0 || nY < 0 )
		return false;
CSize _size = GetSize();
	if( nX >= _size.cx || nY >= _size.cy )
		return false;
	nY = _size.cy - nY - 1;
ULONG nBPP = GetBPP();
ULONG nScanLineSize = stat_RcScanLineSize( _size.cx, nBPP );
LPBYTE pScanLine = stat_RcScanLine( m_pImageBuffer, ULONG(nY), nScanLineSize );
	switch( nBPP )
	{
	case 32:
		pScanLine += nX * 4;
		if( bSetRed )
			pScanLine[2] = _pixel.rgbRed;
		if( bSetGreen )
			pScanLine[1] = _pixel.rgbGreen;
		if( bSetBlue )
			pScanLine[0] = _pixel.rgbBlue;
		if( bSetAlpha )
		{
			pScanLine[3] = _pixel.rgbReserved;
			if( PreMultipliedRGBChannelsGet() )
			{
				pScanLine[0] = BYTE( pScanLine[0] * _pixel.rgbReserved / 255 );
				pScanLine[1] = BYTE( pScanLine[1] * _pixel.rgbReserved / 255 );
				pScanLine[2] = BYTE( pScanLine[2] * _pixel.rgbReserved / 255 );
			}
		}
		OnContentChanged();
		return true;
	case 24:
		pScanLine += nX * 3;
		if( bSetRed )
			pScanLine[2] = _pixel.rgbRed;
		if( bSetGreen )
			pScanLine[1] = _pixel.rgbGreen;
		if( bSetBlue )
			pScanLine[0] = _pixel.rgbBlue;
		OnContentChanged();
		return true;
	case 16:
	case 15:
		{
			pScanLine += nX * 2;
			WORD * pWordPixel16 = ( WORD * ) pScanLine;
			if(		stat_MaskRed( m_pImageBuffer ) == 0xF800
				&&	stat_MaskGreen( m_pImageBuffer ) == 0x07E0
				&&	stat_MaskBlue( m_pImageBuffer ) == 0x001F
				)
			{
				*pWordPixel16 =
					WORD( 
						  ( bSetRed   ? ( ( _pixel.rgbRed >> 3 ) << 11 ) : 0 )
						| ( bSetGreen ? ( ( _pixel.rgbGreen >> 2 ) << 5 ) : 0 )
						| ( bSetBlue  ?   ( _pixel.rgbBlue >> 3 ) : 0 )
						);
			}
			else
			{
				*pWordPixel16 =
					WORD( 
						  ( bSetRed   ? ( ( _pixel.rgbRed >> 3 ) << 10 ) : 0 )
						| ( bSetGreen ? ( ( _pixel.rgbGreen >> 3 ) << 5 ) : 0 )
						| ( bSetBlue  ?   ( _pixel.rgbBlue >> 3 ) : 0 )
						);
			}
		}
		OnContentChanged();
		return true;
	} // switch( nBPP )
	return false;
}

bool CExtBitmap::ColorizeMono(
	COLORREF clrTransparent,
	COLORREF clrTarget,
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return false;
ULONG nBPP = GetBPP();
	if( nBPP < 15 )
		return false;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CSize _size = GetSize();
CRect _rcFrame;
	if( pRect != NULL )
	{
		_rcFrame = (*pRect);
		if( _rcFrame.left < 0 )
			_rcFrame.left = 0;
		if( _rcFrame.top < 0 )
			_rcFrame.top = 0;
		if( _rcFrame.right > _size.cx )
			_rcFrame.right = _size.cx;
		if( _rcFrame.bottom > _size.cy )
			_rcFrame.bottom = _size.cy;
	}
	else
		_rcFrame.SetRect( 0, 0, _size.cx, _size.cy );
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return false;
double dHTarget = 0.0, dSTarget = 0.0, dLTarget = 0.0;
	stat_RGBtoHSL( clrTarget, &dHTarget, &dSTarget, &dLTarget );
INT nX, nY;
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			COLORREF clr =
				RGB(
					_pixel.rgbRed,
					_pixel.rgbGreen,
					_pixel.rgbBlue
					);
			if( clrTransparent != COLORREF(-1L) )
			{
				if(	clr == clrTransparent )
					continue;
			} // if( clrTransparent != COLORREF(-1L) )
			double dHPixel = 0.0, dSPixel = 0.0, dLPixel = 0.0;
			stat_RGBtoHSL( clr, &dHPixel, &dSPixel, &dLPixel );
			clr = stat_HLStoRGB( dHTarget, dLPixel, dSTarget );
			_pixel.rgbRed   = GetRValue( clr );
			_pixel.rgbGreen = GetGValue( clr );
			_pixel.rgbBlue  = GetBValue( clr );
			VERIFY( SetPixel( nX, nY, _pixel ) );
		}
	}
	OnContentChanged();
	return true;
}

bool CExtBitmap::AdjustAlpha(
	double percentAlpha,
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return false;
ULONG nBPP = GetBPP();
	if( nBPP < 15 )
		return false;
	if( percentAlpha == 0.0 )
		return true;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CSize _size = GetSize();
CRect _rcFrame;
	if( pRect != NULL )
	{
		_rcFrame = (*pRect);
		if( _rcFrame.left < 0 )
			_rcFrame.left = 0;
		if( _rcFrame.top < 0 )
			_rcFrame.top = 0;
		if( _rcFrame.right > _size.cx )
			_rcFrame.right = _size.cx;
		if( _rcFrame.bottom > _size.cy )
			_rcFrame.bottom = _size.cy;
	}
	else
		_rcFrame.SetRect( 0, 0, _size.cx, _size.cy );
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return false;
INT nX, nY;
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			if( _pixel.rgbReserved == 0 )
				continue;
			double A = double(_pixel.rgbReserved) / 255.0;
			if( percentAlpha > 0.0 )
				A =
					A +
					(1.0 - A) * percentAlpha
					;
			else if ( percentAlpha < 0.0 )
				A =
					A * (1.0 + percentAlpha);
			if( A < 0.0 )
				A = 0.0;
			else if( A > 1.0 )
				A = 1.0;
			BYTE nA = BYTE(INT(A*255.0));
			if( _pixel.rgbReserved == nA )
				continue;
			_pixel.rgbReserved = nA;
			VERIFY( SetPixel( nX, nY, _pixel ) );
		}
	}
	OnContentChanged();
	return true;
}

bool CExtBitmap::AdjustHLS(
	COLORREF clrTransparent, // = RGB(0,0,0)
	COLORREF clrTolerance, // = RGB(0,0,0)
	double percentH, // = 0.0
	double percentL, // = 0.0
	double percentS, // = 0.0
	LPCRECT pRect // = NULL
	)
{
	if( IsEmpty() )
		return false;
ULONG nBPP = GetBPP();
	if( nBPP < 15 )
		return false;
	if(		percentH == 0.0
		&&	percentL == 0.0
		&&	percentS == 0.0
		)
		return true;
	if( PreMultipliedRGBChannelsGet() )
	{
		PreMultiplyRGBChannels( false );
		PreMultipliedRGBChannelsSet( false );
	}
CSize _size = GetSize();
CRect _rcFrame;
	if( pRect != NULL )
	{
		_rcFrame = (*pRect);
		if( _rcFrame.left < 0 )
			_rcFrame.left = 0;
		if( _rcFrame.top < 0 )
			_rcFrame.top = 0;
		if( _rcFrame.right > _size.cx )
			_rcFrame.right = _size.cx;
		if( _rcFrame.bottom > _size.cy )
			_rcFrame.bottom = _size.cy;
	}
	else
		_rcFrame.SetRect( 0, 0, _size.cx, _size.cy );
	if(		_rcFrame.left >= _rcFrame.right
		||	_rcFrame.top >= _rcFrame.bottom
		)
		return false;
BYTE nLowRed	= GetRValue( clrTransparent );
BYTE nLowGreen	= GetGValue( clrTransparent );
BYTE nLowBlue	= GetBValue( clrTransparent );
BYTE nHighRed	= BYTE( min( 0xff, ( nLowRed + GetRValue(clrTolerance) ) ) );
BYTE nHighGreen	= BYTE( min( 0xff, ( nLowGreen + GetGValue(clrTolerance) ) ) );
BYTE nHighBlue	= BYTE( min( 0xff, ( nLowBlue + GetBValue(clrTolerance) ) ) );
INT nX, nY;
	for( nY = _rcFrame.top; nY < _rcFrame.bottom; nY++ )
	{
		for( nX = _rcFrame.left; nX < _rcFrame.right; nX++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( nX, nY, _pixel ) );
			if( clrTransparent != COLORREF(-1L) )
			{
				if( _pixel.rgbRed >= nLowRed && _pixel.rgbRed <= nHighRed )
				{
					if( _pixel.rgbGreen >= nLowGreen && _pixel.rgbGreen <= nHighGreen )
					{
						if( _pixel.rgbBlue >= nLowBlue && _pixel.rgbBlue <= nHighBlue )
							continue;
					} // if( nColorPart >= nLowGreen && nColorPart <= nHighGreen )
				} // if( nColorPart >= nLowRed && nColorPart <= nHighRed )
			} // if( clrTransparent != COLORREF(-1L) )
			COLORREF clr =
				RGB(
					_pixel.rgbRed,
					_pixel.rgbGreen,
					_pixel.rgbBlue
					);
			clr =
				CExtBitmap::stat_HLS_Adjust(
					clr,
					percentH,
					percentL,
					percentS
					);
			_pixel.rgbRed   = GetRValue(clr);
			_pixel.rgbGreen = GetGValue(clr);
			_pixel.rgbBlue  = GetBValue(clr);
			VERIFY( SetPixel( nX, nY, _pixel ) );
		}
	}
	OnContentChanged();
	return true;
}

HGLOBAL CExtBitmap::GenerateHGLOBAL(
    HPALETTE hPalDst // = NULL
	) const
{
	if( IsEmpty() )
		return NULL;
HBITMAP hBitmap = CreateBitmap( false );
	if( hBitmap == NULL )
		return NULL;
HGLOBAL hGlobal = stat_HBITMAPtoHGLOBAL( hBitmap, hPalDst );
	::DeleteObject( hBitmap );
	return hGlobal;
}

HGLOBAL CExtBitmap::stat_HBITMAPtoHGLOBAL(
	HBITMAP hBmpSrc,
    HPALETTE hPalDst // = NULL
	)
{
	if( hBmpSrc == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	if( hPalDst == NULL )
		hPalDst = (HPALETTE)::GetStockObject( DEFAULT_PALETTE );

BITMAP bmpSrcDescription;
	::memset( &bmpSrcDescription, 0, sizeof(BITMAP) );
	::GetObject( hBmpSrc, sizeof(BITMAP), &bmpSrcDescription );
	ASSERT( bmpSrcDescription.bmWidth > 0 && bmpSrcDescription.bmHeight > 0 );

BITMAPINFOHEADER bmpSrcInfoHdr;
	::memset( &bmpSrcInfoHdr, 0, sizeof( BITMAPINFOHEADER ) );
	bmpSrcInfoHdr.biSize = sizeof( BITMAPINFOHEADER );
	bmpSrcInfoHdr.biWidth = bmpSrcDescription.bmWidth;
	bmpSrcInfoHdr.biHeight = bmpSrcDescription.bmHeight;
	bmpSrcInfoHdr.biPlanes = 1;
	bmpSrcInfoHdr.biBitCount = (WORD)
		( bmpSrcDescription.bmPlanes * bmpSrcDescription.bmBitsPixel );
	bmpSrcInfoHdr.biCompression = BI_RGB;

INT nColors =
		(bmpSrcInfoHdr.biBitCount > 8)
			? 0
			: (1 << bmpSrcInfoHdr.biBitCount)
		;
DWORD dwLenAllocPart =
		bmpSrcInfoHdr.biSize
		+ nColors * sizeof(RGBQUAD)
		;

HDC hDcSrc = ::CreateCompatibleDC( NULL );
	if( hDcSrc == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	hPalDst = ::SelectPalette( hDcSrc, hPalDst, FALSE );
	::RealizePalette( hDcSrc );

HGLOBAL	hGlobalDst =
		::GlobalAlloc(
			GMEM_FIXED,
			dwLenAllocPart
			);
	if( hGlobalDst == NULL )
	{
		ASSERT( FALSE );
		::SelectPalette( hDcSrc, hPalDst, FALSE );
		::DeleteDC( hDcSrc );
		return NULL;
	} // if( hBmpDst == NULL )
	
LPBITMAPINFOHEADER  lpBmpSrcInfoHdr =
		(LPBITMAPINFOHEADER)hGlobalDst;
	*lpBmpSrcInfoHdr = bmpSrcInfoHdr;
	::GetDIBits(
		hDcSrc,
		hBmpSrc,
		0L,
		(DWORD)bmpSrcInfoHdr.biHeight,
		(LPBYTE)NULL,
		(LPBITMAPINFO)lpBmpSrcInfoHdr,
		(DWORD)DIB_RGB_COLORS
		);
	bmpSrcInfoHdr = *lpBmpSrcInfoHdr;
	if( bmpSrcInfoHdr.biSizeImage == 0 )
		bmpSrcInfoHdr.biSizeImage = 
			(
				(
					(
						(
							bmpSrcInfoHdr.biWidth
							* bmpSrcInfoHdr.biBitCount
						) + 31
					) & ~31
				) / 8
			) * bmpSrcInfoHdr.biHeight;

	dwLenAllocPart += bmpSrcInfoHdr.biSizeImage;
HGLOBAL hGlobalTmp =
		::GlobalReAlloc(
			hGlobalDst,
			dwLenAllocPart,
			GMEM_MOVEABLE
			);
	if( hGlobalTmp == NULL  )
	{
		ASSERT( FALSE );
		::GlobalFree( hGlobalDst ) ;
		::SelectPalette( hDcSrc, hPalDst, FALSE );
		::DeleteDC( hDcSrc );
		return NULL;
	} // if( bmpSrcInfoHdr.biSizeImage == 0 )
	hGlobalDst = hGlobalTmp;

	lpBmpSrcInfoHdr = (LPBITMAPINFOHEADER)hGlobalDst;
	if(	!	(::GetDIBits(
				hDcSrc,
				hBmpSrc,
				0L,
				(DWORD)bmpSrcInfoHdr.biHeight,
				(LPBYTE)lpBmpSrcInfoHdr + (bmpSrcInfoHdr.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpBmpSrcInfoHdr,
				(DWORD)DIB_RGB_COLORS
				)
			)
		)
	{
		ASSERT( FALSE );
		::GlobalFree( hGlobalDst );
		hGlobalDst = NULL;
	}
	::SelectPalette( hDcSrc, hPalDst, FALSE );
	::DeleteDC( hDcSrc );
	return hGlobalDst;
}

HBITMAP CExtBitmap::stat_CloneBitmap(
	HBITMAP hBmpSrc,
	const RECT * pRcPart, // = NULL
	COLORREF ** ppSurface // = NULL // if not NULL - create 32 bit surface
	)
{
	if( hBmpSrc == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
BITMAP _bmpInfo;
	::memset( &_bmpInfo, 0, sizeof(BITMAP) );
	::GetObject( hBmpSrc, sizeof(BITMAP), &_bmpInfo );
#ifdef _DEBUG
DWORD dwLastError = ::GetLastError();
	dwLastError;
#endif // _DEBUG
	ASSERT( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 );
CPoint _ptSrc( 0, 0 );
CSize _sizeDest( _bmpInfo.bmWidth, _bmpInfo.bmHeight );
	if( pRcPart != NULL )
	{
		ASSERT( pRcPart->left < pRcPart->right );
		ASSERT( pRcPart->top < pRcPart->bottom );
		ASSERT( 0 <= pRcPart->left && pRcPart->left <= _bmpInfo.bmWidth );
		ASSERT( 0 <= pRcPart->right && pRcPart->right <= _bmpInfo.bmWidth );
		ASSERT( 0 <= pRcPart->top && pRcPart->top <= _bmpInfo.bmHeight );
		ASSERT( 0 <= pRcPart->bottom && pRcPart->bottom <= _bmpInfo.bmHeight );
		_sizeDest.cx = pRcPart->right - pRcPart->left;
		_sizeDest.cy = pRcPart->bottom - pRcPart->top;
		ASSERT( _sizeDest.cx <= _bmpInfo.bmWidth );
		ASSERT( _sizeDest.cy <= _bmpInfo.bmHeight );
		_ptSrc.x = pRcPart->left;
		_ptSrc.y = pRcPart->top;
	} // if( pRcPart != NULL )

HDC hSrcDC = ::CreateCompatibleDC( NULL );
	if( hSrcDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
HGDIOBJ hBmpSrcOld = ::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrc );
HBITMAP hBmpDst = NULL;
	if( ppSurface != NULL )
	{
		BITMAPINFOHEADER bih;
		::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
		bih.biSize = sizeof( BITMAPINFOHEADER );
		bih.biWidth = _sizeDest.cx;
		bih.biHeight = _sizeDest.cy;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = _sizeDest.cx * _sizeDest.cy;
		hBmpDst =
			::CreateDIBSection(
				hSrcDC,
				(LPBITMAPINFO)&bih,
				DIB_RGB_COLORS,
				(void **)ppSurface,
				NULL,
				NULL
				);
		ASSERT( hBmpDst != NULL );
		ASSERT( (*ppSurface) != NULL );
	} // if( ppSurface != NULL )
	else
	{
		hBmpDst =
			::CreateCompatibleBitmap(
				hSrcDC,
				_sizeDest.cx,
				_sizeDest.cy
				);
		ASSERT( hBmpDst != NULL );
	} // else from if( ppSurface != NULL )
	if( hBmpDst == NULL )
	{
		ASSERT( FALSE );
		::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrcOld );
		::DeleteDC( hSrcDC );
		return NULL;
	}
HDC hDstDC = ::CreateCompatibleDC( NULL );
	if( hDstDC == NULL )
	{
		ASSERT( FALSE );
		::DeleteObject( hBmpDst );
		::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrcOld );
		::DeleteDC( hSrcDC );
		return NULL;
	}
HGDIOBJ hBmpDstOld = ::SelectObject( hDstDC, (HGDIOBJ)hBmpDst );
	VERIFY(
		::BitBlt(
			hDstDC,
			0, 0, _sizeDest.cx, _sizeDest.cy,
			hSrcDC,
			_ptSrc.x, _ptSrc.y,
			SRCCOPY
			)
		);
	::SelectObject( hDstDC, (HGDIOBJ)hBmpDstOld );
	::DeleteDC( hDstDC );
	::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrcOld );
	::DeleteDC( hSrcDC );
	return hBmpDst;
}

bool CExtBitmap::stat_CloneBitmap(
	CBitmap & bmpDst,
	const CBitmap & bmpSrc,
	const RECT * pRcPart, // = NULL
	COLORREF ** ppSurface // = NULL // if not NULL - create 32 bit surface
	)
{
	if( bmpSrc.GetSafeHandle() == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( bmpDst.GetSafeHandle() != NULL )
		bmpDst.DeleteObject();
HBITMAP hBmpDst =
		stat_CloneBitmap(
			(HBITMAP)bmpSrc.GetSafeHandle(),
			pRcPart,
			ppSurface
			);
	if( hBmpDst == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	bmpDst.Attach( hBmpDst );
	return true;
}

HRGN CExtBitmap::GenerateColorHRGN(
	bool bRTL,
	COLORREF clrTransparent,
	COLORREF clrTolerance // = RGB(0,0,0)
	) const
{
	if( IsEmpty() )
	{
		ASSERT( FALSE );
		return NULL;
	}
ULONG nBPP = GetBPP();
	if( nBPP != 32 )
	{
		CExtBitmap other = (*this);
		other.Make32(false);
		return other.GenerateColorHRGN( bRTL, clrTransparent, clrTolerance );
	}
CSize _sizeBitmap = GetSize();
ULONG nScanLineSize = stat_RcScanLineSize( _sizeBitmap.cx, nBPP );
LPBYTE p32bitColorSurface = stat_RcSurface( m_pImageBuffer ); 
	ASSERT( p32bitColorSurface != NULL );
static DWORD g_nReallocSize = ( g_PaintManager.m_bIsWinNT4 ? 1 : 200 );
DWORD nMaxParts = g_nReallocSize;
HANDLE hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + ( sizeof(RECT) * nMaxParts ) );
RGNDATA * pRgnData = (RGNDATA *) ::GlobalLock( hGlobal );
	::memset( LPVOID(pRgnData), 0, sizeof(RGNDATAHEADER) + ( sizeof(RECT) * nMaxParts ) );
	pRgnData->rdh.dwSize	= sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType		= RDH_RECTANGLES;
	pRgnData->rdh.nCount	= pRgnData->rdh.nRgnSize = 0;
	::SetRect( &pRgnData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0 );
BYTE nLowRed	= GetRValue( clrTransparent );
BYTE nLowGreen	= GetGValue( clrTransparent );
BYTE nLowBlue	= GetBValue( clrTransparent );
BYTE nHighRed	= BYTE( min( 0xff, ( nLowRed + GetRValue(clrTolerance) ) ) );
BYTE nHighGreen	= BYTE( min( 0xff, ( nLowGreen + GetGValue(clrTolerance) ) ) );
BYTE nHighBlue	= BYTE( min( 0xff, ( nLowBlue + GetBValue(clrTolerance) ) ) );
HRGN hRgnResult = NULL;
	for( INT _nY = 0; _nY < _sizeBitmap.cy; _nY++ )
	{
		INT nY = _sizeBitmap.cy - _nY - 1;
		for( INT nX = 0; nX < _sizeBitmap.cx; nX++ )
		{
			LPLONG ptrLinePart = (LPLONG)p32bitColorSurface + nX;
			INT nSavedX = nX;
			for( ; nX < _sizeBitmap.cx; ptrLinePart++, nX++ )
			{
				BYTE nColorPart = GetRValue( (*ptrLinePart) );
				if( nColorPart >= nLowRed && nColorPart <= nHighRed )
				{
					nColorPart = GetGValue( (*ptrLinePart) );
					if( nColorPart >= nLowGreen && nColorPart <= nHighGreen )
					{
						nColorPart = GetBValue( (*ptrLinePart) );
						if( nColorPart >= nLowBlue && nColorPart <= nHighBlue )
							break;
					} // if( nColorPart >= nLowGreen && nColorPart <= nHighGreen )
				} // if( nColorPart >= nLowRed && nColorPart <= nHighRed )
			} // for( ; nX < _sizeBitmap.cx; ptrLinePart++, nX++ )

			if( nX > nSavedX )
			{
				if( pRgnData->rdh.nCount >= nMaxParts )
				{
					::GlobalUnlock( hGlobal );
					DWORD dwPrevParts = nMaxParts;
					nMaxParts += g_nReallocSize;
					hGlobal = ::GlobalReAlloc( hGlobal, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxParts), GMEM_MOVEABLE );
					pRgnData = (RGNDATA *) ::GlobalLock( hGlobal );
					::memset( LPVOID( LPBYTE(pRgnData) + sizeof(RGNDATAHEADER) + ( sizeof(RECT) * dwPrevParts ) ), 0, sizeof(RECT) * ( nMaxParts - dwPrevParts ) );
				} // if( pRgnData->rdh.nCount >= nMaxParts )
				LPRECT pRect = (LPRECT)&pRgnData->Buffer;
				::SetRect( &pRect[pRgnData->rdh.nCount], nSavedX, nY, nX, nY + 1 );
				if( nSavedX < pRgnData->rdh.rcBound.left )
					pRgnData->rdh.rcBound.left = nSavedX;
				if( nY < pRgnData->rdh.rcBound.top )
					pRgnData->rdh.rcBound.top = nY;
				if( nX > pRgnData->rdh.rcBound.right )
					pRgnData->rdh.rcBound.right = nX;
				if( (nY + 1) > pRgnData->rdh.rcBound.bottom )
					pRgnData->rdh.rcBound.bottom = nY+1;
				pRgnData->rdh.nCount ++;
				if( pRgnData->rdh.nCount == DWORD( g_PaintManager.m_bIsWinNT4 ? 1 : 2000 ) )
				{
					if( bRTL )
					{
						INT nPart, nLeft, nRight, nCount = INT( pRgnData->rdh.nCount );
						LPRECT pRect = (LPRECT)&pRgnData->Buffer;
						for( nPart = 0; nPart < nCount; nPart ++, pRect ++ )
						{
							nLeft = _sizeBitmap.cx - pRect->left;
							nRight = _sizeBitmap.cx - pRect->right;
							pRect->right = nLeft;
							pRect->left = nRight;
						} // for( nPart = 0; nPart < nCount; nPart ++, pRect ++ )
					} // if( bRTL )
					HRGN hRgnPart = ::ExtCreateRegion( NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxParts), pRgnData );
					if( hRgnResult != NULL )
					{
						::CombineRgn( hRgnResult, hRgnResult, hRgnPart, RGN_OR );
						::DeleteObject( hRgnPart );
					} // if( hRgnResult != NULL )
					else
						hRgnResult = hRgnPart;
					pRgnData->rdh.nCount = 0;
					::SetRect( &pRgnData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0 );
				} // if( pRgnData->rdh.nCount ==  ( g_PaintManager.m_bIsWinNT4 ? 1 : 2000 ) )
			} // if( nX > nSavedX )
		} // for( INT nX = 0; nX < _sizeBitmap.cx; nX++ )
		p32bitColorSurface += nScanLineSize;
	} // for( INT _nY = 0; _nY < _sizeBitmap.cy; _nY++ )
	if( bRTL )
	{
		INT nPart, nLeft, nRight, nCount = INT( pRgnData->rdh.nCount );
		LPRECT pRect = (LPRECT)&pRgnData->Buffer;
		for( nPart = 0; nPart < nCount; nPart ++, pRect ++ )
		{
			nLeft = _sizeBitmap.cx - pRect->left;
			nRight = _sizeBitmap.cx - pRect->right;
			pRect->right = nLeft;
			pRect->left = nRight;
		} // for( nPart = 0; nPart < nCount; nPart ++, pRect ++ )
	} // if( bRTL )
HRGN hRgnPart = ::ExtCreateRegion( NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxParts), pRgnData );
	if( hRgnResult != NULL )
	{
		::CombineRgn( hRgnResult, hRgnResult, hRgnPart, RGN_OR );
		::DeleteObject( hRgnPart );
	} // if( hRgnResult != NULL )
	else
		hRgnResult = hRgnPart;
	::GlobalFree( hGlobal );
	return hRgnResult;
}

bool CExtBitmap::SetPixel(
	INT nX,
	INT nY,
	COLORREF clr, // if COLORREF( -1L ) - do not set color
	INT nAlpha // = -1 // less than zero - do not set alpha
	)
{
	if( IsEmpty() )
		return false;
	if( clr == COLORREF(-1L) && nAlpha < 0 )
		return false;
bool bSetColor = false, bSetAlpha = false;
RGBQUAD _pixel = { 0, 0, 0, 0 };
	if( clr != COLORREF(-1L) )
	{
		bSetColor = true;
		_pixel.rgbRed = GetRValue( clr );
		_pixel.rgbGreen = GetGValue( clr );
		_pixel.rgbBlue = GetBValue( clr );
	}
	if( nAlpha >= 0 )
	{
		bSetAlpha = true;
		if( nAlpha > 0x0FF )
			nAlpha = 0x0FF;
		_pixel.rgbReserved = BYTE( nAlpha );
	} // if( nAlpha >= 0 )
	return
		SetPixel(
			nX,
			nY,
			_pixel,
			bSetColor,
			bSetColor,
			bSetColor,
			bSetAlpha
			);
}

HRGN CExtBitmap::GenerateSkinPartColorRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcSrc,
	const RECT & rcPadding,
	COLORREF clrTransparent,
	bool bLeft,
	bool bTop
	) const
{
	if( IsEmpty() )
		return NULL;
CRect rcIn = rcSrc;
CRect rcOut = rcDst;
	if( bLeft )
	{
		rcIn.right = rcIn.left + rcPadding.left;
		rcOut.right = rcOut.left + rcPadding.left;
	}
	else
	{
		rcIn.left = rcIn.right - rcPadding.right;
		rcOut.left = rcOut.right - rcPadding.right;
	}
	if( bTop )
	{
		rcIn.bottom = rcIn.top + rcPadding.top;
		rcOut.bottom = rcOut.top + rcPadding.top;
	}
	else
	{
		rcIn.top = rcIn.bottom - rcPadding.bottom;
		rcOut.top = rcOut.bottom - rcPadding.bottom;
	}
INT xIn, yIn, xOut, yOut;
CRgn _rgnRetVal;
	for( yIn = rcIn.top, yOut = rcOut.top; yIn < rcIn.bottom; yIn++, yOut++ )
	{
		for( xIn = rcIn.left, xOut = rcOut.left; xIn < rcIn.right; xIn++, xOut++ )
		{
			COLORREF clr = GetPixel( xIn, yIn );
			ASSERT( clr != COLORREF(-1L) );
			if( clr != clrTransparent )
				continue;
			CRgn _rgnPixel;
			if( bRTL )
			{
				if( ! _rgnPixel.CreateRectRgn(
						rcOut.right - 1 - xOut + rcOut.left,
						yOut,
						rcOut.right - xOut + rcOut.left,
						yOut + 1
						)
					)
				{
					ASSERT( FALSE );
					return NULL;
				}
			} // if( bRTL )
			else
			{
				if( ! _rgnPixel.CreateRectRgn(
						xOut,
						yOut,
						xOut + 1,
						yOut + 1
						)
					)
				{
					ASSERT( FALSE );
					return NULL;
				}
			} // else from if( bRTL )
			if( _rgnRetVal.GetSafeHandle() == NULL )
			{
				if( ! _rgnRetVal.CreateRectRgn( 0, 0, 0, 0 ) )
				{
					ASSERT( FALSE );
					return NULL;
				}
			}
			int nCombineRetVal =
				_rgnRetVal.CombineRgn( &_rgnRetVal, &_rgnPixel, RGN_OR );
			if( nCombineRetVal == RGN_ERROR )
			{
				ASSERT( FALSE );
				return NULL;
			}
		} // for( xIn = rcIn.left, xOut = rcOut.left; xIn < rcIn.right; xIn++, xOut++ )
	} // for( yIn = rcIn.top, yOut = rcOut.top; yIn < rcIn.bottom; yIn++, yOut++ )
	return ( ( _rgnRetVal.GetSafeHandle() != NULL ) ? ((HRGN)_rgnRetVal.Detach()) : NULL );
}

HRGN CExtBitmap::GenerateSkinPartColorRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcPadding,
	COLORREF clrTransparent,
	bool bLeft,
	bool bTop
	) const
{
	if( IsEmpty() )
		return NULL;
CSize _size = GetSize();
RECT rcSrc = { 0, 0, _size.cx, _size.cy };
	return
		GenerateSkinPartColorRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			clrTransparent,
			bLeft,
			bTop
			);
}

HRGN CExtBitmap::GenerateSkinFrameColorRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcSrc,
	const RECT & rcPadding,
	COLORREF clrTransparent
	) const
{
	if( IsEmpty() )
		return NULL;
	rcDst;
	rcSrc;
HRGN hRgnLT =
		GenerateSkinPartColorRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			clrTransparent,
			true,
			true
			);
HRGN hRgnLB =
		GenerateSkinPartColorRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			clrTransparent,
			true,
			false
			);
HRGN hRgnRT =
		GenerateSkinPartColorRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			clrTransparent,
			false,
			true
			);
HRGN hRgnRB =
		GenerateSkinPartColorRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			clrTransparent,
			false,
			false
			);
	if(		hRgnLT == NULL
		&&	hRgnLB == NULL
		&&	hRgnRT == NULL
		&&	hRgnRB == NULL
		)
		return NULL;
HRGN hRgnRetVal =
		::CreateRectRgnIndirect( &rcDst );
	if( hRgnRetVal != NULL )
	{
		if( hRgnLT != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnLT, RGN_DIFF );
		if( hRgnLB != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnLB, RGN_DIFF );
		if( hRgnRT != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnRT, RGN_DIFF );
		if( hRgnRB != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnRB, RGN_DIFF );
	} // if( hRgnRetVal != NULL )
	if( hRgnLT != NULL )
		DeleteObject( hRgnLT );
	if( hRgnLB != NULL )
		DeleteObject( hRgnLB );
	if( hRgnRT != NULL )
		DeleteObject( hRgnRT );
	if( hRgnRB != NULL )
		DeleteObject( hRgnRB );
	return hRgnRetVal; // NOT IMPL
}

HRGN CExtBitmap::GenerateSkinFrameColorRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcPadding,
	COLORREF clrTransparent
	) const
{
	if( IsEmpty() )
		return NULL;
CSize _size = GetSize();
RECT rcSrc = { 0, 0, _size.cx, _size.cy };
	return
		GenerateSkinFrameColorRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			clrTransparent
			);
}

HRGN CExtBitmap::GenerateSkinPartAlphaRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcSrc,
	const RECT & rcPadding,
	bool bLeft,
	bool bTop,
	BYTE nAlphaMin, // = BYTE(0)
	BYTE nAlphaMax // = BYTE(0)
	) const
{
	if( IsEmpty() )
		return NULL;
CRect rcIn = rcSrc;
CRect rcOut = rcDst;
	if( bLeft )
	{
		rcIn.right = rcIn.left + rcPadding.left;
		rcOut.right = rcOut.left + rcPadding.left;
	}
	else
	{
		rcIn.left = rcIn.right - rcPadding.right;
		rcOut.left = rcOut.right - rcPadding.right;
	}
	if( bTop )
	{
		rcIn.bottom = rcIn.top + rcPadding.top;
		rcOut.bottom = rcOut.top + rcPadding.top;
	}
	else
	{
		rcIn.top = rcIn.bottom - rcPadding.bottom;
		rcOut.top = rcOut.bottom - rcPadding.bottom;
	}
INT xIn, yIn, xOut, yOut;
CRgn _rgnRetVal;
	for( yIn = rcIn.top, yOut = rcOut.top; yIn < rcIn.bottom; yIn++, yOut++ )
	{
		for( xIn = rcIn.left, xOut = rcOut.left; xIn < rcIn.right; xIn++, xOut++ )
		{
			RGBQUAD _pixel;
			VERIFY( GetPixel( xIn, yIn, _pixel ) );
			if( !( nAlphaMin <= _pixel.rgbReserved && _pixel.rgbReserved <= nAlphaMax ) )
				continue;
			CRgn _rgnPixel;
			if( bRTL )
			{
				if( ! _rgnPixel.CreateRectRgn(
						rcOut.right - 1 - xOut + rcOut.left,
						yOut,
						rcOut.right - xOut + rcOut.left,
						yOut + 1
						)
					)
				{
					ASSERT( FALSE );
					return NULL;
				}
			} // if( bRTL )
			else
			{
				if( ! _rgnPixel.CreateRectRgn(
						xOut,
						yOut,
						xOut + 1,
						yOut + 1
						)
					)
				{
					ASSERT( FALSE );
					return NULL;
				}
			} // else from if( bRTL )
			if( _rgnRetVal.GetSafeHandle() == NULL )
			{
				if( ! _rgnRetVal.CreateRectRgn( 0, 0, 0, 0 ) )
				{
					ASSERT( FALSE );
					return NULL;
				}
			}
			int nCombineRetVal =
				_rgnRetVal.CombineRgn( &_rgnRetVal, &_rgnPixel, RGN_OR );
			if( nCombineRetVal == RGN_ERROR )
			{
				ASSERT( FALSE );
				return NULL;
			}
		} // for( xIn = rcIn.left, xOut = rcOut.left; xIn < rcIn.right; xIn++, xOut++ )
	} // for( yIn = rcIn.top, yOut = rcOut.top; yIn < rcIn.bottom; yIn++, yOut++ )
	return ( ( _rgnRetVal.GetSafeHandle() != NULL ) ? ((HRGN)_rgnRetVal.Detach()) : NULL );
}

HRGN CExtBitmap::GenerateSkinPartAlphaRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcPadding,
	bool bLeft,
	bool bTop,
	BYTE nAlphaMin, // = BYTE(0)
	BYTE nAlphaMax // = BYTE(0)
	) const
{
	if( IsEmpty() )
		return NULL;
CSize _size = GetSize();
RECT rcSrc = { 0, 0, _size.cx, _size.cy };
	return
		GenerateSkinPartAlphaRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			bLeft,
			bTop,
			nAlphaMin,
			nAlphaMax
			);
}

HRGN CExtBitmap::GenerateSkinFrameAlphaRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcSrc,
	const RECT & rcPadding,
	BYTE nAlphaMin, // = BYTE(0)
	BYTE nAlphaMax // = BYTE(0)
	) const
{
	if( IsEmpty() )
		return NULL;
	rcDst;
	rcSrc;
HRGN hRgnLT =
		GenerateSkinPartAlphaRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			true,
			true,
			nAlphaMin,
			nAlphaMax
			);
HRGN hRgnLB =
		GenerateSkinPartAlphaRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			true,
			false,
			nAlphaMin,
			nAlphaMax
			);
HRGN hRgnRT =
		GenerateSkinPartAlphaRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			false,
			true,
			nAlphaMin,
			nAlphaMax
			);
HRGN hRgnRB =
		GenerateSkinPartAlphaRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			false,
			false,
			nAlphaMin,
			nAlphaMax
			);
	if(		hRgnLT == NULL
		&&	hRgnLB == NULL
		&&	hRgnRT == NULL
		&&	hRgnRB == NULL
		)
		return NULL;
HRGN hRgnRetVal =
		::CreateRectRgnIndirect( &rcDst );
	if( hRgnRetVal != NULL )
	{
		if( hRgnLT != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnLT, RGN_DIFF );
		if( hRgnLB != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnLB, RGN_DIFF );
		if( hRgnRT != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnRT, RGN_DIFF );
		if( hRgnRB != NULL )
			::CombineRgn( hRgnRetVal, hRgnRetVal, hRgnRB, RGN_DIFF );
	} // if( hRgnRetVal != NULL )
	if( hRgnLT != NULL )
		DeleteObject( hRgnLT );
	if( hRgnLB != NULL )
		DeleteObject( hRgnLB );
	if( hRgnRT != NULL )
		DeleteObject( hRgnRT );
	if( hRgnRB != NULL )
		DeleteObject( hRgnRB );
	return hRgnRetVal; // NOT IMPL
}

HRGN CExtBitmap::GenerateSkinFrameAlphaRGN(
	bool bRTL,
	const RECT & rcDst,
	const RECT & rcPadding,
	BYTE nAlphaMin, // = BYTE(0)
	BYTE nAlphaMax // = BYTE(0)
	) const
{
	if( IsEmpty() )
		return NULL;
CSize _size = GetSize();
RECT rcSrc = { 0, 0, _size.cx, _size.cy };
	return
		GenerateSkinFrameAlphaRGN(
			bRTL,
			rcDst,
			rcSrc,
			rcPadding,
			nAlphaMin,
			nAlphaMax
			);
}

CSize CExtBitmap::GetSize() const
{
	if( IsEmpty() )
		return CSize( 0, 0 );
LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( m_pImageBuffer );
	return CSize( pBIH->biWidth, pBIH->biHeight );
}

#pragma pack(push, 1)

bool CExtBitmap::AssignFromHICON(
	HICON hIcon,
	bool bForceLowColorMode // = false
	)
{
// 	Empty();
// 	if( hIcon == NULL )
// 		return true;
// ICONINFO _iconInfo;
// 	::memset( &_iconInfo, 0, sizeof(ICONINFO) );
// 	if( ! ::GetIconInfo( hIcon, &_iconInfo ) )
// 		return false;
// 	ASSERT( _iconInfo.hbmColor != NULL || _iconInfo.hbmMask != NULL );
// CSize _sizeIcon(
// 		(INT)( (BYTE)(_iconInfo.xHotspot * 2 ) ),
// 		(INT)( (BYTE)(_iconInfo.yHotspot * 2 ) )
// 		);
// 	if( _sizeIcon.cx == 0 || _sizeIcon.cy == 0 )
// 	{
// 		BITMAP _bmpInfo;
// 		::memset( &_bmpInfo, 0, sizeof(BITMAP) );
// 		::GetObject(
// 			(_iconInfo.hbmColor != NULL) ? _iconInfo.hbmColor : _iconInfo.hbmMask,
// 			sizeof(BITMAP),
// 			&_bmpInfo
// 			);
// 		ASSERT( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 );
// 		_sizeIcon.cx = (INT)_bmpInfo.bmWidth;
// 		_sizeIcon.cy = (INT)_bmpInfo.bmHeight;
// 	} // if( m_sizeIcon.cx == 0 || m_sizeIcon.cy == 0 )
// 	ASSERT( _sizeIcon.cx > 0 );
// 	ASSERT( _sizeIcon.cy > 0 );
// 	if( _iconInfo.hbmColor == NULL && _iconInfo.hbmMask != NULL )
// 	{
// 		CRect rc( 0, 0, _sizeIcon.cx, _sizeIcon.cy );
// 		if( FromBitmap( _iconInfo.hbmMask, &rc, false, false ) )
// 		{
// 		} // if( FromBitmap ...
// 		else
// 			Empty();
// 	} // if( _iconInfo.hbmColor == NULL && _iconInfo.hbmMask != NULL )
// 	else if( _iconInfo.hbmColor != NULL )
// 	{
// 		if( FromBitmap( _iconInfo.hbmColor, false ) )
// 		{
// 			if( Make32( false ) )
// 			{
// 				if( _iconInfo.hbmMask != NULL )
// 				{
// 					CExtBitmap bmpMask;
// 					if(		bmpMask.FromBitmap( _iconInfo.hbmMask )
// 						&&	bmpMask.Make32( false )
// 						)
// 					{
// 						INT nX, nY;
// 						for( nY = 0; nY < _sizeIcon.cy; nY ++ )
// 						{
// 							for( nX = 0; nX < _sizeIcon.cx; nX ++ )
// 							{
// 								RGBQUAD _pixel;
// 								VERIFY( GetPixel( nX, nY, _pixel ) );
// 								COLORREF clrMaskPixel = bmpMask.GetPixel( nX, nY );
// 								if( clrMaskPixel == RGB(0,0,0) )
// 									_pixel.rgbReserved = 0xFF;
// 								else
// 									_pixel.rgbReserved = 0x0;
// 								VERIFY( SetPixel( nX, nY, _pixel ) );
// 							} // for( nX = 0; nX < _sizeIcon.cx; nX ++ )
// 						} // for( nY = 0; nY < _sizeIcon.cy; nY ++ )
// 					} // if( bmpMask.FromBitmap( _iconInfo.hbmMask ...
// 					else
// 						Empty();
// 				} // if( _iconInfo.hbmMask != NULL )
// 			} // if( Make32( false ) )
// 			else
// 				Empty();
// 		} // if( FromBitmap( _iconInfo.hbmColor, false ) )
// 	} // else if( _iconInfo.hbmColor != NULL )
// 	if( _iconInfo.hbmColor != NULL )
// 		::DeleteObject( _iconInfo.hbmColor );
// 	if( _iconInfo.hbmMask != NULL )
// 		::DeleteObject( _iconInfo.hbmMask );
// 	OnContentChanged();
// bool bIsEmpty = IsEmpty();
// 	return ( ! bIsEmpty );


	Empty();
	if( hIcon == NULL )
		return true;
ICONINFO _iconInfo;
	::memset( &_iconInfo, 0, sizeof(ICONINFO) );
	if( ! ::GetIconInfo( hIcon, &_iconInfo ) )
		return false;
	ASSERT( _iconInfo.hbmColor != NULL || _iconInfo.hbmMask != NULL );
CSize _sizeIcon(
		(INT)( (BYTE)(_iconInfo.xHotspot * 2 ) ),
		(INT)( (BYTE)(_iconInfo.yHotspot * 2 ) )
		);
BITMAP _bmpInfo;
	::memset( &_bmpInfo, 0, sizeof(BITMAP) );
	::GetObject(
		(_iconInfo.hbmColor != NULL) ? _iconInfo.hbmColor : _iconInfo.hbmMask,
		sizeof(BITMAP),
		&_bmpInfo
		);
	ASSERT( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 );
//	if( _sizeIcon.cx == 0 || _sizeIcon.cy == 0 )
	if( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 && _bmpInfo.bmBitsPixel > 1 )
	{
		_sizeIcon.cx = (INT)_bmpInfo.bmWidth;
		_sizeIcon.cy = (INT)_bmpInfo.bmHeight;
	} // if( m_sizeIcon.cx == 0 || m_sizeIcon.cy == 0 )
	//ASSERT( _sizeIcon.cx > 0 );
	//ASSERT( _sizeIcon.cy > 0 );
	if(		(! bForceLowColorMode )
///		&&	_bmpInfo.bmBitsPixel == 32
//// 		&&	_iconInfo.hbmColor != NULL
//// 		&&	_iconInfo.hbmMask == NULL
//		&&	g_PaintManager.m_bIsWinXPorLater
//		&&	CExtPaintManager::stat_GetBPP() >= 32
		)
	{ // XP alpha icons under Windows XP or later + 32 bit desktop
		CExtImageList _il;
		if(	_il.Create( _sizeIcon.cx, _sizeIcon.cy, ILC_COLOR32, 1, 1 ) )
		{
			if( _il.Add( hIcon ) >= 0 )
// 			if( _il.Add(
// 					CBitmap::FromHandle( _iconInfo.hbmColor ),
// 					CBitmap::FromHandle( _iconInfo.hbmMask )
// 					) >= 0
// 				)
			{
				IMAGEINFO _ii = { 0 };
				_il.GetImageInfo( 0, &_ii );
				ASSERT( _ii.hbmImage != NULL );
				ASSERT( _ii.hbmMask == NULL );
				if( ! FromBitmap(
						_ii.hbmImage,
						&_ii.rcImage,
						false,
						true
						)
					)
					Empty();
				else
				{
					bool bAssignedEmpty = true;
					for( INT nY = 0; nY < _sizeIcon.cy; nY++ )
					{
						for( INT nX = 0; nX < _sizeIcon.cx; nX++ )
						{
							RGBQUAD _pixel;
							VERIFY( GetPixel( nX, nY, _pixel ) );
							if( _pixel.rgbReserved != 0 )
							{
								bAssignedEmpty = false;
								break;
							}
						}
						if( ! bAssignedEmpty )
							break;
					}
					if( bAssignedEmpty )
						Empty();
				}
				::DeleteObject( _ii.hbmImage );
			}
			_il.DeleteImageList();
		}
		//////////////::DestroyIcon( hIcon );
	} // XP alpha icons under Windows XP or later + 32 bit desktop
	
	if( IsEmpty() )
	{ // old style icon
		if( _iconInfo.hbmColor == NULL && _iconInfo.hbmMask != NULL )
		{
			CRect rc( 0, 0, _sizeIcon.cx, _sizeIcon.cy );
			if( FromBitmap( _iconInfo.hbmMask, &rc, false, false ) )
			{
			} // if( FromBitmap ...
			else
				Empty();
		} // if( _iconInfo.hbmColor == NULL && _iconInfo.hbmMask != NULL )
		else if( _iconInfo.hbmColor != NULL )
		{
			if( FromBitmap( _iconInfo.hbmColor, false ) )
			{
				if( Make32( false ) )
				{
					if( _iconInfo.hbmMask != NULL )
					{
						CExtBitmap bmpMask;
						if(		bmpMask.FromBitmap( _iconInfo.hbmMask )
							&&	bmpMask.Make32( false )
							)
						{
							INT nX, nY;
							for( nY = 0; nY < _sizeIcon.cy; nY ++ )
							{
								for( nX = 0; nX < _sizeIcon.cx; nX ++ )
								{
									RGBQUAD _pixel;
									VERIFY( GetPixel( nX, nY, _pixel ) );
									COLORREF clrMaskPixel = bmpMask.GetPixel( nX, nY );
									if( clrMaskPixel == RGB(0,0,0) )
										_pixel.rgbReserved = 0xFF;
									else
										_pixel.rgbReserved = 0x0;
									VERIFY( SetPixel( nX, nY, _pixel ) );
								} // for( nX = 0; nX < _sizeIcon.cx; nX ++ )
							} // for( nY = 0; nY < _sizeIcon.cy; nY ++ )
						} // if( bmpMask.FromBitmap( _iconInfo.hbmMask ...
						else
							Empty();
					} // if( _iconInfo.hbmMask != NULL )
				} // if( Make32( false ) )
				else
					Empty();
			} // if( FromBitmap( _iconInfo.hbmColor, false ) )
		} // else if( _iconInfo.hbmColor != NULL )
	} // old style icon
	if( _iconInfo.hbmColor != NULL )
		::DeleteObject( _iconInfo.hbmColor );
	if( _iconInfo.hbmMask != NULL )
		::DeleteObject( _iconInfo.hbmMask );
bool bIsEmpty = IsEmpty();
	if( ! bIsEmpty )
		OnLoad();
	return ( ! bIsEmpty );
}

bool CExtBitmap::AssignFromOther(
	const CExtBitmap & other,
	bool bEnableThrowException // = false
	)
{
	Empty();
	if( other.IsEmpty() )
		return true;
CSize _size = other.GetSize();
ULONG nBPP = other.GetBPP();
	m_pImageBuffer =
		stat_AllocHL(
			_size.cx,
			_size.cy,
			nBPP,
			stat_MaskRed( LPBYTE( other.m_pImageBuffer ) ),
			stat_MaskGreen( LPBYTE( other.m_pImageBuffer ) ),
			stat_MaskBlue( LPBYTE( other.m_pImageBuffer ) ),
			stat_MaskAlpha( LPBYTE( other.m_pImageBuffer ) ),
			stat_RunTimeFlagsGet( LPBYTE( other.m_pImageBuffer ) )
			);
	if( m_pImageBuffer == NULL )
	{
		if( bEnableThrowException )
			::AfxThrowUserException();
		return false;
	}
	UINT nSizeInBytes = 
		stat_RcImageBufferSize(
			_size.cx,
			_size.cy,
			nBPP
			) - sizeof( BITMAPINFOHEADER );
	__EXT_MFC_MEMCPY(
		LPBYTE( stat_RcInfoHeader( m_pImageBuffer ) )
			+ sizeof( BITMAPINFOHEADER ),
		nSizeInBytes,
		LPBYTE( stat_RcInfoHeader( LPBYTE( other.m_pImageBuffer ) ) )
			+ sizeof( BITMAPINFOHEADER ),
		nSizeInBytes
		);
	OnLoad();
	return true;
}

ULONG CExtBitmap::GetBPP() const
{
	if( IsEmpty() )
		return 0;
LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( m_pImageBuffer );
	return pBIH->biBitCount;
}

ULONG CExtBitmap::stat_RcLine(
	ULONG nWidth,
	ULONG nBPP
	)
{
	return ( ( nWidth * nBPP ) + 7 ) / 8;
}

ULONG CExtBitmap::stat_RcScanLineSize(
	ULONG nLineIndex
	)
{
	return nLineIndex + 3 & (~3);
}

ULONG CExtBitmap::stat_RcScanLineSize(
	ULONG nWidth,
	ULONG nBPP
	)
{
	return stat_RcScanLineSize( stat_RcLine( nWidth, nBPP ) );
}

ULONG CExtBitmap::stat_RcUPE( ULONG nBPP )
{
	if(		nBPP >= 1
		&&	nBPP <= 8
		)
		return ( 1 << nBPP );
	return 0;
}

LPBYTE CExtBitmap::stat_RcOffsetToScanLine(
	LPBYTE pBitsStart,
	ULONG nScanLineSize,
	ULONG nLineIndex
	)
{
	return ( pBitsStart + ( nScanLineSize * nLineIndex ) );
}

ULONG CExtBitmap::stat_RcImageBufferSize(
	ULONG nWidth,
	ULONG hHeight,
	ULONG nBPP
	)
{
ULONG nImageBufferSize = sizeof( BITMAPINFOHEADER );
ULONG nUPE = stat_RcUPE( nBPP );
	nImageBufferSize += sizeof( RGBQUAD ) * nUPE;
	if( ( nImageBufferSize % 16 ) != 0 )
	{
		ULONG nAlignmentRest =
			16 - ( nImageBufferSize % 16 );
		nImageBufferSize += nAlignmentRest;  
	}
ULONG nScanLineSize = stat_RcScanLineSize( nWidth, nBPP );
	nImageBufferSize += nScanLineSize * hHeight; 
	return nImageBufferSize;
}

LPBITMAPINFOHEADER CExtBitmap::stat_RcInfoHeader(
	LPBYTE pImageBuffer
	)
{
	ASSERT( pImageBuffer != NULL );
	return ( (LPBITMAPINFOHEADER) pImageBuffer );
}

LPBITMAPINFO CExtBitmap::stat_RcInfo(
	LPBYTE pImageBuffer
	)
{
	ASSERT( pImageBuffer != NULL );
	return ( (LPBITMAPINFO) pImageBuffer );
}

LPBYTE CExtBitmap::stat_RcScanLine(
	LPBYTE pImageBuffer,
	ULONG nLineIndex,
	ULONG nScanLineSize
	)
{
	ASSERT( pImageBuffer != NULL );
LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
ULONG nUPE = stat_RcUPE( pBIH->biBitCount );
ULONG nOffset = sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * nUPE;
	if( ( nOffset % 16 ) != 0 )
	{
		ULONG nAlignmentRest =
			16 - ( nOffset % 16 );
		nOffset += nAlignmentRest;  
	}
	return
		stat_RcOffsetToScanLine(
			pImageBuffer,
			nScanLineSize,
			nLineIndex
			) + nOffset;
}

LPBYTE CExtBitmap::stat_RcSurface(
	LPBYTE pImageBuffer
	)
{
	ASSERT( pImageBuffer != NULL );
LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
ULONG nUPE = stat_RcUPE( pBIH->biBitCount );
ULONG nOffset = sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * nUPE;
	if( ( nOffset % 16 ) != 0 )
	{
		ULONG nAlignmentRest =
			16 - ( nOffset % 16 );
		nOffset += nAlignmentRest;  
	}
	return pImageBuffer + nOffset;
}

LPVOID CExtBitmap::stat_AllocLL(
	__EXT_MFC_ULONG_PTR amount,
	__EXT_MFC_ULONG_PTR alignment
	)
{
	ASSERT( alignment >= sizeof(__EXT_MFC_ULONG_PTR) );
LPBYTE mem_real = new BYTE[ amount + alignment * 2 ];
LPBYTE mem_align = (LPBYTE)
		( (__EXT_MFC_ULONG_PTR)
				(	alignment
				-	( (__EXT_MFC_ULONG_PTR)mem_real % (__EXT_MFC_ULONG_PTR)alignment )
				)
			+ (__EXT_MFC_ULONG_PTR)mem_real
		);
	mem_align += alignment;
	*((__EXT_MFC_ULONG_PTR*)mem_align - 1) = (__EXT_MFC_ULONG_PTR)mem_real;
	return mem_align;
}

void CExtBitmap::stat_FreeLL( LPVOID pMemToFree )
{
	pMemToFree = (LPVOID)*((__EXT_MFC_ULONG_PTR*)pMemToFree-1);
LPBYTE pMemToDelete = (LPBYTE)pMemToFree;
	delete [] pMemToDelete;
}

LPBYTE CExtBitmap::stat_AllocHL(
	ULONG nWidth,
	ULONG hHeight,
	ULONG nBPP,
	ULONG nMaskRed,
	ULONG nMaskGreen,
	ULONG nMaskBlue,
	ULONG nMaskAlpha,
	ULONG nFlags
	)
{
	if(		nBPP != 1
		&&	nBPP != 4
		&&	nBPP != 8
		&&	nBPP != 16
		&&	nBPP != 24
		&&	nBPP != 32
		)
		nBPP = 8;
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
ULONG nImageBufferSize = stat_RcImageBufferSize( nWidth, hHeight, nBPP );
	nImageBufferSize += nMaskBufferSize; 
LPBYTE pImageBuffer = (LPBYTE)
		stat_AllocLL(
			nImageBufferSize * sizeof( BYTE ),
			16
			);
	::memset( pImageBuffer, 0, nImageBufferSize );
ULONG * pMaskBuffer = (ULONG *) pImageBuffer;
	pMaskBuffer[0] = nMaskRed;
	pMaskBuffer[1] = nMaskGreen;
	pMaskBuffer[2] = nMaskBlue;
	pMaskBuffer[3] = nMaskAlpha;
	pMaskBuffer[4] = nFlags;
LPBITMAPINFOHEADER pBIH = (LPBITMAPINFOHEADER)( LPBYTE(pImageBuffer) + nMaskBufferSize );
	pBIH->biSize = sizeof( BITMAPINFOHEADER );
	pBIH->biWidth = nWidth;
	pBIH->biHeight = hHeight;
	pBIH->biPlanes = 1;
	pBIH->biCompression = 0;
	pBIH->biBitCount = (WORD)nBPP;
	pBIH->biClrUsed = stat_RcUPE( nBPP );
	pBIH->biClrImportant = pBIH->biClrUsed;
	return LPBYTE(pBIH);
}

void CExtBitmap::stat_FreeHL( LPVOID pMemToFree )
{
LPBYTE pMemToFreeReal = LPBYTE( pMemToFree );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pMemToFreeReal -= nMaskBufferSize;
	stat_FreeLL( pMemToFreeReal );
}

LPBYTE CExtBitmap::stat_BmpLoad(
	CArchive & ar,
	bool bEnableThrowException,
	bool bResourceFormat
	)
{
	ASSERT( ar.IsLoading() );
LPBYTE pImageBuffer = NULL;
	try
	{
		if( ! bResourceFormat )
		{
			BITMAPFILEHEADER _BFH;
			ar.Read( &_BFH, sizeof( BITMAPFILEHEADER) );
		} // if( ! bResourceFormat )
		BITMAPINFOHEADER _BIH;
		ar.Read( &_BIH, sizeof( BITMAPINFOHEADER ) );
		int nWidth = _BIH.biWidth;
		int hHeight = _BIH.biHeight;
		int nBPP = _BIH.biBitCount;
		int nCompression = _BIH.biCompression;
		int nClrUsed = _BIH.biClrUsed;
		int nScanLineSize = stat_RcScanLineSize( nWidth, nBPP );
		ULONG nUPE = stat_RcUPE( nBPP );
		switch( nBPP )
		{
		case 32:
			if( _BIH.biCompression == BI_BITFIELDS )
			{
				int nSkipIndex;
				for( nSkipIndex = 0; nSkipIndex < 5; nSkipIndex++ )
				{
					BYTE _byte;
					ar.Read( &_byte, sizeof( BYTE ) );
				} // for( nSkipIndex = 0; nSkipIndex < 5; nSkipIndex++ )
			}
			// continue to 24
		case 24:
		{
			if( _BIH.biCompression == BI_BITFIELDS )
			{
				DWORD arrBitMaskDefinions[3];
				ar.Read(
					arrBitMaskDefinions,
					3 * sizeof( DWORD )
					);
				pImageBuffer =
					stat_AllocHL(
						nWidth,
						hHeight,
						nBPP,
						arrBitMaskDefinions[0],
						arrBitMaskDefinions[1],
						arrBitMaskDefinions[2],
						0,
						0 // run-time flags
						);
			}
			else
			{
				if( nBPP == 32 )
					pImageBuffer =
						stat_AllocHL(
							nWidth,
							hHeight,
							nBPP,
							0x00FF0000,
							0x0000FF00,
							0x000000FF,
							0xFF000000,
							0 // run-time flags
							);
				else
					pImageBuffer =
						stat_AllocHL(
							nWidth,
							hHeight,
							nBPP,
							0x00FF0000,
							0x0000FF00,
							0x000000FF,
							0,
							0 // run-time flags
							);
			}
			LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
			pBIH->biXPelsPerMeter = _BIH.biXPelsPerMeter;
			pBIH->biYPelsPerMeter = _BIH.biYPelsPerMeter;
			ar.Read(
				stat_RcSurface( pImageBuffer ),
				hHeight * nScanLineSize
				);
		} // 32, 24
		break;
		case 16:
		{
			if( _BIH.biCompression == BI_BITFIELDS )
			{
				int nSkipIndex;
				for( nSkipIndex = 0; nSkipIndex < 4; nSkipIndex++ )
				{
					BYTE _byte;
					ar.Read( &_byte, sizeof( BYTE ) );
				}
				DWORD arrBitMaskDefinions[3];
				ar.Read( arrBitMaskDefinions, 3 * sizeof( DWORD ) );
				pImageBuffer =
					stat_AllocHL(
						nWidth,
						hHeight,
						nBPP,
						arrBitMaskDefinions[0],
						arrBitMaskDefinions[1],
						arrBitMaskDefinions[2],
						0,
						0 // run-time flags
						);
			} // if( _BIH.biCompression == BI_BITFIELDS )
			else
				pImageBuffer =
					stat_AllocHL(
						nWidth,
						hHeight,
						nBPP,
						0x7C00,
						0x03E0,
						0x001F,
						0,
						0 // run-time flags
						);
			LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
			pBIH->biXPelsPerMeter = _BIH.biXPelsPerMeter;
			pBIH->biYPelsPerMeter = _BIH.biYPelsPerMeter;
			ar.Read(
				stat_RcSurface( pImageBuffer ),
				hHeight * nScanLineSize
				);
		} // 16
		break;
		case 8:
		case 4:
		case 1:
		{
			if(		nClrUsed <= 0 // ( nClrUsed <= 0 && (!(nClrUsed==0 && nBPP==1)) )
				||	nClrUsed > int(nUPE)
				)
				nClrUsed = int(nUPE);
			pImageBuffer =
				stat_AllocHL(
					nWidth,
					hHeight,
					nBPP,
					0,
					0,
					0,
					0,
					0 // run-time flags
					);
			LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
			pBIH->biXPelsPerMeter = _BIH.biXPelsPerMeter;
			pBIH->biYPelsPerMeter = _BIH.biYPelsPerMeter;
			RGBQUAD * pPalette = (RGBQUAD *)( pImageBuffer + sizeof( BITMAPINFOHEADER ) );
			ar.Read( pPalette, nClrUsed * sizeof( RGBQUAD ) );
			switch( nCompression )
			{
			case BI_RGB:
				if( hHeight > 0 )
					ar.Read(
						stat_RcSurface( pImageBuffer ),
						hHeight * nScanLineSize
						);
				else
				{
					int nLineIndex, nAbsHeight = abs( hHeight );
					for( nLineIndex = 0; nLineIndex < nAbsHeight; nLineIndex++ )
					{
						ar.Read(
							stat_RcScanLine(
								pImageBuffer,
								nAbsHeight - nLineIndex - 1,
								nScanLineSize
								),
							nScanLineSize
							);
					} // for( nLineIndex = 0; nLineIndex < nAbsHeight; nLineIndex++ )
				}
			break;
			case BI_RLE4:
			{
				BYTE _byte0 = 0, _byte1 = 0;
				int nLineIndex = 0, nBitCount = 0, nAlignState = 0;
				bool bLowPartOfByte = false;
				for( ; true; )
				{
					ar.Read( &_byte0, sizeof( BYTE ) );
					switch( _byte0 )
					{
					case 0:
						ar.Read( &_byte0, sizeof( BYTE ) );
						switch( _byte0 )
						{
							case 0:
								nBitCount = 0;
								nLineIndex ++;
								bLowPartOfByte = false;
								break;
							case 1:
								return pImageBuffer;
							case 2:
								{
									BYTE nDX = 0, nDY = 0;
									ar.Read( &nDX, sizeof( BYTE ) );
									ar.Read( &nDY, sizeof( BYTE ) );
									nBitCount += nDX / 2;
									nLineIndex += nDY;
								}
							break;
							default:
							{
								ar.Read( &_byte1, sizeof( BYTE ) );
								LPBYTE pScanLine =
									stat_RcScanLine(
										pImageBuffer,
										nLineIndex,
										nScanLineSize
										);
								INT nLP;
								for( nLP = 0; nLP < _byte0; nLP++ )
								{
									if( bLowPartOfByte )
									{
										(*( pScanLine + nBitCount )) |= _byte1 & 0x0F;
										if( nLP != _byte0 - 1 )
											ar.Read( &_byte1, sizeof( BYTE ) );
										nBitCount++;
									} // if( bLowPartOfByte )
									else
										(*( pScanLine + nBitCount )) |= _byte1&0xF0;
									bLowPartOfByte = !bLowPartOfByte;
								} // for( nLP = 0; nLP < _byte0; nLP++ )
								nAlignState = _byte0 % 4;
								if(		nAlignState == 1
									||	nAlignState == 2
									)
									ar.Read( &_byte1, sizeof( BYTE ) );
							}
							break;
						}
					break;
					default:
					{
						LPBYTE pScanLine =
							stat_RcScanLine(
								pImageBuffer,
								nLineIndex,
								nScanLineSize
								);
						ar.Read( &_byte1, sizeof( BYTE ) );
						ULONG nLP;
						for( nLP = 0; nLP < _byte0; nLP++ )
						{
							if( bLowPartOfByte )
							{
								(*( pScanLine + nBitCount )) |= _byte1&0x0F;
								nBitCount++;
							} // if( bLowPartOfByte )
							else
								(*( pScanLine + nBitCount )) |= _byte1&0xF0;
							bLowPartOfByte = !bLowPartOfByte;
						} // for( nLP = 0; nLP < _byte0; nLP++ )
					}
					break;
					} // switch( _byte0 )
				} // for( ; true; )
			}
			break;
			case BI_RLE8:
			{
				BYTE _byte0 = 0, _byte1 = 0;
				int nLineIndex = 0, nBitCount = 0;
				for( ; true; )
				{
					ar.Read( &_byte0, sizeof( BYTE ) );
					switch( _byte0 )
					{
					case 0:
						ar.Read( &_byte0, sizeof( BYTE ) );
						switch( _byte0 )
						{
						case 0:
							nBitCount = 0;
							nLineIndex ++;
						break;
						case 1:
							return pImageBuffer;
						case 2:
							{
								BYTE nDX = 0, nDY = 0;
								ar.Read( &nDX, sizeof( BYTE ) );
								ar.Read( &nDY, sizeof( BYTE ) );
								nBitCount += nDX;
								nLineIndex += nDY;
							}
						break;
						default:
							{
								if( nLineIndex >= hHeight )
									return pImageBuffer;
								int nCount = min( (int)_byte0, (nWidth - nBitCount) );
								ar.Read(
									stat_RcScanLine(
										pImageBuffer,
										nLineIndex,
										nScanLineSize
										) + nBitCount,
									sizeof( BYTE ) * nCount
									);
								if( ( _byte0 & 1 ) == 1)
									ar.Read( &_byte1, sizeof( BYTE ) );
								nBitCount += _byte0;													
								break;	
							}
						}
					break;
					default:
						{
							if( nLineIndex >= hHeight )
								return pImageBuffer;
							INT nCount = min( (int)_byte0, (nWidth - nBitCount) );
							LPBYTE pScanLine =
								stat_RcScanLine(
									pImageBuffer,
									nLineIndex,
									nScanLineSize
									);
							ar.Read( &_byte1, sizeof( BYTE ) );
							INT nIndex;
							for( nIndex = 0; nIndex < nCount; nIndex++ )
							{
								(*( pScanLine + nBitCount )) = _byte1;
								nBitCount++;					
							} // for( nIndex = 0; nIndex < nCount; nIndex++ )
							break;
						}
					} // switch( _byte0 )
				} // for( ; true; )
				break;
			}
// 			default:
// #if _MFC_VER >= 0x0800
// 				::AfxThrowArchiveException( CArchiveException::genericException );
// #else
// 				::AfxThrowArchiveException( CArchiveException::generic );
// #endif
// 			break;
			} // switch( nCompression )
		} // 8, 4, 1
		break;
		} // switch( nBPP )
	} // try
	catch( CException * pException )
	{
		if( pImageBuffer != NULL )
		{
			delete [] pImageBuffer;
			pImageBuffer = NULL;
		}
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( pImageBuffer != NULL )
		{
			delete [] pImageBuffer;
			pImageBuffer = NULL;
		}
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return pImageBuffer;
}

bool CExtBitmap::stat_BmpSave(
	LPBYTE pImageBuffer,
	CArchive & ar,
	bool bEnableThrowException,
	bool bUseIndexedRLE
	)
{
	ASSERT( ar.IsStoring() );
	ASSERT( pImageBuffer != NULL );
LPBYTE pBufferForRLE = NULL;
	try
	{
		LPBITMAPINFOHEADER pBIH = stat_RcInfoHeader( pImageBuffer );
		BITMAPFILEHEADER _BFH;
		_BFH.bfType = 0x4D42;
		_BFH.bfSize =
			sizeof( BITMAPFILEHEADER )
			+ sizeof( BITMAPINFOHEADER )
			+ pBIH->biWidth * pBIH->biHeight;
		_BFH.bfOffBits =
			sizeof( BITMAPFILEHEADER )
			+ sizeof( BITMAPINFOHEADER )
			+ pBIH->biClrUsed * sizeof( RGBQUAD );
		_BFH.bfReserved1 = 0;
		_BFH.bfReserved2 = 0;
		bool bSaveBitFields16 = ( pBIH->biBitCount == 16 ) ? true : false;
		if( bSaveBitFields16 )
		{
			_BFH.bfSize += 3 * sizeof( DWORD );
			_BFH.bfOffBits += 3 * sizeof( DWORD );
		}
		ar.Write( &_BFH, sizeof( BITMAPFILEHEADER ) );
		BITMAPINFOHEADER _BIH;
		__EXT_MFC_MEMCPY(
			&_BIH,
			sizeof( BITMAPINFOHEADER ),
			pBIH,
			sizeof( BITMAPINFOHEADER )
			);
		if( bSaveBitFields16 )
			_BIH.biCompression = BI_BITFIELDS;
		else if( _BIH.biBitCount == 8 && bUseIndexedRLE )
			_BIH.biCompression = BI_RLE8;
		else
			_BIH.biCompression = BI_RGB;
		ar.Write( &_BIH, sizeof( BITMAPINFOHEADER ) );
		if( bSaveBitFields16 )
		{
			DWORD dwVal;
			dwVal = (DWORD)stat_MaskRed( pImageBuffer );
			ar.Write( &dwVal, sizeof( DWORD ) );
			dwVal = (DWORD)stat_MaskGreen( pImageBuffer );
			ar.Write( &dwVal, sizeof( DWORD ) );
			dwVal = (DWORD)stat_MaskBlue( pImageBuffer );
			ar.Write( &dwVal, sizeof( DWORD ) );
		} // if( bSaveBitFields16 )
		if( pBIH->biBitCount < 16 )
		{
			RGBQUAD * pPalette = (RGBQUAD *)( pImageBuffer + sizeof( BITMAPINFOHEADER ) );
			typedef struct { BYTE m_nBlue, m_nGreen, m_nRed, m_nAlpha; } _BRGA;
			_BRGA _brga;
			DWORD dwClrIndex;
			for( dwClrIndex = 0; dwClrIndex < pBIH->biClrUsed; dwClrIndex++ )
			{
				_brga.m_nBlue = pPalette[dwClrIndex].rgbBlue;
				_brga.m_nGreen = pPalette[dwClrIndex].rgbGreen;
				_brga.m_nRed = pPalette[dwClrIndex].rgbRed;
				_brga.m_nAlpha = pPalette[dwClrIndex].rgbReserved;
				ar.Write( &_brga, sizeof( _BRGA ) );
			} // for( dwClrIndex = 0; dwClrIndex < pBIH->biClrUsed; dwClrIndex++ )
		} // if( pBIH->biBitCount < 16 )
		ULONG nScanLineSize = stat_RcScanLineSize( pBIH->biWidth, pBIH->biBitCount );
		if( pBIH->biBitCount == 8 && bUseIndexedRLE )
		{
			INT nSizeToEncode = stat_RcLine( pBIH->biWidth, pBIH->biBitCount );
			pBufferForRLE = new BYTE[ nScanLineSize * 2 ];
			LONG nLineIndex;
			for( nLineIndex = 0; nLineIndex < pBIH->biHeight; ++nLineIndex )
			{
				LPBYTE pScanPosDst = pBufferForRLE;
				LPBYTE pScanPosSrc = stat_RcScanLine( pImageBuffer, nLineIndex, nScanLineSize );
				BYTE arrLocalBuffer[256];
				INT nLocalBufferSize = 0, nProcessedPos = 0, nPackPos;
				for( nPackPos = 0; nPackPos < nSizeToEncode; ++nPackPos )
				{
					if( ( nPackPos < ( nSizeToEncode - 1) ) && ( pScanPosSrc[nPackPos] == pScanPosSrc[ nPackPos + 1 ] ) )
					{
						INT nSecondPos = nPackPos + 1;
						while( ( nSecondPos < ( nSizeToEncode - 1 ) ) && ( pScanPosSrc[ nSecondPos ] == pScanPosSrc[ nSecondPos + 1 ] ) )
							++ nSecondPos;
						if( ( ( nSecondPos - nPackPos ) + 1 ) > 3 )
						{
							switch( nLocalBufferSize )
							{
							case 0:
							break;
							case 2:
								pScanPosDst[ nProcessedPos ++ ] = 1;
								pScanPosDst[ nProcessedPos ++ ] = arrLocalBuffer[0];
								pScanPosDst[ nProcessedPos ++ ] = 1;
								pScanPosDst[ nProcessedPos ++ ] = arrLocalBuffer[1];
							break;
							case 1:
								pScanPosDst[ nProcessedPos ++ ] = BYTE( nLocalBufferSize );
								pScanPosDst[ nProcessedPos ++ ] = arrLocalBuffer[0];
							break;
							default:
								pScanPosDst[ nProcessedPos ++ ] = 0;
								pScanPosDst[ nProcessedPos ++ ] = BYTE( nLocalBufferSize );
								__EXT_MFC_MEMCPY(
									pScanPosDst + nProcessedPos, 
									nLocalBufferSize,
									arrLocalBuffer, 
									nLocalBufferSize
									);
								nProcessedPos += nLocalBufferSize;
								if( ( nLocalBufferSize & 1 ) == 1 )
									nProcessedPos ++;
							break;
							} // switch( nLocalBufferSize )
							pScanPosDst[ nProcessedPos ++ ] = BYTE( (nSecondPos - nPackPos) + 1 );
							pScanPosDst[ nProcessedPos ++ ] = pScanPosSrc[nPackPos];
							nLocalBufferSize = 0;
						} // if( ( ( nSecondPos - nPackPos ) + 1 ) > 3 )
						else
						{
							INT nRestPos;
							for( nRestPos = 0; nRestPos < (nSecondPos - nPackPos) + 1; ++nRestPos)
							{
								arrLocalBuffer[ nLocalBufferSize ++ ] = pScanPosSrc[nPackPos + nRestPos];

								if( nLocalBufferSize == 254 )
								{
									pScanPosDst[ nProcessedPos ++ ] = 0;
									pScanPosDst[ nProcessedPos ++ ] = BYTE( nLocalBufferSize );
									__EXT_MFC_MEMCPY(
										pScanPosDst + nProcessedPos,
										nLocalBufferSize,
										arrLocalBuffer,
										nLocalBufferSize
										);
									nProcessedPos += nLocalBufferSize;
									nLocalBufferSize = 0;
								} // if( nLocalBufferSize == 254 )
							} // for( nRestPos = 0; nRestPos < (nSecondPos - nPackPos) + 1; ++nRestPos)
						} // else from if( ( ( nSecondPos - nPackPos ) + 1 ) > 3 )
						nPackPos = nSecondPos;
					} // if( ( nPackPos < ( nSizeToEncode - 1) ) && ( pScanPosSrc[nPackPos] == pScanPosSrc[ nPackPos + 1 ] ) )
					else
						arrLocalBuffer[ nLocalBufferSize ++ ] = pScanPosSrc[nPackPos];
					if( nLocalBufferSize == 254 )
					{
						pScanPosDst[ nProcessedPos ++ ] = 0;
						pScanPosDst[ nProcessedPos ++ ] = BYTE( nLocalBufferSize );
						__EXT_MFC_MEMCPY(
							pScanPosDst + nProcessedPos,
							nLocalBufferSize,
							arrLocalBuffer,
							nLocalBufferSize
							);
						nProcessedPos += nLocalBufferSize;
						nLocalBufferSize = 0;
					} // if( nLocalBufferSize == 254 )
				} // for( nPackPos = 0; nPackPos < nSizeToEncode; ++nPackPos )
				switch( nLocalBufferSize )
				{
				case 0:
				break;
				case 2:
					pScanPosDst[ nProcessedPos ++ ] = 1;
					pScanPosDst[ nProcessedPos ++ ] = arrLocalBuffer[0];
					pScanPosDst[ nProcessedPos ++ ] = 1;
					pScanPosDst[ nProcessedPos ++ ] = arrLocalBuffer[1];
				break;
				case 1:
					pScanPosDst[ nProcessedPos ++ ] = BYTE( nLocalBufferSize );
					pScanPosDst[ nProcessedPos ++ ] = arrLocalBuffer[0];
				break;
				default:
					pScanPosDst[ nProcessedPos ++ ] = 0;
					pScanPosDst[ nProcessedPos ++ ] = BYTE( nLocalBufferSize );
					__EXT_MFC_MEMCPY(
						pScanPosDst + nProcessedPos, 
						nLocalBufferSize,
						arrLocalBuffer, 
						nLocalBufferSize
						);
					nProcessedPos += nLocalBufferSize;
					if( (nLocalBufferSize & 1) == 1 )
						nProcessedPos ++;
					break;			
				} // switch( nLocalBufferSize )
				pScanPosDst[ nProcessedPos ++ ] = 0;
				pScanPosDst[ nProcessedPos ++ ] = 0;
				ar.Write( pBufferForRLE, nProcessedPos );
			} // for( nLineIndex = 0; nLineIndex < pBIH->biHeight; ++nLineIndex )
			pBufferForRLE[0] = 0;
			pBufferForRLE[1] = 1;
			ar.Write( pBufferForRLE, 2 );
			delete [] pBufferForRLE;
			pBufferForRLE = NULL;
		} // if( pBIH->biBitCount == 8 && bUseIndexedRLE )
		else
			ar.Write(
				stat_RcSurface( pImageBuffer ),
				pBIH->biHeight * nScanLineSize
				);
		return true;
	} // try
	catch( CException * pException )
	{
		if( pBufferForRLE != NULL )
			delete [] pBufferForRLE;
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( pBufferForRLE != NULL )
			delete [] pBufferForRLE;
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return false;
}

ULONG CExtBitmap::stat_MaskRed( LPBYTE pImageBuffer )
{
	ASSERT( pImageBuffer != NULL );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pImageBuffer -= nMaskBufferSize;
ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
	return pMaskBuffer[0];
}

ULONG CExtBitmap::stat_MaskGreen( LPBYTE pImageBuffer )
{
	ASSERT( pImageBuffer != NULL );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pImageBuffer -= nMaskBufferSize;
ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
	return pMaskBuffer[1];
}

ULONG CExtBitmap::stat_MaskBlue( LPBYTE pImageBuffer )
{
	ASSERT( pImageBuffer != NULL );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pImageBuffer -= nMaskBufferSize;
ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
	return pMaskBuffer[2];
}

ULONG CExtBitmap::stat_MaskAlpha( LPBYTE pImageBuffer )
{
	ASSERT( pImageBuffer != NULL );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pImageBuffer -= nMaskBufferSize;
ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
	return pMaskBuffer[3];
}

ULONG CExtBitmap::stat_RunTimeFlagsGet( LPBYTE pImageBuffer )
{
	ASSERT( pImageBuffer != NULL );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pImageBuffer -= nMaskBufferSize;
ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
	return pMaskBuffer[4];
}

void CExtBitmap::stat_RunTimeFlagsSet( LPBYTE pImageBuffer, ULONG nFlags )
{
	ASSERT( pImageBuffer != NULL );
ULONG nMaskBufferSize = max( sizeof(ULONG) * 5, 32 );
	pImageBuffer -= nMaskBufferSize;
ULONG * pMaskBuffer = (ULONG *)pImageBuffer;
	pMaskBuffer[4] = nFlags;
}

bool CExtBitmap::stat_PreMultipliedRGBChannelsGet( LPBYTE pImageBuffer )
{
ULONG nFlags = stat_RunTimeFlagsGet( pImageBuffer );
	if( ( nFlags & __EXT_BMP_FLAG_PREMULTIPLIED_RGB_CHANNELS ) != 0 )
		return true;
	else
		return false;
}

void CExtBitmap::stat_PreMultipliedRGBChannelsSet( LPBYTE pImageBuffer, bool bPreMultiplied )
{
ULONG nFlags = stat_RunTimeFlagsGet( pImageBuffer );
	if( bPreMultiplied )
		nFlags |= __EXT_BMP_FLAG_PREMULTIPLIED_RGB_CHANNELS;
	else
		nFlags &= ~(__EXT_BMP_FLAG_PREMULTIPLIED_RGB_CHANNELS);
	stat_RunTimeFlagsSet( pImageBuffer, nFlags );
}

LPBYTE CExtBitmap::stat_RcTo32(
	LPBYTE pImageBuffer,
	bool bEnableThrowException
	)
{
	ASSERT( pImageBuffer != NULL );
LPBITMAPINFOHEADER pBIH = (LPBITMAPINFOHEADER)pImageBuffer;
ULONG nBPP = pBIH->biBitCount;
ULONG nLineIndex, nX;
ULONG nWidth = pBIH->biWidth;
ULONG hHeight = pBIH->biHeight;
ULONG nScanLineSizeSrc = stat_RcScanLineSize( nWidth, nBPP );
ULONG nScanLineSizeDst = stat_RcScanLineSize( nWidth, 32 );
LPBYTE pImageBufferNew = NULL;
RGBQUAD * pPalette = (RGBQUAD *)( pImageBuffer + sizeof( BITMAPINFOHEADER ) );
	try
	{
		switch( nBPP )
		{
		case 24:
			pImageBufferNew =
				stat_AllocHL(
					nWidth,
					hHeight,
					32,
					0x00FF0000,
					0x0000FF00,
					0x000000FF,
					0xFF000000,
					0 // run-time flags
					);
			for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
			{
				LPBYTE pScanLineDst = stat_RcScanLine( pImageBufferNew, nLineIndex, nScanLineSizeDst );
				LPBYTE pScanLineSrc = stat_RcScanLine( pImageBuffer, nLineIndex, nScanLineSizeSrc );
				for( nX = 0; nX < nWidth; nX++ )
				{
					pScanLineDst[2]   = pScanLineSrc[2];
					pScanLineDst[1] = pScanLineSrc[1];
					pScanLineDst[0]  = pScanLineSrc[0];
					pScanLineDst[3] = 0xFF;
					pScanLineDst += 4;
					pScanLineSrc += 3;
				} // for( nX = 0; nX < nWidth; nX++ )
			} // for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
		break;
		case 16:
			{
				pImageBufferNew =
					stat_AllocHL(
						nWidth,
						hHeight,
						32,
						0x00FF0000,
						0x0000FF00,
						0x000000FF,
						0xFF000000,
						0 // run-time flags
						);
				bool b565 =
					(	( stat_MaskRed( pImageBuffer ) == 0xF800 )
					&&	( stat_MaskGreen( pImageBuffer ) == 0x07E0 )
					&&	( stat_MaskBlue( pImageBuffer ) == 0x001F )
					) ? true : false;
				for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
				{
					LPBYTE pScanLineDst = stat_RcScanLine( pImageBufferNew, nLineIndex, nScanLineSizeDst );
					LPBYTE pScanLineSrc = stat_RcScanLine( pImageBuffer, nLineIndex, nScanLineSizeSrc );
					WORD * nBitCount = ( WORD * )pScanLineSrc;
					if( b565 )
					{
						for( nX = 0; nX < nWidth; nX++ )
						{
							pScanLineDst[2] = ( BYTE ) ( (((nBitCount[nX] & 0xF800) >> 11) * 0xFF) / 0x1F );
							pScanLineDst[1] = ( BYTE ) ( (((nBitCount[nX] & 0x07E0) >> 5) * 0xFF) / 0x3F );
							pScanLineDst[0] = ( BYTE ) ( ( (nBitCount[nX] & 0x001F) * 0xFF) / 0x1F );
							pScanLineDst[3] = ( BYTE ) ( 0xFF );
							pScanLineDst += 4;
						} // for( nX = 0; nX < nWidth; nX++ )
					} // if( b565 )
					else
					{
						for( nX = 0; nX < nWidth; nX++ )
						{
							pScanLineDst[2] = ( BYTE ) ( (((nBitCount[nX] & 0x7C00) >> 10) * 0xFF) / 0x1F );
							pScanLineDst[1] = ( BYTE ) ( (((nBitCount[nX] & 0x03E0) >> 5) * 0xFF) / 0x1F );
							pScanLineDst[0] = ( BYTE ) ( ( (nBitCount[nX] & 0x001F) * 0xFF) / 0x1F );
							pScanLineDst[3] = ( BYTE ) ( 0xFF );
							pScanLineDst += 4;
						} // for( nX = 0; nX < nWidth; nX++ )
					} // else from if( b565 )
				} // for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
			}
		break;
		case 8:
			pImageBufferNew =
				stat_AllocHL(
					nWidth,
					hHeight,
					32,
					0x00FF0000,
					0x0000FF00,
					0x000000FF,
					0xFF000000,
					0 // run-time flags
					);
			for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
			{
				LPBYTE pScanLineDst = stat_RcScanLine( pImageBufferNew, nLineIndex, nScanLineSizeDst );
				LPBYTE pScanLineSrc = stat_RcScanLine( pImageBuffer, nLineIndex, nScanLineSizeSrc );
				for( nX = 0; nX < nWidth; nX++ )
				{
					pScanLineDst[0] = pPalette[pScanLineSrc[nX]].rgbBlue;
					pScanLineDst[1] = pPalette[pScanLineSrc[nX]].rgbGreen;
					pScanLineDst[2] = pPalette[pScanLineSrc[nX]].rgbRed;
					pScanLineDst[3] = 0xFF;
					pScanLineDst += 4;
				} // for( nX = 0; nX < nWidth; nX++ )
			}
		break;
		case 4:
			pImageBufferNew =
				stat_AllocHL(
					nWidth,
					hHeight,
					32,
					0x00FF0000,
					0x0000FF00,
					0x000000FF,
					0xFF000000,
					0 // run-time flags
					);
			for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
			{
				LPBYTE pScanLineDst = stat_RcScanLine( pImageBufferNew, nLineIndex, nScanLineSizeDst );
				LPBYTE pScanLineSrc = stat_RcScanLine( pImageBuffer, nLineIndex, nScanLineSizeSrc );
				bool bLowPartOfByte = false;
				ULONG nShiftX = 0;
				for( nX = 0; nX < nWidth ; ++nX )
				{
					if( bLowPartOfByte )
					{
						pScanLineDst[0] = pPalette[((pScanLineSrc[nShiftX])&0x0F)].rgbBlue;
						pScanLineDst[1] = pPalette[((pScanLineSrc[nShiftX])&0x0F)].rgbGreen;
						pScanLineDst[2] = pPalette[((pScanLineSrc[nShiftX])&0x0F)].rgbRed;
						nShiftX++;
					} // if( bLowPartOfByte )
					else
					{
						pScanLineDst[0] = pPalette[((pScanLineSrc[nShiftX])&0xF0) >> 4].rgbBlue;
						pScanLineDst[1] = pPalette[((pScanLineSrc[nShiftX])&0xF0) >> 4].rgbGreen;
						pScanLineDst[2] = pPalette[((pScanLineSrc[nShiftX])&0xF0) >> 4].rgbRed;
					} // else from if( bLowPartOfByte )
					bLowPartOfByte = !bLowPartOfByte;
					pScanLineDst[3] = 0xFF;
					pScanLineDst += 4;
				} // for( nX = 0; nX < nWidth ; ++nX )
			} // for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
		break;		
		case 1:
			pImageBufferNew =
				stat_AllocHL(
					nWidth,
					hHeight,
					32,
					0x00FF0000,
					0x0000FF00,
					0x000000FF,
					0xFF000000,
					0 // run-time flags
					);
			for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
			{
				LPBYTE pScanLineDst = stat_RcScanLine( pImageBufferNew, nLineIndex, nScanLineSizeDst );
				LPBYTE pScanLineSrc = stat_RcScanLine( pImageBuffer, nLineIndex, nScanLineSizeSrc );
				for( nX = 0; nX < nWidth; nX++ )
				{
					ULONG nIndex = ( pScanLineSrc[nX>>3] & ( 0x80 >> ( nX & 0x07 ) ) ) != 0 ? 1 : 0;
					pScanLineDst[0] = pPalette[nIndex].rgbBlue;
					pScanLineDst[1] = pPalette[nIndex].rgbGreen;
					pScanLineDst[2] = pPalette[nIndex].rgbRed;
					pScanLineDst[3] = 0xFF;
					pScanLineDst += 4;
				} // for( nX = 0; nX < nWidth; nX++ )
			} // for( nLineIndex = 0; nLineIndex < hHeight; nLineIndex++ )
		break;
		} // switch( nBPP )
	} // try
	catch( CException * pException )
	{
		if( pImageBufferNew != NULL )
		{
			delete [] pImageBufferNew;
			pImageBufferNew = NULL;
		}
		if( bEnableThrowException )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( pImageBufferNew != NULL )
		{
			delete [] pImageBuffer;
			pImageBufferNew = NULL;
		}
		if( bEnableThrowException )
			throw;
	} // catch( ... )
	return pImageBufferNew;
}

#pragma pack(pop)

const BYTE * CExtBitmap::GetBuffer() const
{
	return m_pImageBuffer;
}

ULONG CExtBitmap::MaskRed() const
{
	if( IsEmpty() )
		return 0;
	return stat_MaskRed( (LPBYTE)m_pImageBuffer );
}

ULONG CExtBitmap::MaskGreen() const
{
	if( IsEmpty() )
		return 0;
	return stat_MaskGreen( (LPBYTE)m_pImageBuffer );
}

ULONG CExtBitmap::MaskBlue() const
{
	if( IsEmpty() )
		return 0;
	return stat_MaskBlue( (LPBYTE)m_pImageBuffer );
}

ULONG CExtBitmap::MaskAlpha() const
{
	if( IsEmpty() )
		return 0;
	return stat_MaskAlpha( (LPBYTE)m_pImageBuffer );
}

ULONG CExtBitmap::RunTimeFlagsGet() const
{
	if( IsEmpty() )
		return 0;
	return stat_RunTimeFlagsGet( (LPBYTE)m_pImageBuffer );
}

void CExtBitmap::RunTimeFlagsSet( ULONG nFlags ) const
{
	if( IsEmpty() )
		return;
	stat_RunTimeFlagsSet( (LPBYTE)m_pImageBuffer, nFlags );
}

void CExtBitmap::OnLoad()
{
	ASSERT( ! IsEmpty() );
	OnContentChanged();
}

void CExtBitmap::OnContentChanged()
{
}

__EXT_MFC_SAFE_LPCTSTR CExtBitmap::stat_GetDrawMethodName(
	CExtBitmap::e_DrawMethod_t eDM
	)
{
	switch( eDM )
	{
	case __EDM_HOLLOW:
		return _T("Hollow");
	case __EDM_STRETCH:
		return _T("Stretch");
	case __EDM_CENTER:
		return _T("Center");
	case __EDM_TILE_H:
		return _T("TileH");
	case __EDM_TILE_V:
		return _T("TileV");
	case __EDM_TILE:
		return _T("Tile");
	} // switch( eDM )
	return _T("");
}

CExtBitmap::e_DrawMethod_t CExtBitmap::stat_GetDrawMethodFromString(
	__EXT_MFC_SAFE_LPCTSTR str
	)
{
CExtSafeString strDrawMethodString( str );
	if( strDrawMethodString.IsEmpty() )
		return __EDM_DEFAULT;
int iDrawMethod;
	for( iDrawMethod = 0; iDrawMethod < (int)__EDM_COUNT; ++iDrawMethod )
	{
		USES_CONVERSION;
		if( ! strDrawMethodString.CompareNoCase(
				stat_GetDrawMethodName( (e_DrawMethod_t)iDrawMethod )
				)
			)
			return (e_DrawMethod_t)iDrawMethod;
	}
	return __EDM_DEFAULT;
}

INT CExtBitmap::GetBitmapColorMap( // returns bitmap's color bit count or zero if error
	CMap < COLORREF, COLORREF, DWORD, DWORD > & _map // key is color, value is count of color entries in bitmap
	) const
{
	_map.RemoveAll();
	if( IsEmpty() )
		return 0;
INT nBPP = INT( GetBPP() );
CSize _size = GetSize();
INT nX, nY;
	for( nY = 0; nY < _size.cy; nY ++ )
	{
		for( nX = 0; nX < _size.cx; nX ++ )
		{
			COLORREF clr = GetPixel( nX, nY );
			DWORD dwEntries = 0;
			if( _map.Lookup( clr, dwEntries ) )
			{
				ASSERT( dwEntries > 0 );
				dwEntries++;
				_map.SetAt( clr, dwEntries );
			}
			else
				_map.SetAt( clr, 1 );
		}
	}
	return nBPP;
}

INT CExtBitmap::GetBitmapColorArray( // returns bitmap's color bit count or zero if error
	CArray < COLORREF, COLORREF > & arr
	) const
{
	arr.RemoveAll();
CMap < COLORREF, COLORREF, DWORD, DWORD > _map;
INT nBPP = GetBitmapColorMap( _map );
	if( nBPP == 0 )
		return 0;
#ifdef _DEBUG
INT nCount = (INT)_map.GetCount();
#endif // _DEBUG
	ASSERT( nCount > 0 );
POSITION pos = _map.GetStartPosition();
	ASSERT( pos != NULL );
	for( ; pos != NULL; )
	{
		COLORREF clrToInsert = RGB(0,0,0);
		DWORD dwEntries = 0;
		_map.GetNextAssoc( pos, clrToInsert, dwEntries );
		ASSERT( dwEntries > 0 );
		arr.Add( clrToInsert );
	} // for( ; pos != NULL; )
	ASSERT( nCount == arr.GetSize() );
	return nBPP;
}

COLORREF CExtBitmap::stat_RGBtoCMY( COLORREF clr )
{
int nR = GetRValue( clr );
int nG = GetGValue( clr );
int nB = GetBValue( clr );
int nC = 255-nR;
int nM = 255-nG;
int nY = 255-nB;
COLORREF clrCMY = RGB( nC, nM, nY );
	return clrCMY;
}

COLORREF CExtBitmap::stat_CMYtoRGB( COLORREF clr )
{
int nC = GetRValue( clr );
int nM = GetGValue( clr );
int nY = GetBValue( clr );
int nR = 255 - nC;
int nG = 255 - nM;
int nB = 255 - nY;
COLORREF clrRGB = RGB( nR, nG, nB );
	return clrRGB;
}

//----------------------------------------------------------------------
// Conversion between the HSL (Hue, Saturation, and Luminosity) 
// and RBG color model.
//----------------------------------------------------------------------
// The conversion algorithms presented here come from the book by 
// Fundamentals of Interactive Computer Graphics by Foley and van Dam. 
// In the example code, HSL values are represented as floating point 
// number in the range 0 to 1. RGB tridrants use the Windows convention 
// of 0 to 255 of each element. 
//----------------------------------------------------------------------
double CExtBitmap::stat_HuetoRGB(double m1, double m2, double h )
{
   if( h < 0 ) h += 1.0;
   if( h > 1 ) h -= 1.0;
   if( 6.0*h < 1 )
      return (m1+(m2-m1)*h*6.0);
   if( 2.0*h < 1 )
      return m2;
   if( 3.0*h < 2.0 )
      return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
   return m1;
}

COLORREF CExtBitmap::stat_HLStoRGB( double H, double L, double S )
{
double r, g, b;
double m1, m2;
	if( S==0 )
	{
		r = g = b = L;
	}
	else
	{
		if( L <= 0.5 )
			m2 = L*(1.0+S);
		else
			m2 = L+S-L*S;
		m1 = 2.0*L-m2;
		r = stat_HuetoRGB(m1,m2,H+1.0/3.0);
		g = stat_HuetoRGB(m1,m2,H);
		b = stat_HuetoRGB(m1,m2,H-1.0/3.0);
	}
	return RGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255));
}

void CExtBitmap::stat_RGBtoHSL( COLORREF rgb, double *H, double *S, double *L )
{   
// double delta;
// double r = (double)GetRValue(rgb)/255;
// double g = (double)GetGValue(rgb)/255;
// double b = (double)GetBValue(rgb)/255;   
// double cmax = max(r,max(g,b));
// double cmin = min(r,min(g,b));   
// 	*L = (cmax + cmin) / 2.0;   
// 	
// 	if(cmax==cmin) 
// 	{
// 		*S = 0;      
// 		*H = 0; // it's really undefined   
// 	}
// 	else 
// 	{
// 		if( *L < 0.5 ) 
// 			*S = (cmax-cmin)/(cmax+cmin);      
// 		else
// 			*S = (cmax-cmin)/(2.0-cmax-cmin);      
// 		delta = cmax - cmin;
// 		if( r == cmax ) 
// 			*H = (g-b)/delta;      
// 		else if( g == cmax )
// 			*H = 2.0 +(b-r)/delta;
// 		else          
// 			*H = 4.0 + (r-g)/delta;
// 		*H /= 6.0; 
// 		if( *H < 0.0 )
// 			*H += 1;  
// 	}

INT nr = GetRValue( rgb );
INT ng = GetGValue( rgb );
INT nb = GetBValue( rgb );
double delta;
double r = double( nr ) / 255.0;
double g = double( ng ) / 255.0;
double b = double( nb ) / 255.0;
INT ncmax = max( nr, max( ng, nb ) );
INT ncmin = min( nr, min( ng, nb ) );
double cmax = double( ncmax ) / 255.0;
double cmin = double( ncmin ) / 255.0;
	*L = ( cmax + cmin ) / 2.0;
	if( ncmax == ncmin )
	{
		*S = 0;
		*H = 0; // undefined
	}
	else
	{
		if( *L < 0.5 )
			*S = ( cmax - cmin ) / ( cmax + cmin );
		else
			*S = ( cmax - cmin ) / ( 2.0 - cmax - cmin );
		delta = cmax - cmin;
		if( nr == ncmax )
			*H = ( g -b ) / delta;
		else if( ng == ncmax )
			*H = 2.0 +( b - r ) / delta;
		else
			*H = 4.0 + ( r - g) / delta;
		*H /= 6.0;
		if( *H < 0.0 )
			*H += 1;
	}
}

BYTE CExtBitmap::stat_GetRGBFromHue(float rm1, float rm2, float rh)
{
	if (rh > 360.0f)
		rh -= 360.0f;
	else if (rh < 0.0f)
		rh += 360.0f;
	if (rh <  60.0f)
		rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;   
	else if (rh < 180.0f)
		rm1 = rm2;
	else if (rh < 240.0f)
		rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;      
	return static_cast < BYTE > (rm1 * 255);
}

COLORREF CExtBitmap::stat_GetRGBFromHLSExtend( double H, double L, double S )
{
WORD R, G, B;
	if( S == 0.0 )
	{
		R = G = B = unsigned char(L * 255.0);
	}
	else
	{
		float rm1, rm2;
		
		if( L <= 0.5f )
			rm2 = (float)(L + L * S);
		else
			rm2 = (float)(L + S - L * S);
		
		rm1 = (float)(2.0f * L - rm2);
		R = stat_GetRGBFromHue(rm1, rm2, (float)(H + 120.0f));
		G = stat_GetRGBFromHue(rm1, rm2, (float)(H));
		B = stat_GetRGBFromHue(rm1, rm2, (float)(H - 120.0f));
	}
	return RGB(R, G, B);
}

COLORREF CExtBitmap::stat_HLS_Adjust(
	COLORREF clr,
	double percentH, // = 0.0
	double percentL, // = 0.0
	double percentS  // = 0.0
	)
{
double H = 0.0, L = 0.0, S = 0.0;
	stat_RGBtoHSL( clr, &H, &S, &L );

    // Not sure what kind of hack job this prof-uis code was:
    /*
    if( percentH > 0.0 )
        H =
			H +
			(1.0 - H) * percentH
			;
    else if ( percentH < 0.0 )
        H =
			H * (1.0 + percentH);

	if( H < 0.0 )
		H = 0.0;
	else if( H > 1.0 )
		H = 1.0;*/
    // This is the correct implementation for hue adjustment:
    H += (percentH * 0.5);
    if (H < 0.0)
    {
        H += 1.0;
    }
    if (H > 1.0)
    {
        H -= 1.0;
    }


    if( percentL > 0.0 )
        L =
			L +
			(1.0 - L) * percentL
			;
    else if ( percentL < 0.0 )
        L =
			L * (1.0 + percentL);
	if( L < 0.0 )
		L = 0.0;
	else if( L > 1.0 )
		L = 1.0;

    if ( percentS > 0.0 )
        S =
			S +
			(1.0 - S) * percentS
			;
    else if ( percentS < 0.0 )
        S =
			S * (1.0 + percentS);
	if( S < 0.0 )
		S = 0.0;
	else if( S > 1.0 )
		S = 1.0;

    return stat_HLStoRGB( H, L, S );
}

COLORREF CExtBitmap::stat_RGB_Enlight( 
	COLORREF clrLight, 
	COLORREF clrDark, 
	INT nEnlighten
	)
{
INT nInDarkRed       = GetRValue( clrDark );
INT nInLightRed      = GetRValue( clrLight );
INT nInDarkGreen     = GetGValue( clrDark );
INT nInLightGreen    = GetGValue( clrLight );
INT nInDarkBlue      = GetBValue( clrDark );
INT nInLightBlue     = GetBValue( clrLight );
INT nDivMixer        = ( nEnlighten < 100 ) ? 100 : 1000;
INT nDivAdjuster     = nDivMixer / 2;
INT nDarken          = nDivMixer - nEnlighten;
INT nEnlightenRed    = nInLightRed * nEnlighten;
INT nDarkenRed       = nInDarkRed * nDarken;
INT nEnlightenGreen  = nInLightGreen * nEnlighten;
INT nDarkenGreen     = nInDarkGreen * nDarken;
INT nEnlightenBlue   = nInLightBlue * nEnlighten;
INT nDarkenBlue      = nInDarkBlue * nDarken;
INT nOutRed          =
	( nDarkenRed + nDivAdjuster + nEnlightenRed )
	/ nDivMixer;
INT nOutGreen        =
	( nDarkenGreen + nDivAdjuster + nEnlightenGreen )
	/ nDivMixer;
INT nOutBlue         =
	( nDarkenBlue + nDivAdjuster + nEnlightenBlue )
	/ nDivMixer;
	return ( RGB( nOutRed, nOutGreen, nOutBlue ) );
}

COLORREF CExtBitmap::stat_RGB_Blend( 
	COLORREF clrFrom, 
	COLORREF clrTo, 
	INT nAlpha
	)
{
	INT nOutRed = 
		( GetRValue( clrFrom ) * nAlpha + GetRValue( clrTo ) * ( 255 - nAlpha ) ) / 255;
	INT nOutGreen = 
		( GetGValue( clrFrom ) * nAlpha + GetGValue( clrTo ) * ( 255 - nAlpha ) ) / 255;
	INT nOutBlue = 
		( GetBValue( clrFrom ) * nAlpha + GetBValue( clrTo ) * ( 255 - nAlpha ) ) / 255; 

	return RGB(
		nOutRed < 0 ? 0 : ( nOutRed > 255 ? 255 : nOutRed ),
		nOutGreen < 0 ? 0 : ( nOutGreen > 255 ? 255 : nOutGreen ),
		nOutBlue < 0 ? 0 : ( nOutBlue > 255 ? 255 : nOutBlue )
		);
}

/////////////////////////////////////////////////////////////////////////////
// CExtBitmapCache

CExtBitmapCache::CExtBitmapCache()
	: m_hBitmapCache( NULL )
{
}

CExtBitmapCache::CExtBitmapCache(
	const CExtBitmap & other
	)
	: m_hBitmapCache( NULL )
	, CExtBitmap( other )
{
//	_InitCache( *this );
}

CExtBitmapCache::~CExtBitmapCache()
{
	Empty();
}

bool CExtBitmapCache::AssignFromOther(
	const CExtBitmap & other,
	bool bEnableThrowException // = false
	)
{
	Empty();
bool bRetVal =
		CExtBitmap::AssignFromOther(
			other,
			bEnableThrowException
			);
//	_InitCache( other );
	return bRetVal;
}

void CExtBitmapCache::_InitCache( const CExtBitmap & _bmp )
{
	if( m_hBitmapCache != NULL )
	{
		::DeleteObject( m_hBitmapCache );
		m_hBitmapCache = NULL;
	} // if( m_hBitmapCache != NULL )
	if(		g_PaintManager.m_pfnMsImgAlphaBlend == NULL
		||	_bmp.IsEmpty()
		||	_bmp.GetBPP() != 32
		||	_bmp.MaskAlpha() == 0
		)
		return;
bool bPreMultiplied = PreMultipliedRGBChannelsGet();
	if( bPreMultiplied )
	{
		m_hBitmapCache = CreateBitmap( false );
	} // if( bPreMultiplied )
	else
	{
		bool bAutoPreMultiplyEnabled = IsAutoPreMultiplyRGBChannels();
		if( bAutoPreMultiplyEnabled )
		{
			PreMultipliedRGBChannelsSet( true );
			PreMultiplyRGBChannels();
			m_hBitmapCache = CreateBitmap( false );
		} // if( bAutoPreMultiplyEnabled )
		else
		{
			CExtBitmap _bmpCrt = _bmp;
			if( _bmpCrt.IsEmpty() )
				return;
			_bmpCrt.PreMultiplyRGBChannels();
			m_hBitmapCache = _bmpCrt.CreateBitmap( false );
		} // else from if( bAutoPreMultiplyEnabled )
	} // else from if( bPreMultiplied )
}

void CExtBitmapCache::Empty()
{
	if( m_hBitmapCache != NULL )
	{
		::DeleteObject( m_hBitmapCache );
		m_hBitmapCache = NULL;
	} // if( m_hBitmapCache != NULL )
	CExtBitmap::Empty();
}

bool CExtBitmapCache::IsEmpty() const
{
bool bRetVal = CExtBitmap::IsEmpty();
	return bRetVal;
}

int CExtBitmapCache::AlphaBlend(
	HDC hDC,
	int nDstX,
	int nDstY,
	int nDstWidth,
	int nDstHeight,
	int nSrcX,
	int nSrcY,
	int nSrcWidth,
	int nSrcHeight,
	BYTE nSCA // = 0xFF
	) const
{
	ASSERT( hDC != NULL );
	if( IsEmpty() || hDC == NULL )
		return GDI_ERROR;

	if( m_hBitmapCache == NULL )
	{
		( const_cast < CExtBitmapCache * > ( this ) ) ->
			_InitCache( *this );
	}

	if(		g_PaintManager.m_pfnMsImgAlphaBlend != NULL
		&&	m_hBitmapCache != NULL
		)
	{
		bool bRTL = false;
		if( CDC::FromHandle(hDC)->GetLayout() == LAYOUT_RTL )
 			bRTL = true;
		if( ! bRTL )
		{
			int nRetVal = GDI_ERROR;
			HDC hSrcDC = ::CreateCompatibleDC( NULL );
			if( hSrcDC != NULL )
			{
				HGDIOBJ hOld = ::SelectObject( hSrcDC, m_hBitmapCache );
				CExtPaintManager::CExtPaintManagerAutoPtr::__EXT_MFC_BLENDFUNCTION
					_bf = { AC_SRC_OVER, 0, nSCA, __EXT_MFC_AC_SRC_ALPHA };
				if( g_PaintManager.m_pfnMsImgAlphaBlend(
						hDC,
						nDstX,
						nDstY,
						nDstWidth,
						nDstHeight,
						hSrcDC,
						nSrcX,
						nSrcY,
						nSrcWidth,
						nSrcHeight,
						_bf
						)
					)
					nRetVal = 0;
				::SelectObject( hSrcDC, hOld );
				::DeleteDC( hSrcDC );
			} // if( hSrcDC != NULL )
			if( nRetVal == 0 )
				return nRetVal;
		} // if( ! bRTL )
	} // if( g_PaintManager.m_pfnMsImgAlphaBlend != NULL . . .
	return
		CExtBitmap::AlphaBlend(
			hDC,
			nDstX,
			nDstY,
			nDstWidth,
			nDstHeight,
			nSrcX,
			nSrcY,
			nSrcWidth,
			nSrcHeight,
			nSCA
			);
}

void CExtBitmapCache::OnContentChanged()
{
	CExtBitmap::OnContentChanged();
	if( m_hBitmapCache != NULL )
	{
		::DeleteObject( m_hBitmapCache );
		m_hBitmapCache = NULL;
	} // if( m_hBitmapCache != NULL )
//	_InitCache( *this );
}

/////////////////////////////////////////////////////////////////////////////
// CExtCmdIcon
// command icon class

CExtCmdIcon::CExtCmdIcon()
	: m_dwFlags( __EXT_ICON_FLAGS_DEFAULT )
{
}

CExtCmdIcon::CExtCmdIcon( const CExtCmdIcon & other )
	: m_dwFlags( __EXT_ICON_FLAGS_DEFAULT )
{
	(*this) = other;
}

// CExtCmdIcon::CExtCmdIcon(
// 	HICON hIcon,
// 	bool bCreateCopy // if false - delete hIcon
// 	)
// 	: m_dwFlags( __EXT_ICON_FLAGS_DEFAULT )
// {
// 	AssignFromHICON( hIcon, bCreateCopy );
// }

// CExtCmdIcon::CExtCmdIcon(
// 	HBITMAP hBitmap,
// 	COLORREF clrTransparent, // = RGB( 0, 0, 0 )
// 	LPCRECT pRectBitmapSrc, // = NULL
// 	bool bCloneBitmap // = true // if false - pRectBitmapSrc must be NULL and will delete hBitmap
// 	)
// 	: m_dwFlags( __EXT_ICON_FLAGS_DEFAULT )
// {
// 	AssignFromHBITMAP(
// 		hBitmap,
// 		clrTransparent,
// 		pRectBitmapSrc,
// 		bCloneBitmap
// 		);
// }

CExtCmdIcon::~CExtCmdIcon()
{
	Empty();
}

CExtCmdIcon & CExtCmdIcon :: operator = ( const CExtCmdIcon & other )
{
	AssignFromOther( other );
	return (*this);
}

void CExtCmdIcon::AssignFromOther( const CExtCmdIcon & other )
{
	Empty();
	m_dwFlags = other.m_dwFlags;
	m_bmpNormal = other.m_bmpNormal;
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_DISABLED) != 0 )
		m_bmpDisabled = other.m_bmpDisabled;
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_HOVER) != 0 )
		m_bmpHover = other.m_bmpHover;
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_PRESSED) != 0 )
		m_bmpPressed = other.m_bmpPressed;
	OnContentChanged();
}

void CExtCmdIcon::AssignFromHICON(
	HICON hIcon,
	bool bCreateCopy, // if false - delete hIcon
	bool bForceLowColorMode // = false
	)
{
	Empty();
	if( hIcon == NULL )
		return;
	m_bmpNormal.AssignFromHICON(
		hIcon,
		bForceLowColorMode
		);
	if( ! bCreateCopy )
		::DestroyIcon( hIcon );
	OnContentChanged();
}

void CExtCmdIcon::AssignFromHBITMAP(
	HBITMAP hBitmap,
	COLORREF clrTransparent, // = COLORREF(-1L) // COLORREF(-1L) - no force transparent pixels
	LPCRECT pRectBitmapSrc, // = NULL
	bool bCloneBitmap // = true // if false - pRectBitmapSrc must be NULL and will delete hBitmap 
	)
{
	Empty();
	if( hBitmap == NULL )
		return;
	ASSERT( bCloneBitmap || pRectBitmapSrc == NULL );
	if( ! m_bmpNormal.FromBitmap( hBitmap, pRectBitmapSrc, false, false ) )
		return;
	if(		clrTransparent != COLORREF(-1L)
		&&	m_bmpNormal.Make32( false )
		)
		m_bmpNormal.AlphaColor( clrTransparent, RGB(0,0,0), 0 );
	if( ! bCloneBitmap )
		::DeleteObject( hBitmap );
	OnContentChanged();
}

bool CExtCmdIcon::IsEmpty() const
{
bool bIsEmpty = m_bmpNormal.IsEmpty();
	return bIsEmpty;
}

void CExtCmdIcon::Empty()
{
	m_dwFlags = __EXT_ICON_FLAGS_DEFAULT;
	m_bmpNormal.Empty();
	m_bmpDisabled.Empty();
	m_bmpHover.Empty();
	m_bmpPressed.Empty();
	OnContentChanged();
}

void CExtCmdIcon::Serialize( CArchive & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwFlags;
	}
	else
	{
		Empty();
		ar >> m_dwFlags;
	}
	m_bmpNormal.SerializeUnpacked( ar );
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_DISABLED) != 0 )
		m_bmpDisabled.SerializeUnpacked( ar );
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_HOVER) != 0 )
		m_bmpHover.SerializeUnpacked( ar );
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_PRESSED) != 0 )
		m_bmpPressed.SerializeUnpacked( ar );
	if( ! ar.IsStoring() )
		OnContentChanged();
}

CSize CExtCmdIcon::GetSize() const
{
	return m_bmpNormal.GetSize();
}

CExtCmdIcon :: operator CSize () const
{
	return GetSize();
}

void CExtCmdIcon::SyncSysColors(
	CExtPaintManager * pPM
	)
{
	if( IsEmpty() )
		return;
	
	if(		pPM == NULL 
		&&	(m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_DISABLED) == 0
		)
	{

		m_bmpDisabled.Empty();
		m_bmpDisabled = m_bmpNormal;
		m_bmpDisabled.Make32();
		m_bmpDisabled.MakeMono( ::GetSysColor( COLOR_3DFACE ) );
		m_bmpDisabled.AdjustAlpha( -0.5 );
		OnContentChanged();
		return;
	}

	if( pPM->AdjustIcon( this ) )
	{
		OnContentChanged();
		return;
	}
}

void CExtCmdIcon::OnGlobalPaintManagerChanged()
{
	OnEmptyGeneratedBitmaps();
}

void CExtCmdIcon::OnContentChanged()
{
}

void CExtCmdIcon::OnEmptyGeneratedBitmaps()
{
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_DISABLED) == 0 )
		m_bmpDisabled.Empty();
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_HOVER) == 0 )
		m_bmpHover.Empty();
	if( (m_dwFlags&__EXT_ICON_PERSISTENT_BITMAP_PRESSED) == 0 )
		m_bmpPressed.Empty();
}

void CExtCmdIcon::OnSysColorChange(
	CExtPaintManager * pPM
	)
{
	OnEmptyGeneratedBitmaps();
	SyncSysColors( pPM );
}

void CExtCmdIcon::OnSettingChange(
	CExtPaintManager * pPM,
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
	uFlags;
	lpszSection;
	OnEmptyGeneratedBitmaps();
	SyncSysColors( pPM );
}

void CExtCmdIcon::OnDisplayChange(
	CExtPaintManager * pPM,
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	nDepthBPP;
	ptSizes;
	OnEmptyGeneratedBitmaps();
	SyncSysColors( pPM );
}

void CExtCmdIcon::OnThemeChanged(
	CExtPaintManager * pPM,
	WPARAM wParam,
	LPARAM lParam
	)
{
	wParam;
	lParam;
	OnEmptyGeneratedBitmaps();
	SyncSysColors( pPM );
}

bool CExtCmdIcon::Scale(
	const SIZE & newSize
	)
{
	if(		IsEmpty()
		||	newSize.cx <= 0
		||	newSize.cy <= 0
		)
	{
//		ASSERT( FALSE );
		return false;
	}
CSize _size = GetSize();
	if( _size.cx == newSize.cx && _size.cy == newSize.cy )
		return true;
	if(		( m_bmpNormal.IsEmpty()   || m_bmpNormal.Scale(   newSize.cx, newSize.cy ) )
		&&	( m_bmpDisabled.IsEmpty() || m_bmpDisabled.Scale( newSize.cx, newSize.cy ) )
		&&	( m_bmpHover.IsEmpty()    || m_bmpHover.Scale(    newSize.cx, newSize.cy ) )
		&&	( m_bmpPressed.IsEmpty()  || m_bmpPressed.Scale(  newSize.cx, newSize.cy ) )
		)
	{
		OnContentChanged();
		return true;
	}
	return false;
}

bool CExtCmdIcon::CreateScaledCopy(
	const CExtCmdIcon & other,
	const SIZE & newSize
	)
{
	if(		other.IsEmpty()
		||	newSize.cx <= 0
		||	newSize.cy <= 0
		)
	{
//		ASSERT( FALSE );
		return false;
	}
	Empty();
	AssignFromOther( other );
	return Scale( newSize );
}

bool CExtCmdIcon::CreateCheckMarkIcon(
	CExtPaintManager * pPM,
	COLORREF clrMark, // = COLORREF(-1L)
	COLORREF clrTransparent, // = COLORREF(-1L)
	CSize sizeIcon // = CSize(16,16)
	)
{
	if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	{
		ASSERT( FALSE );
		return false;
	} // if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	Empty();
	if( clrMark == COLORREF(-1L) )
		clrMark = pPM->GetColor( CExtPaintManager::CLR_TEXT_OUT, NULL );
	if( clrTransparent == COLORREF(-1L) )
		clrTransparent = (clrMark == RGB(255,0,255)) ? RGB(128,255,128) : RGB(255,0,255);
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
LONG nIconPixelCount = sizeIcon.cx * sizeIcon.cy;
	BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = sizeIcon.cx;
	bih.biHeight = sizeIcon.cy;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = nIconPixelCount;
COLORREF * pOutSurface = NULL;
HBITMAP hDIB =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void**)&pOutSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDC );
		return false;
	}
	ASSERT( pOutSurface != NULL );
HGDIOBJ hOldBitmap = ::SelectObject( hDC, hDIB );
CDC dc;
	dc.Attach( hDC );
	dc.FillSolidRect( 0, 0, sizeIcon.cx, sizeIcon.cy, clrTransparent );
	CExtPaintManager::stat_PaintCheckMark( dc, sizeIcon.cx/2 - 3, sizeIcon.cy/2 - 3, clrMark );
	dc.Detach();
	::SelectObject( hDC, hOldBitmap );
	::DeleteDC( hDC );
	AssignFromHBITMAP( hDIB, clrTransparent, NULL, false );
	return true;
}

bool CExtCmdIcon::CreateIndeterminatedSquareIcon(
	CExtPaintManager * pPM,
	COLORREF clrMark, // = COLORREF(-1L)
	COLORREF clrTransparent, // = COLORREF(-1L)
	CSize sizeIcon // = CSize(16,16)
	)
{
	if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	{
		ASSERT( FALSE );
		return false;
	} // if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	Empty();
	if( clrMark == COLORREF(-1L) )
		clrMark = pPM->GetColor( CExtPaintManager::CLR_TEXT_OUT, NULL );
	if( clrTransparent == COLORREF(-1L) )
		clrTransparent = (clrMark == RGB(255,0,255)) ? RGB(128,255,128) : RGB(255,0,255);
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
LONG nIconPixelCount = sizeIcon.cx * sizeIcon.cy;
	BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = sizeIcon.cx;
	bih.biHeight = sizeIcon.cy;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = nIconPixelCount;
COLORREF * pOutSurface = NULL;
HBITMAP hDIB =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void**)&pOutSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDC );
		return false;
	}
	ASSERT( pOutSurface != NULL );
HGDIOBJ hOldBitmap = ::SelectObject( hDC, hDIB );
CDC dc;
	dc.Attach( hDC );
	dc.FillSolidRect( 0, 0, sizeIcon.cx, sizeIcon.cy, clrTransparent );
	CExtPaintManager::stat_PaintIndeterminatedSquare( dc, sizeIcon.cx/2 - 3, sizeIcon.cy/2 - 3, clrMark );
	dc.Detach();
	::SelectObject( hDC, hOldBitmap );
	::DeleteDC( hDC );
	AssignFromHBITMAP( hDIB, clrTransparent, NULL, false );
	return true;
}

bool CExtCmdIcon::CreateRadioMarkIcon(
	CExtPaintManager * pPM,
	COLORREF clrMark, // = COLORREF(-1L)
	COLORREF clrTransparent, // = COLORREF(-1L)
	CSize sizeIcon // = CSize(16,16)
	)
{
	if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	{
		ASSERT( FALSE );
		return false;
	} // if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	Empty();
	if( clrMark == COLORREF(-1L) )
		clrMark = pPM->GetColor( CExtPaintManager::CLR_TEXT_OUT, NULL );
	if( clrTransparent == COLORREF(-1L) )
		clrTransparent = (clrMark == RGB(255,0,255)) ? RGB(128,255,128) : RGB(255,0,255);
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
LONG nIconPixelCount = sizeIcon.cx * sizeIcon.cy;
	BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = sizeIcon.cx;
	bih.biHeight = sizeIcon.cy;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = nIconPixelCount;
COLORREF * pOutSurface = NULL;
HBITMAP hDIB =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void**)&pOutSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDC );
		return false;
	}
	ASSERT( pOutSurface != NULL );
HGDIOBJ hOldBitmap = ::SelectObject( hDC, hDIB );
CDC dc;
	dc.Attach( hDC );
	dc.FillSolidRect( 0, 0, sizeIcon.cx, sizeIcon.cy, clrTransparent );
	CExtPaintManager::stat_PaintRadioDot( dc, sizeIcon.cx/2 - 3, sizeIcon.cy/2 - 3, clrMark );
	dc.Detach();
	::SelectObject( hDC, hOldBitmap );
	::DeleteDC( hDC );
	AssignFromHBITMAP( hDIB, clrTransparent, NULL, false );
	return true;
}

bool CExtCmdIcon::CreateEmptyIcon(
	CExtPaintManager * pPM,
	COLORREF clrTransparent, // = COLORREF(-1L)
	CSize sizeIcon // = CSize(16,16)
	)
{
	pPM;
	if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	{
		ASSERT( FALSE );
		return false;
	} // if( sizeIcon.cx <= 0 || sizeIcon.cy <= 0 )
	Empty();
	if( clrTransparent == COLORREF(-1L) )
		clrTransparent = RGB(255,0,255);
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
LONG nIconPixelCount = sizeIcon.cx * sizeIcon.cy;
	BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
	bih.biSize = sizeof( BITMAPINFOHEADER );
	bih.biWidth = sizeIcon.cx;
	bih.biHeight = sizeIcon.cy;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = nIconPixelCount;
COLORREF * pOutSurface = NULL;
HBITMAP hDIB =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void**)&pOutSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDC );
		return false;
	}
	ASSERT( pOutSurface != NULL );
HGDIOBJ hOldBitmap = ::SelectObject( hDC, hDIB );
CDC dc;
	dc.Attach( hDC );
	dc.FillSolidRect( 0, 0, sizeIcon.cx, sizeIcon.cy, clrTransparent );
	dc.Detach();
	::SelectObject( hDC, hOldBitmap );
	::DeleteDC( hDC );
	AssignFromHBITMAP( hDIB, clrTransparent, NULL, false );
	return true;
}

HICON CExtCmdIcon::ExtractHICON(
	bool bReplaceSysColors, // = true
	COLORREF clrTransparent // = COLORREF(-1L)
	) const
{
	if( IsEmpty() )
		return NULL;
HICON hIcon =
		m_bmpNormal.CreateHICON(
			bReplaceSysColors,
			clrTransparent
			);
	return hIcon;
}

CExtBitmap & CExtCmdIcon::GetBitmap(
	CExtCmdIcon::e_paint_type_t ePT, // = __PAINT_NORMAL
	CExtPaintManager * pPM // = NULL
	)
{
CExtBitmap * pBitmap = &m_bmpNormal;
	switch( ePT )
	{
	case __PAINT_NORMAL:
	break;
	case __PAINT_DISABLED:
		if(		m_bmpDisabled.IsEmpty()
			&&	( ! m_bmpNormal.IsEmpty() )
			)
			SyncSysColors( pPM );
		if( ! m_bmpDisabled.IsEmpty() )
			pBitmap = &m_bmpDisabled;
	break;
	case __PAINT_HOVER:
		if(		m_bmpHover.IsEmpty()
			&&	( ! m_bmpNormal.IsEmpty() )
			)
			SyncSysColors( pPM );
		if( ! m_bmpHover.IsEmpty() )
			pBitmap = &m_bmpHover;
	break;
	case __PAINT_PRESSED:
		if(		m_bmpPressed.IsEmpty()
			&&	( ! m_bmpNormal.IsEmpty() )
			)
			SyncSysColors( pPM );
		if( ! m_bmpPressed.IsEmpty() )
			pBitmap = &m_bmpPressed;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( ePT )
	return (*pBitmap);
}

const CExtBitmap & CExtCmdIcon::GetBitmap(
	CExtCmdIcon::e_paint_type_t ePT, // = __PAINT_NORMAL
	CExtPaintManager * pPM // = NULL
	) const
{
	return
		( const_cast < CExtCmdIcon * > ( this ) )
		-> GetBitmap( ePT, pPM );
}

void CExtCmdIcon::Paint(
	CExtPaintManager * pPM,
	HDC hDC,
	const RECT & rc,
	e_paint_type_t ePT, // = __PAINT_NORMAL
	BYTE nSCA // = BYTE(255)
	) const
{
	Paint(
		pPM,
		hDC,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		ePT,
		nSCA
		);
}

void CExtCmdIcon::Paint(
	CExtPaintManager * pPM,
	HDC hDC,
	INT nX,
	INT nY,
	INT nDX, // = -1 // -1 use default size
	int nDY, // = -1 // -1 use default size
	e_paint_type_t ePT, // = CExtCmdIcon:: __PAINT_NORMAL
	BYTE nSCA // = BYTE(255)
	) const
{
	if( ePT == __PAINT_INVISIBLE )
		return;
	if( hDC == NULL || IsEmpty() )
		return;
	if( nDX == 0 || nDY == 0 )
		return;
CSize _size = GetSize();
	if( nDX < 0 || nDY < 0 )
	{
		if( nDX < 0 )
			nDX = _size.cx;
		if( nDY < 0 )
			nDY = _size.cy;
	} // if( nDX < 0 || nDY < 0 )
const CExtBitmap & _bmp = GetBitmap( ePT, pPM );
	if( ! _bmp.IsEmpty() )
		_bmp.AlphaBlend(
			hDC,
			nX,
			nY,
			nDX,
			nDY,
			0,
			0,
			_size.cx,
			_size.cy,
			nSCA
			);
}

void CExtCmdIcon::PaintAccentEmboss(
	CExtPaintManager * pPM,
	HDC hDC,
	COLORREF clrAccent,
	INT nX,
	INT nY,
	INT nDX, // = -1 // -1 use default size
	int nDY,  // = -1 // -1 use default size
	BYTE nSCA // = BYTE(255)
	)
{
	pPM;
	if( IsEmpty() )
		return;
	ASSERT( ! m_bmpNormal.IsEmpty() );
	if( nDX == 0 || nDY == 0 )
		return;
CSize _size = GetSize();
	if( nDX < 0 || nDY < 0 )
	{
		if( nDX < 0 )
			nDX = _size.cx;
		if( nDY < 0 )
			nDY = _size.cy;
	} // if( nDX < 0 || nDY < 0 )
	clrAccent = ::GetNearestColor( hDC, clrAccent );
CExtBitmap _bmpDisabled = m_bmpNormal;
	_bmpDisabled.AdjustHLS( COLORREF(-1L), 0, 0.0, -0.75, 0.0 );
	_bmpDisabled.AdjustHLS( COLORREF(-1L), 0, 0.0, 0.75, 0.0 );
	_bmpDisabled.MakeMono( clrAccent );
CExtBitmap _bmpL = _bmpDisabled;
	_bmpL.AdjustHLS( COLORREF(-1L), 0, 0.0, 0.75, 0.0 );
	if( ! _bmpL.IsEmpty() )
		_bmpL.AlphaBlend(
			hDC,
			nX+1,
			nY+1,
			nDX,
			nDY,
			0,
			0,
			_size.cx,
			_size.cy,
			nSCA
			);
	if( ! _bmpDisabled.IsEmpty() )
		_bmpDisabled.AlphaBlend(
			hDC,
			nX,
			nY,
			nDX,
			nDY,
			0,
			0,
			_size.cx,
			_size.cy,
			nSCA
			);
}

void CExtCmdIcon::PaintAccentMono(
	CExtPaintManager * pPM,
	HDC hDC,
	COLORREF clrAccent,
	INT nX,
	INT nY,
	INT nDX, // = -1 // -1 use default size
	int nDY,  // = -1 // -1 use default size
	BYTE nSCA // = BYTE(255)
	)
{
	pPM;
	if( IsEmpty() )
		return;
	ASSERT( ! m_bmpNormal.IsEmpty() );
	if( nDX == 0 || nDY == 0 )
		return;
CSize _size = GetSize();
	if( nDX < 0 || nDY < 0 )
	{
		if( nDX < 0 )
			nDX = _size.cx;
		if( nDY < 0 )
			nDY = _size.cy;
	} // if( nDX < 0 || nDY < 0 )
CExtBitmap _bmp = m_bmpNormal;
	_bmp.Make32();
double H = 0.0, S = 0.0, L = 0.0;
	CExtBitmap::stat_RGBtoHSL( clrAccent, &H, &S, &L );
	clrAccent = CExtBitmap::stat_HLStoRGB( H, L, S );
	_bmp.AdjustHLS( COLORREF(-1L), 0, 0.0, -1.0, -1.0 );
	_bmp.MakeMono( clrAccent );
	_bmp.AdjustHLS( COLORREF(-1L), 0, 0.0, 0.5, 0.0 );
	if( ! _bmp.IsEmpty() )
		_bmp.AlphaBlend(
			hDC,
			nX,
			nY,
			nDX,
			nDY,
			0,
			0,
			_size.cx,
			_size.cy,
			nSCA
			);
}

void CExtCmdIcon::PaintAccentColorized(
	CExtPaintManager * pPM,
	HDC hDC,
	COLORREF clrAccent,
	INT nX,
	INT nY,
	INT nDX, // = -1 // -1 use default size
	int nDY, // = -1 // -1 use default size
	BYTE nSCA // = BYTE(255)
	)
{
	pPM;
	if( IsEmpty() )
		return;
	ASSERT( ! m_bmpNormal.IsEmpty() );
	if( nDX == 0 || nDY == 0 )
		return;
CSize _size = GetSize();
	if( nDX < 0 || nDY < 0 )
	{
		if( nDX < 0 )
			nDX = _size.cx;
		if( nDY < 0 )
			nDY = _size.cy;
	} // if( nDX < 0 || nDY < 0 )
CExtBitmap _bmp = m_bmpNormal;
	_bmp.Make32();
double H = 0.0, S = 0.0, L = 0.0;
	CExtBitmap::stat_RGBtoHSL( clrAccent, &H, &S, &L );
	clrAccent = CExtBitmap::stat_HLStoRGB( H, L, S );
	_bmp.MakeMono( clrAccent );
	if( ! _bmp.IsEmpty() )
		_bmp.AlphaBlend(
			hDC,
			nX,
			nY,
			nDX,
			nDY,
			0,
			0,
			_size.cx,
			_size.cy,
			nSCA
			);
}

bool CExtCmdIcon::ExtractEditableBitmap(
	HBITMAP & hBitmap,
	COLORREF & clrTransparent,
	CArray <COLORREF,COLORREF> * pArr, // = NULL // recommended palette, item 0 is clrTransparent
	COLORREF ** ppClrSurface // = NULL // if not NULL - force 32 bit
	) const
{
	if( IsEmpty() )
		return false;
	return
		m_bmpNormal.ExtractEditableBitmap(
			hBitmap,
			clrTransparent,
			pArr,
			ppClrSurface
			);
}


