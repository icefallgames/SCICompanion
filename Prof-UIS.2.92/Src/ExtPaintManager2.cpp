
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

#if (defined __PROF_UIS_BUILTIN_MULTIMON_STUB)
	#define COMPILE_MULTIMON_STUBS
#endif // __PROF_UIS_BUILTIN_MULTIMON_STUB

// disable warning 4706
#pragma warning( push )
#pragma warning ( disable : 4706 )
	#include <multimon.h>
// rollback warning 4706
#pragma warning( pop )

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXTDOCKBAR_H)
	#include <../Src/ExtDockBar.h>
#endif

#if (!defined __EXT_STATUSCONTROLBAR_H)
	#include <ExtStatusControlBar.h>
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	#if (!defined __EXTSHORTCUTLISTWND_H)
		#include <ExtShortcutListWnd.h>
	#endif
#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)

#if (!defined __EXT_BUTTON_H)
	#include <ExtButton.h>
#endif

#if (!defined __EXT_GROUPBOX_H)
	#include <ExtGroupBox.h>
#endif

#if (!defined __PROF_UIS_RES_2007_H)
	#include <Resources/Res2007/Res2007.h>
#endif

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __EXT_MFC_NO_REPORTGRIDWND)
	#if (!defined __EXT_REPORT_GRID_WND_H)
		#include <ExtReportGridWnd.h>
	#endif
#endif 

#if (!defined __EXT_MFC_NO_DATE_PICKER)
	#if (!defined __EXT_DATE_PICKER_H)
		#include <ExtDatePicker.h>
	#endif
#endif 

#if (!defined __EXT_MFC_NO_TOOLBOX_CTRL)
	#if (!defined __EXTTOOLBOXWND_H)
		#include <ExtToolBoxWnd.h>
	#endif
#endif 

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	#if (!defined __EXT_RIBBON_BAR_H)
		#include <ExtRibbonBar.h>
	#endif // (!defined __EXT_RIBBON_BAR_H)
#endif

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	#if (!defined __EXT_POPUP_CTRL_MENU_H)
		#include <ExtPopupCtrlMenu.h>
	#endif
#endif

#if (!defined __EXT_MFC_NO_GRIDWND)
	#if (!defined __EXT_GRIDWND_H)
		#include <ExtGridWnd.h>
	#endif 
#endif

#if (!defined __EXT_TEMPL_H)
	#include <ExtTempl.h>
#endif

#if (!defined __EXT_SPIN_H)
	#include <ExtSpinWnd.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MFC_NO_DURATIONWND)
	#if (!defined __EXT_DURATIONWND_H)
		#include "ExtDurationWnd.h"
	#endif
#endif

#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
	#if (!defined __EXT_PAGECONTAINERWND_H)
		#include <ExtPageContainerWnd.h>
	#endif
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_EDIT_H)
	#include <ExtEdit.h>
#endif

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (! defined __VSSYM32_H__)
	#include <vssym32/vssym32.h>
#endif // (! defined __VSSYM32_H__)

#include <math.h>
#include <shlwapi.h>

#include <Resources/Resource.h>

#if (! defined CLEARTYPE_QUALITY )
	#define CLEARTYPE_QUALITY 5
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// VC+ 6.0 static builds specific: disable warnings 4305, 4309
// warning C4305: 'initializing' : truncation from 'int' to 'short'
// warning C4309: 'initializing' : truncation of constant value
#if _MFC_VER < 0x700
	#ifdef __EXT_PROFUIS_STATIC_LINK
		#pragma warning( push )
		#pragma warning ( disable : 4305 )
		#pragma warning ( disable : 4309 )
	#endif
#endif

IMPLEMENT_SERIAL(  CExtPaintManager,                         CObject,                                  VERSIONABLE_SCHEMA|1 );
IMPLEMENT_SERIAL(  CExtPaintManagerXP,                       CExtPaintManager,                         VERSIONABLE_SCHEMA|1 );
IMPLEMENT_SERIAL(  CExtPaintManagerNativeXP,                 CExtPaintManager,                         VERSIONABLE_SCHEMA|1 );

CExtPaintManager::CExtPaintManagerAutoPtr g_PaintManager;

//	CExtSWMR::CExtSWMR(
//		__EXT_MFC_SAFE_LPCTSTR strName //  = NULL
//		)
//		: m_bConstructed( false )
//		, m_hProtectWriter( NULL )
//		, m_hProtectReaders( NULL )
//		, m_hEnumReaders( NULL )
//	{
//		Create( strName );
//	}
//
//	CExtSWMR::~CExtSWMR()
//	{
//		Destroy();
//	}
//
//	LPCTSTR CExtSWMR::stat_GenerateObjectName(
//		__EXT_MFC_SAFE_LPCTSTR strPrefix,
//		__EXT_MFC_SAFE_LPCTSTR strSuffix,
//		__EXT_MFC_SAFE_LPTSTR strOutNameBuffer,
//		size_t nBufferSize,
//		bool & bConstructed
//		)
//	{
//		bConstructed = true;
//		if( strSuffix == NULL )
//			return NULL;
//		if( strPrefix == NULL )
//			strPrefix = _T("");
//	size_t nLenPrefix = size_t( _tcslen( strPrefix ) );
//	size_t nLenSuffix = size_t( _tcslen( strSuffix ) );
//		if( ( nLenPrefix + nLenSuffix ) >= nBufferSize )
//		{
//			bConstructed = false;
//			return NULL;
//		}
//		__EXT_MFC_STRCPY( strOutNameBuffer, nBufferSize, strPrefix );
//		__EXT_MFC_STRCAT( strOutNameBuffer, nBufferSize, strSuffix );
//		return strOutNameBuffer;
//	}
//
//	bool CExtSWMR::Create(
//		__EXT_MFC_SAFE_LPCTSTR strName //  = NULL
//		)
//	{
//		Destroy();
//	TCHAR strObjectNameBuffer[ 512 ];
//	LPCTSTR strObjectName =
//			stat_GenerateObjectName(
//				_T("~CExtSWMR~ProtectWriter~"),
//				strName,
//				strObjectNameBuffer,
//				sizeof(strObjectNameBuffer)/sizeof(strObjectNameBuffer[0]),
//				m_bConstructed
//				);
//		if( m_bConstructed )
//			m_hProtectWriter =
//				::CreateMutex(
//					NULL,
//					FALSE,
//					strObjectName
//					);
//		strObjectName =
//			stat_GenerateObjectName(
//				_T("~CExtSWMR~EventsNoReaders~"),
//				strName,
//				strObjectNameBuffer,
//				sizeof(strObjectNameBuffer)/sizeof(strObjectNameBuffer[0]),
//				m_bConstructed
//				);
//		if( m_bConstructed )
//			m_hProtectReaders =
//				::CreateEvent(
//					NULL,
//					TRUE,
//					TRUE,
//					strObjectName
//					);
//		strObjectName =
//			stat_GenerateObjectName(
//				_T("~CExtSWMR~EnumReaders~"),
//				strName,
//				strObjectNameBuffer,
//				sizeof(strObjectNameBuffer)/sizeof(strObjectNameBuffer[0]),
//				m_bConstructed
//				);
//		if( m_bConstructed )
//			m_hEnumReaders =
//				::CreateSemaphore(
//				NULL,
//				0,
//				0x7FFFFFFF,
//				strObjectName
//				);
//		if(		m_hEnumReaders == NULL
//			||	m_hProtectReaders == NULL
//			||	m_hProtectWriter == NULL
//			)
//		{
//			Destroy();
//			m_bConstructed = false;
//		}
//		else
//			m_bConstructed = true;
//		return m_bConstructed;
//	}
//
//	void CExtSWMR::Destroy()
//	{
//		if( m_hEnumReaders != NULL ) 
//		{
//			::CloseHandle( m_hEnumReaders );
//			m_hEnumReaders = NULL;
//		}
//		if( m_hProtectReaders != NULL ) 
//		{
//			::CloseHandle( m_hProtectReaders );
//			m_hProtectReaders = NULL;
//		}
//		if( m_hProtectWriter != NULL )
//		{
//			::CloseHandle( m_hProtectWriter );
//			m_hProtectWriter = NULL;
//		}
//		m_bConstructed = false;
//	}
//
//	bool CExtSWMR::IsConstructed() const
//	{
//		return m_bConstructed;
//	}
//
//	CExtSWMR::LockSection::LockSection(
//		CExtSWMR & _SWMR,
//		bool bRead,
//		DWORD dwTimeout // = INFINITE
//		)
//		: m_SWMR( _SWMR )
//		, m_bRead( bRead )
//		, m_bLocked( false )
//	{
//		if( ! m_SWMR.IsConstructed() )
//			return;
//	DWORD dwWaitResult = WAIT_ABANDONED;
//		if( m_bRead )
//			dwWaitResult = m_SWMR.WaitToRead( dwTimeout );
//		else
//			dwWaitResult = m_SWMR.WaitToWrite( dwTimeout );
//		m_bLocked = ( dwWaitResult == WAIT_ABANDONED || dwWaitResult == WAIT_TIMEOUT ) ? false : true;
//	}
//
//	CExtSWMR::LockSection::~LockSection()
//	{
//		if( ! m_bLocked )
//			return;
//		if( m_bRead )
//			m_SWMR.DoneReading();
//		else
//			m_SWMR.DoneWriting();
//	}
//
//	bool CExtSWMR::LockSection::IsLocked() const
//	{
//		return m_bLocked;
//	}
//
//	DWORD CExtSWMR::WaitToWrite(
//		DWORD dwTimeout // = INFINITE
//		)
//	{
//	HANDLE arrToWait[2];
//		arrToWait[0] = m_hProtectWriter;
//		arrToWait[1] = m_hProtectReaders;
//	DWORD dwWaitResult =
//			::WaitForMultipleObjects(
//				2,
//				arrToWait,
//				TRUE,
//				dwTimeout
//				);
//		return dwWaitResult;
//	}
//
//	void CExtSWMR::DoneWriting()
//	{
//		::ReleaseMutex( m_hProtectWriter );
//	}
//
//	DWORD CExtSWMR::WaitToRead(
//		DWORD dwTimeout // = INFINITE
//		)
//	{
//	DWORD dwWaitResult =
//			::WaitForSingleObject(
//				m_hProtectWriter,
//				dwTimeout
//				);
//		if( dwWaitResult != WAIT_TIMEOUT )
//		{
//			LONG nPrev = 0;
//			::ReleaseSemaphore(
//				m_hEnumReaders,
//				1,
//				&nPrev
//				);
//			if( nPrev == 0)
//				::ResetEvent( m_hProtectReaders );
//		} // if( dwWaitResult != WAIT_TIMEOUT )
//		::ReleaseMutex( m_hProtectWriter );
//		return dwWaitResult;
//	}
//
//	void CExtSWMR::DoneReading()
//	{
//	HANDLE arrToWait[2];
//		arrToWait[0] = m_hProtectWriter;
//		arrToWait[1] = m_hEnumReaders;
//		::WaitForMultipleObjects(
//			2,
//			arrToWait,
//			TRUE,
//			INFINITE
//			);
//	DWORD dwWaitResult =
//			::WaitForSingleObject(
//				m_hEnumReaders,
//				0
//				);
//	bool bLast = ( dwWaitResult == WAIT_TIMEOUT ) ? true : false;
//		if( bLast )
//			::SetEvent( m_hProtectReaders );
//		else
//			::ReleaseSemaphore(
//				m_hEnumReaders,
//				1,
//				NULL
//				);
//		::ReleaseMutex( m_hProtectWriter );
//	}

CExtPmSynchronizeSink::CExtPmSynchronizeSink()
	: m_bPmSynchronizeSinkIsSynchronizing( false )
	, m_dwPmSynchronizeSinkThreadID( ::GetCurrentThreadId() )
{
}

CExtPmSynchronizeSink::~CExtPmSynchronizeSink()
{
}

DWORD CExtPmSynchronizeSink::PmSynchronizeSink_GetThreadID()
{
	return m_dwPmSynchronizeSinkThreadID;
}

bool CExtPmSynchronizeSink::PmSynchronizeSink_IsSynchronizing()
{
	return m_bPmSynchronizeSinkIsSynchronizing;
}

bool CExtPmSynchronizeSink::PmSynchronizeSink_IsEqual(
	CExtPmSynchronizeSink * pOther
	)
{
	if( this == pOther )
		return true;
	return false;
}

INT CExtPmSynchronizeSink::GetIndexOfPmSynchronizeSink(
	CExtPmSynchronizeSink * pPmSynchronizeSink
	)
{
__PROF_UIS_MANAGE_STATE;
	if( pPmSynchronizeSink == NULL )
		return -1;
INT nIndex, nCount = INT( m_arrPmSynchronizeSink.GetSize() );
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtPmSynchronizeSink * pRegistered = m_arrPmSynchronizeSink[ nIndex ];
		ASSERT( pRegistered != NULL );
		if( pRegistered->PmSynchronizeSink_IsEqual( pPmSynchronizeSink ) )
			return nIndex;
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	return -1;
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::PmSynchronizeSink_PerformSynchronizationWith(
	CExtPmSynchronizeSink * pPmSynchronizeSink,
	bool bRegister, // = true
	bool bSynchronizeItNow // = true
	)
{
__PROF_UIS_MANAGE_STATE;
	if( bRegister )
	{
		if( pPmSynchronizeSink == NULL )
			return false;
		if( pPmSynchronizeSink->PmSynchronizeSink_IsEqual( this ) )
			return false;
		DWORD dwOwnThreadID = PmSynchronizeSink_GetThreadID();
		DWORD dwOtherThreadID = PmSynchronizeSink_GetThreadID();
		if( dwOwnThreadID != dwOtherThreadID )
			return false;
		INT nIndex = GetIndexOfPmSynchronizeSink( pPmSynchronizeSink );
		if( nIndex < 0 )
			m_arrPmSynchronizeSink.Add( pPmSynchronizeSink );
		if( bSynchronizeItNow )
		{
			LPSTREAM pStream = PmSynchronizeSink_GetData();
			if( pStream != NULL )
			{
				pPmSynchronizeSink ->
					PmSynchronizeSink_SynchronizeFrom(
						pStream
						);
				pStream->Release();
			} // if( pStream != NULL )
		} // if( bSynchronizeItNow )
		return true;
	} // if( bRegister )
	else
	{
		if( pPmSynchronizeSink == NULL )
		{
			m_arrPmSynchronizeSink.RemoveAll();
			return true;
		}
		INT nIndex = GetIndexOfPmSynchronizeSink( pPmSynchronizeSink );
		if( nIndex < 0 )
			return false;
		m_arrPmSynchronizeSink.RemoveAt( nIndex );
		return true;
	} // else from if( bRegister )
}

LPSTREAM CExtPaintManager::CExtPaintManagerAutoPtr::PmSynchronizeSink_GetData(
	HGLOBAL hGlobal // = NULL
	)
{
__PROF_UIS_MANAGE_STATE;
CExtPaintManager * pPM = GetPM();
	if( pPM == NULL )
		return NULL;
LPSTREAM pStream = NULL;
SCODE sc =
		::CreateStreamOnHGlobal(
			hGlobal,
			( hGlobal != NULL ) ? FALSE : TRUE,
			&pStream
			);
	if( sc != S_OK )
	{
		ASSERT( FALSE );
		return NULL;
	}
	ASSERT( pStream != NULL );
COleStreamFile _file;
	try
	{
		_file.Attach( pStream );
		CArchive ar( &_file, CArchive::store|CArchive::bNoFlushOnDelete );
		CRuntimeClass * pRTC = pPM->GetRuntimeClass();
		ASSERT( pRTC != NULL );
		ar.WriteClass( pRTC );
		pPM->SerializeSynchronizationData( ar );
		ar.Flush();
		ULARGE_INTEGER liNewPosition;
		LARGE_INTEGER liOff;
		LISet32( liOff, 0 );
		sc = pStream->Seek( liOff, STREAM_SEEK_SET, &liNewPosition );
		if( sc != S_OK )
		{
			ASSERT( FALSE );
			::AfxThrowUserException();
		}
		return pStream;
	} // try
	catch( CException * pException )
	{
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
	} // catch( ... )
	_file.Detach();
	pStream->Release();
	return NULL;
}

void CExtPaintManager::CExtPaintManagerAutoPtr::PmSynchronizeSink_SynchronizeFrom(
	LPSTREAM pStream
	)
{
__PROF_UIS_MANAGE_STATE;
	if( pStream == NULL )
		return;
	if( PmSynchronizeSink_IsSynchronizing() )
		return;
	m_bPmSynchronizeSinkIsSynchronizing = true;
COleStreamFile _file;
	_file.Attach( pStream );
	try
	{
		CArchive ar( &_file, CArchive::load|CArchive::bNoFlushOnDelete, 0 );
		CRuntimeClass * pRTC = ar.ReadClass();
		ASSERT( pRTC != NULL );
		CObject * pObj = pRTC->CreateObject();
		if( pObj != NULL )
		{
			ASSERT_VALID( pObj );
			CExtPaintManager * pPaintManager =
				DYNAMIC_DOWNCAST( CExtPaintManager, pObj );
			if( pPaintManager == NULL )
			{
				ASSERT( FALSE );
				delete pObj;
			} // if( pPaintManager == NULL )
			else
			{
				pPaintManager->SerializeSynchronizationData( ar );
				InstallPaintManager( pPaintManager );
				PmSynchronizeSink_SynchronizeOthers();
			} // else from if( pPaintManager == NULL )
		} // if( pObj != NULL )
	} // try
	catch( CException * pException )
	{
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
	} // catch( ... )
	_file.Detach();
	m_bPmSynchronizeSinkIsSynchronizing = false;
}

void CExtPmSynchronizeSink::PmSynchronizeSink_SynchronizeOthers(
	HGLOBAL hGlobal // = NULL
	)
{
__PROF_UIS_MANAGE_STATE;
LPSTREAM pStream = PmSynchronizeSink_GetData( hGlobal );
	if( pStream == NULL )
		return;
ULARGE_INTEGER liNewPosition;
LARGE_INTEGER liOff;
INT nIndex, nCount = INT( m_arrPmSynchronizeSink.GetSize() );
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtPmSynchronizeSink * pRegistered = m_arrPmSynchronizeSink[ nIndex ];
		ASSERT( pRegistered != NULL );
		pRegistered->PmSynchronizeSink_SynchronizeFrom( pStream );
		LISet32( liOff, 0 );
		SCODE sc = pStream->Seek( liOff, STREAM_SEEK_SET, &liNewPosition );
		if( sc != S_OK )
		{
			ASSERT( FALSE );
			break;
		}
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	pStream->Release();
}

CExtPaintManager::CExtPaintManagerAutoPtr::CExtPaintManagerAutoPtr()
	: m_pPaintManager( NULL )
	, m_bHaveScrollLines( false )
	, m_uCachedScrollLines( 0 )
	, m_bUxValidColorsExtracted( false )
	, m_eCurrentTheme( ThemeUnknown )
	, m_strOsVer( _T("Unknown") )
	, m_bIsWin32s( false )
	, m_bIsWin9x( false )
	, m_bIsWin95( false )
	, m_bIsWin98( false )
	, m_bIsWin98orLater( false )
	, m_bIsWinNT( false )
	, m_bIsWinNT4( false )
	, m_bIsWin2000( false )
	, m_bIsWinXP( false )
	, m_bIsWinNT4orLater( false )
	, m_bIsWin2000orLater( false )
	, m_bIsWinXPorLater( false )
	, m_bIsWinVista( false )
	, m_bIsWinVistaOrLater( false )
	, m_bIsWin7( false )
	, m_bIsWin7OrLater( false )
	, m_hDllMsImg( NULL )
	, m_hDllUser32( NULL )
	, m_hDllSHLWAPI( NULL )
	, m_pfnMsImgGradientFill( NULL )
	, m_pfnMsImgAlphaBlend( NULL )
	, m_pfnMsImgTransparentBlt( NULL )
	, m_pfnAnimateWindow( NULL )
	, m_pfnSetLayeredWindowAttributes( NULL )
	, m_pfnUpdateLayeredWindow( NULL )
	, m_pfnPrintWindow( NULL )
	, m_pfnSHAutoComplete( NULL )
	, m_nLPX( 96 )
	, m_nLPY( 96 )
	, m_nLPZ( ( m_nLPX + m_nLPY ) / 2 )
	, m_bEnableUiScalingX( true )
	, m_bEnableUiScalingY( true )
	, m_bEnableUiScalingZ( true )
	, m_bLayeredHighlighting2005( false )
	, m_bAllowWndUpdateSourceDetection( true )
	, m_bAutoSkinScrollBars( true )
	, m_nInstallPaintManagerCounter( 0 )
	, m_nMsgPaintManagerChanged( ::RegisterWindowMessage( _T("CExtPaintManager::CExtPaintManagerAutoPtr::m_nMsgPaintManagerChanged") ) )
{
#ifdef WM_THEMECHANGED
	ASSERT( __ExtMfc_WM_THEMECHANGED == WM_THEMECHANGED );
#endif // WM_THEMECHANGED
	
	memset((char *)&m_osVerData,0,sizeof(OSVERSIONINFO));
	m_osVerData.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

CExtSafeString sTmp( _T("") );
	VERIFY( ::GetVersionEx(&m_osVerData) );
	switch(m_osVerData.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		m_bIsWin32s = true;
		sTmp = _T("Win32s");
	break;
	case VER_PLATFORM_WIN32_WINDOWS:
	{
		m_bIsWin9x = true;
		m_bIsWin95 =
			(	( m_osVerData.dwMajorVersion == 4 )
			&&	( m_osVerData.dwMinorVersion == 0 )
			);
		m_bIsWin98orLater = 
			(	( m_osVerData.dwMajorVersion > 4 )
			||	(	( m_osVerData.dwMajorVersion == 4 )
				&&	( m_osVerData.dwMinorVersion > 0 )
				)
			);
		sTmp = _T("Windows ");
		if( m_bIsWin98orLater )
		{
			if(		( m_osVerData.dwMajorVersion == 4 )
				&&	( m_osVerData.dwMinorVersion > 0 )
				)
			{
				m_bIsWin98 = true;
				sTmp += _T("98");
			}
			else
				sTmp += _T("98 or later");
		}
		else
		{
			sTmp += _T("95");
		}
	}
	break;
	case VER_PLATFORM_WIN32_NT:
		m_bIsWinNT = true;
		if( m_osVerData.dwMajorVersion == 6 )
		{
			if( m_osVerData.dwMinorVersion == 0 )
			{
				m_bIsWinVista = true;
				sTmp = _T("Windows Vista");
			}
			else
			{
				m_bIsWin7 = true;
				sTmp = _T("Windows 7");
			}
		}
		else if( m_osVerData.dwMajorVersion == 5 )
		{
			if( m_osVerData.dwMinorVersion == 0 )
			{
				m_bIsWin2000 = true;
				sTmp = _T("Windows 2000");
			}
			else if( m_osVerData.dwMinorVersion == 1 )
			{
				m_bIsWinXP = true;
				sTmp = _T("Windows XP");
			}
		}
		else if( m_osVerData.dwMajorVersion == 4 )
		{
			m_bIsWinNT4 = true;
			sTmp = _T("Windows NT");
		}
		else
			sTmp = _T("Windows NT");
		
		if( m_osVerData.dwMajorVersion >= 4 )
			m_bIsWinNT4orLater = true;
		if( m_osVerData.dwMajorVersion >= 5 )
		{
			m_bIsWin2000orLater = true;
			if(	!	(	m_osVerData.dwMajorVersion == 5
					&&	m_osVerData.dwMinorVersion == 0
					)
				)
				m_bIsWinXPorLater = true;
			if( m_osVerData.dwMajorVersion >= 6 )
			{
				m_bIsWinVistaOrLater = true;
				if( m_osVerData.dwMinorVersion >= 1 )
					m_bIsWin7OrLater = true;
			}
		}
	break;
	} // switch(m_osVerData.dwPlatformId)

	if( ! m_bIsWin2000orLater )
		m_bAutoSkinScrollBars = false;

	m_strOsVer.Format(
		_T("%s v.%lu.%lu (build %lu) %s"),
		sTmp,
		m_osVerData.dwMajorVersion, 
		m_osVerData.dwMinorVersion, 
		m_osVerData.dwBuildNumber,
		m_osVerData.szCSDVersion
		);
	
	// init uxtheme.dll data
	InitUserExApi();

	// init msimg32.dll data
	if( m_bIsWin2000orLater )
	{
		m_hDllMsImg = ::LoadLibrary( _T("msimg32.dll") );
		if( m_hDllMsImg != NULL )
		{
			m_pfnMsImgGradientFill		= (PFNGRADIENTFILL)			::GetProcAddress( m_hDllMsImg, "GradientFill" );
			m_pfnMsImgAlphaBlend		= (PFNALPHABLEND)			::GetProcAddress( m_hDllMsImg, "AlphaBlend" );
			m_pfnMsImgAlphaDIBBlend		= (PFNALPHADIBBLEND)		::GetProcAddress( m_hDllMsImg, "AlphaDIBBlend" );
			m_pfnMsImgTransparentBlt	= (PFNTRANSPARENTBLT)		::GetProcAddress( m_hDllMsImg, "TransparentBlt" );
			m_pfnMsImgTransparentDIBits	= (PFNTRANSPARENTDIBITS)	::GetProcAddress( m_hDllMsImg, "TransparentDIBits" );
		}
	} // if( m_bIsWin2000orLater )

	// init user32.dll data
	m_hDllUser32 = ::LoadLibrary( _T("user32.dll") );
	if( m_hDllUser32 != NULL )
	{
		m_pfnAnimateWindow				= (PFNANIMATEWINDOW)				::GetProcAddress( m_hDllUser32, "AnimateWindow" );
		m_pfnSetLayeredWindowAttributes	= (PFNSETLAYEREDWINDOWATTRIBUTES)	::GetProcAddress( m_hDllUser32, "SetLayeredWindowAttributes" );
		m_pfnUpdateLayeredWindow		= (PFNUPDATELAYEREDWINDOW)			::GetProcAddress( m_hDllUser32, "UpdateLayeredWindow" );
		m_pfnPrintWindow				= (PFNPRINTWINDOW)					::GetProcAddress( m_hDllUser32, "PrintWindow" );
		BOOL ( WINAPI * pfnSetProcessDPIAware )( VOID ) = ( BOOL ( WINAPI * )( VOID ) ) ::GetProcAddress( m_hDllUser32, "SetProcessDPIAware" );
		if( pfnSetProcessDPIAware != NULL )
			pfnSetProcessDPIAware();
// 		VOID ( WINAPI * pfnDisableProcessWindowsGhosting )( VOID ) = ( VOID ( WINAPI * )( VOID ) ) ::GetProcAddress( m_hDllUser32, "DisableProcessWindowsGhosting" );
// 		if( pfnDisableProcessWindowsGhosting != NULL )
// 			pfnDisableProcessWindowsGhosting();
	} // if( m_hDllUser32 != NULL )

	// init SHLWAPI.DLL data
	m_hDllSHLWAPI = ::LoadLibrary( _T("SHLWAPI.DLL") );
	if( m_hDllSHLWAPI != NULL )
	{
		m_pfnSHAutoComplete = (PFNSHAUTOCOMPLETE) ::GetProcAddress( m_hDllSHLWAPI, "SHAutoComplete" );
	} // if( m_hDllSHLWAPI != NULL )

// 	VERIFY(
// 		InitPaintManagerInstance()
// 		);

CWindowDC dcDesktop( NULL );
	m_nLPX = ::GetDeviceCaps( dcDesktop.m_hDC, LOGPIXELSX );
	m_nLPY = ::GetDeviceCaps( dcDesktop.m_hDC, LOGPIXELSY );
	m_nLPZ = ( m_nLPX + m_nLPY ) / 2;

	if(		m_bIsWin2000orLater
		&&	CExtPaintManager::stat_GetBPP() >= 15
		&&	m_pfnUpdateLayeredWindow != NULL
		)
		m_bLayeredHighlighting2005 = true;
}

void CExtPaintManager::CExtPaintManagerAutoPtr::InitUserExApi()
{
	m_bUxValidColorsExtracted = false;
	m_eCurrentTheme = ThemeUnknown;

	if( m_UxTheme.IsAppThemed() )
	{
		// theme name
		WCHAR pszThemeName[MAX_PATH] = L"";
		WCHAR pszColor[MAX_PATH] = L"";

		if( m_UxTheme.GetCurrentThemeName(
				pszThemeName, 
				MAX_PATH, 
				pszColor, 
				MAX_PATH, 
				NULL, 
				0 
				) == S_OK
			)
		{	
			if( wcsstr( __EXT_MFC_WCSLWR( pszThemeName, wcslen( pszThemeName ) + 1 ), L"luna.msstyles") != 0 )
			{
				if( wcscmp( __EXT_MFC_WCSLWR( pszColor, wcslen( pszColor ) + 1 ), L"normalcolor") == 0 )
					m_eCurrentTheme = ThemeLunaBlue;
				else if( wcscmp( __EXT_MFC_WCSLWR( pszColor, wcslen( pszColor ) + 1 ), L"homestead") == 0 )
					m_eCurrentTheme = ThemeLunaOlive;
				else if( wcscmp( __EXT_MFC_WCSLWR( pszColor, wcslen( pszColor ) + 1 ), L"metallic") == 0 )
					m_eCurrentTheme = ThemeLunaSilver;
			}
			else if( wcsstr( __EXT_MFC_WCSLWR( pszThemeName, wcslen( pszThemeName ) + 1 ), L"royale.msstyles") != 0 )
			{
				if( wcscmp( __EXT_MFC_WCSLWR( pszColor, wcslen( pszColor ) + 1 ), L"normalcolor") == 0 )
					m_eCurrentTheme = ThemeLunaRoyale;
			}
		}
		
		HWND hWndDesktop = ::GetDesktopWindow();
		ASSERT( hWndDesktop != NULL );
		if(	m_UxTheme.OpenThemeData( hWndDesktop, L"TASKBAND" ) != NULL )
		{
			if(		m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_EDGELIGHTCOLOR,&m_clrUxTaskBandEdgeLightColor) == S_OK
				&&	m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_EDGEHIGHLIGHTCOLOR,&m_clrUxTaskBandEdgeHighLightColor) == S_OK
				&&	m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_EDGESHADOWCOLOR,&m_clrUxTaskBandEdgeShadowColor) == S_OK
				&&	m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_EDGEDKSHADOWCOLOR,&m_clrUxTaskBandEdgeDkShadowColor) == S_OK
				&&	m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_EDGEFILLCOLOR,&m_clrUxTaskBandEdgeFillColor) == S_OK
				&&	m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_FILLCOLORHINT,&m_clrUxTaskBandFillColorHint) == S_OK
				&&	m_UxTheme.GetThemeColor(TDP_FLASHBUTTON,0,TMT_ACCENTCOLORHINT,&m_clrUxTaskBandAccentColorHint) == S_OK
				)
				m_bUxValidColorsExtracted = true;
			m_UxTheme.CloseThemeData();
		}
	} // if( m_UxTheme.IsAppThemed() )
}

CExtPaintManager::CExtPaintManagerAutoPtr::~CExtPaintManagerAutoPtr()
{
	UnadviseAll();
	if( m_hDllMsImg != NULL )
		::FreeLibrary( m_hDllMsImg );
	if( m_hDllUser32 != NULL )
		::FreeLibrary( m_hDllUser32 );
	if( m_pPaintManager != NULL )
		delete m_pPaintManager;
	m_pPaintManager = NULL;
}

void CExtPaintManager::CExtPaintManagerAutoPtr::Advise(
	CExtPmBridge * pPmBridge,
	bool bAddTail // = true
	)
{
	ASSERT( pPmBridge != NULL );
	if( bAddTail )
		m_listBridges.AddTail( pPmBridge );
	else
		m_listBridges.AddHead( pPmBridge );
}

void CExtPaintManager::CExtPaintManagerAutoPtr::Unadvise( CExtPmBridge * pPmBridge )
{
	ASSERT( pPmBridge != NULL );
POSITION pos = m_listBridges.Find( pPmBridge );
	if( pos != NULL )
		m_listBridges.RemoveAt( pos );
}

void CExtPaintManager::CExtPaintManagerAutoPtr::UnadviseAll()
{
	m_listBridges.RemoveAll();
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::InitPaintManagerInstance()
{
	if( m_pPaintManager != NULL )
		return true;
	return
		InstallPaintManager(
			(CExtPaintManager *)NULL
			);
}

CExtPaintManager * CExtPaintManager::CExtPaintManagerAutoPtr::GetPM()
{
	InitPaintManagerInstance();
	ASSERT( m_pPaintManager != NULL );
	return m_pPaintManager;
}

CExtPaintManager *
	CExtPaintManager::CExtPaintManagerAutoPtr::
		operator -> ()
{
	return GetPM();
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::
	InstallPaintManager(
		CExtPaintManager * pPaintManager
		)
{
	ASSERT( m_nInstallPaintManagerCounter >= 0 );
	if( m_nInstallPaintManagerCounter != 0 )
		return false;

	if( m_pPaintManager != NULL )
	{
		if( pPaintManager == m_pPaintManager )
			return true;
		if( m_pPaintManager != NULL )
		{
			delete m_pPaintManager;
			m_pPaintManager = NULL;
		}
	} // if( m_pPaintManager != NULL )

	if( pPaintManager != NULL )
		m_pPaintManager = pPaintManager;
	else
	{
		try
		{
			m_pPaintManager = new __DEFAULT_PAINT_MANAGER_CLASS;
		}
		catch( CException * pException )
		{
			pException->Delete();
		}
		catch( ... )
		{
		}
		if( m_pPaintManager == NULL )
			return false;
	}

	m_nInstallPaintManagerCounter ++;
	m_pPaintManager->SyncSysColors();
	m_pPaintManager->InitTranslatedColors();
	m_pPaintManager->InitHelperBrushes();

	g_CmdManager.OnGlobalPaintManagerChanged();

POSITION pos = m_listBridges.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPmBridge * pPmBridge = m_listBridges.GetNext( pos );
		ASSERT( pPmBridge != NULL );
		pPmBridge->PmBridge_OnPaintManagerChanged(
			m_pPaintManager
			);
	} // for( ; pos != NULL; )

	PmSynchronizeSink_SynchronizeOthers();
	m_nInstallPaintManagerCounter --;

	return true;
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::
	InstallPaintManager(
		CRuntimeClass * pRtcPaintManager
		)
{
	if( pRtcPaintManager == NULL )
		return InitPaintManagerInstance();
CObject* pObj = pRtcPaintManager->CreateObject();
	if( pObj == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( pObj );
CExtPaintManager * pPaintManager =
		DYNAMIC_DOWNCAST( CExtPaintManager, pObj );
	if( pPaintManager == NULL )
	{
		delete pObj;
		ASSERT( FALSE );
		return false;
	}
	return InstallPaintManager( pPaintManager );
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::PaintManagerStateLoad(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	HKEY hKeyRoot, // = HKEY_CURRENT_USER
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
bool bRetVal = false;
	try
	{
		CExtSafeString sRegKeyPath =
			CExtCmdManager::GetSubSystemRegKeyPath(
				__PROF_UIS_REG_PAINT_MANAGER,
				sSectionNameProfile,
				sSectionNameCompany,
				sSectionNameProduct
				);
		CMemFile _file;
		if( ! CExtCmdManager::FileObjFromRegistry(
				_file,
				LPCTSTR(sRegKeyPath),
				hKeyRoot,
				bEnableThrowExceptions
				)
			)
		{
			ASSERT( ! bEnableThrowExceptions );
			return false;
		}
		CArchive ar(
			&_file,
			CArchive::load
			);
		bRetVal = 
			PaintManagerStateSerialize(
				ar,
				bEnableThrowExceptions
				);
		ar.Close();
		ASSERT( bRetVal || (! bEnableThrowExceptions ) );
	}
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
	}
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
	}
	return bRetVal;
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::PaintManagerStateSave(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%\%sSectionNameProfile%
	HKEY hKeyRoot, // = HKEY_CURRENT_USER
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
bool bRetVal = false;
	try
	{
		CExtSafeString sRegKeyPath =
			CExtCmdManager::GetSubSystemRegKeyPath(
				__PROF_UIS_REG_PAINT_MANAGER,
				sSectionNameProfile,
				sSectionNameCompany,
				sSectionNameProduct
				);
		CMemFile _file;
		CArchive ar(
			&_file,
			CArchive::store
			);
		bRetVal = 
			PaintManagerStateSerialize(
				ar,
				bEnableThrowExceptions
				);
		ASSERT( bRetVal || (! bEnableThrowExceptions ) );
		if( ! bRetVal )
			return false;
		ar.Flush();
		ar.Close();
		bRetVal =
			CExtCmdManager::FileObjToRegistry(
				_file,
				sRegKeyPath,
				hKeyRoot,
				bEnableThrowExceptions
				);
		ASSERT( bRetVal || (! bEnableThrowExceptions ) );
	}
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
	}
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
	}
	return bRetVal;
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::PaintManagerStateSerialize(
	CArchive & ar,
	bool bEnableThrowExceptions // = false
	)
{
	try
	{
		DWORD dwGeneralFlags = 0;
		DWORD dwReserved = 0L;
		if( ar.IsStoring() )
		{
			ar << dwGeneralFlags;
			ar << dwReserved;
			ar << dwReserved;
			ar << dwReserved;
			ar << dwReserved;
			CExtPaintManager * pPM = g_PaintManager.GetPM();
			CRuntimeClass * pRTC = pPM->GetRuntimeClass();
			ASSERT( pRTC != NULL );
			ar.WriteClass( pRTC );

			pPM->OnStateSerialize(
				ar,
				bEnableThrowExceptions
				);
		} // if( ar.IsStoring() )
		else
		{
			ar >> dwGeneralFlags;
			ar >> dwReserved;
			ar >> dwReserved;
			ar >> dwReserved;
			ar >> dwReserved;
			CRuntimeClass * pRTC = ar.ReadClass();
			ASSERT( pRTC != NULL );
			CExtPaintManager * pPM = 
				STATIC_DOWNCAST( CExtPaintManager, pRTC->CreateObject() );
			ASSERT_VALID( pPM );
			ASSERT_KINDOF( CExtPaintManager, pPM );

			pPM->OnStateSerialize(
				ar,
				bEnableThrowExceptions
				);
			
			InstallPaintManager( pPM );
		} // else from if( ar.IsStoring() )
		dwGeneralFlags;
		dwReserved;
		return true;
	}
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
	}
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
	}
	return false;
}

UINT CExtPaintManager::CExtPaintManagerAutoPtr::GetMouseWheelScrollLines()
{
	if( m_bHaveScrollLines )
		return m_uCachedScrollLines;
	m_bHaveScrollLines = true;
	m_uCachedScrollLines = 3;

#if _MFC_VER < 0x700
	if( ! afxData.bWin4 )
	{
		HKEY hKey;
		if(	::RegOpenKeyEx(
				HKEY_CURRENT_USER,
				_T("Control Panel\\Desktop"),
				0,
				KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS
				)
		{
			TCHAR szData[256];
			DWORD dwKeyDataType;
			DWORD dwDataBufSize = _countof(szData);
			if( RegQueryValueEx(
					hKey,
					_T("WheelScrollLines"),
					NULL,
					&dwKeyDataType,
					(LPBYTE) &szData,
					&dwDataBufSize
					) == ERROR_SUCCESS
					)
			{
				m_uCachedScrollLines =
					_tcstoul( szData, NULL, 10 );
			}
			RegCloseKey(hKey);
		}
	}
	else
#endif // _MFC_VER < 0x700
		if( ! m_bIsWin95 )
			::SystemParametersInfo(
				SPI_GETWHEELSCROLLLINES,
				0,
				&m_uCachedScrollLines,
				0
				);

	if( m_uCachedScrollLines < 2 )
		m_uCachedScrollLines = 2;

	return m_uCachedScrollLines;
}

bool CExtPaintManager::CExtPaintManagerAutoPtr::IsWndUpdateSource(
	CWnd * pWndNotifySrc
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL );
	ASSERT( m_pPaintManager != NULL );

	if( ! m_bAllowWndUpdateSourceDetection )
		return false;

#ifndef __EXT_MFC_NO_STATUSBAR
	if( pWndNotifySrc->IsKindOf(RUNTIME_CLASS(CExtStatusControlBar)) )
		return true;
#endif // __EXT_MFC_NO_STATUSBAR

CFrameWnd * pFrame = DYNAMIC_DOWNCAST( CFrameWnd, pWndNotifySrc );
	if( pFrame == NULL )
		pFrame = pWndNotifySrc->GetParentFrame();
	if( pFrame == NULL )
		return true;
	if( pFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd)) )
		pFrame = pFrame->GetParentFrame();
	if(		pFrame == NULL
		||	pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd))
		)
		return true;
CWnd * pWnd = pFrame->GetControlBar( AFX_IDW_STATUS_BAR );

#ifndef __EXT_MFC_NO_STATUSBAR
	if(		pWnd != NULL
		&&	pWnd->IsKindOf(RUNTIME_CLASS(CExtStatusControlBar))
		)
	{
		if( pWnd == pWndNotifySrc )
			return true;
		return false;
	}
#endif // __EXT_MFC_NO_STATUSBAR

	pWnd = pFrame->GetControlBar( AFX_IDW_DOCKBAR_TOP );
	if(		pWnd != NULL
		&&	pWnd->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar))
		)
	{
		if( pWnd == pWndNotifySrc )
			return true;
		return false;
	}
	pWnd = pFrame->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM );
	if(		pWnd != NULL
		&&	pWnd->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar))
		)
	{
		if( pWnd == pWndNotifySrc )
			return true;
		return false;
	}
	pWnd = pFrame->GetControlBar( AFX_IDW_DOCKBAR_LEFT );
	if(		pWnd != NULL
		&&	pWnd->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar))
		)
	{
		if( pWnd == pWndNotifySrc )
			return true;
		return false;
	}
	pWnd = pFrame->GetControlBar( AFX_IDW_DOCKBAR_RIGHT );
	if(		pWnd != NULL
		&&	pWnd->IsKindOf(RUNTIME_CLASS(CExtDockOuterBar))
		)
	{
		if( pWnd == pWndNotifySrc )
			return true;
		return false;
	}
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
	for(	pWnd = pFrame->GetWindow(GW_CHILD);
			pWnd != NULL;
			pWnd = pWnd->GetWindow(GW_HWNDNEXT)
			)
	{
		CExtTabWnd * pExtTabWnd = DYNAMIC_DOWNCAST( CExtTabMdiWnd, pWnd );
		if(		pExtTabWnd != NULL
			&&	pExtTabWnd->_IsMdiTabCtrl()
			)
		{
			if( pWnd == pWndNotifySrc )
				return true;
			return false;
		}
	}
#endif // if (!defined __EXT_MFC_NO_TABMDI_CTRL)
	return true;
}

void CExtPaintManager::CExtPaintManagerAutoPtr::OnSysColorChange(
	CWnd * pWndNotifySrc
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL );
	ASSERT( m_pPaintManager != NULL );
	if( ! IsWndUpdateSource( pWndNotifySrc ) )
		return;
	m_pPaintManager->OnSysColorChange();

POSITION pos = m_listBridges.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPmBridge * pPmBridge = m_listBridges.GetNext( pos );
		ASSERT( pPmBridge != NULL );
		pPmBridge->PmBridge_OnSysColorChange(
			m_pPaintManager,
			pWndNotifySrc
			);
	} // for( ; pos != NULL; )
}

void CExtPaintManager::CExtPaintManagerAutoPtr::OnSettingChange(
	CWnd * pWndNotifySrc,
	UINT uFlags,
	__EXT_MFC_SAFE_LPCTSTR lpszSection
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL );
	ASSERT( m_pPaintManager != NULL );
	if( ! IsWndUpdateSource( pWndNotifySrc ) )
		return;

	if(		m_bIsWin2000orLater
		&&	CExtPaintManager::stat_GetBPP() >= 15
		&&	m_pfnUpdateLayeredWindow != NULL
		)
		m_bLayeredHighlighting2005 = true;

	m_bHaveScrollLines = false;
	m_pPaintManager->OnSettingChange( uFlags, lpszSection );

POSITION pos = m_listBridges.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPmBridge * pPmBridge = m_listBridges.GetNext( pos );
		ASSERT( pPmBridge != NULL );
		pPmBridge->PmBridge_OnSettingChange(
			m_pPaintManager,
			pWndNotifySrc,
			uFlags,
			lpszSection
			);
	} // for( ; pos != NULL; )
}

void CExtPaintManager::CExtPaintManagerAutoPtr::OnDisplayChange(
	CWnd * pWndNotifySrc,
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL );
	ASSERT( m_pPaintManager != NULL );
	if( ! IsWndUpdateSource( pWndNotifySrc ) )
		return;

	if(		m_bIsWin2000orLater
		&&	CExtPaintManager::stat_GetBPP() >= 15
		&&	m_pfnUpdateLayeredWindow != NULL
		)
		m_bLayeredHighlighting2005 = true;

	m_bHaveScrollLines = false;
	m_pPaintManager->OnDisplayChange( nDepthBPP, ptSizes );

POSITION pos = m_listBridges.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPmBridge * pPmBridge = m_listBridges.GetNext( pos );
		ASSERT( pPmBridge != NULL );
		pPmBridge->PmBridge_OnDisplayChange(
			m_pPaintManager,
			pWndNotifySrc,
			nDepthBPP,
			ptSizes
			);
	} // for( ; pos != NULL; )
}

void CExtPaintManager::CExtPaintManagerAutoPtr::OnThemeChanged(
	CWnd * pWndNotifySrc,
	WPARAM wParam,
	LPARAM lParam
	)
{
	ASSERT_VALID( pWndNotifySrc );
	ASSERT( pWndNotifySrc->GetSafeHwnd() != NULL );
	ASSERT( m_pPaintManager != NULL );
	if( ! IsWndUpdateSource( pWndNotifySrc ) )
		return;

	m_bHaveScrollLines = false;
	m_pPaintManager->OnThemeChanged( wParam, lParam );

POSITION pos = m_listBridges.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPmBridge * pPmBridge = m_listBridges.GetNext( pos );
		ASSERT( pPmBridge != NULL );
		pPmBridge->PmBridge_OnThemeChanged(
			m_pPaintManager,
			pWndNotifySrc,
			wParam,
			lParam
			);
	} // for( ; pos != NULL; )
}

const BYTE CExtPaintManager::g_arrWinXpTreeBoxExpandedBmpData_small[] = {
	0x42, 0x4D, 0x32, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD3, 0xC2, 0xB0, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5,
	0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xD3, 0xC2,
	0xB0, 0x00, 0xB5, 0x98, 0x78, 0xBF, 0xCC, 0xD2, 0xAE, 0xBE, 0xC6, 0xA8, 0xB8, 0xC2, 0xA7, 0xB8,
	0xC1, 0xA7, 0xB8, 0xC1, 0xA6, 0xB7, 0xC0, 0xAA, 0xBA, 0xC3, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98,
	0x78, 0xD9, 0xE1, 0xE4, 0xCF, 0xD8, 0xDC, 0xC9, 0xD3, 0xD8, 0xC7, 0xD2, 0xD7, 0xC6, 0xD1, 0xD6,
	0xC0, 0xCC, 0xD2, 0xBB, 0xC8, 0xCF, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 0x78, 0xEE, 0xF2, 0xF2,
	0xEC, 0xF0, 0xF0, 0xE7, 0xED, 0xED, 0xE6, 0xEB, 0xEC, 0xE3, 0xE9, 0xEA, 0xD9, 0xE0, 0xE3, 0xCC,
	0xD6, 0xDB, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 0x78, 0xF1, 0xF5, 0xF5, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD2, 0xDB, 0xDF, 0xB5, 0x98,
	0x78, 0x00, 0xB5, 0x98, 0x78, 0xF5, 0xF7, 0xF7, 0xF5, 0xF7, 0xF7, 0xF4, 0xF7, 0xF7, 0xF4, 0xF7,
	0xF7, 0xF4, 0xF6, 0xF6, 0xEB, 0xF0, 0xF1, 0xDA, 0xE1, 0xE5, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98,
	0x78, 0xFB, 0xFC, 0xFC, 0xFB, 0xFD, 0xFD, 0xFB, 0xFD, 0xFD, 0xFB, 0xFD, 0xFD, 0xFB, 0xFC, 0xFC,
	0xFA, 0xFC, 0xFC, 0xF3, 0xF6, 0xF7, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 0x78, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xB5, 0x98, 0x78, 0x00, 0xD3, 0xC2, 0xB0, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5,
	0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xD3, 0xC2,
	0xB0, 0x00,
};

const BYTE CExtPaintManager::g_arrWinXpTreeBoxCollapsedBmpData_small[] = {
	0x42, 0x4D, 0x32, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
	0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD3, 0xC2, 0xB0, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 
	0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xD3, 0xC2, 
	0xB0, 0x00, 0xB5, 0x98, 0x78, 0xBF, 0xCC, 0xD2, 0xAE, 0xBE, 0xC6, 0xA8, 0xB8, 0xC2, 0xA7, 0xB8, 
	0xC1, 0xA7, 0xB8, 0xC1, 0xA6, 0xB7, 0xC0, 0xAA, 0xBA, 0xC3, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 
	0x78, 0xD9, 0xE1, 0xE4, 0xCF, 0xD8, 0xDC, 0xC9, 0xD3, 0xD8, 0x00, 0x00, 0x00, 0xC6, 0xD1, 0xD6, 
	0xC0, 0xCC, 0xD2, 0xBB, 0xC8, 0xCF, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 0x78, 0xEE, 0xF2, 0xF2, 
	0xEC, 0xF0, 0xF0, 0xE7, 0xED, 0xED, 0x00, 0x00, 0x00, 0xE3, 0xE9, 0xEA, 0xD9, 0xE0, 0xE3, 0xCC, 
	0xD6, 0xDB, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 0x78, 0xF1, 0xF5, 0xF5, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD2, 0xDB, 0xDF, 0xB5, 0x98, 
	0x78, 0x00, 0xB5, 0x98, 0x78, 0xF5, 0xF7, 0xF7, 0xF5, 0xF7, 0xF7, 0xF4, 0xF7, 0xF7, 0x00, 0x00, 
	0x00, 0xF4, 0xF6, 0xF6, 0xEB, 0xF0, 0xF1, 0xDA, 0xE1, 0xE5, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 
	0x78, 0xFB, 0xFC, 0xFC, 0xFB, 0xFD, 0xFD, 0xFB, 0xFD, 0xFD, 0x00, 0x00, 0x00, 0xFB, 0xFC, 0xFC, 
	0xFA, 0xFC, 0xFC, 0xF3, 0xF6, 0xF7, 0xB5, 0x98, 0x78, 0x00, 0xB5, 0x98, 0x78, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xB5, 0x98, 0x78, 0x00, 0xD3, 0xC2, 0xB0, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 
	0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xB5, 0x98, 0x78, 0xD3, 0xC2, 
	0xB0, 0x00,
};

const BYTE CExtPaintManager::g_arrWinXpTreeBoxExpandedBmpData_big[] = {
	0x42,0x4D,0xC2,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
	0x00,0x00,0x0B,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
	0x00,0x00,0x8C,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xD7,0xD1,0xCB,0x60,0x48,0x30,0x60,0x48,0x30,0x60,
	0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,
	0x30,0x60,0x48,0x30,0xD7,0xD1,0xCB,0x00,0x00,0x00,0xC0,0xA8,0x90,0xF0,0xE0,0xE0,
	0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xF0,
	0xD0,0xC0,0xF0,0xC8,0xC0,0xF0,0xC8,0xB0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,
	0x90,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,
	0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xF0,0xC8,0xC0,0x60,0x48,0x30,0x00,
	0x00,0x00,0xC0,0xA8,0x90,0xFF,0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,
	0xD0,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,
	0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,0x90,0xFF,0xF0,0xF0,0xFF,0xF0,0xE0,0xF0,
	0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,0xF0,0xD0,
	0xC0,0xF0,0xD0,0xC0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,0x90,0xFF,0xF8,0xF0,
	0xFF,0xF0,0xF0,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,
	0x48,0x30,0xF0,0xD8,0xD0,0xF0,0xD0,0xC0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,
	0xA0,0xFF,0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,0xF0,0xFF,0xF0,0xE0,0xF0,0xE8,0xE0,
	0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,0x60,0x48,0x30,0x00,
	0x00,0x00,0xC0,0xA8,0xA0,0xFF,0xF8,0xFF,0xFF,0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,
	0xF0,0xFF,0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,
	0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xB0,0xA0,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,
	0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,0xF0,0xFF,0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,
	0xE0,0xF0,0xE0,0xD0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xB0,0xA0,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,0xF0,0xFF,
	0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0x60,0x48,0x30,0x00,0x00,0x00,0xEF,0xEB,
	0xE7,0xC0,0xB0,0xA0,0xC0,0xB0,0xA0,0xC0,0xA8,0xA0,0xC0,0xA8,0xA0,0xC0,0xA8,0xA0,
	0xC0,0xA8,0x90,0xC0,0xA0,0x90,0xC0,0xA0,0x90,0xC0,0xA0,0x90,0xD7,0xD1,0xCB,0x00,
	0x00,0x00,0x00
};

const BYTE CExtPaintManager::g_arrWinXpTreeBoxCollapsedBmpData_big[] = {
	0x42,0x4D,0xC2,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
	0x00,0x00,0x0B,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
	0x00,0x00,0x8C,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xD7,0xD1,0xCB,0x60,0x48,0x30,0x60,0x48,0x30,0x60,
	0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,
	0x30,0x60,0x48,0x30,0xD7,0xD1,0xCB,0x00,0x00,0x00,0xC0,0xA8,0x90,0xF0,0xE0,0xE0,
	0xE0,0xD0,0xC0,0xE0,0xC8,0xB0,0xE0,0xC0,0xB0,0xE0,0xC0,0xB0,0xD0,0xB8,0xA0,0xD0,
	0xB8,0xA0,0xD0,0xB8,0xA0,0xD0,0xB8,0xA0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,
	0x90,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,
	0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xD0,0xB8,0xA0,0x60,0x48,0x30,0x00,
	0x00,0x00,0xC0,0xA8,0x90,0xFF,0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,
	0xD0,0x60,0x48,0x30,0xF0,0xD8,0xD0,0xF0,0xD0,0xC0,0xF0,0xD0,0xC0,0xD0,0xB8,0xA0,
	0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,0x90,0xFF,0xF0,0xF0,0xFF,0xF0,0xE0,0xF0,
	0xE8,0xE0,0xF0,0xE0,0xE0,0x60,0x48,0x30,0xF0,0xD8,0xD0,0xF0,0xD8,0xD0,0xF0,0xD0,
	0xC0,0xD0,0xB8,0xA0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,0x90,0xFF,0xF8,0xF0,
	0xFF,0xF0,0xF0,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,0x48,0x30,0x60,
	0x48,0x30,0xF0,0xD8,0xD0,0xE0,0xC0,0xB0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xA8,
	0xA0,0xFF,0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,0xF0,0xFF,0xF0,0xE0,0x60,0x48,0x30,
	0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xF0,0xD8,0xD0,0xE0,0xC0,0xB0,0x60,0x48,0x30,0x00,
	0x00,0x00,0xC0,0xA8,0xA0,0xFF,0xF8,0xFF,0xFF,0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,
	0xF0,0x60,0x48,0x30,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0xF0,0xE0,0xD0,0xE0,0xC0,0xB0,
	0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xB0,0xA0,0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,
	0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,0xF0,0xFF,0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,
	0xE0,0xE0,0xD0,0xC0,0x60,0x48,0x30,0x00,0x00,0x00,0xC0,0xB0,0xA0,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xF8,0xFF,0xFF,0xF8,0xF0,0xFF,0xF8,0xF0,0xFF,0xF0,0xF0,0xFF,
	0xF0,0xE0,0xF0,0xE8,0xE0,0xF0,0xE0,0xE0,0x60,0x48,0x30,0x00,0x00,0x00,0xEF,0xEB,
	0xE7,0xC0,0xB0,0xA0,0xC0,0xB0,0xA0,0xC0,0xA8,0xA0,0xC0,0xA8,0xA0,0xC0,0xA8,0xA0,
	0xC0,0xA8,0x90,0xC0,0xA0,0x90,0xC0,0xA0,0x90,0xC0,0xA0,0x90,0xD7,0xD1,0xCB,0x00,
	0x00,0x00,0x00
};

void CExtPaintManager::LoadWinXpTreeBox(
	CExtCmdIcon & _icon,
	bool bExpanded,
	bool bBig // = false
	)
{
	ASSERT_VALID( this );
	_icon.Empty();
	if( bBig )
	{
		if( bExpanded )
			_icon.m_bmpNormal.LoadBMP_Buffer(
				g_arrWinXpTreeBoxExpandedBmpData_big,
				sizeof(g_arrWinXpTreeBoxExpandedBmpData_big) / sizeof( g_arrWinXpTreeBoxExpandedBmpData_big[0] ),
				true
				);
		else
			_icon.m_bmpNormal.LoadBMP_Buffer(
				g_arrWinXpTreeBoxCollapsedBmpData_big,
				sizeof(g_arrWinXpTreeBoxCollapsedBmpData_big) / sizeof( g_arrWinXpTreeBoxCollapsedBmpData_big[0] ),
				true
				);
		CSize _size = _icon.m_bmpNormal.GetSize();
		RECT rc;
		::SetRect( &rc, 0, 0, 1, 1 );
		_icon.m_bmpNormal.AlphaRect( rc, 0x20 );
		::SetRect( &rc, _size.cx-1, 0, _size.cx, 1 );
		_icon.m_bmpNormal.AlphaRect( rc, 0x20 );
		::SetRect( &rc, 0, _size.cy-1, 1, _size.cy );
		_icon.m_bmpNormal.AlphaRect( rc, 0x20 );
		::SetRect( &rc, _size.cx-1, _size.cy-1, _size.cx, _size.cy );
		_icon.m_bmpNormal.AlphaRect( rc, 0x20 );
	}
	else
	{
		if( bExpanded )
			_icon.m_bmpNormal.LoadBMP_Buffer(
				g_arrWinXpTreeBoxExpandedBmpData_small,
				sizeof(g_arrWinXpTreeBoxExpandedBmpData_small) / sizeof( g_arrWinXpTreeBoxExpandedBmpData_small[0] ),
				true
				);
		else
			_icon.m_bmpNormal.LoadBMP_Buffer(
				g_arrWinXpTreeBoxCollapsedBmpData_small,
				sizeof(g_arrWinXpTreeBoxCollapsedBmpData_small) / sizeof( g_arrWinXpTreeBoxCollapsedBmpData_small[0] ),
				true
				);
		_icon.m_bmpNormal.AlphaColor( RGB(255,0,255), RGB(0,0,0), 0 );
	}	
}

bool CExtPaintManager::glyph_t::stat_DefGlyphRgnTransparencyFunc(
	const glyph_t & _glyph,
	int x,
	int y,
	UINT nColor,
	LPVOID lpCookie
	)
{
	_glyph;
	x;
	y;
	lpCookie;
	return (nColor == 0) ? true : false;
}

CExtPaintManager::glyph_t::glyph_t()
{
	_Init();
}

CExtPaintManager::glyph_t::glyph_t(
	const SIZE & _size,
	UINT nColors,
	const UINT * pColorIndexes
	)
{
	m_bDynamicData = false;

	ASSERT( _size.cx > 0 );
	ASSERT( _size.cy > 0 );
	m_size.cx = _size.cx;
	m_size.cy = _size.cy;
	
	ASSERT( nColors > 0 );
	m_nColors = nColors;
	
	ASSERT( pColorIndexes != NULL );
	m_pColorIndexes = pColorIndexes;
}

CExtPaintManager::glyph_t::glyph_t(
	int cx,
	int cy,
	UINT nColors,
	const UINT * pColorIndexes
	)
{
	m_bDynamicData = false;
	
	ASSERT( cx > 0 );
	ASSERT( cy > 0 );
	m_size.cx = cx;
	m_size.cy = cy;
	
	ASSERT( nColors > 0 );
	m_nColors = nColors;

	ASSERT( pColorIndexes != NULL );
	m_pColorIndexes = pColorIndexes;
}

CExtPaintManager::glyph_t::glyph_t(
	const glyph_t & other,
	UINT nRotateAngle // = 0
			)
{
	_Init();
	_CopyFromOther(other);
#ifdef _DEBUG
	if( other.m_pColorIndexes != NULL )
	{
		ASSERT( m_pColorIndexes != NULL );
		ASSERT( m_pColorIndexes != other.m_pColorIndexes );
		ASSERT( m_bDynamicData );
	}
#endif // _DEBUG
	ASSERT(
		nRotateAngle == 0
		|| nRotateAngle == 90
		|| nRotateAngle == 180
		|| nRotateAngle == 270
		);
	Rotate(nRotateAngle);
}

CExtPaintManager::glyph_t::~glyph_t()
{
	_Done();
}

UINT CExtPaintManager::glyph_t::GetColorPixel(
	int x,
	int y
	) const
{
	ASSERT( x >= 0 && x < m_size.cx );
	ASSERT( y >= 0 && y < m_size.cy );
	ASSERT( m_size.cx > 0 );
	ASSERT( m_size.cy > 0 );
	ASSERT( m_nColors > 0 );
	ASSERT( m_pColorIndexes != NULL );
	int nOffset =
		y * m_size.cx + x;
	UINT nValue =
		*( m_pColorIndexes + nOffset );
	return nValue;
}

void CExtPaintManager::glyph_t::Rotate(UINT nRotateAngle)
{
	ASSERT(
		nRotateAngle == 0
		|| nRotateAngle == 90
		|| nRotateAngle == 180
		|| nRotateAngle == 270
		);
	ASSERT( m_bDynamicData );
	ASSERT( m_size.cx > 0 );
	ASSERT( m_size.cy > 0 );
	ASSERT( m_nColors > 0 );
	ASSERT( m_pColorIndexes != NULL );
	if( nRotateAngle == 0 )
		return;
	
	int nBufferSize =
		m_size.cx * m_size.cy;
	ASSERT( nBufferSize > 0 );
	UINT * pNew = new UINT[nBufferSize];
	ASSERT( pNew != NULL );
	if( pNew == NULL )
		return;

	int x, y;
	UINT * pOld = (UINT *)m_pColorIndexes;
	switch( nRotateAngle )
	{
	case 90:
	{
		for( y = 0; y<m_size.cy; y++ )
		{
			for( x = 0; x<m_size.cx; x++, pOld++ )
			{
				int nNewOffset =
					x*m_size.cy + (m_size.cy - y - 1);
				ASSERT(
					nNewOffset >= 0
					&&
					nNewOffset < nBufferSize
					);
				UINT * p = pNew + nNewOffset;
				*p = *pOld;
			} // for( x = 0; x<m_size.cx; x++, pOld++ )
		} // for( y = 0; y<m_size.cy; y++ )
		m_size = CSize(m_size.cy,m_size.cx);
	}
	break; // case 90
	case 180:
	{
		for( y = 0; y<m_size.cy; y++ )
		{
			for( x = 0; x<m_size.cx; x++, pOld++ )
			{
				int nNewOffset =
					(m_size.cy-y-1)*m_size.cx
					+ (m_size.cx-x-1);
				ASSERT(
					nNewOffset >= 0
					&&
					nNewOffset < nBufferSize
					);
				UINT * p = pNew + nNewOffset;
				*p = *pOld;
			} // for( x = 0; x<m_size.cx; x++, pOld++ )
		} // for( y = 0; y<m_size.cy; y++ )
	}
	break; // case 180
	case 270:
	{
		for( y = 0; y<m_size.cy; y++ )
		{
			for( x = 0; x<m_size.cx; x++, pOld++ )
			{
				int nNewOffset =
					(m_size.cx-x-1)*m_size.cy + y;
				ASSERT(
					nNewOffset >= 0
					&&
					nNewOffset < nBufferSize
					);
				UINT * p = pNew + nNewOffset;
				*p = *pOld;
			} // for( x = 0; x<m_size.cx; x++, pOld++ )
		} // for( y = 0; y<m_size.cy; y++ )
		m_size = CSize(m_size.cy,m_size.cx);
	}
	break; // case 270
	} // switch( nRotateAngle )
	UINT nSizeInBytes = 
		nBufferSize * sizeof(UINT);
	__EXT_MFC_MEMCPY(
		(UINT *)m_pColorIndexes,
		nSizeInBytes,
		pNew,
		nSizeInBytes
		);
	delete [] pNew; // ::free(pNew);
}

CSize CExtPaintManager::glyph_t::Size() const
{
	return m_size;
}

const UINT * CExtPaintManager::glyph_t::GetSurface() const
{
	ASSERT( m_size.cx > 0 );
	ASSERT( m_size.cy > 0 );
	ASSERT( m_nColors > 0 );
	ASSERT( m_pColorIndexes != NULL );
	return m_pColorIndexes;
}

UINT CExtPaintManager::glyph_t::GetColorsCount() const
{
	ASSERT( m_size.cx > 0 );
	ASSERT( m_size.cy > 0 );
	ASSERT( m_nColors > 0 );
	ASSERT( m_pColorIndexes != NULL );
	return m_nColors;
}

void CExtPaintManager::glyph_t::_Init()
{
	m_size.cx = m_size.cy = 0;
	m_nColors = 1;
	m_pColorIndexes = NULL;
	m_bDynamicData = false;
}

void CExtPaintManager::glyph_t::_Done()
{
	if( m_bDynamicData )
	{
		ASSERT( m_pColorIndexes != NULL );
		delete [] ((UINT *)m_pColorIndexes); // ::free( (void *)m_pColorIndexes );
		m_pColorIndexes = NULL;
	}
	_Init();
}

bool CExtPaintManager::glyph_t::_CopyFromOther( const glyph_t & other )
{
	_Done();

	if( other.m_pColorIndexes != NULL )
	{
		int nBufferSize =
			other.m_size.cx * other.m_size.cy;
		ASSERT( nBufferSize > 0 );
		m_pColorIndexes = new UINT[nBufferSize];
		ASSERT( m_pColorIndexes != NULL );
		if( m_pColorIndexes == NULL )
			return false;
		UINT nSizeInBytes = 
			nBufferSize * sizeof(UINT);
		__EXT_MFC_MEMCPY(
			(void *)m_pColorIndexes,
			nSizeInBytes,
			other.m_pColorIndexes,
			nSizeInBytes
			);
		m_bDynamicData = true;
	}
	m_size.cx = other.m_size.cx;
	m_size.cy = other.m_size.cy;
	
	m_nColors = other.m_nColors;

	return true;
}

void CExtPaintManager::glyph_t::_SetColorPixel(
	int x,
	int y,
	UINT nValue
	)
{
	ASSERT( x >= 0 && x < m_size.cx );
	ASSERT( y >= 0 && y < m_size.cy );
	ASSERT( m_bDynamicData );
	ASSERT( m_size.cx > 0 );
	ASSERT( m_size.cy > 0 );
	ASSERT( m_nColors > 0 );
	ASSERT( m_pColorIndexes != NULL );
	int nOffset =
		y * m_size.cx + x;
	UINT * ptr =
		(UINT *)m_pColorIndexes + nOffset;
	*ptr = nValue;
}

CRgn & CExtPaintManager::glyph_t::GenerateRgn(
	CRgn & rgn,
	bool (*pGlyphRgnTransparencyFunc)(
		const glyph_t & _glyph,
		int x,
		int y,
		UINT nColor,
		LPVOID lpCookie
		), // = stat_DefGlyphRgnTransparencyFunc
	LPVOID lpCallbackCookie,	// = NULL
	LPLONG pMinNonTransparentX, // = NULL
	LPLONG pMinNonTransparentY, // = NULL
	LPLONG pMaxNonTransparentX, // = NULL
	LPLONG pMaxNonTransparentY, // = NULL
	LPLONG pMinTransparentX,	// = NULL
	LPLONG pMinTransparentY,	// = NULL
	LPLONG pMaxTransparentX,	// = NULL
	LPLONG pMaxTransparentY		// = NULL
	) const
{
	if( pMinNonTransparentX != NULL )
		*pMinNonTransparentX = 0;
	if( pMinNonTransparentY != NULL )
		*pMinNonTransparentY = 0;
	if( pMaxNonTransparentX != NULL )
		*pMaxNonTransparentX = 0;
	if( pMaxNonTransparentY != NULL )
		*pMaxNonTransparentY = 0;

	if( pMinTransparentX != NULL )
		*pMinTransparentX = 0;
	if( pMinTransparentY != NULL )
		*pMinTransparentY = 0;
	if( pMaxTransparentX != NULL )
		*pMaxTransparentX = 0;
	if( pMaxTransparentY != NULL )
		*pMaxTransparentY = 0;

	if( rgn.GetSafeHandle() != NULL )
		rgn.DeleteObject();
	ASSERT( rgn.GetSafeHandle() == NULL );
	
	CSize _size = Size();
	if( _size.cx == 0 || _size.cy == 0 )
	{
		ASSERT( FALSE );
		return rgn;
	}
	
	if( !rgn.CreateRectRgn(0,0,0,0) )
	{
		ASSERT( FALSE );
		return rgn;
	}

	for( int x = 0; x < _size.cx; x++ )
	{
		for( int y = 0; y < _size.cy; y++ )
		{
			UINT nColor = GetColorPixel( x, y );
			if( pGlyphRgnTransparencyFunc(
					*this,
					x,
					y,
					nColor,
					lpCallbackCookie
					)
				)
			{ // if pixel transparent

				if( pMinTransparentX != NULL
					&& (*pMinTransparentX) > x
					)
					(*pMinTransparentX) = x;
				if( pMinTransparentY != NULL
					&& (*pMinTransparentY) > y
					)
					(*pMinTransparentY) = y;

				if( pMaxTransparentX != NULL
					&& (*pMaxTransparentX) < x
					)
					(*pMaxTransparentX) = x;
				if( pMaxTransparentY != NULL
					&& (*pMaxTransparentY) < y
					)
					(*pMaxTransparentY) = y;

				continue;
			} // if pixel transparent

			if( pMinNonTransparentX != NULL
				&& (*pMinNonTransparentX) > x
				)
				(*pMinNonTransparentX) = x;
			if( pMinNonTransparentY != NULL
				&& (*pMinNonTransparentY) > y
				)
				(*pMinNonTransparentY) = y;

			if( pMaxNonTransparentX != NULL
				&& (*pMaxNonTransparentX) < x
				)
				(*pMaxNonTransparentX) = x;
			if( pMaxNonTransparentY != NULL
				&& (*pMaxNonTransparentY) < y
				)
				(*pMaxNonTransparentY) = y;

			CRgn rgnPixel;
			if( !rgnPixel.CreateRectRgn(
					x,
					y,
					x+1,
					y+1
					)
				)
			{
				ASSERT( FALSE );
				return rgn;
			}
			rgn.CombineRgn( &rgn, &rgnPixel, RGN_OR );
		} // for( int y = 0; y < _size.cy; y++ )
	} // for( int x = 0; x < _size.cx; x++ )
	
	return rgn;
}

DWORD CExtPaintManager::stat_DllGetVersion( __EXT_MFC_SAFE_LPCTSTR lpszDllName )
{
// For security purposes, LoadLibrary should be provided with a 
// fully-qualified path to the DLL. The lpszDllName variable should be
// tested to ensure that it is a fully qualified path before it is used.
HINSTANCE hInstanceDLL = ::LoadLibrary( lpszDllName );
    if( hInstanceDLL == NULL )
		return 0;
DWORD dwVersion = 0;
DLLGETVERSIONPROC pDllGetVersion =  (DLLGETVERSIONPROC) ::GetProcAddress( hInstanceDLL, "DllGetVersion" );
	// Because some DLLs might not implement this function, you
    // must test for it explicitly. Depending on the particular 
    // DLL, the lack of a DllGetVersion function can be a useful
    // indicator of the version.
    if( pDllGetVersion != NULL )
    {
        DLLVERSIONINFO _dvi;
        ::memset( &_dvi, 0, sizeof(_dvi) );
        _dvi.cbSize = sizeof(_dvi);
        HRESULT hr = (*pDllGetVersion)( &_dvi );
        if( SUCCEEDED( hr ) )
           dwVersion = MAKELONG( _dvi.dwMinorVersion, _dvi.dwMajorVersion );
    }
    ::FreeLibrary( hInstanceDLL );
    return dwVersion;
}

HBITMAP CExtPaintManager::stat_GetScreenSurfacePart(
	const RECT & rcScreenSurfacePart,
	COLORREF ** ppClrSurface // = NULL
	)
{
	if( ppClrSurface != NULL )
		(*ppClrSurface) = NULL;
INT cx = rcScreenSurfacePart.right - rcScreenSurfacePart.left;
	if( cx <= 0 )
		return NULL;
INT cy = rcScreenSurfacePart.bottom - rcScreenSurfacePart.top;
	if( cy <= 0 )
		return NULL;
CDC dcmm;
	if( ! dcmm.CreateCompatibleDC( NULL ) )
		return NULL;
BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth          = cx;
	bih.biHeight         = cy;
	bih.biPlanes         = 1;
	bih.biBitCount       = 32;
	bih.biCompression    = BI_RGB;
	bih.biSizeImage      = cx * cy;
	bih.biXPelsPerMeter  = 0;
	bih.biYPelsPerMeter  = 0;
	bih.biClrUsed        = 0;
	bih.biClrImportant   = 0;
HBITMAP hDIB =
		::CreateDIBSection(
			dcmm.GetSafeHdc(),
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)ppClrSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL )
		return NULL;
HGDIOBJ hOldBmp = ::SelectObject( dcmm.m_hDC, ((HGDIOBJ)hDIB) );
CWindowDC dcDesktop( NULL );
	VERIFY(
		dcmm.BitBlt(
			0,
			0,
			cx,
			cy,
			&dcDesktop,
			rcScreenSurfacePart.left,
			rcScreenSurfacePart.top,
			SRCCOPY
			)
		);
	::SelectObject( dcmm.m_hDC, hOldBmp );
	dcmm.DeleteDC();
	return hDIB;
}

HBITMAP CExtPaintManager::stat_PrintWnd(
	HWND hWnd,
	UINT nMessage, // = WM_PRINTCLIENT
	LPARAM lParamPrint, // = PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN
	HDC hSrcDC, // = NULL
	const RECT * pRectFillSrc // = NULL
	)
{
	ASSERT( hWnd != NULL && ::IsWindow(hWnd) );
	ASSERT( nMessage == WM_PRINT || nMessage == WM_PRINTCLIENT );
CRect rcWnd, rcClient;
	::GetWindowRect( hWnd, &rcWnd );
	::GetClientRect( hWnd, &rcClient );
CSize _sizeDest =
		( (lParamPrint&PRF_NONCLIENT) != 0 )
			? rcWnd.Size()
			: rcClient.Size()
			;
HDC hDC = ::CreateCompatibleDC( NULL );
	if( hDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof(BITMAPINFOHEADER) );
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = _sizeDest.cx;
	bih.biHeight = _sizeDest.cy;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = _sizeDest.cx * _sizeDest.cy;
COLORREF * pSurface = NULL;
HBITMAP hBmpSufrace =
		::CreateDIBSection(
			hDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)&pSurface,
			NULL,
			NULL
			);
	ASSERT( hBmpSufrace != NULL );
	if( hBmpSufrace == NULL )
	{
		::DeleteDC( hDC );
		ASSERT( FALSE );
		return NULL;
	}
	ASSERT( pSurface != NULL );
HGDIOBJ hBmpOld = ::SelectObject( hDC, (HGDIOBJ)hBmpSufrace );
	if(		hSrcDC != NULL
		&&	pRectFillSrc != NULL
		&&	pRectFillSrc->left < pRectFillSrc->right
		&&	pRectFillSrc->top < pRectFillSrc->bottom
		)
		::BitBlt(
			hDC,
			0,
			0,
			pRectFillSrc->right - pRectFillSrc->left,
			pRectFillSrc->bottom - pRectFillSrc->top,
			hSrcDC,
			pRectFillSrc->left,
			pRectFillSrc->top,
			SRCCOPY
			);
LRESULT lResult =
	::SendMessage(
		hWnd,
		nMessage,
		(WPARAM)hDC,
		lParamPrint
		);
	lResult;
//	if( lResult != 0 )
//	{
//		bool bOverPaint = true;
//		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
//		if( pWnd != NULL )
//		{
//			CRuntimeClass * pRTC = pWnd->GetRuntimeClass();
//			ASSERT( pRTC != NULL );
//			if( ::strncmp( pRTC->m_lpszClassName, "CExt", 4 ) == 0 )
//				bOverPaint = false;
//		}
//		if( bOverPaint )
//		{
//			COLORREF clrBk = ::GetSysColor( COLOR_WINDOW );
//			COLORREF clrBkOld = ::SetBkColor( hDC, clrBk );
//			CRect rect( 0, 0, _sizeDest.cx, _sizeDest.cy );
//			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL );
//			::SetBkColor( hDC, clrBkOld );
//		}
//	}
	::SelectObject( hDC, (HGDIOBJ)hBmpOld );
	::DeleteDC( hDC );
	return hBmpSufrace;
}

void CExtPaintManager::stat_PrintChildren(
	HWND hWndStart,
	UINT nMessage,
	HDC hDC,
	LPARAM lParamPrint,
	bool bDeepTreeWalk, // = true
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
{
	ASSERT( hWndStart != NULL && ::IsWindow(hWndStart) );
	ASSERT( nMessage == WM_PRINT || nMessage == WM_PRINTCLIENT );
	ASSERT( hDC != NULL );
	ASSERT( (lParamPrint&PRF_CHILDREN) != 0 );
CRect rcStartWnd, rcStartClient;
	::GetWindowRect( hWndStart, &rcStartWnd );
	::GetClientRect( hWndStart, &rcStartClient );
	::ClientToScreen( hWndStart, ((LPPOINT)(&rcStartClient)) );
	::ClientToScreen( hWndStart, ((LPPOINT)(&rcStartClient))+1 );
CPoint ptStartNcOffset(
		rcStartWnd.left - rcStartClient.left,
		rcStartWnd.top - rcStartClient.top
		);
	if(		(lParamPrint&PRF_NONCLIENT) != 0
		&&	(	ptStartNcOffset.x != 0
			||	ptStartNcOffset.y != 0
			)
		)
		::OffsetViewportOrgEx(
			hDC,
			-ptStartNcOffset.x,
			-ptStartNcOffset.y,
			NULL
			);
HWND hWndChild = ::GetWindow( hWndStart, GW_CHILD );
	for( ; hWndChild != NULL; hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT ) )
	{
		__EXT_MFC_LONG_PTR dwChildStyle = ::__EXT_MFC_GetWindowLong( hWndChild, GWL_STYLE );
		if( (dwChildStyle&WS_VISIBLE) == 0 )
			continue;
		CRect rcChildWnd, rcChildClient;
		::GetWindowRect( hWndChild, &rcChildWnd );
		::GetClientRect( hWndChild, &rcChildClient );
		::ClientToScreen( hWndChild, ((LPPOINT)(&rcChildClient)) );
		::ClientToScreen( hWndChild, ((LPPOINT)(&rcChildClient))+1 );
		CPoint ptChildNcOffset(
			rcChildWnd.left - rcChildClient.left,
			rcChildWnd.top - rcChildClient.top
			);
		CPoint ptChildRenderOffset( 0, 0 );
		if( (lParamPrint&PRF_NONCLIENT) != 0 )
		{
			ptChildRenderOffset.x = rcStartClient.left - rcChildWnd.left;
			ptChildRenderOffset.y = rcStartClient.top - rcChildWnd.top;
		}
		else
		{
			ptChildRenderOffset.x = rcStartClient.left - rcChildClient.left;
			ptChildRenderOffset.y = rcStartClient.top - rcChildClient.top;
		}
		if(		ptChildRenderOffset.x != 0
			||	ptChildRenderOffset.y != 0
			)
			::OffsetViewportOrgEx(
				hDC,
				-ptChildRenderOffset.x,
				-ptChildRenderOffset.y,
				NULL
				);
		::SendMessage(
			hWndChild,
			nMessage,
			(WPARAM)hDC,
			lParamPrint
			);
		if( bDeepTreeWalk )
			stat_PrintChildren(
				hWndChild,
				nMessage,
				hDC,
				lParamPrint,
				bDeepTreeWalk,
				pHelperSrc,
				lParam
				);
		if(		ptChildRenderOffset.x != 0
			||	ptChildRenderOffset.y != 0
			)
			::OffsetViewportOrgEx(
				hDC,
				ptChildRenderOffset.x,
				ptChildRenderOffset.y,
				NULL
				);
	} // for( ; hWndChild != NULL; hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT ) )
	if(		(lParamPrint&PRF_NONCLIENT) != 0
		&&	(	ptStartNcOffset.x != 0
			||	ptStartNcOffset.y != 0
			)
		)
		::OffsetViewportOrgEx(
			hDC,
			ptStartNcOffset.x,
			ptStartNcOffset.y,
			NULL
			);
}

CExtPaintManager::SCROLLBARSKINDATA::SCROLLBARSKINDATA()
{
	Empty();
}

CExtPaintManager::SCROLLBARSKINDATA::~SCROLLBARSKINDATA()
{
	Empty();
}

void CExtPaintManager::SCROLLBARSKINDATA::Empty()
{
	m_clrZsLineLightEnabled = COLORREF(-1L);
	m_clrZsLineDarkEnabled = COLORREF(-1L);
	m_clrZsLineLightDisabled = COLORREF(-1L);
	m_clrZsLineDarkDisabled = COLORREF(-1L);
	m_bmpEntireArea.Empty();
	m_rcEntirePaddingR.SetRect( 0, 0, 0, 0 );
	m_rcEntirePaddingC.SetRect( 0, 0, 0, 0 );
INT i, j;
	for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
	{
		for( j = 0; j < INT(__ESBMHT_PART_COUNT); j++ )
		{
			CRect & rcPadding = m_arrPartPadding[i][j];
			rcPadding.SetRect( 0, 0, 0, 0 );
			CExtBitmap & _bmp = m_arrPartBmp[i][j];
			_bmp.Empty();
			m_arrClrSolidBackground[i][j] = COLORREF(-1L);
		}
	}
}

CExtBitmap & CExtPaintManager::SCROLLBARSKINDATA::GetPart(
	CExtPaintManager::e_scroll_bar_skin_part_t ePartState,
	CExtPaintManager::e_scroll_bar_mouse_hover_type_t eSBMHT, // part type
	RECT * pRectPadding, // = NULL
	COLORREF * pClrSolidBackground // = NULL
	)
{
CExtBitmap & _bmp =
		m_arrPartBmp
			[ INT(ePartState) ]
			[ INT(eSBMHT) ]
			;
COLORREF clrSolidBackground =
			m_arrClrSolidBackground
				[ INT(ePartState) ]
				[ INT(eSBMHT) ]
				;
	if( pClrSolidBackground != NULL )
		(*pClrSolidBackground) = clrSolidBackground;
	if( pRectPadding != NULL )
	{
		const RECT & rcPadding =
			m_arrPartPadding
				[ INT(ePartState) ]
				[ INT(eSBMHT) ]
				;
		::CopyRect( pRectPadding, &rcPadding );
	} // if( pRectPadding != NULL )
	if( _bmp.IsEmpty() && INT(ePartState) > 0 )
	{
		return
			GetPart(
				(CExtPaintManager::e_scroll_bar_skin_part_t)(INT(ePartState)-1),
				eSBMHT,
				pRectPadding,
				( clrSolidBackground == COLORREF(-1L) )
					? pClrSolidBackground
					: NULL
				);
	} // if( _bmp.IsEmpty() && INT(ePartState) > 0 )
	return _bmp;
}

const CExtBitmap & CExtPaintManager::SCROLLBARSKINDATA::GetPart(
	CExtPaintManager::e_scroll_bar_skin_part_t ePartState,
	CExtPaintManager::e_scroll_bar_mouse_hover_type_t eSBMHT, // part type
	RECT * pRectPadding, // = NULL
	COLORREF * pClrSolidBackground // = NULL
	) const
{
	return
		( const_cast < SCROLLBARSKINDATA * > ( this ) )
		-> GetPart( ePartState, eSBMHT, pRectPadding, pClrSolidBackground );
}

void CExtPaintManager::SCROLLBARSKINDATA::Load2007_R2(
	UINT nResourceID_BumpyGlyphStack,
	INT nBumpyGlyphStackItemHeight,
	UINT nResourceID_GripperStack,
	INT nGripperStackItemHeight,
	const RECT & rcBumpyGlyphStackItemPadding,
	COLORREF clrSolidBackground,
	COLORREF clrPressedPageAreas,
	UINT nResourceID_ArrowUp_Normal,
	UINT nResourceID_ArrowUp_Disabled,
	UINT nResourceID_ArrowDown_Normal,
	UINT nResourceID_ArrowDown_Disabled,
	bool bRotateArrowsAndBk270,
	UINT nResourceID_BkEntire,
	UINT nResourceID_BkPage,
	const RECT & rcEntirePaddingR,
	const RECT & rcEntirePaddingC
	)
{
	Empty();
INT i, j;
	for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
	{
		for( j = 0; j < INT(__ESBMHT_PART_COUNT); j++ )
		{
			CRect & rcPadding = m_arrPartPadding[i][j];
			rcPadding.SetRect( 0, 0, 0, 0 );
			CExtBitmap & _bmp = m_arrPartBmp[i][j];
			_bmp.Empty();
			m_arrClrSolidBackground[i][j] = clrSolidBackground;
			if(		( j == INT(__ESBMHT_PAGE_UP) || j == INT(__ESBMHT_PAGE_DOWN) )
				&&	i == INT(__ESBSPT_PRESSED)
				)
				m_arrClrSolidBackground[i][j] = clrPressedPageAreas;
		}
	}
CExtBitmap _bmpBumpyGlyphStack, _bmpGripperStack;
	VERIFY( _bmpBumpyGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_BumpyGlyphStack) ) );
	VERIFY( _bmpGripperStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_GripperStack) ) );
	_bmpBumpyGlyphStack.PreMultipliedRGBChannelsSet( true );
	_bmpGripperStack.PreMultipliedRGBChannelsSet( true );
CSize _sizeBumpyGlyph = _bmpBumpyGlyphStack.GetSize();
CSize _sizeGripper = _bmpGripperStack.GetSize();
CRect rcBumpyGlyph( 0, 0, _sizeBumpyGlyph.cx, nBumpyGlyphStackItemHeight );
CRect rcGripper( 0, 0, _sizeGripper.cx, nGripperStackItemHeight );
	// cold parts
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_GLYPH_THUMB_GRIPPER)].
		FromBitmap( _bmpGripperStack, rcGripper )
		);
	// normal parts
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)] = rcBumpyGlyphStackItemPadding;
	// hot parts
	rcBumpyGlyph.OffsetRect( 0, nBumpyGlyphStackItemHeight );
	rcGripper.OffsetRect( 0, nGripperStackItemHeight );
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_GLYPH_THUMB_GRIPPER)].
		FromBitmap( _bmpGripperStack, rcGripper )
		);
	// pressed parts
	rcBumpyGlyph.OffsetRect( 0, nBumpyGlyphStackItemHeight );
	rcGripper.OffsetRect( 0, nGripperStackItemHeight );
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_GLYPH_THUMB_GRIPPER)].
		FromBitmap( _bmpGripperStack, rcGripper )
		);
	// arrows
CExtBitmap _bmpArrowUpNormal, _bmpArrowUpDisabled, _bmpArrowDownNormal, _bmpArrowDownDisabled;
	VERIFY( _bmpArrowUpNormal.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowUp_Normal) ) );
	VERIFY( _bmpArrowUpDisabled.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowUp_Disabled) ) );
	VERIFY( _bmpArrowDownNormal.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowDown_Normal) ) );
	VERIFY( _bmpArrowDownDisabled.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowDown_Disabled) ) );
	_bmpArrowUpNormal.PreMultipliedRGBChannelsSet( true );
	_bmpArrowUpDisabled.PreMultipliedRGBChannelsSet( true );
	_bmpArrowDownNormal.PreMultipliedRGBChannelsSet( true );
	_bmpArrowDownDisabled.PreMultipliedRGBChannelsSet( true );
CSize _sizeArrowUpNormal = _bmpArrowUpNormal.GetSize();
CSize _sizeArrowUpDisabled = _bmpArrowUpDisabled.GetSize();
CSize _sizeArrowDownNormal = _bmpArrowDownNormal.GetSize();
CSize _sizeArrowDownDisabled = _bmpArrowDownDisabled.GetSize();
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_GLYPH_ARROW_UP)].
			CreateRotated9xStack(
				_bmpArrowUpNormal,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowUpNormal.cx,
				_sizeArrowUpNormal.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_GLYPH_ARROW_UP)].
			CreateRotated9xStack(
				_bmpArrowUpDisabled,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowUpDisabled.cx,
				_sizeArrowUpDisabled.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_GLYPH_ARROW_DOWN)].
			CreateRotated9xStack(
				_bmpArrowDownNormal,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowDownNormal.cx,
				_sizeArrowDownNormal.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_GLYPH_ARROW_DOWN)].
			CreateRotated9xStack(
				_bmpArrowDownDisabled,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowDownDisabled.cx,
				_sizeArrowDownDisabled.cy,
				false,
				false
				)
		);

	// backgrounds
CExtBitmap _bmpBkEntire, _bmpBkPage;
	VERIFY( _bmpBkEntire.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_BkEntire) ) );
	VERIFY( _bmpBkPage.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_BkPage) ) );
	_bmpBkEntire.PreMultipliedRGBChannelsSet( true );
	_bmpBkPage.PreMultipliedRGBChannelsSet( true );
CSize _sizeBkEntire = _bmpBkEntire.GetSize();
CSize _sizeBkPage = _bmpBkPage.GetSize();
	VERIFY(
		m_bmpEntireArea.
			CreateRotated9xStack(
				_bmpBkEntire,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeBkEntire.cx,
				_sizeBkEntire.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_PAGE_DOWN)].
			CreateRotated9xStack(
				_bmpBkPage,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeBkPage.cx,
				_sizeBkPage.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_PAGE_UP)].AssignFromOther(
			m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_PAGE_DOWN)]
			)
		);
	m_rcEntirePaddingR = rcEntirePaddingR;
	m_rcEntirePaddingC = rcEntirePaddingC;
}

void CExtPaintManager::SCROLLBARSKINDATA::Load2007_Zoom(
	UINT nResourceID_ButtonPlus,
	UINT nResourceID_ButtonMinus,
	UINT nResourceID_ButtonThumb,
	COLORREF clrZsLineLightEnabled,
	COLORREF clrZsLineDarkEnabled,
	COLORREF clrZsLineLightDisabled,
	COLORREF clrZsLineDarkDisabled,
	UINT nDockID,
	bool bResourceColorChannelsPreMultiplied // = true
	)
{
	Empty();
	ASSERT(
			nDockID == AFX_IDW_DOCKBAR_TOP
		||	nDockID == AFX_IDW_DOCKBAR_BOTTOM
		||	nDockID == AFX_IDW_DOCKBAR_LEFT
		||	nDockID == AFX_IDW_DOCKBAR_RIGHT
		);
INT nAngleCw90 = 0;
	if(      nDockID == AFX_IDW_DOCKBAR_BOTTOM )
			 nAngleCw90 = 180;
	else if( nDockID == AFX_IDW_DOCKBAR_LEFT )
			 nAngleCw90 = 270;
	else if( nDockID == AFX_IDW_DOCKBAR_RIGHT )
			 nAngleCw90 = 90;
INT i, j;
	for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
	{
		for( j = 0; j < INT(__ESBMHT_PART_COUNT); j++ )
		{
			CRect & rcPadding = m_arrPartPadding[i][j];
			rcPadding.SetRect( 0, 0, 0, 0 );
			CExtBitmap & _bmp = m_arrPartBmp[i][j];
			_bmp.Empty();
			m_arrClrSolidBackground[i][j] = COLORREF(-1L);
		}
	}

	m_clrZsLineLightEnabled = clrZsLineLightEnabled;
	m_clrZsLineDarkEnabled = clrZsLineDarkEnabled;
	m_clrZsLineLightDisabled = clrZsLineLightDisabled;
	m_clrZsLineDarkDisabled = clrZsLineDarkDisabled;

CRect rcGlyph;
CExtBitmap _bmpGlyphStack;

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonPlus) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	rcGlyph.SetRect( 0, 0, _bmpGlyphStack.GetSize().cx, _bmpGlyphStack.GetSize().cy / 3 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_BUTTON_DOWN)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)].AdjustAlpha( -0.25 );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rcGlyph ) );

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonMinus) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	rcGlyph.SetRect( 0, 0, _bmpGlyphStack.GetSize().cx, _bmpGlyphStack.GetSize().cy / 3 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_BUTTON_UP)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)].AdjustAlpha( -0.25 );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rcGlyph ) );

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonThumb) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	rcGlyph.SetRect( 0, 0, _bmpGlyphStack.GetSize().cx, _bmpGlyphStack.GetSize().cy / 3 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)].AdjustAlpha( -0.25 );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	if( nAngleCw90 != 0 )
	{
		for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
		{
			CExtBitmap & _bmp1 = m_arrPartBmp[i][__ESBMHT_THUMB];
			if( _bmp1.IsEmpty() )
				continue;
			CExtBitmap _bmp2;
			VERIFY( _bmp2.CreateRotated9xStack( _bmp1, nAngleCw90, 1, _bmp1.GetSize().cx, _bmp1.GetSize().cy, false, false ) );
			_bmp1 = _bmp2;
		}
	}
}

void CExtPaintManager::SCROLLBARSKINDATA::Load2010_Zoom_R1(
	UINT nResourceID_ButtonPlus,
	UINT nResourceID_ButtonMinus,
	UINT nResourceID_ButtonThumb,
	COLORREF clrZsLineLightEnabled,
	COLORREF clrZsLineDarkEnabled,
	COLORREF clrZsLineLightDisabled,
	COLORREF clrZsLineDarkDisabled,
	UINT nDockID,
	bool bResourceColorChannelsPreMultiplied // = true
	)
{
	Empty();
	ASSERT(
			nDockID == AFX_IDW_DOCKBAR_TOP
		||	nDockID == AFX_IDW_DOCKBAR_BOTTOM
		||	nDockID == AFX_IDW_DOCKBAR_LEFT
		||	nDockID == AFX_IDW_DOCKBAR_RIGHT
		);
INT nAngleCw90 = 0;
	if(      nDockID == AFX_IDW_DOCKBAR_BOTTOM )
			 nAngleCw90 = 180;
	else if( nDockID == AFX_IDW_DOCKBAR_LEFT )
			 nAngleCw90 = 270;
	else if( nDockID == AFX_IDW_DOCKBAR_RIGHT )
			 nAngleCw90 = 90;
INT i, j;
	for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
	{
		for( j = 0; j < INT(__ESBMHT_PART_COUNT); j++ )
		{
			CRect & rcPadding = m_arrPartPadding[i][j];
			rcPadding.SetRect( 0, 0, 0, 0 );
			CExtBitmap & _bmp = m_arrPartBmp[i][j];
			_bmp.Empty();
			m_arrClrSolidBackground[i][j] = COLORREF(-1L);
		}
	}

	m_clrZsLineLightEnabled = clrZsLineLightEnabled;
	m_clrZsLineDarkEnabled = clrZsLineDarkEnabled;
	m_clrZsLineLightDisabled = clrZsLineLightDisabled;
	m_clrZsLineDarkDisabled = clrZsLineDarkDisabled;

CRect rcGlyph;
CExtBitmap _bmpGlyphStack;

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonPlus) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	rcGlyph.SetRect( 0, 0, _bmpGlyphStack.GetSize().cx, _bmpGlyphStack.GetSize().cy / 3 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
			m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].MakeMono();
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_BUTTON_DOWN)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)].AdjustAlpha( -0.25 );
//rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rcGlyph ) );

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonMinus) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	rcGlyph.SetRect( 0, 0, _bmpGlyphStack.GetSize().cx, _bmpGlyphStack.GetSize().cy / 3 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
			m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].MakeMono();
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_BUTTON_UP)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)].AdjustAlpha( -0.25 );
//rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rcGlyph ) );

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonThumb) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	rcGlyph.SetRect( 0, 0, _bmpGlyphStack.GetSize().cx, _bmpGlyphStack.GetSize().cy / 2 /*3*/ );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
			m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].MakeMono();
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)].AdjustAlpha( -0.25 );
//rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	rcGlyph.OffsetRect( 0, rcGlyph.Height() );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rcGlyph ) );
	if( nAngleCw90 != 0 )
	{
		for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
		{
			CExtBitmap & _bmp1 = m_arrPartBmp[i][__ESBMHT_THUMB];
			if( _bmp1.IsEmpty() )
				continue;
			CExtBitmap _bmp2;
			VERIFY( _bmp2.CreateRotated9xStack( _bmp1, nAngleCw90, 1, _bmp1.GetSize().cx, _bmp1.GetSize().cy, false, false ) );
			_bmp1 = _bmp2;
		}
	}
}

void CExtPaintManager::SCROLLBARSKINDATA::Load2010_Zoom_R2(
	UINT nResourceID_ButtonPlus,
	UINT nResourceID_ButtonMinus,
	UINT nResourceID_ButtonThumb,
	COLORREF clrZsLineLightEnabled,
	COLORREF clrZsLineDarkEnabled,
	COLORREF clrZsLineLightDisabled,
	COLORREF clrZsLineDarkDisabled,
	UINT nDockID,
	bool bResourceColorChannelsPreMultiplied // = true
	)
{
	Empty();
	ASSERT(
			nDockID == AFX_IDW_DOCKBAR_TOP
		||	nDockID == AFX_IDW_DOCKBAR_BOTTOM
		||	nDockID == AFX_IDW_DOCKBAR_LEFT
		||	nDockID == AFX_IDW_DOCKBAR_RIGHT
		);
INT nAngleCw90 = 0;
	if(      nDockID == AFX_IDW_DOCKBAR_BOTTOM )
			 nAngleCw90 = 180;
	else if( nDockID == AFX_IDW_DOCKBAR_LEFT )
			 nAngleCw90 = 270;
	else if( nDockID == AFX_IDW_DOCKBAR_RIGHT )
			 nAngleCw90 = 90;
INT i, j;
	for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
	{
		for( j = 0; j < INT(__ESBMHT_PART_COUNT); j++ )
		{
			CRect & rcPadding = m_arrPartPadding[i][j];
			rcPadding.SetRect( 0, 0, 0, 0 );
			CExtBitmap & _bmp = m_arrPartBmp[i][j];
			_bmp.Empty();
			m_arrClrSolidBackground[i][j] = COLORREF(-1L);
		}
	}

	m_clrZsLineLightEnabled = clrZsLineLightEnabled;
	m_clrZsLineDarkEnabled = clrZsLineDarkEnabled;
	m_clrZsLineLightDisabled = clrZsLineLightDisabled;
	m_clrZsLineDarkDisabled = clrZsLineDarkDisabled;

CExtBitmap _bmpGlyphStack;

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonPlus) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
CSize sizeStack = _bmpGlyphStack.GetSize();
CSize sizeGryph( sizeStack.cx, sizeStack.cy / 3 );
CRect rc1( 0, 0, sizeGryph.cx, sizeGryph.cy );
CRect rc2 = rc1; rc2.OffsetRect( 0, sizeGryph.cy );
CRect rc3 = rc2; rc3.OffsetRect( 0, sizeGryph.cy );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rc3 ) );
			//m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].MakeMono();
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_BUTTON_DOWN)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_DOWN)].AdjustAlpha( -0.25 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rc1 ) );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)].FromBitmap( _bmpGlyphStack, rc2 ) );

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonMinus) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	sizeStack = _bmpGlyphStack.GetSize();
	sizeGryph.cx = sizeStack.cx; sizeGryph.cy = sizeStack.cy / 3;
	rc1.SetRect( 0, 0, sizeGryph.cx, sizeGryph.cy );
	rc2 = rc1; rc2.OffsetRect( 0, sizeGryph.cy );
	rc3 = rc2; rc3.OffsetRect( 0, sizeGryph.cy );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rc3 ) );
			//m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].MakeMono();
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_BUTTON_UP)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_BUTTON_UP)].AdjustAlpha( -0.25 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rc1 ) );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)].FromBitmap( _bmpGlyphStack, rc2 ) );

	VERIFY( _bmpGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ButtonThumb) ) );
	_bmpGlyphStack.PreMultipliedRGBChannelsSet( bResourceColorChannelsPreMultiplied );
	sizeStack = _bmpGlyphStack.GetSize();
	sizeGryph.cx = sizeStack.cx; sizeGryph.cy = sizeStack.cy / 3;
	rc1.SetRect( 0, 0, sizeGryph.cx, sizeGryph.cy );
	rc2 = rc1; rc2.OffsetRect( 0, sizeGryph.cy );
	rc3 = rc2; rc3.OffsetRect( 0, sizeGryph.cy );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rc1 ) );
			//m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].MakeMono();
	        m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
	        m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)] = m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)].MakeMono();
			m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_THUMB)].AdjustAlpha( -0.25 );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rc2 ) );
	VERIFY( m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)].FromBitmap( _bmpGlyphStack, rc3 ) );
	if( nAngleCw90 != 0 )
	{
		for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
		{
			CExtBitmap & _bmp1 = m_arrPartBmp[i][__ESBMHT_THUMB];
			if( _bmp1.IsEmpty() )
				continue;
			CExtBitmap _bmp2;
			VERIFY( _bmp2.CreateRotated9xStack( _bmp1, nAngleCw90, 1, _bmp1.GetSize().cx, _bmp1.GetSize().cy, false, false ) );
			_bmp1 = _bmp2;
		}
	}
}

void CExtPaintManager::SCROLLBARSKINDATA::Load2007_R1(
	UINT nResourceID_BumpyGlyphStack,
	INT nBumpyGlyphStackItemHeight,
	UINT nResourceID_GripperStack,
	const RECT & rcBumpyGlyphStackItemPadding,
	COLORREF clrSolidBackground,
	COLORREF clrPressedPageAreas,
	UINT nResourceID_ArrowUp_Normal,
	UINT nResourceID_ArrowUp_Disabled,
	UINT nResourceID_ArrowDown_Normal,
	UINT nResourceID_ArrowDown_Disabled,
	bool bRotateArrowsAndBk270,
	UINT nResourceID_BkEntire,
	UINT nResourceID_BkPage,
	const RECT & rcEntirePaddingR,
	const RECT & rcEntirePaddingC
	)
{
	Empty();
INT i, j;
	for( i = 0; i < INT(__ESBSPT_PART_COUNT); i++ )
	{
		for( j = 0; j < INT(__ESBMHT_PART_COUNT); j++ )
		{
			CRect & rcPadding = m_arrPartPadding[i][j];
			rcPadding.SetRect( 0, 0, 0, 0 );
			CExtBitmap & _bmp = m_arrPartBmp[i][j];
			_bmp.Empty();
			m_arrClrSolidBackground[i][j] = clrSolidBackground;
			if(		( j == INT(__ESBMHT_PAGE_UP) || j == INT(__ESBMHT_PAGE_DOWN) )
				&&	i == INT(__ESBSPT_PRESSED)
				)
				m_arrClrSolidBackground[i][j] = clrPressedPageAreas;
		}
	}
CExtBitmap _bmpBumpyGlyphStack, _bmpGripperStack;
	VERIFY( _bmpBumpyGlyphStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_BumpyGlyphStack) ) );
	VERIFY( _bmpGripperStack.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_GripperStack) ) );
	_bmpBumpyGlyphStack.PreMultipliedRGBChannelsSet( true );
	_bmpGripperStack.PreMultipliedRGBChannelsSet( true );
CSize _sizeBumpyGlyph = _bmpBumpyGlyphStack.GetSize();
CSize _sizeGripper = _bmpGripperStack.GetSize();
CRect rcBumpyGlyph( 0, 0, _sizeBumpyGlyph.cx, nBumpyGlyphStackItemHeight );
CRect rcGripper( 0, 0, _sizeGripper.cx, _sizeGripper.cy );
	// cold parts
	rcBumpyGlyph.OffsetRect( 0, nBumpyGlyphStackItemHeight );
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_COLD)][INT(__ESBMHT_THUMB)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_GLYPH_THUMB_GRIPPER)].
		FromBitmap( _bmpGripperStack, rcGripper )
		);
	// normal parts
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_DOWN)] = rcBumpyGlyphStackItemPadding;
	// hot parts
	rcBumpyGlyph.OffsetRect( 0, nBumpyGlyphStackItemHeight );
	//rcGripper.OffsetRect( 0, nGripperStackItemHeight );
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_UP)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_HOT)][INT(__ESBMHT_BUTTON_DOWN)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_HOT)][INT(__ESBMHT_THUMB)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_HOT)][INT(__ESBMHT_GLYPH_THUMB_GRIPPER)].
		FromBitmap( _bmpGripperStack, rcGripper )
		);
	// pressed parts
	rcBumpyGlyph.OffsetRect( 0, nBumpyGlyphStackItemHeight );
	//rcGripper.OffsetRect( 0, nGripperStackItemHeight );
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_UP)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_BUTTON_DOWN)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)].
		FromBitmap( _bmpBumpyGlyphStack, rcBumpyGlyph )
		);
	m_arrPartPadding[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_THUMB)] = rcBumpyGlyphStackItemPadding;
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_GLYPH_THUMB_GRIPPER)].
		FromBitmap( _bmpGripperStack, rcGripper )
		);
	// arrows
CExtBitmap _bmpArrowUpNormal, _bmpArrowUpDisabled, _bmpArrowDownNormal, _bmpArrowDownDisabled;
	VERIFY( _bmpArrowUpNormal.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowUp_Normal) ) );
	VERIFY( _bmpArrowUpDisabled.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowUp_Disabled) ) );
	VERIFY( _bmpArrowDownNormal.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowDown_Normal) ) );
	VERIFY( _bmpArrowDownDisabled.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_ArrowDown_Disabled) ) );
// 	_bmpArrowUpNormal.PreMultipliedRGBChannelsSet( true );
// 	_bmpArrowUpDisabled.PreMultipliedRGBChannelsSet( true );
// 	_bmpArrowDownNormal.PreMultipliedRGBChannelsSet( true );
// 	_bmpArrowDownDisabled.PreMultipliedRGBChannelsSet( true );
CSize _sizeArrowUpNormal = _bmpArrowUpNormal.GetSize();
CSize _sizeArrowUpDisabled = _bmpArrowUpDisabled.GetSize();
CSize _sizeArrowDownNormal = _bmpArrowDownNormal.GetSize();
CSize _sizeArrowDownDisabled = _bmpArrowDownDisabled.GetSize();
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_GLYPH_ARROW_UP)].
			CreateRotated9xStack(
				_bmpArrowUpNormal,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowUpNormal.cx,
				_sizeArrowUpNormal.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_GLYPH_ARROW_UP)].
			CreateRotated9xStack(
				_bmpArrowUpDisabled,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowUpDisabled.cx,
				_sizeArrowUpDisabled.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_COLD)][INT(__ESBMHT_GLYPH_ARROW_DOWN)].
			CreateRotated9xStack(
				_bmpArrowDownNormal,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowDownNormal.cx,
				_sizeArrowDownNormal.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_DISABLED)][INT(__ESBMHT_GLYPH_ARROW_DOWN)].
			CreateRotated9xStack(
				_bmpArrowDownDisabled,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeArrowDownDisabled.cx,
				_sizeArrowDownDisabled.cy,
				false,
				false
				)
		);
	// backgrounds
CExtBitmap _bmpBkEntire, _bmpBkPage;
	VERIFY( _bmpBkEntire.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_BkEntire) ) );
	VERIFY( _bmpBkPage.LoadBMP_Resource( MAKEINTRESOURCE(nResourceID_BkPage) ) );
	_bmpBkEntire.PreMultipliedRGBChannelsSet( true );
	_bmpBkPage.PreMultipliedRGBChannelsSet( true );
CSize _sizeBkEntire = _bmpBkEntire.GetSize();
CSize _sizeBkPage = _bmpBkPage.GetSize();
	VERIFY(
		m_bmpEntireArea.
			CreateRotated9xStack(
				_bmpBkEntire,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeBkEntire.cx,
				_sizeBkEntire.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_PAGE_DOWN)].
			CreateRotated9xStack(
				_bmpBkPage,
				bRotateArrowsAndBk270 ? 270 : 0,
				1,
				_sizeBkPage.cx,
				_sizeBkPage.cy,
				false,
				false
				)
		);
	VERIFY(
		m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_PAGE_UP)].AssignFromOther(
			m_arrPartBmp[INT(__ESBSPT_PRESSED)][INT(__ESBMHT_PAGE_DOWN)]
			)
		);
	m_rcEntirePaddingR = rcEntirePaddingR;
	m_rcEntirePaddingC = rcEntirePaddingC;
}

CExtPaintManager::PAINTSCROLLBARDATA::PAINTSCROLLBARDATA()
	: m_bHorzBar( false )
	, m_bEnabled( false )
	, m_bHoveredAreaIsPressed( false )
	, m_bHelperLightAccent( true )
	, m_eSBMHT( CExtPaintManager::__ESBMHT_NOWHERE )
	, m_rcBar( 0, 0, 0, 0 )
	, m_rcButtonUp( 0, 0, 0, 0 )
	, m_rcButtonDown( 0, 0, 0, 0 )
	, m_rcThumb( 0, 0, 0, 0 )
	, m_rcPageUp( 0, 0, 0, 0 )
	, m_rcPageDown( 0, 0, 0, 0 )
	, m_pHelperSrc( NULL )
	, m_lParam( 0L )
	, m_rcScrollableArea( 0, 0, 0, 0 )
{
	m_DSI.cbSize = sizeof(SCROLLINFO);
	::memset( &m_DSI, 0, sizeof(SCROLLINFO) );
}

CExtPaintManager::PAINTSCROLLBARDATA::PAINTSCROLLBARDATA(
	bool bHorzBar,
	bool bEnabled,
	bool bHoveredAreaIsPressed,
	CExtPaintManager::e_scroll_bar_mouse_hover_type_t eSBMHT,
	const RECT & rcBar,
	const RECT & rcButtonUp,
	const RECT & rcButtonDown,
	const RECT & rcThumb,
	const RECT & rcPageUp,
	const RECT & rcPageDown,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0L
	)
	: m_bHorzBar( bHorzBar )
	, m_bEnabled( bEnabled )
	, m_bHoveredAreaIsPressed( bHoveredAreaIsPressed )
	, m_bHelperLightAccent( true )
	, m_eSBMHT( eSBMHT )
	, m_rcBar( rcBar )
	, m_rcButtonUp( rcButtonUp )
	, m_rcButtonDown( rcButtonDown )
	, m_rcThumb( rcThumb )
	, m_rcPageUp( rcPageUp )
	, m_rcPageDown( rcPageDown )
	, m_pHelperSrc( pHelperSrc )
	, m_lParam( lParam )
{
	if( m_bHorzBar )
		m_rcScrollableArea.SetRect(
			m_rcButtonUp.right,
			m_rcButtonUp.top,
			m_rcButtonDown.left,
			m_rcButtonDown.bottom
			);
	else
		m_rcScrollableArea.SetRect(
			m_rcButtonUp.left,
			m_rcButtonUp.bottom,
			m_rcButtonDown.right,
			m_rcButtonDown.top
			);
	m_DSI.cbSize = sizeof(SCROLLINFO);
	::memset( &m_DSI, 0, sizeof(SCROLLINFO) );
}

CExtPaintManager::PAINTSCROLLBARDATA::PAINTSCROLLBARDATA(
	CExtScrollBar * pExtScrollBar,
	bool bHoveredAreaIsPressed, // false
	CExtPaintManager::e_scroll_bar_mouse_hover_type_t eSBMHT, // = CExtPaintManager::__ESBMHT_NOWHERE
	LPARAM lParam // = 0L
	)
	: m_bHorzBar( false )
	, m_bEnabled( false )
	, m_bHoveredAreaIsPressed( bHoveredAreaIsPressed )
	, m_bHelperLightAccent( true )
	, m_eSBMHT( eSBMHT )
	, m_rcBar( 0, 0, 0, 0 )
	, m_rcButtonUp( 0, 0, 0, 0 )
	, m_rcButtonDown( 0, 0, 0, 0 )
	, m_rcThumb( 0, 0, 0, 0 )
	, m_rcPageUp( 0, 0, 0, 0 )
	, m_rcPageDown( 0, 0, 0, 0 )
	, m_pHelperSrc( pExtScrollBar )
	, m_lParam( lParam )
{
	m_DSI.cbSize = sizeof(SCROLLINFO);
	::memset( &m_DSI, 0, sizeof(SCROLLINFO) );
	if( pExtScrollBar == NULL )
		return;
	ASSERT_VALID( pExtScrollBar );
CExtScrollBar::eScrollerOrientation_t _eSO = pExtScrollBar->GetScrollerOrientation();
	if(		_eSO != CExtScrollBar::__ESO_TOP
		&&	_eSO != CExtScrollBar::__ESO_BOTTOM
		&&	_eSO != CExtScrollBar::__ESO_LEFT
		&&	_eSO != CExtScrollBar::__ESO_RIGHT
		)
		return;
	m_bHorzBar =
		(	_eSO == CExtScrollBar::__ESO_TOP
		||	_eSO == CExtScrollBar::__ESO_BOTTOM
		)
		? true : false;
	pExtScrollBar->GetClientRect( &m_rcBar );
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	if( pExtScrollBar->IsWindowEnabled() )
	{
		m_bEnabled = true;
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if(	! pExtScrollBar->GetScrollInfo(
				&_scroll_info,
				SIF_ALL
				)
			)
		{
			pExtScrollBar->GetScrollRange(
				&_scroll_info.nMin,
				&_scroll_info.nMax
				);
			_scroll_info.nPage = 0;
			_scroll_info.nTrackPos
				= _scroll_info.nPos
				= pExtScrollBar->GetScrollPos();
		}
//		if( pExtScrollBar->m_nHelperTrackPos != -1 )
//			_scroll_info.nTrackPos = pExtScrollBar->m_nHelperTrackPos;
		__EXT_MFC_MEMCPY( 
			&m_DSI,
			sizeof(SCROLLINFO),
			&_scroll_info, 
			sizeof(SCROLLINFO) 
			);
		_scroll_info.nTrackPos -= _scroll_info.nMin;
		_scroll_info.nPos -= _scroll_info.nMin;
		_scroll_info.nMax -= _scroll_info.nMin;
		_scroll_info.nMin = 0;
	} // if( pExtScrollBar->IsWindowEnabled() )
	else
		__EXT_MFC_MEMCPY( 
			&m_DSI,
			sizeof(SCROLLINFO),
			&_scroll_info, 
			sizeof(SCROLLINFO) 
			);
	if(		_scroll_info.nMin < _scroll_info.nMax
		&&	_scroll_info.nMax < (INT_MAX-1)
		&&	_scroll_info.nPage > 0
		&&	_scroll_info.nPage == UINT( _scroll_info.nMax - _scroll_info.nMin )
		)
	{
		// layout issue
		_scroll_info.nMax ++;
		if( _scroll_info.nPos > 0 )
			_scroll_info.nPos ++;
		if( _scroll_info.nTrackPos > 0 )
			_scroll_info.nTrackPos ++;
	}
INT nScrollLimit = 0, nExtentMD = 0, nExtentThumb = 0, nExtentBtn = 0, nExtentThumbMin = 4;
	nScrollLimit =
		  _scroll_info.nMax
		- _scroll_info.nMin
		- _scroll_info.nPage
		+ 1
		;
	ASSERT( nScrollLimit >= 0 );
INT nScrollTotalRange = _scroll_info.nMax;
	if( pExtScrollBar->IsKindOf( RUNTIME_CLASS(CExtZoomScrollBar) ) )
	{
		CExtPaintManager::SCROLLBARSKINDATA * _pSBSD =
			pExtScrollBar->PmBridge_GetPM()->
				ScrollBar_GetSkinData(
					m_bHorzBar, pExtScrollBar, lParam, false );
		if( _pSBSD != NULL )
		{
			CExtBitmap & _bmpSkinnedButtonUp =
				_pSBSD->m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_BUTTON_UP)];
			ASSERT( ! _bmpSkinnedButtonUp.IsEmpty() );
			CExtBitmap & _bmpSkinnedButtonThumb =
				_pSBSD->m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)];
			ASSERT( ! _bmpSkinnedButtonUp.IsEmpty() );
			CExtPaintManager * pPM = pExtScrollBar->PmBridge_GetPM();
			ASSERT_VALID( pPM );
			if( m_bHorzBar )
			{
				nExtentBtn   = _bmpSkinnedButtonUp.GetSize().cx;
				nExtentThumb = nExtentThumbMin = _bmpSkinnedButtonThumb.GetSize().cx;
				nExtentBtn   = pPM->UiScalingDo( nExtentBtn, CExtPaintManager::__EUIST_X );
				nExtentThumb = pPM->UiScalingDo( nExtentThumb, CExtPaintManager::__EUIST_X );
			}
			else
			{
				nExtentBtn = _bmpSkinnedButtonUp.GetSize().cy;
				nExtentThumb = nExtentThumbMin = _bmpSkinnedButtonThumb.GetSize().cy;
				nExtentBtn   = pPM->UiScalingDo( nExtentBtn, CExtPaintManager::__EUIST_Y );
				nExtentThumb = pPM->UiScalingDo( nExtentThumb, CExtPaintManager::__EUIST_Y );
			}
		} // if( _pSBSD != NULL )
		else
		{
			if( m_bHorzBar )
			{
				nExtentBtn = pExtScrollBar->m_nHorzButtonWidth;
				nExtentThumb = nExtentThumbMin = pExtScrollBar->m_nHorzThumbMinWidth;
			}
			else
			{
				nExtentBtn = pExtScrollBar->m_nVertButtonHeight;
				nExtentThumb = nExtentThumbMin = pExtScrollBar->m_nVertThumbMinHeight;
			}
		} // else from if( _pSBSD != NULL )
	} // if( pExtScrollBar->IsKindOf( RUNTIME_CLASS(CExtZoomScrollBar) ) )
	else
	{
		if( m_bHorzBar )
		{
			nExtentBtn = pExtScrollBar->m_nHorzButtonWidth;
			nExtentThumbMin = pExtScrollBar->m_nHorzThumbMinWidth;
		}
		else
		{
			nExtentBtn = pExtScrollBar->m_nVertButtonHeight;
			nExtentThumbMin = pExtScrollBar->m_nVertThumbMinHeight;
		}
		if( nScrollLimit > 0L )
		{
			if( _scroll_info.nPage == 0 )
			{
				nExtentThumb = nExtentBtn;
				if( nExtentThumb == 0 )
					nExtentThumb = 6;
			} // if( _scroll_info.nPage == 0 )
			else
			{
				if( nScrollTotalRange > INT(_scroll_info.nPage) )
				{
					nExtentMD = nScrollTotalRange/_scroll_info.nPage;
				} // if( nScrollTotalRange > _scroll_info.nPage )
			} // else from if( _scroll_info.nPage == 0 )
		} // if( nScrollLimit > 0L )
	} // else from if( pExtScrollBar->IsKindOf( RUNTIME_CLASS(CExtZoomScrollBar) ) )
	if( ! m_bEnabled )
		nExtentThumb = nExtentThumbMin = 0;
CRect rcScrollable = m_rcBar;
	m_rcButtonUp = m_rcBar;
	m_rcButtonDown = m_rcBar;
	if( m_bHorzBar )
	{
		rcScrollable.DeflateRect( nExtentBtn, 0 );
		m_rcButtonUp.right = m_rcButtonUp.left + nExtentBtn;
		m_rcButtonDown.left = m_rcButtonDown.right - nExtentBtn;
		if( rcScrollable.Width() > 5 )
		{
			if( nExtentThumb == 0 && nExtentMD != 0L )
				nExtentThumb =
					::MulDiv( rcScrollable.Width(), _scroll_info.nPage, nScrollTotalRange );
			if( m_bEnabled && /*nExtentThumb != 0 &&*/ nExtentThumb < nExtentThumbMin )
				nExtentThumb = nExtentThumbMin;
			if( nExtentThumb != 0 )
			{
				m_rcThumb = rcScrollable;
				m_rcThumb.right = m_rcThumb.left + nExtentThumb;
				if( nScrollLimit > 0 )
				{
					INT nRestSpace = rcScrollable.Width() - m_rcThumb.Width();
					if( nRestSpace < 0 )
						nRestSpace = 0;
					INT nOffset = ::MulDiv( nRestSpace, _scroll_info.nTrackPos, nScrollLimit );
					m_rcThumb.OffsetRect( nOffset, 0 );
					if( _scroll_info.nTrackPos > 0 )
					{
						m_rcPageUp = rcScrollable;
						m_rcPageUp.right = m_rcThumb.left;
					}
					if( _scroll_info.nTrackPos < nScrollLimit )
					{
						m_rcPageDown = rcScrollable;
						m_rcPageDown.left = m_rcThumb.right;
					}
				} // if( nScrollLimit > 0 )
			} // if( nExtentThumb != 0 )
		} // if( rcScrollable.Width() > 5 )
		if(		(! m_rcButtonUp.IsRectEmpty() )
			&&	(! m_rcButtonDown.IsRectEmpty() )
			&&	m_rcButtonUp.right > m_rcButtonDown.left
			)
		{
			m_rcButtonUp.right
				= m_rcButtonDown.left
				= m_rcButtonUp.left
					+ ( m_rcButtonDown.right - m_rcButtonUp.left ) / 2;
			m_rcPageUp.SetRect( 0, 0, 0, 0 );
			m_rcPageDown.SetRect( 0, 0, 0, 0 );
			m_rcThumb.SetRect( 0, 0, 0, 0 );
		}
	} // if( m_bHorzBar )
	else
	{
		rcScrollable.DeflateRect( 0, nExtentBtn );
		m_rcButtonUp.bottom = m_rcButtonUp.top + nExtentBtn;
		m_rcButtonDown.top = m_rcButtonDown.bottom - nExtentBtn;
		if( rcScrollable.Height() > 5 )
		{
			if( nExtentThumb == 0 && nExtentMD != 0L )
				nExtentThumb =
					::MulDiv( rcScrollable.Height(), _scroll_info.nPage, nScrollTotalRange );
			if( m_bEnabled && /*nExtentThumb != 0 &&*/ nExtentThumb < nExtentThumbMin )
				nExtentThumb = nExtentThumbMin;
			if( nExtentThumb != 0 )
			{
				m_rcThumb = rcScrollable;
				m_rcThumb.bottom = m_rcThumb.top + nExtentThumb;
				if( nScrollLimit > 0 )
				{
					INT nRestSpace = rcScrollable.Height() - m_rcThumb.Height();
					if( nRestSpace < 0 )
						nRestSpace = 0;
					INT nOffset = ::MulDiv( nRestSpace, _scroll_info.nTrackPos, nScrollLimit );
					m_rcThumb.OffsetRect( 0, nOffset );
					if( _scroll_info.nTrackPos > 0 )
					{
						m_rcPageUp = rcScrollable;
						m_rcPageUp.bottom = m_rcThumb.top;
					}
					if( _scroll_info.nTrackPos < nScrollLimit )
					{
						m_rcPageDown = rcScrollable;
						m_rcPageDown.top = m_rcThumb.bottom;
					}
				} // if( nScrollLimit > 0 )
			} // if( nExtentThumb != 0 )
		} // if( rcScrollable.Height() > 5 )
		if(		(! m_rcButtonUp.IsRectEmpty() )
			&&	(! m_rcButtonDown.IsRectEmpty() )
			&&	m_rcButtonUp.bottom > m_rcButtonDown.top
			)
		{
			m_rcButtonUp.bottom
				= m_rcButtonDown.top
				= m_rcButtonUp.top
					+ ( m_rcButtonDown.bottom - m_rcButtonUp.top ) / 2;
			m_rcPageUp.SetRect( 0, 0, 0, 0 );
			m_rcPageDown.SetRect( 0, 0, 0, 0 );
			m_rcThumb.SetRect( 0, 0, 0, 0 );
		}
	} // else from if( m_bHorzBar )

	if( m_bHorzBar )
		m_rcScrollableArea.SetRect(
			m_rcButtonUp.right,
			m_rcButtonUp.top,
			m_rcButtonDown.left,
			m_rcButtonDown.bottom
			);
	else
		m_rcScrollableArea.SetRect(
			m_rcButtonUp.left,
			m_rcButtonUp.bottom,
			m_rcButtonDown.right,
			m_rcButtonDown.top
			);
}

CExtPaintManager::PAINTSCROLLBARDATA::~PAINTSCROLLBARDATA()
{
}

COLORREF CExtPaintManager::stat_PaintShadedEllipse_ComputeColorValue(
	double lfA,
	COLORREF clrShadeAccentLight,
	COLORREF clrShadeAccentDark
	)
{
double lfD = - 2.356 - lfA;
	if( lfD < ( - 3.1415926535897932384626433832795 ) )
		lfD = 6.293 + lfD;
	else if( lfD > 3.1415926535897932384626433832795 )
		lfD = 6.293 - lfD;
double lfW = ( ::cos( lfD ) + 1.0 ) *  0.5;
	return
		RGB(
			BYTE( lfW * GetRValue( clrShadeAccentLight ) + ( 1.0 - lfW ) * GetRValue( clrShadeAccentDark ) ),
			BYTE( lfW * GetGValue( clrShadeAccentLight ) + ( 1.0 - lfW ) * GetGValue( clrShadeAccentDark ) ),
			BYTE( lfW * GetBValue( clrShadeAccentLight ) + ( 1.0 - lfW ) * GetBValue( clrShadeAccentDark ) )
			);
}

void CExtPaintManager::stat_PaintShadedEllipse(
	HDC hDC,
	const POINT & ptEllipseCenter,
	LONG nRadiusH,
	LONG nRadiusV,
	COLORREF clrShadeAccentLight,
	COLORREF clrShadeAccentDark
	)
{
	ASSERT(
			hDC != NULL
		&&	( ptEllipseCenter.x + nRadiusH <= LONG_MAX )
		&&	( ptEllipseCenter.y + nRadiusV <= LONG_MAX )
		&&	( ptEllipseCenter.x - nRadiusH  >= LONG_MIN )
		&&	( ptEllipseCenter.y - nRadiusV  >= LONG_MIN )
		);
LONG nX = nRadiusH, nY = 0, nBoxSquare = nRadiusH * nRadiusH;
LONG nTemp0 = 8 * nRadiusH * nRadiusV * nRadiusV; 
LONG nTemp1 = 0;
LONG nTemp2 = 4 * nBoxSquare - nRadiusH * 4 * nRadiusV * nRadiusV + nRadiusV * nRadiusV; 
LONG nTemp3 = nBoxSquare - nTemp0 + 4 * nRadiusV * nRadiusV;
	for( ; nTemp3 < 0; )
	{
		double lfA = double( ::atan2( double(nY), double(nX) ) );
		COLORREF clr = stat_PaintShadedEllipse_ComputeColorValue( lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x + nX, ptEllipseCenter.y + nY, clr );
		clr = stat_PaintShadedEllipse_ComputeColorValue( 3.1415926535897932384626433832795 - lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x - nX, ptEllipseCenter.y + nY, clr );
		clr = stat_PaintShadedEllipse_ComputeColorValue( -3.1415926535897932384626433832795 + lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x - nX, ptEllipseCenter.y - nY, clr );
		clr = stat_PaintShadedEllipse_ComputeColorValue( -lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x + nX, ptEllipseCenter.y - nY, clr );
		nY ++; 
		nTemp1 = nTemp1 + 8 * nBoxSquare; 
		if( nTemp2 < 0 )
		{
			nTemp2 = nTemp2 + nTemp1 + 4 * nBoxSquare;
			nTemp3 = nTemp3 + nTemp1;
		}
		else 
		{
			nX --;
			nTemp0 = nTemp0 - 8 * nRadiusV * nRadiusV;
			nTemp2 = nTemp2 - nTemp0 + nTemp1 + 4 * nBoxSquare;
			nTemp3 = nTemp3 - nTemp0 + nTemp1 + 4 * nRadiusV * nRadiusV; 
		} 
	}  // for( ; nTemp3 < 0; )
	for( ; true; )
	{
		double lfA = double( ::atan2( double(nY), double(nX) ) );
		COLORREF clr = stat_PaintShadedEllipse_ComputeColorValue( lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x + nX, ptEllipseCenter.y + nY, clr );
		clr = stat_PaintShadedEllipse_ComputeColorValue( 3.1415926535897932384626433832795 - lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x - nX, ptEllipseCenter.y + nY, clr );
		clr = stat_PaintShadedEllipse_ComputeColorValue( -3.1415926535897932384626433832795 + lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x - nX, ptEllipseCenter.y - nY, clr );
		clr = stat_PaintShadedEllipse_ComputeColorValue( -lfA, clrShadeAccentLight, clrShadeAccentDark );
		::SetPixelV( hDC, ptEllipseCenter.x + nX, ptEllipseCenter.y - nY, clr );
		nX --;
		nTemp0 = nTemp0 - 8 * nRadiusV * nRadiusV; 
		if( nTemp3 < 0 ) 
		{ 
			nY ++;
			nTemp1 = nTemp1 + 8 * nBoxSquare;
			nTemp3 = nTemp3 - nTemp0 + 4 * nRadiusV * nRadiusV + nTemp1; 
		}
		else 
			nTemp3 = nTemp3 - nTemp0 + 4 * nRadiusV * nRadiusV; 
		if( nX < 0 )
			break;
	} // for( ; true; )
} 

void CExtPaintManager::PAINTSCROLLBARDATA::PaintDefault(
	CDC & dc,
	CExtPaintManager * pPM
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pPM );
	if(		m_rcBar.IsRectEmpty()
		||	(! dc.RectVisible( &m_rcBar ) )
		)
		return;
CExtPaintManager::SCROLLBARSKINDATA * pSBSD =
		pPM->ScrollBar_GetSkinData(
			m_bHorzBar,
			m_pHelperSrc,
			m_lParam,
			m_bHelperLightAccent
			);
	if( pSBSD != NULL )
	{
		PaintSkinned(
			dc,
			pPM,
			*pSBSD
			);
		return;
	} // if( pSBSD != NULL )
	if( m_pHelperSrc != NULL )
	{
		ASSERT_VALID( m_pHelperSrc );
		if( m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtZoomScrollBar) ) )
		{
			bool bTransparent = false;
#if (!defined __EXT_MFC_NO_STATUSBAR)
			CExtStatusControlBar * pWndParent = DYNAMIC_DOWNCAST( CExtStatusControlBar, (((CExtZoomScrollBar*)m_pHelperSrc)->GetParent()) );
			if( pWndParent != NULL )
			{
				CExtPaintManager * pPM = ((CExtZoomScrollBar*)m_pHelperSrc)->PmBridge_GetPM();
				ASSERT_VALID( pPM );
				CRect rcPaint, _rcClient;
				pWndParent->GetWindowRect( &rcPaint );
				((CExtZoomScrollBar*)m_pHelperSrc)->ScreenToClient( &rcPaint );
				((CExtZoomScrollBar*)m_pHelperSrc)->GetClientRect( &_rcClient );
				((CExtZoomScrollBar*)m_pHelperSrc)->ClientToScreen( &_rcClient );
				pWndParent->ScreenToClient( &_rcClient );
				if( pWndParent->m_bCompleteRepaint )
				{
					dc.OffsetViewportOrg(   _rcClient.left, 0 );
					bool bRet = pPM->StatusBar_EraseBackground( dc, rcPaint, pWndParent );
					dc.OffsetViewportOrg( - _rcClient.left, 0 );
					if( ! bRet )
					{
						bool bTransparent = false;
						if( pPM->GetCb2DbTransparentMode(pWndParent) )
						{
							if( pPM->PaintDockerBkgnd( true, dc, ((CExtZoomScrollBar*)m_pHelperSrc) ) )
								bTransparent = true;
						}
						if( ! bTransparent )
							dc.FillSolidRect( 
								&rcPaint, 
								pPM->GetColor( CExtPaintManager::CLR_3DFACE_OUT, pWndParent ) 
								);
					}
					INT nIndex = pWndParent->CommandToIndex( ((CExtZoomScrollBar*)m_pHelperSrc)->GetDlgCtrlID() );
					if( nIndex >= 0 )
					{
						dc.OffsetViewportOrg( _rcClient.left, 0 );
						pPM->StatusBar_ErasePaneBackground( dc, nIndex, rcPaint, pWndParent );
						dc.OffsetViewportOrg( -_rcClient.left, 0 );
						bTransparent = true;
					}
				} // if( pWndParent->m_bCompleteRepaint )
			} // if( pWndParent != NULL )
#endif // (!defined __EXT_MFC_NO_STATUSBAR)
			if( ! bTransparent )
			{
				if(		(! bTransparent )
					&&	pPM->GetCb2DbTransparentMode( m_pHelperSrc )
					)
				{
					CExtPaintManager::stat_ExcludeChildAreas(
						dc,
						((CExtZoomScrollBar*)m_pHelperSrc)->GetSafeHwnd(),
						CExtPaintManager::stat_DefExcludeChildAreaCallback
						);
					if( pPM->PaintDockerBkgnd( true, dc, ((CExtZoomScrollBar*)m_pHelperSrc) ) )
						bTransparent = true;
				}
				if( ! bTransparent )
					dc.FillSolidRect(
						&m_rcBar,
						pPM->GetColor( CExtPaintManager::CLR_3DFACE_OUT, ((CExtZoomScrollBar*)m_pHelperSrc) ) 
						);
			} // if( ! bTransparent )
			CRect rcLineH( m_rcScrollableArea );
			CRect rcLineV( m_rcScrollableArea );
			if( m_bHorzBar )
			{
				rcLineH.bottom = rcLineH.top + 1;
				rcLineH.OffsetRect( 0, m_rcScrollableArea.Height() / 2 + 1 );
				rcLineV.right = rcLineV.left + 1;
				rcLineV.OffsetRect( m_rcScrollableArea.Width() / 2, 0 );
				rcLineV.bottom = rcLineV.top + 7;
				rcLineV.OffsetRect( 0, (m_rcScrollableArea.Height() - 7 ) / 2 + 2 );
			}
			else
			{
				rcLineH.right = rcLineH.left + 1;
				rcLineH.OffsetRect( m_rcScrollableArea.Width() / 2 + 1, 0 );
				rcLineV.right = rcLineV.left + 7;
				rcLineV.OffsetRect( (m_rcScrollableArea.Width() - 7 ) / 2 + 1 , 0 );
				rcLineV.bottom = rcLineV.top + 1;
				rcLineV.OffsetRect( 0, m_rcScrollableArea.Height() / 2 );
			}	
			COLORREF clrZsLineLight = pPM->GetColor( COLOR_3DHIGHLIGHT );
			COLORREF clrZsLineDark =  pPM->GetColor( COLOR_3DSHADOW );
			rcLineH.OffsetRect( 
				m_bHorzBar ? 0 : -1, 
				m_bHorzBar ? -1 : 0
				);
			rcLineV.OffsetRect(
				m_bHorzBar ? -2 : 0,
				m_bHorzBar ? -2 : -1
				);
			if( m_bHorzBar )
				rcLineV.OffsetRect( 2, 0 );
			else
				rcLineV.OffsetRect( 0, 2 );
			if( clrZsLineDark !=  COLORREF(-1L) )
			{
				dc.FillSolidRect( &rcLineH, clrZsLineDark );
				dc.FillSolidRect( &rcLineV, clrZsLineDark );
			} // if( clrZsLineDark !=  COLORREF(-1L) )
			if( clrZsLineLight !=  COLORREF(-1L) )
			{
				rcLineH.OffsetRect( 1, 1 );
				rcLineV.OffsetRect( 1, 1 );
				dc.FillSolidRect( &rcLineH, clrZsLineLight );
				dc.FillSolidRect( &rcLineV, clrZsLineLight );
			} // if( clrZsLineLight !=  COLORREF(-1L) )
			COLORREF clrButtonBk = pPM->GetColor( COLOR_3DFACE, m_pHelperSrc, m_lParam );
			CBrush _brushBkDefault( clrButtonBk );
			HGDIOBJ hOldPen = ::SelectObject( dc.m_hDC, ::GetStockObject( NULL_PEN ) );
			CRect arrRcButtons[] = { m_rcButtonDown, m_rcButtonUp, m_rcThumb };
			INT nButtonIndex, nButtonCount = INT( sizeof(arrRcButtons)/sizeof(arrRcButtons[0]) );
			for( nButtonIndex = 0; nButtonIndex < nButtonCount; nButtonIndex ++ )
			{
				CRect rcButton = arrRcButtons[ nButtonIndex ];
				INT nMetric = min( rcButton.Width(), rcButton.Height() );
				nMetric &= ~1;
				rcButton.left += ( rcButton.Width() - nMetric ) / 2;
				rcButton.right = rcButton.left + nMetric;
				rcButton.top += ( rcButton.Height() - nMetric ) / 2;
				rcButton.bottom = rcButton.top + nMetric;
				bool bHover = false;
				bool bPressed = false;
				if( m_bEnabled )
				{
					switch( nButtonIndex )
					{
					case 0:
						if( m_eSBMHT == __ESBMHT_BUTTON_DOWN )
							bHover = true;
					break;
					case 1:
						if( m_eSBMHT == __ESBMHT_BUTTON_UP )
							bHover = true;
					break;
					case 2:
						if( m_eSBMHT == __ESBMHT_THUMB )
							bHover = true;
					break;
					} // switch( nButtonIndex )
					if( bHover )
						bPressed = m_bHoveredAreaIsPressed;
				} // if( m_bEnabled )
				COLORREF clrButtonText = pPM->GetColor( m_bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED, m_pHelperSrc, m_lParam );
				COLORREF clrButtonShadeOuterTL = pPM->GetColor( COLOR_3DHIGHLIGHT, m_pHelperSrc, m_lParam );
				COLORREF clrButtonShadeOuterRB = pPM->GetColor( COLOR_3DDKSHADOW, m_pHelperSrc, m_lParam );
				COLORREF clrButtonShadeInnerTL = clrButtonShadeOuterTL;
				COLORREF clrButtonShadeInnerRB = pPM->GetColor( COLOR_3DSHADOW, m_pHelperSrc, m_lParam );
				if( bPressed )
				{
					COLORREF clrTmp;
					//clrTmp = clrButtonShadeOuterTL;
					//clrButtonShadeOuterTL = clrButtonShadeOuterRB;
					//clrButtonShadeOuterRB = clrTmp;
					clrTmp = clrButtonShadeInnerTL;
					clrButtonShadeInnerTL = clrButtonShadeInnerRB;
					clrButtonShadeInnerRB = clrTmp;
				} // if( bPressed )
				CBrush * pOldBrush =
					dc.SelectObject(
						( m_bEnabled && ( bPressed || bHover ) )
							? ( bPressed ? (&(pPM->m_brushDarkerDefault)) : (&(pPM->m_brushLighterDefault)) )
							: (&_brushBkDefault)
						);
				if( nButtonIndex == 2 )
					rcButton.InflateRect( 0, 0, 1, 1 );
				dc.Ellipse( &rcButton );
				dc.SelectObject( pOldBrush );
				if( nButtonIndex == 2 )
					rcButton.DeflateRect( 0, 0, 1, 1 );
				CPoint ptCenter = rcButton.CenterPoint();
				INT nHalfMetricToDraw = nMetric/2;
				stat_PaintShadedEllipse(
					dc.m_hDC,
					ptCenter,
					nHalfMetricToDraw,
					nHalfMetricToDraw,
					clrButtonShadeOuterTL,
					clrButtonShadeOuterRB
					);
				nHalfMetricToDraw --;
				stat_PaintShadedEllipse(
					dc.m_hDC,
					ptCenter,
					nHalfMetricToDraw,
					nHalfMetricToDraw,
					clrButtonShadeInnerTL,
					clrButtonShadeInnerRB
					);
				nHalfMetricToDraw /= 2;
				if( nHalfMetricToDraw > 0 )
				{
					switch( nButtonIndex )
					{
					case 0:
						dc.FillSolidRect(
							ptCenter.x,
							ptCenter.y - nHalfMetricToDraw,
							1,
							nHalfMetricToDraw * 2 + 1,
							clrButtonText
							);
					case 1: // continuing case 0
						dc.FillSolidRect(
							ptCenter.x - nHalfMetricToDraw,
							ptCenter.y,
							nHalfMetricToDraw * 2 + 1,
							1,
							clrButtonText
							);
					break;
					} // switch( nButtonIndex )
				} // if( nHalfMetricToDraw > 0 )
			} // for( nButtonIndex = 0; nButtonIndex < nButtonCount; nButtonIndex ++ )
			::SelectObject( dc.m_hDC, hOldPen );
			return;
		} // if( m_pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtZoomScrollBar) ) )
	} // if( m_pHelperSrc != NULL )
	dc.FillRect(
		&m_rcBar,
		&pPM->m_brushLightSystem
		);
	//m_bEnabled = false;
	//m_eSBMHT = __ESBMHT_BUTTON_UP;
	//m_eSBMHT = __ESBMHT_PAGE_DOWN;
	//m_eSBMHT = __ESBMHT_THUMB;
	//m_bHoveredAreaIsPressed = true;
	if( g_PaintManager.m_UxTheme.IsControlsThemed() )
	{
		HWND hWnd = NULL;
		if( m_pHelperSrc != NULL )
		{
			CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, m_pHelperSrc );
			hWnd = pWnd->GetSafeHwnd();
		}
		if( hWnd == NULL )
			hWnd = ::GetDesktopWindow();
		if( hWnd != NULL )
		{
			bool bVistaThemeMode =
				(	g_PaintManager.m_bIsWinVistaOrLater
				&&	g_PaintManager.m_UxTheme.IsControlsThemed()
				) ? true : false;
			if( g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_SCROLLBAR ) != NULL )
			{
				if(		(! m_rcButtonUp.IsRectEmpty() )
					&&	dc.RectVisible( &m_rcButtonUp )
					)
				{
					INT nStateID = 0;
					if( m_bHorzBar )
					{
						if( m_bEnabled )
						{
							if( m_eSBMHT == __ESBMHT_BUTTON_UP )
								nStateID =
									m_bHoveredAreaIsPressed
										? ABS_LEFTPRESSED
										: ABS_LEFTHOT
										;
							else
								nStateID =
									( bVistaThemeMode && m_eSBMHT != __ESBMHT_NOWHERE )
										? ABS_LEFTHOVER
										: ABS_LEFTNORMAL
										;
						}
						else
							nStateID = ABS_LEFTDISABLED;
					}
					else
					{
						if( m_bEnabled )
						{
							if( m_eSBMHT == __ESBMHT_BUTTON_UP )
								nStateID =
									m_bHoveredAreaIsPressed
										? ABS_UPPRESSED
										: ABS_UPHOT
										;
							else
								nStateID =
									( bVistaThemeMode && m_eSBMHT != __ESBMHT_NOWHERE )
										? ABS_UPHOVER
										: ABS_UPNORMAL
										;
						}
						else
							nStateID = ABS_UPDISABLED;
					}
					VERIFY( 
						g_PaintManager.m_UxTheme.DrawBackground(
							hWnd,
							dc.GetSafeHdc(),
							SBP_ARROWBTN,
							nStateID,
							&m_rcButtonUp,
							&m_rcButtonUp
							) == S_OK
						);
				}
				if(		(! m_rcButtonDown.IsRectEmpty() )
					&&	dc.RectVisible( &m_rcButtonDown )
					)
				{
					INT nStateID = 0;
					if( m_bHorzBar )
					{
						if( m_bEnabled )
						{
							if( m_eSBMHT == __ESBMHT_BUTTON_DOWN )
								nStateID =
									m_bHoveredAreaIsPressed
										? ABS_RIGHTPRESSED
										: ABS_RIGHTHOT
										;
							else
								nStateID =
									( bVistaThemeMode && m_eSBMHT != __ESBMHT_NOWHERE )
										? ABS_RIGHTHOVER
										: ABS_RIGHTNORMAL
										;
						}
						else
							nStateID = ABS_RIGHTDISABLED;
					}
					else
					{
						if( m_bEnabled )
						{
							if( m_eSBMHT == __ESBMHT_BUTTON_DOWN )
								nStateID =
									m_bHoveredAreaIsPressed
										? ABS_DOWNPRESSED
										: ABS_DOWNHOT
										;
							else
								nStateID =
									( bVistaThemeMode && m_eSBMHT != __ESBMHT_NOWHERE )
										? ABS_DOWNHOVER
										: ABS_DOWNNORMAL
										;
						}
						else
							nStateID = ABS_DOWNDISABLED;
					}
					VERIFY( 
						g_PaintManager.m_UxTheme.DrawBackground(
							hWnd,
							dc.GetSafeHdc(),
							SBP_ARROWBTN,
							nStateID,
							&m_rcButtonDown,
							&m_rcButtonDown
							) == S_OK
						);
				}
				if(		(! m_rcPageDown.IsRectEmpty() )
					&&	dc.RectVisible( &m_rcPageDown )
					)
				{
					INT nPartID =
						m_bHorzBar
							? SBP_LOWERTRACKHORZ
							: SBP_LOWERTRACKVERT
							;
					INT nStateID = 0;
					if( m_bEnabled )
					{
						if( m_eSBMHT == __ESBMHT_PAGE_DOWN )
							nStateID =
								m_bHoveredAreaIsPressed
									? SCRBS_PRESSED
									: SCRBS_HOT
									;
						else
							nStateID = SCRBS_NORMAL;
					}
					else
						nStateID = SCRBS_DISABLED;
					VERIFY( 
						g_PaintManager.m_UxTheme.DrawBackground(
							hWnd,
							dc.GetSafeHdc(),
							nPartID,
							nStateID,
							&m_rcPageDown,
							&m_rcPageDown
							) == S_OK
						);
				}
				if(		(! m_rcPageUp.IsRectEmpty() )
					&&	dc.RectVisible( &m_rcPageUp )
					)
				{
					INT nPartID =
						m_bHorzBar
							? SBP_UPPERTRACKHORZ
							: SBP_UPPERTRACKVERT
							;
					INT nStateID = 0;
					if( m_bEnabled )
					{
						if( m_eSBMHT == __ESBMHT_PAGE_UP )
							nStateID =
								m_bHoveredAreaIsPressed
									? SCRBS_PRESSED
									: SCRBS_HOT
									;
						else
							nStateID = SCRBS_NORMAL;
					}
					else
						nStateID = SCRBS_DISABLED;
					VERIFY( 
						g_PaintManager.m_UxTheme.DrawBackground(
							hWnd,
							dc.GetSafeHdc(),
							nPartID,
							nStateID,
							&m_rcPageUp,
							&m_rcPageUp
							) == S_OK
						);
				}
				if(		(! m_rcThumb.IsRectEmpty() )
					&&	dc.RectVisible( &m_rcThumb )
					)
				{
					INT nPartID =
						m_bHorzBar
							? SBP_THUMBBTNHORZ
							: SBP_THUMBBTNVERT
							;
					INT nStateID = 0;
					if( m_bEnabled )
					{
						if( m_eSBMHT == __ESBMHT_THUMB )
							nStateID =
								m_bHoveredAreaIsPressed
									? SCRBS_PRESSED
									: SCRBS_HOT
									;
						else
							nStateID = SCRBS_NORMAL;
					}
					else
						nStateID = SCRBS_DISABLED;
					if( m_bEnabled )
					{
						CRect rcXpGrip = m_rcThumb;
						VERIFY( 
							g_PaintManager.m_UxTheme.DrawBackground(
								hWnd,
								dc.GetSafeHdc(),
								nPartID,
								nStateID,
								&m_rcThumb,
								&m_rcThumb
								) == S_OK
							);
						VERIFY( 
							g_PaintManager.m_UxTheme.DrawBackground(
								hWnd,
								dc.GetSafeHdc(),
								m_bHorzBar ? SBP_GRIPPERHORZ : SBP_GRIPPERVERT,
								0,
								&rcXpGrip,
								&rcXpGrip
								) == S_OK
							);
					} // if( m_bEnabled )
				}
				g_PaintManager.m_UxTheme.CloseThemeData();
				return;
			}
		} // if( hWnd != NULL )
	} // if( g_PaintManager.m_UxTheme.IsControlsThemed() )
UINT nStateBase = 0;
	if( m_bEnabled )
	{
		if( m_eSBMHT == __ESBMHT_NOWHERE )
		{
			if( pPM->m_bScrollBarsHaveHoverEffectIn2kStyle )
				nStateBase = DFCS_FLAT;
		}
	}
	else
		nStateBase = DFCS_INACTIVE;
	if(		(! m_rcButtonUp.IsRectEmpty() )
		&&	dc.RectVisible( &m_rcButtonUp )
		)
	{
		UINT nState = m_bHorzBar ? DFCS_SCROLLLEFT : DFCS_SCROLLUP;
		if(		m_bEnabled
			&&	m_bHoveredAreaIsPressed
			&&	m_eSBMHT == __ESBMHT_BUTTON_UP
			)
			nState |= DFCS_PUSHED|DFCS_FLAT;
		dc.DrawFrameControl(
			m_rcButtonUp,
			DFC_SCROLL,
			nState | nStateBase
			);
	}
	if(		(! m_rcButtonDown.IsRectEmpty() )
		&&	dc.RectVisible( &m_rcButtonDown )
		)
	{
		UINT nState = m_bHorzBar ? DFCS_SCROLLRIGHT : DFCS_SCROLLDOWN;
		if(		m_bEnabled
			&&	m_bHoveredAreaIsPressed
			&&	m_eSBMHT == __ESBMHT_BUTTON_DOWN
			)
			nState |= DFCS_PUSHED|DFCS_FLAT;
		dc.DrawFrameControl(
			m_rcButtonDown,
			DFC_SCROLL,
			nState | nStateBase
			);
	}
	if(		m_bEnabled
		&&	m_eSBMHT == __ESBMHT_PAGE_UP
		&&	(! m_rcPageUp.IsRectEmpty() )
		&&	dc.RectVisible( &m_rcPageUp )
		&&	(	m_bHoveredAreaIsPressed
			||	pPM->m_bScrollBarsHaveHoverEffectIn2kStyle
			)
		)
		dc.FillRect(
			&m_rcPageUp,
			m_bHoveredAreaIsPressed
				? (&(pPM->m_brushDarkestSystem))
				: (&(pPM->m_brushDarkSystem))
			);
	if(		m_bEnabled
		&&	m_eSBMHT == __ESBMHT_PAGE_DOWN
		&&	(! m_rcPageDown.IsRectEmpty() )
		&&	dc.RectVisible( &m_rcPageDown )
		&&	(	m_bHoveredAreaIsPressed
			||	pPM->m_bScrollBarsHaveHoverEffectIn2kStyle
			)
		)
		dc.FillRect(
			&m_rcPageDown,
			m_bHoveredAreaIsPressed
				? (&(pPM->m_brushDarkestSystem))
				: (&(pPM->m_brushDarkSystem))
			);
	if(		m_bEnabled
		&&	(! m_rcThumb.IsRectEmpty() )
		&&	dc.RectVisible( &m_rcThumb )
		)
	{
		UINT nState = DFCS_BUTTONPUSH;
		if(		m_bHoveredAreaIsPressed
			&&	pPM->m_bScrollBarsHaveHoverEffectIn2kStyle
			&&	m_eSBMHT == __ESBMHT_THUMB
			)
			nState |= DFCS_PUSHED|DFCS_FLAT;
		dc.DrawFrameControl(
			m_rcThumb,
			DFC_BUTTON,
			nState | nStateBase
			);
	}
}

void CExtPaintManager::PAINTSCROLLBARDATA::PaintSkinned(
	CDC & dc,
	CExtPaintManager * pPM,
	const SCROLLBARSKINDATA & _sbsd,
	BYTE nConstantAlpha // = 0x0FF
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT_VALID( pPM );
	pPM;
	if(		m_rcBar.IsRectEmpty()
		||	(! dc.RectVisible( &m_rcBar ) )
		)
		return;
//CExtScrollBar * pExtScrollBar = DYNAMIC_DOWNCAST( CExtScrollBar, m_pHelperSrc );
CExtZoomScrollBar * pZoomer = DYNAMIC_DOWNCAST( CExtZoomScrollBar, m_pHelperSrc );
	if( pZoomer != NULL )
	{ // zoom scroll bar background and lines
		ASSERT_VALID( pZoomer );
		if( pPM == NULL )
		{
			pPM = pZoomer->PmBridge_GetPM();
			ASSERT_VALID( pPM );
		} // if( pPM == NULL )
		bool bTransparent = false;
#if (!defined __EXT_MFC_NO_STATUSBAR)
		CExtStatusControlBar * pWndParent = DYNAMIC_DOWNCAST( CExtStatusControlBar, pZoomer->GetParent() );
		if( pWndParent != NULL )
		{
			CRect rcPaint, _rcClient;
			pWndParent->GetWindowRect( &rcPaint );
			pZoomer->ScreenToClient( &rcPaint );
			pZoomer->GetClientRect( &_rcClient );
			pZoomer->ClientToScreen( &_rcClient );
			pWndParent->ScreenToClient( &_rcClient );
			if( pWndParent->m_bCompleteRepaint )
			{
				dc.OffsetViewportOrg(   _rcClient.left, 0 );
				bool bRet = pPM->StatusBar_EraseBackground( dc, rcPaint, pWndParent );
				dc.OffsetViewportOrg( - _rcClient.left, 0 );
				if( ! bRet )
				{
					bool bTransparent = false;
					if( pPM->GetCb2DbTransparentMode(pWndParent) )
					{
						if( pPM->PaintDockerBkgnd( true, dc, pZoomer ) )
							bTransparent = true;
					}
					if( ! bTransparent )
						dc.FillSolidRect( 
							&rcPaint, 
							pPM->GetColor( CExtPaintManager::CLR_3DFACE_OUT, pWndParent ) 
							);
				}
				INT nIndex = pWndParent->CommandToIndex( pZoomer->GetDlgCtrlID() );
				if( nIndex >= 0 )
				{
					dc.OffsetViewportOrg( _rcClient.left, 0 );
					pPM->StatusBar_ErasePaneBackground( dc, nIndex, rcPaint, pWndParent );
					dc.OffsetViewportOrg( -_rcClient.left, 0 );
					bTransparent = true;
				}
			} // if( pWndParent->m_bCompleteRepaint )
		} // if( pWndParent != NULL )
#endif // (!defined __EXT_MFC_NO_STATUSBAR)
		if( ! bTransparent )
		{
			if(		(! bTransparent )
				&&	pPM->GetCb2DbTransparentMode( m_pHelperSrc )
				)
			{
				CExtPaintManager::stat_ExcludeChildAreas(
					dc,
					pZoomer->GetSafeHwnd(),
					CExtPaintManager::stat_DefExcludeChildAreaCallback
					);
				if( pPM->PaintDockerBkgnd( true, dc, pZoomer ) )
					bTransparent = true;
			}
			if( ! bTransparent )
				dc.FillSolidRect(
					&m_rcBar,
					pPM->GetColor( CExtPaintManager::CLR_3DFACE_OUT, pZoomer ) 
					);
		} // if( ! bTransparent )
		CRect rcLineH( m_rcScrollableArea );
		CRect rcLineV( m_rcScrollableArea );
		if( m_bHorzBar )
		{
			rcLineH.bottom = rcLineH.top + 1;
			rcLineH.OffsetRect( 0, m_rcScrollableArea.Height() / 2 + 1 );
			rcLineV.right = rcLineV.left + 1;
			rcLineV.OffsetRect( m_rcScrollableArea.Width() / 2, 0 );
			rcLineV.bottom = rcLineV.top + 7;
			rcLineV.OffsetRect( 0, (m_rcScrollableArea.Height() - 7 ) / 2 + 2 );
		}
		else
		{
			rcLineH.right = rcLineH.left + 1;
			rcLineH.OffsetRect( m_rcScrollableArea.Width() / 2 + 1, 0 );
			rcLineV.right = rcLineV.left + 7;
			rcLineV.OffsetRect( (m_rcScrollableArea.Width() - 7 ) / 2 + 1 , 0 );
			rcLineV.bottom = rcLineV.top + 1;
			rcLineV.OffsetRect( 0, m_rcScrollableArea.Height() / 2 );
		}	
		COLORREF clrZsLineLight = 
			m_bEnabled 
				? _sbsd.m_clrZsLineLightEnabled 
				: _sbsd.m_clrZsLineLightDisabled; 
		COLORREF clrZsLineDark = 
			m_bEnabled 
				? _sbsd.m_clrZsLineDarkEnabled 
				: _sbsd.m_clrZsLineDarkDisabled; 
		rcLineH.OffsetRect( 
			m_bHorzBar ? 0 : -1, 
			m_bHorzBar ? -1 : 0
			);
		rcLineV.OffsetRect(
			m_bHorzBar ? -2 : 0,
			m_bHorzBar ? -2 : -1
			);
		if( m_bHorzBar )
			rcLineV.OffsetRect( _sbsd.m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].GetSize().cx / 2 - 2, 0 );
		else
			rcLineV.OffsetRect( 0, _sbsd.m_arrPartBmp[INT(__ESBSPT_NORMAL)][INT(__ESBMHT_THUMB)].GetSize().cy / 2 - 2 );
		if( clrZsLineDark !=  COLORREF(-1L) )
		{
			dc.FillSolidRect( &rcLineH, clrZsLineDark );
			dc.FillSolidRect( &rcLineV, clrZsLineDark );
		} // if( clrZsLineDark !=  COLORREF(-1L) )
		if( clrZsLineLight !=  COLORREF(-1L) )
		{
			rcLineH.OffsetRect( 1, 1 );
			rcLineV.OffsetRect( 1, 1 );
			dc.FillSolidRect( &rcLineH, clrZsLineLight );
			dc.FillSolidRect( &rcLineV, clrZsLineLight );
		} // if( clrZsLineLight !=  COLORREF(-1L) )
	} // zoom scroll bar background and lines
	if( ! _sbsd.m_bmpEntireArea.IsEmpty() )
			_sbsd.m_bmpEntireArea.DrawSkinParts(
				dc.m_hDC,
				m_rcBar,
				_sbsd.m_rcEntirePaddingR,
				CExtBitmap::__EDM_STRETCH,
				true,
				true
				);
INT nDefaultStateIndex = INT(__ESBSPT_DISABLED);
	if( m_bEnabled )
		nDefaultStateIndex =
			( m_eSBMHT == __ESBMHT_NOWHERE )
				? INT(__ESBSPT_COLD)
				: INT(__ESBSPT_NORMAL)
				;
INT nPartNumberEffective, nPartNumber, nPartCount = INT(__ESBMHT_PART_COUNT);
// 	if( pPM == NULL && pExtScrollBar != NULL )
// 	{
// 		pPM = pExtScrollBar->PmBridge_GetPM();
// 		ASSERT_VALID( pPM );
// 	} // if( pPM == NULL && pExtScrollBar != NULL )
	for( nPartNumber = 0; nPartNumber < nPartCount; nPartNumber ++ )
	{
		bool bForceNoFillColor = false;
		CRect rcPart( 0, 0, 0, 0 );
		CExtBitmap::e_DrawMethod_t eDM = CExtBitmap::__EDM_STRETCH;
		nPartNumberEffective = nPartNumber;
		switch( nPartNumber )
		{
		case INT(__ESBMHT_BUTTON_UP):
			nPartNumberEffective = INT(__ESBMHT_PAGE_UP);
		break;
		case INT(__ESBMHT_BUTTON_DOWN):
			nPartNumberEffective = INT(__ESBMHT_PAGE_DOWN);
		break;
		case INT(__ESBMHT_THUMB):
			nPartNumberEffective = INT(__ESBMHT_BUTTON_UP);
		break;
		case INT(__ESBMHT_PAGE_UP):
			nPartNumberEffective = INT(__ESBMHT_BUTTON_DOWN);
		break;
		case INT(__ESBMHT_PAGE_DOWN):
			nPartNumberEffective = INT(__ESBMHT_THUMB);
		break;
		} // switch( nPartNumber )
		INT nCheckHoverPartNumber = nPartNumberEffective;
		switch( nPartNumberEffective )
		{
		case __ESBMHT_SOLID_SCROLLABLE_AREA:
			rcPart = m_rcScrollableArea;
			break;
		case __ESBMHT_GLYPH_ARROW_UP:
			eDM = CExtBitmap::__EDM_CENTER;
			bForceNoFillColor = true;
		case __ESBMHT_BUTTON_UP:
			if( pZoomer != NULL )
				eDM = CExtBitmap::__EDM_CENTER;
			nCheckHoverPartNumber = __ESBMHT_BUTTON_UP;
			rcPart = m_rcButtonUp;
			if( m_bHorzBar )
				rcPart.DeflateRect(
					_sbsd.m_rcEntirePaddingC.left,
					_sbsd.m_rcEntirePaddingC.top,
					0,
					_sbsd.m_rcEntirePaddingC.bottom
					);
			else
				rcPart.DeflateRect(
					_sbsd.m_rcEntirePaddingC.left,
					_sbsd.m_rcEntirePaddingC.top,
					_sbsd.m_rcEntirePaddingC.right,
					0
					);
			break;
		case __ESBMHT_GLYPH_ARROW_DOWN:
			eDM = CExtBitmap::__EDM_CENTER;
			bForceNoFillColor = true;
		case __ESBMHT_BUTTON_DOWN:
			if( pZoomer != NULL )
				eDM = CExtBitmap::__EDM_CENTER;
			nCheckHoverPartNumber = __ESBMHT_BUTTON_DOWN;
			rcPart = m_rcButtonDown;
			if( m_bHorzBar )
				rcPart.DeflateRect(
					0,
					_sbsd.m_rcEntirePaddingC.top,
					_sbsd.m_rcEntirePaddingC.right,
					_sbsd.m_rcEntirePaddingC.bottom
					);
			else
				rcPart.DeflateRect(
					_sbsd.m_rcEntirePaddingC.left,
					0,
					_sbsd.m_rcEntirePaddingC.right,
					_sbsd.m_rcEntirePaddingC.bottom
					);
			break;
		case __ESBMHT_GLYPH_THUMB_GRIPPER:
			eDM = CExtBitmap::__EDM_CENTER;
			bForceNoFillColor = true;
		case __ESBMHT_THUMB:
			if( pZoomer != NULL )
				eDM = CExtBitmap::__EDM_CENTER;
			nCheckHoverPartNumber = __ESBMHT_THUMB;
			rcPart = m_rcThumb;
			if( m_bHorzBar )
				rcPart.DeflateRect(
					0,
					_sbsd.m_rcEntirePaddingC.top,
					0,
					_sbsd.m_rcEntirePaddingC.bottom
					);
			else
				rcPart.DeflateRect(
					_sbsd.m_rcEntirePaddingC.left,
					0,
					_sbsd.m_rcEntirePaddingC.right,
					0
					);
			break;
		case __ESBMHT_PAGE_UP:
			rcPart = m_rcPageUp;
			break;
		case __ESBMHT_PAGE_DOWN:
			rcPart = m_rcPageDown;
			break;
		} // switch( nPartNumberEffective )
		INT nStateIndex = nDefaultStateIndex;
		if(		m_bEnabled 
			&&	INT(m_eSBMHT) == nCheckHoverPartNumber
			)
			nStateIndex =
				m_bHoveredAreaIsPressed
					? INT(__ESBSPT_PRESSED)
					: INT(__ESBSPT_HOT)
					;
		if(		rcPart.left >= rcPart.right
			||	rcPart.top >= rcPart.bottom
			)
			continue;
		COLORREF clrSolidBackground = COLORREF(-1L);
		CRect rcPadding( 0, 0, 0, 0 );
		const CExtBitmap & _bmp =
			_sbsd.GetPart(
				e_scroll_bar_skin_part_t(nStateIndex),
				e_scroll_bar_mouse_hover_type_t(nPartNumberEffective),
				&rcPadding,
				&clrSolidBackground
				);
		if( (! bForceNoFillColor ) && clrSolidBackground != COLORREF(-1L) )
			dc.FillSolidRect( &rcPart, clrSolidBackground );
		if( ! _bmp.IsEmpty() )
		{
			bool bDraw = true;
			if(		eDM == CExtBitmap::__EDM_CENTER
				&&	(	m_pHelperSrc == NULL
					||	pZoomer == NULL
					)
				)
			{
				CSize _sizeBmp = _bmp.GetSize();
				CSize _sizeMin = rcPart.Size();
				_sizeMin.cx -= rcPadding.left + rcPadding.right;
				_sizeMin.cy -= rcPadding.top + rcPadding.bottom;
				if(		_sizeBmp.cx >= _sizeMin.cx
					||	_sizeBmp.cy >= _sizeMin.cy
					)
					bDraw = false;
			}
			if( bDraw && nPartNumberEffective == __ESBMHT_GLYPH_THUMB_GRIPPER )
			{
				if( ! m_rcThumb.IsRectEmpty() )
				{
					CSize _sizeBmp = _bmp.GetSize();
					if( m_bHorzBar )
					{
						if( _sizeBmp.cx >= m_rcThumb.Width()-4 )
							bDraw = false;
					} // if( m_bHorzBar )
					else
					{
						if( _sizeBmp.cy >= m_rcThumb.Height()-4 )
							bDraw = false;
					} // if( m_bHorzBar )
				} // if( ! m_rcThumb.IsRectEmpty() )
			} // if( bDraw && nPartNumberEffective == __ESBMHT_GLYPH_THUMB_GRIPPER )
			if( bDraw )
			{
				if( pZoomer != NULL )
				{
					ASSERT( pPM != NULL );
					if(		pPM->UiScalingIsEnabled( CExtPaintManager::__EUIST_X )
						&&	pPM->UiScalingIsEnabled( CExtPaintManager::__EUIST_Y )
						&&	g_PaintManager.m_nLPX > 96
						&&	g_PaintManager.m_nLPY > 96
						)
					{
						CSize _sizeOld = _bmp.GetSize(); // rcPart.Size();
						CSize _sizeNew(
							pPM->UiScalingDo( _sizeOld.cx, CExtPaintManager::__EUIST_X ),
							pPM->UiScalingDo( _sizeOld.cy, CExtPaintManager::__EUIST_Y )
							);
						_sizeNew.cx = ::MulDiv( _sizeNew.cx, 7, 8 );
						_sizeNew.cy = ::MulDiv( _sizeNew.cy, 7, 8 );
						if( _sizeNew != _sizeOld )
						{
							CRect rcPartNew(
								rcPart.left,
								rcPart.top,
								rcPart.left + _sizeNew.cx,
								rcPart.top + _sizeNew.cy
								);
							rcPartNew.OffsetRect(
								- ( _sizeNew.cx - _sizeOld.cx ) / 2,
								- ( _sizeNew.cy - _sizeOld.cy ) / 2
								);
							rcPart = rcPartNew;
							eDM = CExtBitmap::__EDM_STRETCH;
						} // if( _sizeNew != _sizeOld )
					}
				} // if( pZoomer != NULL )
				_bmp.AlphaBlendSkinParts(
					dc.m_hDC,
					rcPart,
					rcPadding,
					eDM,
					true,
					true,
					nConstantAlpha
					);
			} // if( bDraw )
		} // if( ! _bmp.IsEmpty() )
	} // for( nPartNumber = 0; nPartNumber < nPartCount; nPartNumber ++ )
}

CExtPaintManager::e_scroll_bar_mouse_hover_type_t
	CExtPaintManager::PAINTSCROLLBARDATA::GetHT(
		const POINT & pt
		) const
{
	if( m_rcBar.IsRectEmpty() )
		return __ESBMHT_NOWHERE;
	if(		(! m_rcButtonUp.IsRectEmpty() )
		&&	m_rcButtonUp.PtInRect( pt )
		)
		return __ESBMHT_BUTTON_UP;
	if(		(! m_rcButtonDown.IsRectEmpty() )
		&&	m_rcButtonDown.PtInRect( pt )
		)
		return __ESBMHT_BUTTON_DOWN;
	if(		(! m_rcThumb.IsRectEmpty() )
		&&	m_rcThumb.PtInRect( pt )
		)
		return __ESBMHT_THUMB;
	if(		(! m_rcPageUp.IsRectEmpty() )
		&&	m_rcPageUp.PtInRect( pt )
		)
		return __ESBMHT_PAGE_UP;
	if(		(! m_rcPageDown.IsRectEmpty() )
		&&	m_rcPageDown.PtInRect( pt )
		)
		return __ESBMHT_PAGE_DOWN;
	return __ESBMHT_NOWHERE;
}

CExtPaintManager::e_scroll_bar_mouse_hover_type_t
	CExtPaintManager::PAINTSCROLLBARDATA::AdjustHT(
		const POINT & pt
		)
{
	m_eSBMHT = GetHT( pt );
	return m_eSBMHT;
}

CRect CExtPaintManager::PAINTSCROLLBARDATA::GetAreaRect(
	CExtPaintManager::e_scroll_bar_mouse_hover_type_t eSBMHT
	) const
{
	switch( eSBMHT )
	{
	case __ESBMHT_BUTTON_UP:
		return m_rcButtonUp;
	case __ESBMHT_BUTTON_DOWN:
		return m_rcButtonDown;
	case __ESBMHT_THUMB:
		return m_rcThumb;
	case __ESBMHT_PAGE_UP:
		return m_rcPageUp;
	case __ESBMHT_PAGE_DOWN:
		return m_rcPageDown;
	} // switch( eSBMHT )
	return CRect( 0, 0, 0, 0 );
}

CRect CExtPaintManager::PAINTSCROLLBARDATA::GetAreaRectHT() const
{
	return GetAreaRect( m_eSBMHT );
}

LONG CExtPaintManager::stat_PassPaintMessages(
	bool bPassNcPaint, // = true
	bool bPassEraseBkgnd, // = true
	bool bPassPaint // = true
	)
{
	return
		stat_PassPaintMessages(
			(HWND)NULL,
			bPassNcPaint,
			bPassEraseBkgnd,
			bPassPaint
			);
}

LONG CExtPaintManager::stat_PassPaintMessages(
	HWND hWnd,
	bool bPassNcPaint, // = true
	bool bPassEraseBkgnd, // = true
	bool bPassPaint // = true
	)
{
LONG nCountPassed = 0L;
MSG msg;
	if( bPassNcPaint )
	{
		while( ::PeekMessage( &msg, hWnd, WM_NCPAINT, WM_NCPAINT, PM_NOREMOVE ) )
		{
			if( ! ::PeekMessage( &msg, hWnd, WM_NCPAINT, WM_NCPAINT, PM_REMOVE ) )
				break;
			::DispatchMessage( &msg );
			nCountPassed ++;
		} // while( ::PeekMessage( &msg, NULL, WM_NCPAINT, WM_NCPAINT, PM_NOREMOVE ) )
	} // if( bPassNcPaint )
	if( bPassEraseBkgnd )
	{
		while( ::PeekMessage( &msg, hWnd, WM_ERASEBKGND, WM_ERASEBKGND, PM_NOREMOVE ) )
		{
			if( ! ::PeekMessage( &msg, hWnd, WM_ERASEBKGND, WM_ERASEBKGND, PM_REMOVE ) )
				break;
			::DispatchMessage( &msg );
			nCountPassed ++;
		} // while( ::PeekMessage( &msg, NULL, WM_ERASEBKGND, WM_ERASEBKGND, PM_NOREMOVE ) )
	} // if( bPassEraseBkgnd )
	if( bPassPaint )
	{
		while( ::PeekMessage( &msg, hWnd, WM_PAINT, WM_PAINT, PM_NOREMOVE ) )
		{
			if( ! ::PeekMessage( &msg, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE ) )
				break;
			::DispatchMessage( &msg );
			nCountPassed ++;
		} // while( ::PeekMessage( &msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE ) )
	} // if( bPassPaint )
	return nCountPassed;
}

#if (defined __EXT_PM_BACK_COMPATIBILITY_BITMAP_API)

bool CExtPaintManager::stat_ResizeBitmap(
	CBitmap & bmpSrc,
	CSize _size,
	CBrush & brushFill
	)
{
	ASSERT( bmpSrc.GetSafeHandle() != NULL );
CDC dcSrc, dcDst;
CBitmap * pBmpOldSrc, *pBmpOldDst, bmpNew;
	{ // block
		if(		(! dcSrc.CreateCompatibleDC( NULL ) )
			||	(! dcDst.CreateCompatibleDC( NULL ) )
			)
		{
			ASSERT( FALSE );
			return false;
		}
		pBmpOldSrc = dcSrc.SelectObject( &bmpSrc );
		if( !bmpNew.CreateCompatibleBitmap(
				&dcSrc,
				_size.cx,
				_size.cy
				)
			)
		{
			ASSERT( FALSE );
			dcSrc.SelectObject( pBmpOldSrc );
			return false;
		}
		pBmpOldDst = dcSrc.SelectObject( &bmpNew );
	} // block

CRect rcFill( 0, 0, _size.cx, _size.cy );
	dcDst.FillRect( &rcFill, &brushFill );
	VERIFY(
		dcDst.BitBlt(
			0, 0, _size.cx, _size.cy,
			&dcSrc,
			0, 0,
			SRCCOPY
			)
		);
	dcSrc.SelectObject( pBmpOldSrc );
	dcDst.SelectObject( pBmpOldDst );
	bmpSrc.DeleteObject();
	bmpSrc.Attach( bmpNew.Detach() );
	return true;
}

HICON CExtPaintManager::stat_HBITMAPtoHICON(
	HBITMAP hBmpSrc,
	COLORREF clrTransparent,
	const RECT * pRcPart, // = NULL
	bool bReplaceSysColors // = true
	)
{
	if( hBmpSrc == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}

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
		ASSERT( FALSE );
		return NULL;
	}

COLORREF * pSurface = NULL;
HBITMAP hBmpExtracted = stat_CloneBitmap( hBmpSrc, pRcPart, &pSurface );
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
				int x, y;
				for( x = 0; x < bmpDescription.bmWidth; x++ )
				{
					for( y = 0; y < bmpDescription.bmHeight; y++ )
					{
						COLORREF clr = dcColor.GetPixel( x, y );
						if( clr != clrTransparent )
							dcMask.SetPixel( x, y, RGB(0,0,0) );
						else
							dcColor.SetPixel( x, y, RGB(0,0,0) );
					} // for( y = 0; y < bmpDescription.bmHeight; y++ )
				} // for( x = 0; x < bmpDescription.bmWidth; x++ )
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
			::DeleteObject( ii.hbmColor );
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
		ASSERT( FALSE );
		return NULL;
	}
HICON hIcon = _il.ExtractIcon( nImgIdx );
	ASSERT( hIcon != NULL );
	return hIcon;
}

HBITMAP CExtPaintManager::stat_CloneBitmap(
	HBITMAP hBmpSrc,
	const RECT * pRcPart, // = NULL
	COLORREF ** ppSurface // = NULL // if not NULL - create 32 bit surface
	)
{
	return
		CExtBitmap::stat_CloneBitmap(
			hBmpSrc,
			pRcPart,
			ppSurface
			);
}

bool CExtPaintManager::stat_CloneBitmap(
	CBitmap & bmpDst,
	const CBitmap & bmpSrc,
	const RECT * pRcPart, // = NULL
	COLORREF ** ppSurface // = NULL // if not NULL - create 32 bit surface
	)
{
	return
		CExtBitmap::stat_CloneBitmap(
			bmpDst,
			bmpSrc,
			pRcPart,
			ppSurface
			);
}

INT CExtPaintManager::stat_GetBitmapColorMap( // returns bitmap's color bit count or zero if error
	HBITMAP hBmpSrc,
	CMap < COLORREF, COLORREF, DWORD, DWORD > & _map // key is color, value is count of color entries in bitmap
	)
{
	_map.RemoveAll();
	if( hBmpSrc == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
BITMAP bmpSrcDescription;
	::memset( &bmpSrcDescription, 0, sizeof(BITMAP) );
	::GetObject( hBmpSrc, sizeof(BITMAP), &bmpSrcDescription );
	ASSERT( bmpSrcDescription.bmWidth > 0 && bmpSrcDescription.bmHeight > 0 );

HDC hSrcDC = ::CreateCompatibleDC( NULL );
	if( hSrcDC == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
LONG nPixelCount =
		bmpSrcDescription.bmWidth*bmpSrcDescription.bmHeight;
HGDIOBJ hBmpSrcOld = ::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrc );
BITMAPINFOHEADER bih;
	::memset( &bih, 0, sizeof(BITMAPINFOHEADER) );
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = bmpSrcDescription.bmWidth;
	bih.biHeight = bmpSrcDescription.bmHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = nPixelCount;
RGBQUAD * pColor = NULL;
HBITMAP hBmpDst =
		::CreateDIBSection(
			hSrcDC,
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)&pColor,
			NULL,
			NULL
			);
	if( hBmpDst == NULL )
	{
		ASSERT( FALSE );
		::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrcOld );
		::DeleteDC( hSrcDC );
		return 0;
	}
	ASSERT( pColor != NULL );
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
			0, 0,
			bmpSrcDescription.bmWidth, bmpSrcDescription.bmHeight,
			hSrcDC,
			0, 0,
			SRCCOPY
			)
		);

	for( ; nPixelCount > 0; pColor++, nPixelCount-- )
	{
		COLORREF clrToInsert =
			RGB(
				pColor->rgbRed,
				pColor->rgbGreen,
				pColor->rgbBlue
				);
		DWORD dwEntries = 0;
		if( _map.Lookup( clrToInsert, dwEntries ) )
		{
			ASSERT( dwEntries > 0 );
			dwEntries++;
			_map.SetAt( clrToInsert, dwEntries );
		} // if( _map.Lookup( clrToInsert, dwEntries ) )
		else
			_map.SetAt( clrToInsert, 1 );
	} // for( ; nPixelCount > 0; pColor++, nPixelCount-- )

	::SelectObject( hDstDC, (HGDIOBJ)hBmpDstOld );
	::DeleteDC( hDstDC );
	::SelectObject( hSrcDC, (HGDIOBJ)hBmpSrcOld );
	::DeleteDC( hSrcDC );
	::DeleteObject( hBmpDst );

	return bmpSrcDescription.bmBitsPixel;
}

INT CExtPaintManager::stat_GetBitmapColorArray( // returns bitmap's color bit count or zero if error
	HBITMAP hBmpSrc,
	CArray < COLORREF, COLORREF > & arr,
	bool bMakeUniqueColors // = true // used only if bit count <= 8
	)
{
	arr.RemoveAll();
	if( hBmpSrc == NULL )
	{
		ASSERT( FALSE );
		return 0;
	}
BITMAP bmpSrcDescription;
	::memset( &bmpSrcDescription, 0, sizeof(BITMAP) );
	::GetObject( hBmpSrc, sizeof(BITMAP), &bmpSrcDescription );
	ASSERT( bmpSrcDescription.bmWidth > 0 && bmpSrcDescription.bmHeight > 0 );
	if( bmpSrcDescription.bmBitsPixel <= 8 )
	{
		HGLOBAL hGlobalDst = stat_HBITMAPtoHGLOBAL( hBmpSrc );
		if( hGlobalDst == NULL )
		{
			ASSERT( FALSE );
			return 0;
		}
		INT nBitCount =  (INT) ((LPBITMAPINFOHEADER)hGlobalDst)->biBitCount;
		ASSERT( nBitCount <= 8 );
		INT nColorCount = 1L << nBitCount;
		RGBQUAD * pColor = (RGBQUAD *)
			(	((LPBYTE)hGlobalDst)
				+ ((LPBITMAPINFOHEADER)hGlobalDst)->biSize
			);
		for( INT i = 0; i < nColorCount; i++ )
		{
			COLORREF clrToInsert =
				RGB(
					pColor[i].rgbRed,
					pColor[i].rgbGreen,
					pColor[i].rgbBlue
					);
			arr.Add( clrToInsert );
		} // for( INT i = 0; i < nColorCount; i++ )
		::GlobalFree( hGlobalDst );
		if( bMakeUniqueColors )
		{
			INT nCount = (INT)arr.GetSize();
			if( nCount < 2 )
				return bmpSrcDescription.bmBitsPixel;
			for( INT i = 0; i < (nCount-1); i++ )
			{
				COLORREF clrTest = arr[i];
				for( INT j = i+1; j < nCount;  )
				{
					COLORREF clrToCmp = arr[j];
					if( clrTest == clrToCmp )
					{
						arr.RemoveAt( j, 1 );
						nCount --;
					} // if( clrTest == clrToCmp )
					else
						j++;
				} // for( INT j = i+1; j < nCount; )
			} // for( INT i = 0; i < (nCount-1); i++ )
		} // if( bMakeUniqueColors )
		return bmpSrcDescription.bmBitsPixel;
	} // if( bmpSrcDescription.bmBitsPixel <= 8 )

CMap < COLORREF, COLORREF, DWORD, DWORD > _map;
INT nVal = stat_GetBitmapColorMap( hBmpSrc, _map );
	if( nVal == 0 )
	{
		ASSERT( FALSE );
		return 0;
	}
	ASSERT( nVal == bmpSrcDescription.bmBitsPixel );
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

	return bmpSrcDescription.bmBitsPixel;
}

HGLOBAL CExtPaintManager::stat_HBITMAPtoHGLOBAL(
	HBITMAP hBmpSrc,
    HPALETTE hPalDst // = NULL
	)
{
	return CExtBitmap::stat_HBITMAPtoHGLOBAL( hBmpSrc, hPalDst );
}

HBITMAP CExtPaintManager::stat_LoadBitmapFromMemoryData(
	const BYTE * pDataBmp,
	DWORD dwSizeDataBmp
	)
{
	if( pDataBmp == NULL || dwSizeDataBmp == 0 )
	{
		ASSERT( FALSE );
		return NULL;
	} // if( pDataBmp == NULL || dwSizeDataBmp == 0 )
	try
	{
		CMemFile _file( (BYTE *)pDataBmp, UINT(dwSizeDataBmp), 0 );
		CArchive ar( &_file, CArchive::load );
		HBITMAP hBmp = NULL;
		if( ! stat_SerializeBitmap( ar, &hBmp ) )
		{
			ASSERT( FALSE );
			return NULL;
		} // if( ! stat_SerializeBitmap( ar, &hBmp ) )
		ASSERT( hBmp != NULL );
		return hBmp;
	} // try
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		ASSERT( FALSE );
	} // catch( ... )
	return NULL;
}

bool CExtPaintManager::stat_SerializeBitmap(
	CArchive & ar,
	HBITMAP * p_hBmp,
	DWORD dwResourceSize // = 0L // not 0 only if loaded from resources
	)
{
	ASSERT( p_hBmp != NULL );
	if( ar.IsStoring() )
	{
		ASSERT( (*p_hBmp) != NULL );
		CExtBitmap _bitmap;
		if( ! _bitmap.FromBitmap( *p_hBmp ) )
			return false;
		if( ! _bitmap.SaveBMP( ar, false, false ) )
			return false;
	} // if( ar.IsStoring() )
	else
	{
		ASSERT( (*p_hBmp) == NULL );
		CExtBitmap _bitmap;
		if( ! _bitmap.LoadBMP( ar, true, false, dwResourceSize ? true : false ) )
			return false;
		(*p_hBmp) = _bitmap.CreateBitmap( false );
	} // else from if( ar.IsStoring() )
	return true;
}

bool CExtPaintManager::stat_SerializeBitmap(
	__EXT_MFC_SAFE_LPCTSTR strFileName,
	bool bRead,
	HBITMAP * p_hBmp
	)
{
	ASSERT( strFileName != NULL );
	if( strFileName == NULL )
		return false;
bool bRetVal = true;
	try
	{
		CFile _file(
			strFileName,
			bRead
				? (CFile::modeRead | CFile::typeBinary)
				: (CFile::modeWrite | CFile::modeCreate | CFile::typeBinary)
			);
		CArchive ar(
			&_file,
			bRead ? CArchive::load : CArchive::store
			);
		bRetVal = stat_SerializeBitmap( ar, p_hBmp );
		ASSERT( bRetVal );
		if( !bRead )
			ar.Flush();
		ar.Close();
		_file.Close();
	} // try
	catch( CException * pXept )
	{
		ASSERT( FALSE );
		pXept->Delete();
		return false;
	} // catch( CException * pXept )
	catch( ... )
	{
		ASSERT( FALSE );
		return false;
	} // catch( ... )
	return bRetVal;
}

HBITMAP CExtPaintManager::stat_LoadBitmapResource(
	HINSTANCE hInst,
	HRSRC hRsrc
	)
{
	if( hInst == NULL || hRsrc == NULL )
		return NULL;
HGLOBAL hGlobal = ::LoadResource( hInst, hRsrc );
	if( hGlobal == NULL )
		return NULL;
LPBYTE pData = (LPBYTE)::LockResource( hGlobal );
	if( pData == NULL )
	{
		::FreeResource( hGlobal );
		return NULL;
	} // if( pData == NULL )
DWORD dwResourceSize = ::SizeofResource( hInst, hRsrc );
	ASSERT( dwResourceSize > 0 );
CMemFile _file;
	_file.Attach( pData, dwResourceSize );
	_file.Seek( 0, CFile::begin );
CArchive ar( &_file, CArchive::load );
HBITMAP hBmp = NULL;
bool bSerializationPassed =
		stat_SerializeBitmap( ar, &hBmp, dwResourceSize );
	ASSERT( bSerializationPassed );
	ar.Close();
	_file.Detach();
	::UnlockResource( hGlobal );
	::FreeResource( hGlobal );
	if( !bSerializationPassed )
		return NULL;
	ASSERT( hBmp != NULL );
	return hBmp;
}

HBITMAP CExtPaintManager::stat_LoadBitmapResource(
	__EXT_MFC_SAFE_LPCTSTR sResourceID,
	__EXT_MFC_SAFE_LPCTSTR sResourceType, // = RT_BITMAP, // default bitmap section
	HINSTANCE hInst // = NULL // find it automatically
	)
{
	if( hInst == NULL )
	{
		hInst =
			::AfxFindResourceHandle(
				sResourceID,
				sResourceType
				);
		if( hInst == NULL )
			return NULL;
	} // if( hInst == NULL )
HRSRC hRsrc =
		::FindResource(
			hInst,
			sResourceID,
			sResourceType
			);
	if( hRsrc == NULL )
		return NULL;
	return stat_LoadBitmapResource( hInst, hRsrc );
}

#endif // __EXT_PM_BACK_COMPATIBILITY_BITMAP_API

void CExtPaintManager::stat_GetMonitorParms(
	CExtPaintManager::monitor_parms_t & _mp
	)
{
	VERIFY(
		::GetWindowRect(
			::GetDesktopWindow(),
			&_mp.m_rcMonitor
			)
		);
	VERIFY(
		::SystemParametersInfo(
			SPI_GETWORKAREA,
			0,
			PVOID(&_mp.m_rcWorkArea),
			0
			)
		);
	_mp.m_bPrimaryMonitor = true;
}

void CExtPaintManager::stat_GetMonitorParms(
	CExtPaintManager::monitor_parms_t & _mp,
	const POINT & _ptScreenFrom
	)
{
MONITORINFO _MonitorInfo;
	::memset( &_MonitorInfo, 0, sizeof(MONITORINFO) );
	_MonitorInfo.cbSize = sizeof(MONITORINFO);
	if(	!::GetMonitorInfo(
			MonitorFromPoint(
				_ptScreenFrom, 
				MONITOR_DEFAULTTONEAREST
				),
				&_MonitorInfo
			)
		)
	{
		ASSERT( FALSE );
		stat_GetMonitorParms( _mp );
		return;
	}
	_mp.m_rcMonitor = _MonitorInfo.rcMonitor;
	_mp.m_rcWorkArea = _MonitorInfo.rcWork;
	_mp.m_bPrimaryMonitor = 
		( (_MonitorInfo.dwFlags&MONITORINFOF_PRIMARY) != 0 )
			? true
			: false
			;
}

void CExtPaintManager::stat_GetMonitorParms(
	CExtPaintManager::monitor_parms_t & _mp,
	const RECT & _rcScreenFrom
	)
{
MONITORINFO _MonitorInfo;
	::memset( &_MonitorInfo, 0, sizeof(MONITORINFO) );
	_MonitorInfo.cbSize = sizeof(MONITORINFO);
	if(	!::GetMonitorInfo(
			MonitorFromRect(
				&_rcScreenFrom, 
				MONITOR_DEFAULTTONEAREST
				),
				&_MonitorInfo
			)
		)
	{
		ASSERT( FALSE );
		stat_GetMonitorParms( _mp );
		return;
	}
	_mp.m_rcMonitor = _MonitorInfo.rcMonitor;
	_mp.m_rcWorkArea = _MonitorInfo.rcWork;
	_mp.m_bPrimaryMonitor = 
		( (_MonitorInfo.dwFlags&MONITORINFOF_PRIMARY) != 0 )
			? true
			: false
			;
}

void CExtPaintManager::stat_GetMonitorParms(
	monitor_parms_t & _mp,
	HWND hWndFrom
	)
{
	ASSERT( hWndFrom != NULL && ::IsWindow(hWndFrom) );
RECT rcWnd = { 0, 0, 0, 0 };
	if( ::IsIconic( hWndFrom ) )
	{
		WINDOWPLACEMENT _wp;
		::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
		_wp.length = sizeof(WINDOWPLACEMENT);
		if( ::GetWindowPlacement( hWndFrom, &_wp ) )
			::memcpy( &rcWnd, &_wp.rcNormalPosition, sizeof(RECT) );
		else
			VERIFY(
				::GetWindowRect(
					::GetDesktopWindow(),
					&rcWnd
					)
				);
	}
	else
	{
		VERIFY( ::GetWindowRect( hWndFrom, &rcWnd ) );
	}
	stat_GetMonitorParms( _mp, rcWnd );
}

void CExtPaintManager::stat_GetMonitorParms(
	monitor_parms_t & _mp,
	CWnd * pWndFrom
	)
{
	ASSERT_VALID( pWndFrom );
	stat_GetMonitorParms( _mp, pWndFrom->GetSafeHwnd() );
}

CRect CExtPaintManager::stat_AlignWndRectToMonitor(
	CRect rcInitial,
	bool bUseWorkArea, // = true // if false - use all monitor area
	bool bNoPartialVisibility // = false
	)
{
monitor_parms_t _mp;
	stat_GetMonitorParms( _mp, rcInitial );
CRect rcDesktop = bUseWorkArea ? _mp.m_rcWorkArea : _mp.m_rcMonitor;
	if( rcInitial.left >= rcDesktop.right )
		rcInitial.OffsetRect( rcDesktop.right - rcInitial.right, 0 );
	if( rcInitial.top >= rcDesktop.bottom )
		rcInitial.OffsetRect( 0, rcDesktop.bottom - rcInitial.bottom );
	if( rcInitial.right <= rcDesktop.left )
		rcInitial.OffsetRect( rcDesktop.left - rcInitial.left, 0 );
	if( rcInitial.bottom <= rcDesktop.top )
		rcInitial.OffsetRect( 0, rcDesktop.top - rcInitial.top );
	if( bNoPartialVisibility )
	{
		if( rcInitial.right > rcDesktop.right )
			rcInitial.OffsetRect( rcDesktop.right - rcInitial.right, 0 );
		if( rcInitial.bottom > rcDesktop.bottom )
			rcInitial.OffsetRect( 0, rcDesktop.bottom - rcInitial.bottom );
		if( rcInitial.left < rcDesktop.left )
			rcInitial.OffsetRect( rcDesktop.left - rcInitial.left, 0 );
		if( rcInitial.top < rcDesktop.top )
			rcInitial.OffsetRect( 0, rcDesktop.top - rcInitial.top );
	} // if( bNoPartialVisibility )
	return rcInitial;
}

void CExtPaintManager::stat_DrawDotLineH(
	CDC & dc,
	int x1,
	int x2,
	int y,
	COLORREF clrDots
	)
{
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( (x1&1) != 0 )
		x1++;
	for( ; x1 < x2; x1 += 2 )
		dc.SetPixel( x1, y, clrDots );
}

void CExtPaintManager::stat_DrawDotLineV(
	CDC & dc,
	int x,
	int y1,
	int y2,
	COLORREF clrDots
	)
{
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( (y1&1) != 0 )
		y1++;
	for( ; y1 < y2; y1 += 2 )
		dc.SetPixel( x, y1, clrDots );
}

#if (defined __EXT_PM_BACK_COMPATIBILITY_COLOR_API)

COLORREF CExtPaintManager::stat_RGBtoCMY( COLORREF clr )
{
	return CExtBitmap::stat_RGBtoCMY( clr );
}

COLORREF CExtPaintManager::stat_CMYtoRGB( COLORREF clr )
{
	return CExtBitmap::stat_CMYtoRGB( clr );
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
double CExtPaintManager::stat_HuetoRGB(double m1, double m2, double h )
{
	return CExtBitmap::stat_HuetoRGB( m1,  m2,  h );
}

COLORREF CExtPaintManager::stat_HLStoRGB( double H, double L, double S )
{
	return CExtBitmap::stat_HLStoRGB( H, L, S );
}

void CExtPaintManager::stat_RGBtoHSL( COLORREF rgb, double *H, double *S, double *L )
{   
	CExtBitmap::stat_RGBtoHSL( rgb, H, S, L );
}

BYTE CExtPaintManager::stat_GetRGBFromHue(float rm1, float rm2, float rh)
{
	return CExtBitmap::stat_GetRGBFromHue( rm1,  rm2,  rh );
}

COLORREF CExtPaintManager::stat_GetRGBFromHLSExtend( double H, double L, double S )
{
	return CExtBitmap::stat_GetRGBFromHLSExtend( H, L, S );
}

COLORREF CExtPaintManager::stat_HLS_Adjust(
	COLORREF clr,
	double percentH, // = 0.0
	double percentL, // = 0.0
	double percentS  // = 0.0
	)
{
	return CExtBitmap::stat_HLS_Adjust( clr, percentH, percentL, percentS );
}

COLORREF CExtPaintManager::stat_RGB_Enlight( 
	COLORREF clrLight, 
	COLORREF clrDark, 
	INT nEnlighten
	)
{
	return CExtBitmap::stat_RGB_Enlight( clrLight, clrDark, nEnlighten );
}

COLORREF CExtPaintManager::stat_RGB_Blend( 
	COLORREF clrFrom, 
	COLORREF clrTo, 
	INT nAlpha
	)
{
	return CExtBitmap::stat_RGB_Blend( clrFrom, clrTo, nAlpha );
}

#endif // (defined __EXT_PM_BACK_COMPATIBILITY_COLOR_API)

BOOL CExtPaintManager::stat_PaintParentBk(
	HWND hWnd,
	HDC hDC,
	LPCRECT rectClip // = NULL
	)
{
	ASSERT( hDC != NULL );
	ASSERT( ::IsWindow(hWnd) );
	if( rectClip != NULL )
	{
		CRgn rgn;
		rgn.CreateRectRgnIndirect( rectClip );
		::SelectClipRgn( hDC, (HRGN)rgn.GetSafeHandle() );
	}
HWND hWndParent = ::GetParent( hWnd );
	ASSERT( ::IsWindow(hWndParent) );
CPoint pt( 0, 0 );
	::MapWindowPoints(hWnd,hWndParent,&pt,1);
	VERIFY(
		::OffsetWindowOrgEx(
			hDC,
			pt.x,
			pt.y,
			&pt
			)
		);
LRESULT lRes =
		::SendMessage(
			hWndParent,
			WM_ERASEBKGND,
			(WPARAM)hDC,
			0
			);
	VERIFY(
		::SetWindowOrgEx(
			hDC,
			pt.x,
			pt.y,
			&pt
			)
		);
	::SelectClipRgn( hDC, NULL );
	return (BOOL)lRes;
}

bool CExtPaintManager::stat_DefIsHwndNeedsDirectRepaint(
	HWND hWndChild
	)
{
	DWORD dwChildStyle =
		DWORD( ::__EXT_MFC_GetWindowLong( hWndChild, GWL_STYLE ) );

	CExtSafeString sClassName;
	::GetClassName(
		hWndChild,
		sClassName.GetBuffer( _MAX_PATH + 1 ),
		_MAX_PATH
		);
	sClassName.ReleaseBuffer();

	if(		sClassName.CompareNoCase( __EXT_SIZE_GRIPPER_CLASS_NAME ) == 0
		||	sClassName.CompareNoCase( WC_TABCONTROL ) == 0
		||	sClassName.CompareNoCase( TRACKBAR_CLASS ) == 0
		||	sClassName.CompareNoCase( _T("Static") ) == 0
		)
		return true;
	if(		sClassName.CompareNoCase( _T("Button") ) == 0
		&&	(	g_PaintManager.m_bUxValidColorsExtracted
			||	(dwChildStyle&0x0F) == BS_DEFPUSHBUTTON
			||	(dwChildStyle&0x0F) == BS_PUSHBUTTON
			||	(dwChildStyle&0x0F) == BS_CHECKBOX
			||	(dwChildStyle&0x0F) == BS_AUTOCHECKBOX
			||	(dwChildStyle&0x0F) == BS_RADIOBUTTON
			||	(dwChildStyle&0x0F) == BS_3STATE
			||	(dwChildStyle&0x0F) == BS_AUTO3STATE
			||	(dwChildStyle&0x0F) == BS_GROUPBOX
			||	(dwChildStyle&0x0F) == BS_AUTORADIOBUTTON
			)
		)
		return true;
	return false;
}

bool CExtPaintManager::stat_DefExcludeChildAreaCallback(
	HDC hDC,
	HWND hWnd,
	HWND hWndChild,
	LPVOID pCookie
	)
{
	ASSERT( hDC != NULL );
	ASSERT( hWnd != NULL );
	ASSERT( IsWindow(hWnd) );
	ASSERT( hWndChild != NULL );
	ASSERT( IsWindow(hWndChild) );
	hDC;
	hWnd;
	hWndChild;
	pCookie;
	return stat_DefIsHwndNeedsDirectRepaint( hWndChild );
}

int CExtPaintManager::stat_GetBPP()
{
CWindowDC dc_desktop( NULL );
int nBitsPerPixel = ::GetDeviceCaps( dc_desktop.m_hDC, BITSPIXEL );
	return nBitsPerPixel;
}

static const UINT stat_pixels_glyph_btn_expand_bottom[] =
{
	1,1,1,1,1,
	0,1,1,1,0,
	0,0,1,0,0,
};

static const UINT stat_pixels_glyph_btn_expand_bottom_2003[] =
{
	1,1,1,1,1,
	0,0,0,0,0,
	0,0,0,0,0,
	1,1,1,1,1,
	0,1,1,1,0,
	0,0,1,0,0,
};

const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_bottom(
		5,3,2,
		stat_pixels_glyph_btn_expand_bottom
		);
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_bottom_2003(
		5,6,2,
		stat_pixels_glyph_btn_expand_bottom_2003
		);

static const UINT stat_pixels_glyph_btn_expand_right2[] =
{
	1,1,0,0,1,1,0,0,
	0,1,1,0,0,1,1,0,
	0,0,1,1,0,0,1,1,
	0,1,1,0,0,1,1,0,
	1,1,0,0,1,1,0,0,
};
static const UINT stat_pixels_glyph_btn_expand_right2_2003[] =
{
	0,0,0,0,0,0,
	1,0,0,0,1,0,
	1,1,0,0,1,1,
	1,0,0,0,1,0,
	0,0,0,0,0,0,
};

const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_right2(
		8,5,2,
		stat_pixels_glyph_btn_expand_right2
		);
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_right2_2003(
		6,5,2,
		stat_pixels_glyph_btn_expand_right2_2003
		);

const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_left(
		CExtPaintManager::g_glyph_btn_expand_bottom,
		90
		);

const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_left_2003(
		CExtPaintManager::g_glyph_btn_expand_bottom_2003,
		90
		);

const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_right(
		CExtPaintManager::g_glyph_btn_expand_bottom,
		270
		);
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_right_2003(
		CExtPaintManager::g_glyph_btn_expand_bottom_2003,
		270
		);

const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_bottom2(
		CExtPaintManager::g_glyph_btn_expand_right2,
		90
		);
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_btn_expand_bottom2_2003(
		CExtPaintManager::g_glyph_btn_expand_right2_2003,
		90
		);

static const UINT stat_pixels_glyph_customize_popup_src[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,1,0,0,0,0,0,
	0,0,0,0,0,0,1,1,0,0,0,0,
	0,0,0,0,0,0,1,1,1,0,0,0,
	0,0,0,0,0,0,1,1,1,1,0,0,
	0,0,0,0,0,0,1,1,1,1,1,0,
	0,0,0,0,0,0,1,1,1,1,1,1,
	0,0,0,0,0,0,1,1,1,1,1,0,
	0,0,0,0,0,0,1,1,1,1,0,0,
	0,0,0,0,0,0,1,1,1,0,0,0,
	0,0,0,0,0,0,1,1,0,0,0,0,
	0,0,0,0,0,0,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
};
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_customize_popup_src(
		12,16,2,
		stat_pixels_glyph_customize_popup_src
		);
static const UINT stat_pixels_glyph_customize_dropdown_src[] =
{
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,1,0,0,0,0,0,
	1,0,0,0,0,0,1,1,0,0,0,0,
	1,0,0,0,0,0,1,1,1,0,0,0,
	1,0,0,0,0,0,1,1,1,1,0,0,
	1,0,0,0,0,0,1,1,1,1,1,0,
	1,0,0,0,0,0,1,1,1,1,1,1,
	1,0,0,0,0,0,1,1,1,1,1,0,
	1,0,0,0,0,0,1,1,1,1,0,0,
	1,0,0,0,0,0,1,1,1,0,0,0,
	1,0,0,0,0,0,1,1,0,0,0,0,
	1,0,0,0,0,0,1,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,0,0,
};
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_customize_dropdown_src(
		12,16,2,
		stat_pixels_glyph_customize_dropdown_src
		);

static const UINT stat_pixels_glyph_task_pane_btn_expand_up[] =
{
	0,0,0,1,0,0,0,
	0,0,1,1,1,0,0,
	0,1,1,0,1,1,0,
	1,1,0,0,0,1,1,
	0,0,0,1,0,0,0,
	0,0,1,1,1,0,0,
	0,1,1,0,1,1,0,
	1,1,0,0,0,1,1,
};
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_task_pane_btn_expand_up(
		7,8,2,
		stat_pixels_glyph_task_pane_btn_expand_up
		);

static const UINT stat_pixels_glyph_task_pane_btn_expand_down[] =
{
	1,1,0,0,0,1,1,
	0,1,1,0,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
	1,1,0,0,0,1,1,
	0,1,1,0,1,1,0,
	0,0,1,1,1,0,0,
	0,0,0,1,0,0,0,
};
const CExtPaintManager::glyph_t
	CExtPaintManager::g_glyph_task_pane_btn_expand_down(
		7,8,2,
		stat_pixels_glyph_task_pane_btn_expand_down
		);

void CExtPaintManager::PaintGlyph(
	CDC & dc,
	POINT ptDest,
	const glyph_t & glyph,
	COLORREF * pColorValues
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pColorValues != NULL );
	ASSERT( glyph.Size().cx > 0 );
	ASSERT( glyph.Size().cy > 0 );
CRect rcTestVisiblity(ptDest,glyph.Size());
	if( !dc.RectVisible(&rcTestVisiblity) )
		return;
int x_dest = ptDest.x + glyph.Size().cx;
int y_dest = ptDest.y + glyph.Size().cy;
const UINT * pColorIndex = glyph.GetSurface();
	ASSERT( pColorIndex != NULL );
	for( int y = ptDest.y; y < y_dest; y++ )
	{
		for( int x = ptDest.x; x < x_dest; x++, pColorIndex++ )
		{
			UINT nColorIndex = *pColorIndex;
			ASSERT( nColorIndex < glyph.GetColorsCount() );
			if( nColorIndex == 0 )
				continue;
			COLORREF clr = pColorValues[nColorIndex];
			dc.SetPixel( x, y, clr );
		}
	}
}

void CExtPaintManager::PaintGlyphCentered(
	CDC & dc,
	const RECT & rect,
	const glyph_t & glyph,
	COLORREF * pColorValues
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect _rect(rect);
	_rect.NormalizeRect();
	if( _rect.IsRectEmpty() )
		return;
	ASSERT( glyph.Size().cx > 0 );
	ASSERT( glyph.Size().cy > 0 );
	if(		glyph.Size().cx > _rect.Width()
		||	glyph.Size().cy > _rect.Height()
		)
		return;
CPoint ptDest = _rect.TopLeft();
	ptDest.x +=
		(_rect.Width() - glyph.Size().cx) / 2;
	ptDest.y +=
		(_rect.Height() - glyph.Size().cy) / 2;
	PaintGlyph(
		dc,
		ptDest,
		glyph,
		pColorValues
		);
}

void CExtPaintManager::stat_ExcludeChildAreas(
	HDC hDC,
	HWND hWnd,
	CExtPaintManager::pfnExcludeChildAreaCallback pCallback, // = NULL
	LPVOID pCallbackCookie // = NULL
	)
{
	if( hDC == NULL || !IsWindow(hWnd) )
		return;
HWND hWndChild = GetWindow( hWnd, GW_CHILD );
	for( ; hWndChild != NULL; hWndChild = GetWindow( hWndChild, GW_HWNDNEXT ) )
	{
		if( !IsWindowVisible(hWndChild) )
			continue;
		__EXT_MFC_LONG_PTR dwChildStyle = __EXT_MFC_GetWindowLong( hWndChild, GWL_STYLE );
		if( (dwChildStyle & WS_CHILD) == 0 )
			continue;
		RECT rc;
		if( !::GetWindowRect(hWndChild,&rc) )
			continue;
		if( !::ScreenToClient(hWnd, (LPPOINT)(&rc)) )
			continue;
		if( !::ScreenToClient(hWnd, ((LPPOINT)(&rc))+1) )
			continue;

		if( pCallback != NULL )
		{
			if( pCallback(
					hDC,
					hWnd,
					hWndChild,
					pCallbackCookie
					)
				)
				continue;
		}
		ExcludeClipRect(
			hDC,
			rc.left,rc.top,
			rc.right,rc.bottom
			);
	} // for( ; hWndChild != NULL; hWndChild = GetWindow( hWndChild, GW_HWNDNEXT ) )
}

void CExtPaintManager::stat_TransparentBlt(
	HDC hdcDest,        // handle to destination DC
	int nXOriginDest,   // x-coord of destination upper-left corner
	int nYOriginDest,   // y-coord of destination upper-left corner
	int nWidthDest,     // width of destination rectangle
	int hHeightDest,    // height of destination rectangle
	HDC hdcSrc,         // handle to source DC
	int nXOriginSrc,    // x-coord of source upper-left corner
	int nYOriginSrc,    // y-coord of source upper-left corner
	int nWidthSrc,      // width of source rectangle
	int nHeightSrc,     // height of source rectangle
	UINT crTransparent  // color to make transparent
	)
{
	ASSERT( hdcDest != NULL && hdcSrc != NULL );

//	if( g_PaintManager.m_pfnMsImgTransparentBlt != NULL )
//	{
//		g_PaintManager.m_pfnMsImgTransparentBlt(
//			hdcDest,
//			nXOriginDest,
//			nYOriginDest,
//			nWidthDest,
//			hHeightDest,
//			hdcSrc,
//			nXOriginSrc,
//			nYOriginSrc,
//			nWidthSrc,
//			nHeightSrc,
//			crTransparent
//			);
//		return;
//	} // if( g_PaintManager.m_pfnMsImgTransparentBlt != NULL )

CDC * pDcDest = CDC::FromHandle( hdcDest );
	ASSERT( pDcDest != NULL );
CExtMemoryDC
		dcmm( pDcDest ),
		dcmsk(
			pDcDest,
			NULL,
///			CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY
///				|
				CExtMemoryDC::MDCOPT_TO_MEMORY
				|CExtMemoryDC::MDCOPT_FORCE_BLACK
			)
		;
	if(		( nWidthDest != nWidthSrc || hHeightDest != nHeightSrc )
		&&	nWidthDest >= 0
		&&	hHeightDest >= 0
		)
	{
		VERIFY(
			::StretchBlt(
				dcmm.GetSafeHdc(),
				0,
				0,
				nWidthDest,
				hHeightDest,
				hdcSrc,
				nXOriginSrc,
				nYOriginSrc,
				nWidthSrc,
				nHeightSrc,
				SRCCOPY
				)
			);
	}
	else
	{
		VERIFY(
			::BitBlt(
				dcmm.GetSafeHdc(),
				0,
				0,
				nWidthSrc,
				nHeightSrc,
				hdcSrc,
				nXOriginSrc,
				nYOriginSrc,
				SRCCOPY
				)
			);
	}
int nRealWidthDest  =
		(nWidthDest < 0) ? nWidthSrc : nWidthDest;
int nRealHeightDest =
		(hHeightDest < 0) ? nHeightSrc : hHeightDest;
	dcmm.SetBkColor( crTransparent );
	VERIFY(
		dcmsk.BitBlt(
			0,
			0,
			nRealWidthDest,
			nRealHeightDest,
			&dcmm,
			0,
			0,
			SRCCOPY
			)
		);
	dcmm.SetBkColor( RGB(0,0,0) );
	dcmm.SetTextColor( RGB(255,255,255) );
	VERIFY(
		dcmm.BitBlt(
			0,
			0,
			nRealWidthDest,
			nRealHeightDest,
			&dcmsk,
			0,
			0,
			SRCAND
			)
		);
COLORREF clrBkOld = ::SetBkColor( hdcDest, RGB(255,255,255) );
COLORREF clrTextOld = ::SetTextColor( hdcDest, RGB(0,0,0) );
	VERIFY(
		::BitBlt(
			hdcDest,
			nXOriginDest,
			nYOriginDest,
			nRealWidthDest,
			nRealHeightDest,
			dcmsk.GetSafeHdc(),
			0,
			0,
			SRCAND
			)
		);
	VERIFY(
		::BitBlt(
			hdcDest,
			nXOriginDest,
			nYOriginDest,
			nRealWidthDest,
			nRealHeightDest,
			dcmm.GetSafeHdc(),
			0,
			0,
			SRCPAINT
			)
		);
	dcmm.__Flush( FALSE );
	dcmsk.__Flush( FALSE );
	::SetBkColor( hdcDest, clrBkOld );
	::SetTextColor( hdcDest, clrTextOld );
}

// generate DIB for UI light brush (when BPP <= 8 )
HBITMAP CExtPaintManager::stat_GenLBDIB(
	COLORREF clrDarker, // = COLORREF(-1) // by default COLORREF(-1) is changed to ::GetSysColor(COLOR_BTNFACE)
	COLORREF clrLighter, // = COLORREF(-1) // by default COLORREF(-1) is changed to ::GetSysColor(COLOR_BTNHIGHLIGHT)
	int nColorMaskHorz, // = 1
	int nColorMaskVert, // = 1
	int nSkipMaskHorz, // = 0
	int nSkipMaskVert, // = 0
	COLORREF clrSkip // = COLORREF(-1L) // // default COLORREF(-1) is changed to clrDarker
	)
{
	if( clrSkip == COLORREF(-1L) )
		clrSkip = clrDarker;
CWindowDC dcAlign( NULL );
CDC dc;
	if( !dc.CreateCompatibleDC( &dcAlign ) )
	{
		ASSERT( FALSE );
		return NULL;
	}
CBitmap bmp;
	if( !bmp.CreateCompatibleBitmap( &dcAlign, 16, 16 ) )
	{
		ASSERT( FALSE );
		return NULL;
	}
CBitmap * pOldBmp = dc.SelectObject( &bmp );
COLORREF clrEffectiveDarker =
		( clrDarker == COLORREF(-1) )
			? ( ::GetSysColor( COLOR_BTNFACE ) )
			: clrDarker
			;
COLORREF clrEffectiveLighter =
		( clrLighter == COLORREF(-1) )
			? ( ::GetSysColor( COLOR_BTNHIGHLIGHT ) )
			: clrLighter
			;
	for( int nY = 0; nY < 16; nY++ )
	{
		for( int nX = 0; nX < 16; nX++ )
		{
			if(		(nX & nSkipMaskHorz) != 0
				||	(nY & nSkipMaskVert) != 0
				)
			{
				dc.SetPixel( nX, nY, clrSkip );
				continue;
			}
			COLORREF clr = ( (nY&nColorMaskVert) != 0 )
				? ( ( (nX&nColorMaskHorz) != 0 ) ? clrEffectiveDarker : clrEffectiveLighter )
				: ( ( (nX&nColorMaskHorz) != 0 ) ? clrEffectiveLighter : clrEffectiveDarker )
				;
			dc.SetPixel( nX, nY, clr );
		}
	}
	dc.SelectObject( pOldBmp );
	return (HBITMAP)bmp.Detach();
}

void CExtPaintManager::stat_PaintGradientRect(
	CDC & dc,
	const CRect & rcPaintGradient,
	COLORREF clrLeft,
	COLORREF clrRight,
	bool bHorz, // = false
	UINT nCountOfSteps // = 256
	)
{
	if( g_PaintManager.m_pfnMsImgGradientFill != NULL && g_PaintManager.m_bIsWinNT )
	{
		COLORREF clr1 = bHorz ? clrRight : clrLeft;
		COLORREF clr2 = bHorz ? clrLeft : clrRight;
		TRIVERTEX vert[2];
		vert[0].x      = rcPaintGradient.left;
		vert[0].y      = rcPaintGradient.top;
		vert[0].Red    = (COLOR16)(GetRValue(clr1) << 8);
		vert[0].Green  = (COLOR16)(GetGValue(clr1) << 8);
		vert[0].Blue   = (COLOR16)(GetBValue(clr1) << 8);
		vert[0].Alpha  = 0x0000;
		vert[1].x      = rcPaintGradient.right;
		vert[1].y      = rcPaintGradient.bottom; 
		vert[1].Red    = (COLOR16)(GetRValue(clr2) << 8);
		vert[1].Green  = (COLOR16)(GetGValue(clr2) << 8);
		vert[1].Blue   = (COLOR16)(GetBValue(clr2) << 8);
		vert[1].Alpha  = 0x0000;
		GRADIENT_RECT gRect = { 0, 1 };
		g_PaintManager.m_pfnMsImgGradientFill( dc, vert, 2, &gRect, 1, (!bHorz) ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V );
	} // if( g_PaintManager.m_pfnMsImgGradientFill != NULL && g_PaintManager.m_bIsWinNT )
	else
	{
		if(		rcPaintGradient.left >= rcPaintGradient.right
			||	rcPaintGradient.top >= rcPaintGradient.bottom
			)
			return;
		ASSERT( nCountOfSteps > 0 && nCountOfSteps <= 256 );
		UINT nWidth = rcPaintGradient.Width(), nHeight = rcPaintGradient.Height();
		if( bHorz )
		{
			if( nCountOfSteps > nHeight )
				nCountOfSteps = nHeight;
		} // if( bHorz )
		else
		{
			if( nCountOfSteps > nWidth )
				nCountOfSteps = nWidth;
		} // else from if( bHorz )
		int nBitsPerPixel = ::GetDeviceCaps( dc.m_hDC, BITSPIXEL );
		if( nBitsPerPixel <= 8 )
		{
			UINT nCalcStepSize = 
				bHorz
					? ( nHeight / nCountOfSteps )
					: ( nWidth  / nCountOfSteps )
					;
				static const UINT nLowColorStepMinSize = 4;
			if( nCalcStepSize < nLowColorStepMinSize )
				nCountOfSteps =
					bHorz
						? ( nHeight / nLowColorStepMinSize )
						: ( nWidth  / nLowColorStepMinSize )
						;
		} // if( nBitsPerPixel <= 8 )
		CRect rcItem = rcPaintGradient;
		for( UINT nStep = 0, nBackStep = nCountOfSteps; nStep < nCountOfSteps; nStep++, nBackStep-- )
		{
			COLORREF clrItem =
				RGB(
					( GetRValue(clrLeft)*nBackStep + GetRValue(clrRight)*nStep ) / nCountOfSteps,
					( GetGValue(clrLeft)*nBackStep + GetGValue(clrRight)*nStep ) / nCountOfSteps,
					( GetBValue(clrLeft)*nBackStep + GetBValue(clrRight)*nStep ) / nCountOfSteps
					);
			if( bHorz )
			{
				rcItem.top = rcPaintGradient.bottom - ::MulDiv( nStep, nHeight, nCountOfSteps );
				rcItem.top --;
				if( nStep == ( nCountOfSteps - 1 ) )
				{
					rcItem.top = rcPaintGradient.top;
					if( rcItem.top > rcItem.bottom )
						break;
				}
				ASSERT( rcItem.Height() >= 1 );
				if( nBitsPerPixel <= 8 )
				{
					CBrush _brushStepRect( clrItem );
					dc.FillRect( rcItem, &_brushStepRect );
				}
				else
					dc.FillSolidRect( rcItem, clrItem );
				rcItem.bottom = rcItem.top;
			} // if( bHorz )
			else
			{
				rcItem.right = ::MulDiv( nStep, nWidth, nCountOfSteps ) + rcPaintGradient.left;
				rcItem.right ++;
				if( nStep == ( nCountOfSteps - 1 ) )
				{
					rcItem.right = rcPaintGradient.right;
					if( rcItem.right < rcItem.left )
						break;
				}
				ASSERT( rcItem.Width() >= 1 );
				if( nBitsPerPixel <= 8 )
				{
					CBrush _brushStepRect( clrItem );
					dc.FillRect( rcItem, &_brushStepRect );
				}
				else
					dc.FillSolidRect( rcItem, clrItem );
				rcItem.left = rcItem.right;
			} // else from if( bHorz )
		} // for( UINT nStep = 0, nBackStep = nCountOfSteps; nStep < nCountOfSteps; nStep++, nBackStep-- )
	} // else from if( g_PaintManager.m_pfnMsImgGradientFill != NULL && g_PaintManager.m_bIsWinNT )
}

#if (!defined __EXT_MFC_NO_TAB_CTRL)

void CExtPaintManager::stat_PaintTabItemImpl(
	CDC & dc,
	CRect & rcTabItemsArea,
	bool bTopLeft,
	bool bHorz,
	bool bSelected,
	bool bEnabled,
	bool bCenteredText,
	bool bGroupedMode,
	bool bInGroupActive,
	bool bInvertedVerticalMode,
	bool bDrawIcon,
	const CRect & rcEntireItem,
	CSize sizeTextMeasured,
	CFont * pFont,
	__EXT_MFC_SAFE_LPCTSTR sText,
	bool bNoPrefix,
	CExtCmdIcon * pIcon,
	CExtCmdIcon * pIconTabItemCloseButton,
	INT nPaintStateITICB,
	CRect & rcTabItemCloseButton,
	COLORREF clrText,
	COLORREF clrTabBk,
	COLORREF clrTabBorderLT,
	COLORREF clrTabBorderRB,
	COLORREF clrTabSeparator,
	bool bEnableEndEllipsis, // = true
	CObject * pHelperSrc, // = NULL
	bool bDwmMode // = false
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	pHelperSrc; rcTabItemsArea; sizeTextMeasured; bInGroupActive;

CRect rcItem( rcEntireItem );
CRect rcTabSel( rcItem );

	if( bGroupedMode )
	{
		CRect rcTabMargin( rcItem );
		rcTabMargin.InflateRect(
			0,
			0,
			bHorz ? 1 : 0,
			bHorz ? 0 : 1
			);
		rcTabMargin.InflateRect(
			bHorz ? 0 : 2,
			bHorz ? 2 : 0
			);
		if( clrTabBk != COLORREF(-1L) )
			dc.FillSolidRect(
				&rcTabMargin,
				clrTabBk
				);
		if(		clrTabBorderLT != COLORREF(-1L)
			&&	clrTabBorderRB != COLORREF(-1L)
			)
			dc.Draw3dRect(
				rcTabMargin,
				clrTabBorderLT,
				clrTabBorderRB
				);
	} // if( bGroupedMode )
	else
	{
		if( bSelected )
		{
			rcTabSel.InflateRect(
				bHorz ? __EXTTAB_SEPARATOR_DX : 0,
				bHorz ? 0 : __EXTTAB_SEPARATOR_DY,
				0,
				0
				);
			CRect rcSetMargins(
				( (!bHorz) && (!bTopLeft) ) ? 1 : 0,
				(   bHorz  && (!bTopLeft) ) ? 1 : 0,
				( (!bHorz) &&   bTopLeft  ) ? 1 : 0,
				(   bHorz  &&   bTopLeft  ) ? 1 : 0
				);
			rcTabSel.InflateRect(
				rcSetMargins.left,
				rcSetMargins.top,
				rcSetMargins.right,
				rcSetMargins.bottom
				);
			
			if(		clrTabBorderLT != COLORREF(-1L)
				&&	clrTabBorderRB != COLORREF(-1L)
				)
			{
				CPen penTabBorderLT(PS_SOLID,1,clrTabBorderLT);
				CPen penTabBorderRB(PS_SOLID,1,clrTabBorderRB);
				if( bHorz )
				{
					if( bTopLeft )
					{
						CPen * pOldPen = dc.SelectObject( &penTabBorderLT );
						dc.MoveTo( rcTabSel.left, rcTabSel.bottom );
						dc.LineTo( rcTabSel.left, rcTabSel.top );
						dc.LineTo( rcTabSel.right, rcTabSel.top );
						dc.SelectObject( &penTabBorderRB );
						dc.MoveTo( rcTabSel.right-1, rcTabSel.top );
						dc.LineTo( rcTabSel.right-1, rcTabSel.bottom-1 );
						dc.SelectObject( pOldPen );
					}
					else
					{
						CPen * pOldPen = dc.SelectObject( &penTabBorderLT );
						dc.MoveTo( rcTabSel.left, rcTabSel.top-1 );
						dc.LineTo( rcTabSel.left, rcTabSel.bottom );
						dc.SelectObject( &penTabBorderRB );
						dc.MoveTo( rcTabSel.left, rcTabSel.bottom-1 );
						dc.LineTo( rcTabSel.right-1, rcTabSel.bottom-1 );
						dc.LineTo( rcTabSel.right-1, rcTabSel.top-1 );
						dc.SelectObject( pOldPen );
					}
				}
				else
				{
					if( bTopLeft )
					{
						CPen * pOldPen = dc.SelectObject( &penTabBorderLT );
						dc.MoveTo( rcTabSel.right-1, rcTabSel.top );
						dc.LineTo( rcTabSel.left, rcTabSel.top );
						dc.LineTo( rcTabSel.left, rcTabSel.bottom );
						dc.SelectObject( &penTabBorderRB );
						dc.MoveTo( rcTabSel.left, rcTabSel.bottom-1 );
						dc.LineTo( rcTabSel.right-1, rcTabSel.bottom-1 );
						dc.SelectObject( pOldPen );
					}
					else
					{
						CPen * pOldPen = dc.SelectObject( &penTabBorderLT );
						dc.MoveTo( rcTabSel.left, rcTabSel.top );
						dc.LineTo( rcTabSel.right, rcTabSel.top );
						dc.SelectObject( &penTabBorderRB );
						dc.MoveTo( rcTabSel.right-1, rcTabSel.top );
						dc.LineTo( rcTabSel.right-1, rcTabSel.bottom-1 );
						dc.LineTo( rcTabSel.left, rcTabSel.bottom-1 );
						dc.SelectObject( pOldPen );
					}
				}
			}

			rcTabSel.DeflateRect(
				(rcSetMargins.left == 0)	? 1 : 0,
				(rcSetMargins.top == 0)		? 1 : 0,
				(rcSetMargins.right == 0)	? 1 : 0,
				(rcSetMargins.bottom == 0)	? 1 : 0
				);
			if( clrTabBk != COLORREF(-1L) )
				dc.FillSolidRect( &rcTabSel, clrTabBk );

		} // if( bSelected )
		else
		{
			CRect rcSeparator( rcItem );
			if( bHorz )
			{
				rcSeparator.left = rcSeparator.right - 1;
				rcSeparator.DeflateRect( 0, __EXTTAB_SEPARATOR_GAP_HORZ );
			}
			else
			{
				rcSeparator.top = rcSeparator.bottom - 1;
				rcSeparator.DeflateRect( __EXTTAB_SEPARATOR_GAP_VERT, 0 );
			}
			if( clrTabSeparator != COLORREF(-1L) )
				dc.FillSolidRect( &rcSeparator, clrTabSeparator );
		} // else from if( bSelected )
	} // else from if( bGroupedMode )

	rcItem.DeflateRect(
		bHorz ? __EXTTAB_MARGIN_BORDER_HX : __EXTTAB_MARGIN_BORDER_VX,
		bHorz ? __EXTTAB_MARGIN_BORDER_HY : __EXTTAB_MARGIN_BORDER_VY
		);

CSize _sizeIcon( 0, 0 );
	if( bDrawIcon )
	{
		_sizeIcon = pIcon->GetSize();
		ASSERT( _sizeIcon.cx > 0 && _sizeIcon.cy > 0 );
	} // if( bDrawIcon )

CExtSafeString sItemText( (sText == NULL) ? _T("") : sText );

// IMPORTANT:  the rcText calculation fixed by Genka
CRect rcText(
		rcItem.left
			+	(	bHorz
					? (_sizeIcon.cx +
						((_sizeIcon.cx > 0) ? __EXTTAB_MARGIN_ICON2TEXT_X : 0)
						)
					: 0
				),
		rcItem.top
			+	(	bHorz
					? 0
					: (_sizeIcon.cy +
						((_sizeIcon.cy > 0) ? __EXTTAB_MARGIN_ICON2TEXT_Y : 0)
						)
				),
		rcItem.right,
		rcItem.bottom
		);
	if( !bHorz )
	{
		if( pIconTabItemCloseButton != NULL )
			rcText.bottom = min( rcText.bottom, rcTabItemCloseButton.top );
	} // if( !bHorz )
	else
	{
		if( pIconTabItemCloseButton != NULL )
			rcText.right = min( rcText.right, rcTabItemCloseButton.left );
	}

bool bDrawText = ( ( ! sItemText.IsEmpty() ) && rcText.Width() > 0 && rcText.Height() > 0 ) ? true : false;

	if( bDrawIcon )
	{
		INT nIconAlignment = __ALIGN_VERT_TOP;
		if( (!bDrawText) && !( bGroupedMode && (!bInGroupActive) ) )
		{
			if( bCenteredText )
				nIconAlignment = __ALIGN_HORIZ_CENTER|__ALIGN_VERT_CENTER;
			else
				nIconAlignment |= __ALIGN_HORIZ_CENTER;
		}

		if(		(bHorz && rcItem.Width() >= _sizeIcon.cx )
			||	(!bHorz && rcItem.Height() >= _sizeIcon.cy)
			)
		{
			g_PaintManager->PaintIcon(
				dc,
				bHorz,
				pIcon,
				rcItem,
				false,
				bEnabled,
				false,
				nIconAlignment
				);
		}
	} // if( bDrawIcon )

	if( bDrawText )
	{ // if we have sense to paint text on tab item
		ASSERT( pFont != NULL );
		ASSERT( pFont->GetSafeHandle() != NULL );
		COLORREF clrOldText = dc.SetTextColor( clrText );
		INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CFont * pOldFont = dc.SelectObject( pFont );
		UINT nFormat = DT_SINGLELINE | DT_VCENTER | ( bEnableEndEllipsis ? (DT_END_ELLIPSIS) : 0 );
		INT nTextLen = sItemText.GetLength();
		if( bDwmMode )
		{
			CFont * pOldFont = dc.SelectObject( pFont );
			CExtUxTheme::__EXT_UX_DTTOPTS dto = { sizeof(CExtUxTheme::__EXT_UX_DTTOPTS) };
			dto.dwFlags = __EXT_UX_DTT_TEXTCOLOR;
			dto.iGlowSize = 0;
			dto.dwFlags |= __EXT_UX_DTT_COMPOSITED | __EXT_UX_DTT_GLOWSIZE;
			dto.crText = clrText;
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC, LPCTSTR(sItemText), &rcText,
				DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX, 0,
				::GetDesktopWindow(), VSCLASS_WINDOW, /*__EXT_UX_OTD_NONCLIENT*/ 0, 0, 0, &dto
				);
			dc.SetBkMode( nOldBkMode );
			dc.SelectObject( pOldFont );
		} // if( bDwmMode )
		else
		{
			CExtRichContentLayout::e_layout_orientation_t eLO = CExtRichContentLayout::__ELOT_NORMAL;
			if( ! bHorz )
				eLO = bInvertedVerticalMode ? CExtRichContentLayout::__ELOT_270_CW : CExtRichContentLayout::__ELOT_90_CW;
			if( bCenteredText )
				nFormat |= DT_CENTER;
			else
				nFormat |= DT_LEFT;
			if( bNoPrefix )
				nFormat |= DT_NOPREFIX;
			CExtRichContentLayout::stat_DrawText(
				CExtRichContentLayout::__ELFMT_AUTO_DETECT, eLO,
				dc.m_hDC, LPCTSTR(sItemText), nTextLen, rcText, nFormat, 0
				);
		} // else from if( bDwmMode )
		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldText );
	} // if we have sense to paint text on tab item

	if( pIconTabItemCloseButton != NULL )
	{
		ASSERT( ! pIconTabItemCloseButton->IsEmpty() );
		if( dc.RectVisible( &rcTabItemCloseButton ) )
			pIconTabItemCloseButton->Paint(
				g_PaintManager.GetPM(),
				dc.m_hDC,
				rcTabItemCloseButton,
				(CExtCmdIcon::e_paint_type_t)nPaintStateITICB
				);
	} // if( pIconTabItemCloseButton != NULL )
}

void CExtPaintManager::stat_PaintTabClientAreaImpl(
	CDC & dc,
	CRect & rcClient,
	CRect & rcTabItemsArea,
	CRect & rcTabNearBorderArea,
	DWORD dwOrientation,
	bool bGroupedMode,
	COLORREF clrTabItemsAreaBk,
	COLORREF clrTabNearBorderAreaBk,
	COLORREF clrTabNearMargin
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	rcTabItemsArea;
CRect rcTabNearMargin( rcTabNearBorderArea ); // prepare tab border margin rect

	switch( dwOrientation )
	{
	case __ETWS_ORIENT_TOP:
		rcTabNearMargin.bottom = rcTabNearMargin.top + 1;
	break;
	case __ETWS_ORIENT_BOTTOM:
		rcTabNearMargin.top = rcTabNearMargin.bottom - 1;
	break;
	case __ETWS_ORIENT_LEFT:
		rcTabNearMargin.right = rcTabNearMargin.left + 1;
	break;
	case __ETWS_ORIENT_RIGHT:
		rcTabNearMargin.left = rcTabNearMargin.right - 1;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( dwOrientation )

	// fill all area with tab items background color
	dc.FillSolidRect(
		&rcClient,
		clrTabItemsAreaBk
		);

	if( !bGroupedMode )
	{
		// fill tab border area
		dc.FillSolidRect(
			&rcTabNearBorderArea,
			clrTabNearBorderAreaBk
			);
		// paint tab border margin
		dc.FillSolidRect(
			&rcTabNearMargin,
			clrTabNearMargin
			);
	} // if( !bGroupedMode )
}

void CExtPaintManager::stat_PaintTabButtonImpl(
	CDC & dc,
	CRect & rcButton,
	LONG nHitTest,
	bool bTopLeft,
	bool bHorz,
	bool bEnabled,
	bool bHover,
	bool bPushed,
	bool bGroupedMode,
	COLORREF clrGlyph,
	COLORREF clrTL,
	COLORREF clrBR,
	bool bFlat // = false
	)
{
	bTopLeft;
	bGroupedMode;

CExtPaintManager::glyph_t * pGlyph = NULL;
	switch( nHitTest )
	{
	case __ETWH_BUTTON_LEFTUP:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
				bHorz
					? (	bEnabled
							? CExtPaintManager::__DCBT_ARROW_LEFT
							: CExtPaintManager::__DCBT_HOLLOW_LEFT
						)
					: (	bEnabled
							? CExtPaintManager::__DCBT_ARROW_UP
							: CExtPaintManager::__DCBT_HOLLOW_UP
						)
				];
		ASSERT( pGlyph != NULL );
	break;
	case __ETWH_BUTTON_RIGHTDOWN:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
				bHorz
					? (	bEnabled
						? CExtPaintManager::__DCBT_ARROW_RIGHT
						: CExtPaintManager::__DCBT_HOLLOW_RIGHT
						)
					: (	bEnabled
						? CExtPaintManager::__DCBT_ARROW_DOWN
						: CExtPaintManager::__DCBT_HOLLOW_DOWN
						)
				];
		ASSERT( pGlyph != NULL );
	break;
	case __ETWH_BUTTON_HELP:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
				CExtPaintManager::__DCBT_WND_CONTEXTHELP
				];
		ASSERT( pGlyph != NULL );
	break;
	case __ETWH_BUTTON_CLOSE:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
				CExtPaintManager::__DCBT_CLOSE
				];
		ASSERT( pGlyph != NULL );
	break;
	case __ETWH_BUTTON_TAB_LIST:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
			CExtPaintManager::__DCBT_TABS_LIST_ARROW
				];
		ASSERT( pGlyph != NULL );
	break;
	case __ETWH_BUTTON_SCROLL_HOME:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
				bHorz
					? CExtPaintManager::__DCBT_SCROLL_LEFT
					: CExtPaintManager::__DCBT_SCROLL_TOP
				];
		ASSERT( pGlyph != NULL );
	break;
	case __ETWH_BUTTON_SCROLL_END:
		pGlyph =
			CExtPaintManager::g_DockingCaptionGlyphs[
				bHorz
					? CExtPaintManager::__DCBT_SCROLL_RIGHT
					: CExtPaintManager::__DCBT_SCROLL_BOTTOM
				];
		ASSERT( pGlyph != NULL );
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( nHitTest )

	ASSERT( pGlyph != NULL );
//dc.FillSolidRect( &rcButton, RGB( 255, 128, 128 ) );

COLORREF ColorValues[2] =
{
	RGB(0,0,0),
	clrGlyph
};

	g_PaintManager->PaintGlyphCentered(
		dc,
		rcButton,
		*pGlyph,
		ColorValues
		);

	if(		(!bFlat) 
		&&	(bHover || bPushed) 
		)
		dc.Draw3dRect( &rcButton, clrTL, clrBR );
}

#endif // (!defined __EXT_MFC_NO_TAB_CTRL)

CRect CExtPaintManager::stat_CalcTextDimension(
	CExtRichContentLayout::e_layout_format_t eLFMT,
	CExtRichContentLayout::e_layout_orientation_t eLO,
	CDC & dc,
	CFont & font,
	const CExtSafeString & strText,
	UINT uFlags, // = DT_SINGLELINE|DT_LEFT|DT_CALCRECT
	UINT nExtDTF // = __EXT_DT_CALC_EFFECTIVE_RECT
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( font.GetSafeHandle() != NULL );
CRect rcText(0,0,0,0);
CFont * pOldFont = dc.SelectObject(
		&font
		);
	ASSERT( pOldFont != NULL );

	if( (uFlags&DT_CALCRECT) == 0 )
		uFlags |= DT_CALCRECT;

	if( strText.Find( _T('\t') ) != -1 ) // do tabs expanding
		uFlags |= DT_EXPANDTABS;
 
	CExtRichContentLayout::stat_DrawText(
		eLFMT, eLO,
		dc.m_hDC, LPCTSTR(strText), strText.GetLength(),
		&rcText, uFlags, nExtDTF
		);
	dc.SelectObject(pOldFont);
	rcText.OffsetRect( -rcText.TopLeft() );
	return rcText;
}

CRect CExtPaintManager::stat_CalcTextDimension(
	CDC & dc,
	CFont & font,
	const CExtSafeString & strText,
	UINT uFlags, // = DT_SINGLELINE|DT_LEFT|DT_CALCRECT
	UINT nExtDTF // = __EXT_DT_CALC_EFFECTIVE_RECT
	)
{
	return
		CExtPaintManager::stat_CalcTextDimension(
			CExtRichContentLayout::__ELFMT_AUTO_DETECT, CExtRichContentLayout::__ELOT_NORMAL,
			dc, font, strText, uFlags, nExtDTF
			);
}

int CExtPaintManager::stat_CalcTextWidth(
	CDC & dc,
	CFont & font,
	const CExtSafeString & strText
	)
{
	return
		CExtPaintManager::stat_CalcTextDimension(
			dc,
			font,
			strText
			).Width();
}

void CExtPaintManager::stat_PaintDotNet3dRect(
	CDC & dc,
	LPCRECT lpRect,
	COLORREF clrTopLeft,
	COLORREF clrBottomRight
	)
{
	stat_PaintDotNet3dRect(
		dc,
		lpRect->left,
		lpRect->top,
		lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top,
		clrTopLeft,
		clrBottomRight
		);
}

void CExtPaintManager::stat_PaintDotNet3dRect(
	CDC & dc,
	int x,
	int y,
	int cx,
	int cy,
	COLORREF clrTopLeft,
	COLORREF clrBottomRight
	)
{
const int nPixelGap = 1;
	dc.FillSolidRect(
		x + nPixelGap,
		y,
		cx - 1 - nPixelGap,
		1,
		clrTopLeft
		);
	dc.FillSolidRect(
		x,
		y + nPixelGap,
		1,
		cy - 1 - nPixelGap,
		clrTopLeft
		);
	dc.FillSolidRect(
		x + cx,
		y + nPixelGap,
		-1,
		cy - nPixelGap*2,
		clrBottomRight
		);
	dc.FillSolidRect(
		x + nPixelGap,
		y + cy,
		cx - nPixelGap*2,
		-1,
		clrBottomRight
		);
}

void CExtPaintManager::stat_PaintRadioDot( CDC & dc, int x, int y, COLORREF color )
{
CBrush brush,*pOldBrush;
CPen pen,*pOldPen;
CRect rcDot(x,y,x+6,y+6);

COLORREF color2 =
		dc.GetNearestColor(
			RGB(
				255-GetRValue(color),
				255-GetGValue(color),
				255-GetBValue(color)
			)
		);
	rcDot.OffsetRect(1,1);
	brush.CreateSolidBrush(color2);
	pen.CreatePen(PS_SOLID,0,color2);
	pOldBrush=dc.SelectObject(&brush);
	pOldPen=dc.SelectObject(&pen);
	dc.Ellipse(&rcDot);
	pen.DeleteObject();
	brush.DeleteObject();
	rcDot.OffsetRect(-1,-1);

	brush.CreateSolidBrush(color);
	pen.CreatePen(PS_SOLID,0,color);
	dc.SelectObject(&brush);
	dc.SelectObject(&pen);
	dc.Ellipse(&rcDot);
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CExtPaintManager::stat_PaintIndeterminatedSquare( CDC & dc, int x, int y, COLORREF color )
{
CBrush brush,*pOldBrush;
CPen pen,*pOldPen;
CRect rcSquare(x,y,x+6,y+6);

COLORREF color2 =
		dc.GetNearestColor(
			RGB(
				255-GetRValue(color),
				255-GetGValue(color),
				255-GetBValue(color)
			)
		);
	rcSquare.OffsetRect(1,1);
	brush.CreateSolidBrush(color2);
	pen.CreatePen(PS_SOLID,0,color2);
	pOldBrush=dc.SelectObject(&brush);
	pOldPen=dc.SelectObject(&pen);
	dc.Rectangle(&rcSquare);
	pen.DeleteObject();
	brush.DeleteObject();
	rcSquare.OffsetRect(-1,-1);

	brush.CreateSolidBrush(color);
	pen.CreatePen(PS_SOLID,0,color);
	dc.SelectObject(&brush);
	dc.SelectObject(&pen);
	dc.Rectangle(&rcSquare);
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CExtPaintManager::stat_PaintCheckMark( CDC & dc, int x, int y, COLORREF color )
{
COLORREF color2 =
		dc.GetNearestColor(
			RGB(
				255-GetRValue(color),
				255-GetGValue(color),
				255-GetBValue(color)
			)
		);
 	dc.SetPixel(x,y+2,color);
	dc.SetPixel(x,y+3,color);
	dc.SetPixel(x,y+4,color2);

	dc.SetPixel(x+1,y+3,color);
	dc.SetPixel(x+1,y+4,color);
	dc.SetPixel(x+1,y+5,color2);

	dc.SetPixel(x+2,y+4,color);
	dc.SetPixel(x+2,y+5,color);
	dc.SetPixel(x+2,y+6,color2);

	dc.SetPixel(x+3,y+3,color);
	dc.SetPixel(x+3,y+4,color);
	dc.SetPixel(x+3,y+5,color2);

	dc.SetPixel(x+4,y+2,color);
	dc.SetPixel(x+4,y+3,color);
	dc.SetPixel(x+4,y+4,color2);

	dc.SetPixel(x+5,y+1,color);
	dc.SetPixel(x+5,y+2,color);
	dc.SetPixel(x+5,y+3,color2);

	dc.SetPixel(x+6,y,color);
	dc.SetPixel(x+6,y+1,color);
	dc.SetPixel(x+6,y+2,color2);
}

void CExtPaintManager::stat_PaintGradientRect2steps(
	CDC & dc,
	const CRect & rcPaintGradient,
	COLORREF clrLeft,
	COLORREF clrMiddle,
	COLORREF clrRight,
	bool bHorz, // = false
	INT nNumerator, // = 2 
	INT nDenominator, // = 5
	UINT nCountOfSteps // = 256
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rcPaintGradient1( rcPaintGradient );
CRect rcPaintGradient2( rcPaintGradient );

	if( bHorz )
	{
		rcPaintGradient1.bottom = 
			rcPaintGradient1.top + ::MulDiv( rcPaintGradient.Height(), nNumerator, nDenominator);
		rcPaintGradient2.top = rcPaintGradient1.bottom;
	}
	else
	{
		rcPaintGradient1.right = 
			rcPaintGradient1.left + ::MulDiv( rcPaintGradient.Width(), nNumerator, nDenominator);
		rcPaintGradient2.left = rcPaintGradient1.right;
	}
	
	stat_PaintGradientRect( 
		dc, 
		rcPaintGradient1, 
		!bHorz	? clrLeft : clrMiddle, 
		bHorz	? clrLeft : clrMiddle, 
		bHorz,
		nCountOfSteps
 		);

	stat_PaintGradientRect( 
		dc, 
		rcPaintGradient2, 
		bHorz	? clrRight : clrMiddle, 
		!bHorz	? clrRight : clrMiddle, 
		bHorz,
		nCountOfSteps
 		);
}

HICON CExtPaintManager::stat_GenerateColorIconBox(
	COLORREF clrIcon
	)
{
CWindowDC dc( NULL );

ICONINFO iiNewIcon;
	iiNewIcon.fIcon = true;
HDC hIconDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
HDC hMaskDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
	ASSERT(
		hIconDC != NULL && hMaskDC != NULL
		);
	iiNewIcon.hbmColor =
		::CreateCompatibleBitmap(
			dc.GetSafeHdc(),
			16,
			16
			);
	ASSERT( iiNewIcon.hbmColor != NULL );
	iiNewIcon.hbmMask =
		::CreateBitmap(
			16,
			16,
			1,
			1,
			NULL
			);
	ASSERT( iiNewIcon.hbmMask != NULL );
HGDIOBJ hOldIconDC =
		::SelectObject(
			hIconDC,
			iiNewIcon.hbmColor
			);
	ASSERT( hOldIconDC != NULL );
HGDIOBJ hOldMaskDC =
		::SelectObject(
			hMaskDC,
			iiNewIcon.hbmMask
			);
	ASSERT( hOldMaskDC != NULL );
HBRUSH hStockWhiteBrush = (HBRUSH)
		::GetStockObject( WHITE_BRUSH );
HBRUSH hStockBlackBrush = (HBRUSH)
		::GetStockObject( BLACK_BRUSH );
CBrush brColorItem( clrIcon );
CRect rcColorItem(0,0,16,16);
	::FillRect(
		hMaskDC,
		&rcColorItem,
		hStockWhiteBrush
		);
	::FillRect(
		hIconDC,
		&rcColorItem,
		hStockBlackBrush
		);
// 	rcColorItem.DeflateRect( 1, 2 );
	::FillRect(
		hMaskDC,
		&rcColorItem,
		hStockBlackBrush
		);
	::FillRect(
		hIconDC,
		&rcColorItem,
		(HBRUSH)brColorItem.GetSafeHandle()
		);
// 	rcColorItem.DeflateRect( 2, 2 );
// 	::FillRect(
// 		hMaskDC,
// 		&rcColorItem,
// 		hStockWhiteBrush
// 		);
// 	::FillRect(
// 		hIconDC,
// 		&rcColorItem,
// 		hStockBlackBrush
// 		);
	::SelectObject(hIconDC,hOldIconDC);
	::SelectObject(hMaskDC,hOldMaskDC);
HICON hIcon = ::CreateIconIndirect(&iiNewIcon);
	ASSERT( hIcon != NULL );
	::DeleteObject(iiNewIcon.hbmColor);
	::DeleteObject(iiNewIcon.hbmMask);
	::DeleteDC(hMaskDC);
	::DeleteDC(hIconDC);

	return hIcon;

/*	
CWindowDC dc( NULL );

ICONINFO iiNewIcon;
	::memset( &iiNewIcon, 0, sizeof(ICONINFO) );
	iiNewIcon.fIcon = true;
HDC hIconDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
HDC hMaskDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
	ASSERT(
		hIconDC != NULL && hMaskDC != NULL
		);
	iiNewIcon.hbmColor =
		::CreateCompatibleBitmap(
			dc.GetSafeHdc(),
			16,
			16
			);
	ASSERT( iiNewIcon.hbmColor != NULL );
	iiNewIcon.hbmMask =
		::CreateBitmap(
			16,
			16,
			1,
			1,
			NULL
			);
	ASSERT( iiNewIcon.hbmMask != NULL );
HGDIOBJ hOldIconDC =
		::SelectObject(
			hIconDC,
			iiNewIcon.hbmColor
			);
	ASSERT( hOldIconDC != NULL );
HGDIOBJ hOldMaskDC =
		::SelectObject(
			hMaskDC,
			iiNewIcon.hbmMask
			);
	ASSERT( hOldMaskDC != NULL );

CRect rcColorItem(0,0,16,16);
CRect rcColorBox(0,1,16,15);
	::FillRect(
		hMaskDC,
		&rcColorItem,
		(HBRUSH)::GetStockObject( WHITE_BRUSH )
		);
	::FillRect(
		hMaskDC,
		&rcColorBox,
		(HBRUSH)::GetStockObject( BLACK_BRUSH )
		);
CBrush brColorItem( clrIcon );
	::FillRect(
		hIconDC,
		&rcColorBox,
		(HBRUSH)::GetStockObject( BLACK_BRUSH )
		);
	rcColorBox.DeflateRect( 1, 1, 1, 1 );
	::FillRect(
		hIconDC,
		&rcColorBox,
		(HBRUSH)brColorItem.GetSafeHandle()
		);

	::SelectObject(hIconDC,hOldIconDC);
	::SelectObject(hMaskDC,hOldMaskDC);
HICON hIcon = ::CreateIconIndirect(&iiNewIcon);
	ASSERT( hIcon != NULL );
	::DeleteObject(iiNewIcon.hbmColor);
	::DeleteObject(iiNewIcon.hbmMask);
	::DeleteDC(hMaskDC);
	::DeleteDC(hIconDC);

	return hIcon;
*/
}

HICON CExtPaintManager::stat_GenerateColorIconFrame(
	COLORREF clrIcon
	)
{
CWindowDC dc( NULL );

ICONINFO iiNewIcon;
	iiNewIcon.fIcon = true;
HDC hIconDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
HDC hMaskDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
	ASSERT(
		hIconDC != NULL && hMaskDC != NULL
		);
	iiNewIcon.hbmColor =
		::CreateCompatibleBitmap(
			dc.GetSafeHdc(),
			16,
			16
			);
	ASSERT( iiNewIcon.hbmColor != NULL );
	iiNewIcon.hbmMask =
		::CreateBitmap(
			16,
			16,
			1,
			1,
			NULL
			);
	ASSERT( iiNewIcon.hbmMask != NULL );
HGDIOBJ hOldIconDC =
		::SelectObject(
			hIconDC,
			iiNewIcon.hbmColor
			);
	ASSERT( hOldIconDC != NULL );
HGDIOBJ hOldMaskDC =
		::SelectObject(
			hMaskDC,
			iiNewIcon.hbmMask
			);
	ASSERT( hOldMaskDC != NULL );
HBRUSH hStockWhiteBrush = (HBRUSH)
		::GetStockObject( WHITE_BRUSH );
HBRUSH hStockBlackBrush = (HBRUSH)
		::GetStockObject( BLACK_BRUSH );
CBrush brColorItem( clrIcon );
CRect rcColorItem(0,0,16,16);
	::FillRect(
		hMaskDC,
		&rcColorItem,
		hStockWhiteBrush
		);
	::FillRect(
		hIconDC,
		&rcColorItem,
		hStockBlackBrush
		);
	rcColorItem.DeflateRect( 1, 2 );
	::FillRect(
		hMaskDC,
		&rcColorItem,
		hStockBlackBrush
		);
	::FillRect(
		hIconDC,
		&rcColorItem,
		(HBRUSH)brColorItem.GetSafeHandle()
		);
	rcColorItem.DeflateRect( 2, 2 );
	::FillRect(
		hMaskDC,
		&rcColorItem,
		hStockWhiteBrush
		);
	::FillRect(
		hIconDC,
		&rcColorItem,
		hStockBlackBrush
		);
	::SelectObject(hIconDC,hOldIconDC);
	::SelectObject(hMaskDC,hOldMaskDC);
HICON hIcon = ::CreateIconIndirect(&iiNewIcon);
	ASSERT( hIcon != NULL );
	::DeleteObject(iiNewIcon.hbmColor);
	::DeleteObject(iiNewIcon.hbmMask);
	::DeleteDC(hMaskDC);
	::DeleteDC(hIconDC);

	return hIcon;
}

HICON CExtPaintManager::stat_GenerateColorIconChar(
	COLORREF clrIcon,
	HFONT hFontForLetter,
	__EXT_MFC_SAFE_TCHAR tChr
	)
{
	ASSERT( tChr != _T('\0') );
	ASSERT( hFontForLetter != NULL );
CWindowDC dc( NULL );
ICONINFO iiNewIcon;
	::memset( &iiNewIcon, 0, sizeof(ICONINFO) );
	iiNewIcon.fIcon = true;
HDC hIconDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
HDC hMaskDC = ::CreateCompatibleDC( dc.GetSafeHdc() );
	ASSERT(
		hIconDC != NULL && hMaskDC != NULL
		);
	iiNewIcon.hbmColor =
		::CreateCompatibleBitmap(
			dc.GetSafeHdc(),
			16,
			16
			);
	ASSERT( iiNewIcon.hbmColor != NULL );
	iiNewIcon.hbmMask =
		::CreateBitmap(
			16,
			16,
			1,
			1,
			NULL
			);
	ASSERT( iiNewIcon.hbmMask != NULL );
HGDIOBJ hOldIconDC =
		::SelectObject(
			hIconDC,
			iiNewIcon.hbmColor
			);
	ASSERT( hOldIconDC != NULL );
HGDIOBJ hOldMaskDC =
		::SelectObject(
			hMaskDC,
			iiNewIcon.hbmMask
			);
	ASSERT( hOldMaskDC != NULL );

HBRUSH hStockWhiteBrush = (HBRUSH)
		::GetStockObject( WHITE_BRUSH );
HBRUSH hStockBlackBrush = (HBRUSH)
		::GetStockObject( BLACK_BRUSH );
CRect rcColorItem(0,0,16,16);
	::FillRect(
		hMaskDC,
		&rcColorItem,
		hStockWhiteBrush
		);
	::FillRect(
		hIconDC,
		&rcColorItem,
		hStockBlackBrush
		);

	ASSERT( hFontForLetter != NULL );
HGDIOBJ hOldFontIconDC =
		::SelectObject( hIconDC, hFontForLetter );
HGDIOBJ hOldFontMaskDC =
		::SelectObject( hMaskDC, hFontForLetter );

CRect rcText( rcColorItem );
	rcText.bottom -= 4;
CExtSafeString sText( (TCHAR)tChr, 1 );

	::SetTextColor( hIconDC, RGB(0,0,0) );
	::SetBkMode( hIconDC, TRANSPARENT );
	CExtRichContentLayout::stat_DrawText(
		hIconDC,
		LPCTSTR(sText), sText.GetLength(),
		&rcText,
		DT_SINGLELINE|DT_CENTER|DT_VCENTER, 0
		);
	
	::SetTextColor( hMaskDC, RGB(0,0,0) );
	::SetBkMode( hMaskDC, TRANSPARENT );
	CExtRichContentLayout::stat_DrawText(
		hMaskDC,
		LPCTSTR(sText), sText.GetLength(),
		&rcText,
		DT_SINGLELINE|DT_CENTER|DT_VCENTER, 0
		);

	::SelectObject( hIconDC, hOldFontIconDC );
	::SelectObject( hMaskDC, hOldFontMaskDC );

CRect rcColorBar( rcText );
	rcColorBar.OffsetRect( 0, rcColorBar.Height() + 1 );
	rcColorBar.bottom = rcColorBar.top + 3;
	rcColorBar.DeflateRect( 2, 0 );

CBrush brColor( clrIcon );
	::FillRect(
		hIconDC,
		&rcColorBar,
		(HBRUSH)brColor.GetSafeHandle()
		);
	::FillRect(
		hMaskDC,
		&rcColorBar,
		hStockBlackBrush
		);

	::SelectObject(hIconDC,hOldIconDC);
	::SelectObject(hMaskDC,hOldMaskDC);
HICON hIcon = ::CreateIconIndirect(&iiNewIcon);
	ASSERT( hIcon != NULL );
	::DeleteObject(iiNewIcon.hbmColor);
	::DeleteObject(iiNewIcon.hbmMask);
	::DeleteDC(hMaskDC);
	::DeleteDC(hIconDC);
	return hIcon;

}

HICON CExtPaintManager::stat_GenerateColorIconChar(
	COLORREF clrIcon,
	HFONT hFontForLetter
	)
{
	return stat_GenerateColorIconChar( clrIcon, hFontForLetter, _T('A') );
}

#if (defined __EXT_PM_BACK_COMPATIBILITY_BITMAP_API)

HBITMAP CExtPaintManager::stat_ColorizeMonochromeBitmap(
	HBITMAP hBitmapSrc,
	COLORREF clrTransparent,
	COLORREF clrTarget
	)
{
	if( hBitmapSrc == NULL )
		return NULL;
HDC hDstDC =
		::CreateCompatibleDC( NULL );
	if( hDstDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	} // if( hDstDC == NULL )
BITMAP _bmpInfoSrcBmp;
	::memset(
		&_bmpInfoSrcBmp,
		0,
		sizeof(BITMAP)
		);
	::GetObject(
		hBitmapSrc,
		sizeof(BITMAP),
		&_bmpInfoSrcBmp
		);
BITMAPINFOHEADER _bmpInfoHdr =
{
	sizeof(BITMAPINFOHEADER),
	_bmpInfoSrcBmp.bmWidth,
	_bmpInfoSrcBmp.bmHeight,
	1,
	32,
	BI_RGB,
	0,
	0,
	0,
	0,
	0
};
LPVOID p32bitColorSurface = NULL; 
HBITMAP hDstDIB =
		::CreateDIBSection(
			hDstDC,
			(BITMAPINFO *)&_bmpInfoHdr,
			DIB_RGB_COLORS,
			&p32bitColorSurface,
			NULL,
			0
			);
	if( hDstDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDstDC );
		return NULL;
	} // if( hDstDIB == NULL )
	ASSERT( p32bitColorSurface != NULL );
HBITMAP hOldDst = (HBITMAP)
		::SelectObject( hDstDC, hDstDIB );
HDC hSrcDC =
		::CreateCompatibleDC( hDstDC );
	if( hSrcDC == NULL )
	{
		ASSERT( FALSE );
		::DeleteObject(
			::SelectObject(
				hDstDC,
				hOldDst
				)
			);
		::DeleteDC( hDstDC );
		return NULL;
	} // if( hSrcDC == NULL )
BITMAP _bmpInfoDstDIB;
	::memset(
		&_bmpInfoDstDIB,
		0,
		sizeof(BITMAP)
		);
	::GetObject(
		hDstDIB,
		sizeof(BITMAP),
		&_bmpInfoDstDIB
		);
	for( ; (_bmpInfoDstDIB.bmWidthBytes % 4) != 0; _bmpInfoDstDIB.bmWidthBytes ++ );
HBITMAP hOldSrc = (HBITMAP)
		::SelectObject(
			hSrcDC,
			hBitmapSrc
			);
	::BitBlt(
		hDstDC,
		0,
		0,
		_bmpInfoSrcBmp.bmWidth,
		_bmpInfoSrcBmp.bmHeight,
		hSrcDC,
		0,
		0,
		SRCCOPY
		);

double dHTarget = 0.0, dSTarget = 0.0, dLTarget = 0.0;
	CExtBitmap::stat_RGBtoHSL( clrTarget, &dHTarget, &dSTarget, &dLTarget );

BYTE * pDstColorSurface =
		(BYTE *)_bmpInfoDstDIB.bmBits
		+ (_bmpInfoDstDIB.bmHeight - 1) * _bmpInfoDstDIB.bmWidthBytes;
	for( int nY = 0; nY < _bmpInfoSrcBmp.bmHeight; nY++ )
	{
		for( int nX = 0; nX < _bmpInfoSrcBmp.bmWidth; nX++ )
		{
			LPLONG ptrLinePart = (LPLONG)pDstColorSurface + nX;
			COLORREF clr = (COLORREF)(*ptrLinePart);

			if( clr != clrTransparent )
			{
				double dHPixel = 0.0, dSPixel = 0.0, dLPixel = 0.0;
				CExtBitmap::stat_RGBtoHSL( clr, &dHPixel, &dSPixel, &dLPixel );
				clr = CExtBitmap::stat_HLStoRGB( dHTarget, dLPixel, dSTarget );
			}
			
			INT nR = GetBValue( clr );
			INT nG = GetGValue( clr );
			INT nB = GetRValue( clr );
			(*ptrLinePart) = RGB( nR, nG, nB );

		} // for( int nX = 0; nX < _bmpInfoSrcBmp.bmWidth; nX++ )
		pDstColorSurface -= _bmpInfoDstDIB.bmWidthBytes;
	} // for( int nY = 0; nY < _bmpInfoSrcBmp.bmHeight; nY++ )
	::SelectObject(
		hSrcDC,
		hOldSrc
		);
	::DeleteDC( hSrcDC );
	::SelectObject(
		hDstDC,
		hOldDst
		);
	::DeleteDC( hDstDC );
	return hDstDIB;
}

HBITMAP CExtPaintManager::stat_HLS_Adjust_Bitmap(
	HBITMAP hBitmapSrc,
	COLORREF clrTransparent, // = RGB(0,0,0)
	COLORREF clrTolerance, // = RGB(0,0,0)
	double percentH, // = 0.0
	double percentL, // = 0.0
	double percentS // = 0.0
	)
{
	if( hBitmapSrc == NULL )
		return NULL;
	if(		percentH == 0.0
		&&	percentL == 0.0
		&&	percentS == 0.0
		)
		return NULL;
HDC hDstDC =
		::CreateCompatibleDC( NULL );
	if( hDstDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	} // if( hDstDC == NULL )
BITMAP _bmpInfoSrcBmp;
	::memset(
		&_bmpInfoSrcBmp,
		0,
		sizeof(BITMAP)
		);
	::GetObject(
		hBitmapSrc,
		sizeof(BITMAP),
		&_bmpInfoSrcBmp
		);
BITMAPINFOHEADER _bmpInfoHdr =
{
	sizeof(BITMAPINFOHEADER),
	_bmpInfoSrcBmp.bmWidth,
	_bmpInfoSrcBmp.bmHeight,
	1,
	32,
	BI_RGB,
	0,
	0,
	0,
	0,
	0
};
LPVOID p32bitColorSurface = NULL; 
HBITMAP hDstDIB =
		::CreateDIBSection(
			hDstDC,
			(BITMAPINFO *)&_bmpInfoHdr,
			DIB_RGB_COLORS,
			&p32bitColorSurface,
			NULL,
			0
			);
	if( hDstDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDstDC );
		return NULL;
	} // if( hDstDIB == NULL )
	ASSERT( p32bitColorSurface != NULL );
HBITMAP hOldDst = (HBITMAP)
		::SelectObject( hDstDC, hDstDIB );
HDC hSrcDC =
		::CreateCompatibleDC( hDstDC );
	if( hSrcDC == NULL )
	{
		ASSERT( FALSE );
		::DeleteObject(
			::SelectObject(
				hDstDC,
				hOldDst
				)
			);
		::DeleteDC( hDstDC );
		return NULL;
	} // if( hSrcDC == NULL )
BITMAP _bmpInfoDstDIB;
	::memset(
		&_bmpInfoDstDIB,
		0,
		sizeof(BITMAP)
		);
	::GetObject(
		hDstDIB,
		sizeof(BITMAP),
		&_bmpInfoDstDIB
		);
	for( ; (_bmpInfoDstDIB.bmWidthBytes % 4) != 0; _bmpInfoDstDIB.bmWidthBytes ++ );
HBITMAP hOldSrc = (HBITMAP)
		::SelectObject(
			hSrcDC,
			hBitmapSrc
			);
	::BitBlt(
		hDstDC,
		0,
		0,
		_bmpInfoSrcBmp.bmWidth,
		_bmpInfoSrcBmp.bmHeight,
		hSrcDC,
		0,
		0,
		SRCCOPY
		);
BYTE nLowRed	= GetRValue( clrTransparent );
BYTE nLowGreen	= GetGValue( clrTransparent );
BYTE nLowBlue	= GetBValue( clrTransparent );
BYTE nHighRed	= BYTE( min( 0xff, ( nLowRed + GetRValue(clrTolerance) ) ) );
BYTE nHighGreen	= BYTE( min( 0xff, ( nLowGreen + GetGValue(clrTolerance) ) ) );
BYTE nHighBlue	= BYTE( min( 0xff, ( nLowBlue + GetBValue(clrTolerance) ) ) );
BYTE * pDstColorSurface =
		(BYTE *)_bmpInfoDstDIB.bmBits
		+ (_bmpInfoDstDIB.bmHeight - 1) * _bmpInfoDstDIB.bmWidthBytes;
	for( int nY = 0; nY < _bmpInfoSrcBmp.bmHeight; nY++ )
	{
		for( int nX = 0; nX < _bmpInfoSrcBmp.bmWidth; nX++ )
		{
			LPLONG ptrLinePart = (LPLONG)pDstColorSurface + nX;

			BYTE nColorPart = GetRValue( (*ptrLinePart) );
			if( nColorPart >= nLowRed && nColorPart <= nHighRed )
			{
				nColorPart = GetGValue( (*ptrLinePart) );
				if( nColorPart >= nLowGreen && nColorPart <= nHighGreen )
				{
					nColorPart = GetBValue( (*ptrLinePart) );
					if( nColorPart >= nLowBlue && nColorPart <= nHighBlue )
						continue;
				} // if( nColorPart >= nLowGreen && nColorPart <= nHighGreen )
			} // if( nColorPart >= nLowRed && nColorPart <= nHighRed )
			(*ptrLinePart) =
				CExtBitmap::stat_HLS_Adjust(
					(*ptrLinePart),
					percentH,
					percentL,
					percentS
					);
		} // for( int nX = 0; nX < _bmpInfoSrcBmp.bmWidth; nX++ )
		pDstColorSurface -= _bmpInfoDstDIB.bmWidthBytes;
	} // for( int nY = 0; nY < _bmpInfoSrcBmp.bmHeight; nY++ )
	::SelectObject(
		hSrcDC,
		hOldSrc
		);
	::DeleteDC( hSrcDC );
	::SelectObject(
		hDstDC,
		hOldDst
		);
	::DeleteDC( hDstDC );
	return hDstDIB;
}

bool CExtPaintManager::stat_HLS_Adjust_Bitmap(
	CBitmap & _bmp,
	COLORREF clrTransparent, // = RGB(0,0,0)
	COLORREF clrTolerance, // = RGB(0,0,0)
	double percentH, // = 0.0
	double percentL, // = 0.0
	double percentS // = 0.0
	)
{
HBITMAP hBitmapSrc = (HBITMAP)_bmp.GetSafeHandle();
	if( hBitmapSrc == NULL )
		return false;
	if(		percentH == 0.0
		&&	percentL == 0.0
		&&	percentS == 0.0
		)
		return true;
HBITMAP hBitmapDst =
		CExtPaintManager::stat_HLS_Adjust_Bitmap(
			hBitmapSrc,
			clrTransparent,
			clrTolerance,
			percentH,
			percentL,
			percentS
			);
	if( hBitmapDst == NULL )
		return false;
	_bmp.Detach();
	_bmp.Attach( hBitmapDst );
	::DeleteObject( hBitmapSrc );
	return true;
}

// based on the source code by Jean-Edouard Lachand-Robert
HRGN CExtPaintManager::stat_HBITMAPtoHRGN(
	HBITMAP hBitmap,
	COLORREF clrTransparent,
	COLORREF clrTolerance // = RGB(0,0,0)
	)
{
	if( hBitmap == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	} // if( hBitmap == NULL )
HDC hDstDC = ::CreateCompatibleDC( NULL );
	if( hDstDC == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	} // if( hDstDC == NULL )
BITMAP _bmpInfoSrcBmp;
	::memset( &_bmpInfoSrcBmp, 0, sizeof(BITMAP) );
	::GetObject( hBitmap, sizeof(BITMAP), &_bmpInfoSrcBmp );
BITMAPINFOHEADER _bmpInfoHdr =
{
	sizeof(BITMAPINFOHEADER),
	_bmpInfoSrcBmp.bmWidth,
	_bmpInfoSrcBmp.bmHeight,
	1,
	32,
	BI_RGB,
	0,
	0,
	0,
	0,
	0
};
LPVOID p32bitColorSurface = NULL; 
HBITMAP hDstDIB = ::CreateDIBSection( hDstDC, (BITMAPINFO *)&_bmpInfoHdr, DIB_RGB_COLORS, &p32bitColorSurface, NULL, 0 );
	if( hDstDIB == NULL )
	{
		ASSERT( FALSE );
		::DeleteDC( hDstDC );
		return NULL;
	} // if( hDstDIB == NULL )
	ASSERT( p32bitColorSurface != NULL );
HBITMAP hOldDst = (HBITMAP) ::SelectObject( hDstDC, hDstDIB );
HDC hSrcDC = ::CreateCompatibleDC( hDstDC );
	if( hSrcDC == NULL )
	{
		ASSERT( FALSE );
		::DeleteObject( ::SelectObject( hDstDC, hOldDst ) );
		::DeleteDC( hDstDC );
		return NULL;
	} // if( hSrcDC == NULL )
BITMAP _bmpInfoDstDIB;
	::memset( &_bmpInfoDstDIB, 0, sizeof(BITMAP) );
	::GetObject( hDstDIB, sizeof(BITMAP), &_bmpInfoDstDIB );
	for( ; (_bmpInfoDstDIB.bmWidthBytes % 4) != 0; _bmpInfoDstDIB.bmWidthBytes ++ );
HBITMAP hOldSrc = (HBITMAP) ::SelectObject( hSrcDC, hBitmap );
	::BitBlt( hDstDC, 0, 0, _bmpInfoSrcBmp.bmWidth, _bmpInfoSrcBmp.bmHeight, hSrcDC, 0, 0, SRCCOPY );
static const DWORD g_nReallocSize = 200;
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
BYTE * pDstColorSurface = (BYTE *)_bmpInfoDstDIB.bmBits + (_bmpInfoDstDIB.bmHeight - 1) * _bmpInfoDstDIB.bmWidthBytes;
HRGN hRgnResult = NULL;
	for( int nY = 0; nY < _bmpInfoSrcBmp.bmHeight; nY++ )
	{
		for( int nX = 0; nX < _bmpInfoSrcBmp.bmWidth; nX++ )
		{
			LPLONG ptrLinePart = (LPLONG)pDstColorSurface + nX;
			int nSavedX = nX;
			for( ; nX < _bmpInfoSrcBmp.bmWidth; ptrLinePart++, nX++ )
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
			} // for( ; nX < _bmpInfoSrcBmp.bmWidth; ptrLinePart++, nX++ )

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
				if( pRgnData->rdh.nCount == 2000 )
				{
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
				} // if( pRgnData->rdh.nCount == 2000 )
			} // if( nX > nSavedX )
		} // for( int nX = 0; nX < _bmpInfoSrcBmp.bmWidth; nX++ )
		pDstColorSurface -= _bmpInfoDstDIB.bmWidthBytes;
	} // for( int nY = 0; nY < _bmpInfoSrcBmp.bmHeight; nY++ )
HRGN hRgnPart =
		::ExtCreateRegion( NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxParts), pRgnData );
	if( hRgnResult != NULL )
	{
		::CombineRgn( hRgnResult, hRgnResult, hRgnPart, RGN_OR );
		::DeleteObject( hRgnPart );
	} // if( hRgnResult != NULL )
	else
		hRgnResult = hRgnPart;
	::GlobalFree( hGlobal );
	::SelectObject( hSrcDC, hOldSrc );
	::DeleteDC( hSrcDC );
	::DeleteObject( ::SelectObject( hDstDC, hOldDst ) );
	::DeleteDC( hDstDC );
	return hRgnResult;
}

#endif // __EXT_PM_BACK_COMPATIBILITY_BITMAP_API

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)

CExtPaintManager::DockMarkerBase * CExtPaintManager::DockMarker_CreateObject(
	int eResizablePanelDockingType
	)
{
	ASSERT_VALID( this );
	if( eResizablePanelDockingType == INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2008_XP) )
		return (new DockMarker2008XP( this, eResizablePanelDockingType ) );
	if( eResizablePanelDockingType == INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2008_VISTA) )
		return (new DockMarker2008Vista( this, eResizablePanelDockingType ) );
	if( eResizablePanelDockingType == INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2010) )
		return (new DockMarker2010( this, eResizablePanelDockingType ) );
	switch( m_eStyle2005 )
	{
	case __ES2005_BETA1:
		return (new DockMarkerBase( this, eResizablePanelDockingType ) );
	case __ES2005_BETA2:
	case __ES2005_RC:
		return (new DockMarkerBeta2( this, eResizablePanelDockingType ) );
	default:
		{
			ASSERT( FALSE );
			AfxThrowNotSupportedException();
		}
#if _MFC_VER < 0x710
		return NULL;
#endif // #if _MFC_VER < 0x710
	} // switch( m_eStyle2005 )
}

CExtPaintManager::DockMarkerBase::DockMarkerBase(
	CExtPaintManager * pPM,
	int eResizablePanelDockingType,
	eDockMarkerType_t eDockMarkerType, // = __EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
	: m_eDockMarkerType( __EDMT_EMPTY )
	, m_size( 0, 0 )
	, m_rcScreen( 0, 0, 0, 0 )
	, m_bSparsedMode( false )
	, m_ptOffsetInner( 0, 0 )
	, m_pPM( pPM )
	, m_eResizablePanelDockingType( eResizablePanelDockingType )
{
	ASSERT_VALID( pPM );
	ASSERT(
			INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_MIN_VALUE) <= m_eResizablePanelDockingType
		&&	m_eResizablePanelDockingType <= INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_MAX_VALUE)
		);
INT nBPP = CExtPaintManager::stat_GetBPP();
	if( m_eResizablePanelDockingType == INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2008_AUTO) )
	{
		m_eResizablePanelDockingType = INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2008_XP);
		if(		g_PaintManager.m_bIsWinVistaOrLater
			&&	g_PaintManager.m_DWM.IsCompositionEnabled()
			)
			m_eResizablePanelDockingType = INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2008_VISTA);
	}
	if(		m_eResizablePanelDockingType == INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2008_XP)
		&&	(!(
				g_PaintManager.m_bIsWin2000orLater
			&&	nBPP >= 24
			))
		)
		m_eResizablePanelDockingType = INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2005);
	if(		nBPP >= 24 )
		m_eResizablePanelDockingType = INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2010);
	if( eDockMarkerType != __EDMT_EMPTY )
		{ VERIFY( Create( false, eDockMarkerType, rcScreen ) ); }
}

CExtPaintManager::DockMarkerBeta2::DockMarkerBeta2(
	CExtPaintManager * pPM,
	int eResizablePanelDockingType,
	eDockMarkerType_t eDockMarkerType, // = __EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
	: CExtPaintManager::DockMarkerBase( pPM, eResizablePanelDockingType )
{
	if( eDockMarkerType != __EDMT_EMPTY )
		{ VERIFY( Create( false, eDockMarkerType, rcScreen ) ); }
}

CExtPaintManager::DockMarker2008XP::DockMarker2008XP(
	CExtPaintManager * pPM,
	int eResizablePanelDockingType,
	eDockMarkerType_t eDockMarkerType, // = __EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
	: CExtPaintManager::DockMarkerBase( pPM, eResizablePanelDockingType )
{
	if( eDockMarkerType != __EDMT_EMPTY )
		{ VERIFY( Create( false, eDockMarkerType, rcScreen ) ); }
}

CExtPaintManager::DockMarker2008Vista::DockMarker2008Vista(
	CExtPaintManager * pPM,
	int eResizablePanelDockingType,
	eDockMarkerType_t eDockMarkerType, // = __EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
	: CExtPaintManager::DockMarkerBase( pPM, eResizablePanelDockingType )
{
	if( eDockMarkerType != __EDMT_EMPTY )
		{ VERIFY( Create( false, eDockMarkerType, rcScreen ) ); }
}

CExtPaintManager::DockMarker2010::DockMarker2010(
	CExtPaintManager * pPM,
	int eResizablePanelDockingType,
	eDockMarkerType_t eDockMarkerType, // = __EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
	: CExtPaintManager::DockMarkerBase( pPM, eResizablePanelDockingType )
{
	if( eDockMarkerType != __EDMT_EMPTY )
		{ VERIFY( Create( false, eDockMarkerType, rcScreen ) ); }
}

CExtPaintManager::DockMarkerBase::~DockMarkerBase()
{
	Empty();
}

CExtPaintManager::DockMarkerBeta2::~DockMarkerBeta2()
{
	Empty();
}

CExtPaintManager::DockMarker2008XP::~DockMarker2008XP()
{
	Empty();
}

CExtPaintManager::DockMarker2008Vista::~DockMarker2008Vista()
{
	Empty();
}

CExtPaintManager::DockMarker2010::~DockMarker2010()
{
	Empty();
}

bool CExtPaintManager::DockMarkerBase::IsEmpty() const
{
	if( m_eDockMarkerType == __EDMT_EMPTY )
	{
		ASSERT( m_bmpIn.IsEmpty() );
		ASSERT( m_bmpOut.IsEmpty() );
		ASSERT( m_rgn.GetSafeHandle() == NULL );
		ASSERT( m_size.cx == 0 );
		ASSERT( m_size.cy == 0 );
		ASSERT( m_arrChildren.GetSize() == 0 );
		return true;
	} // if( m_rgn.GetSafeHandle() == NULL )
	return false;
}

bool CExtPaintManager::DockMarkerBase::CreateFromBitmapResources(
	CRect rcScreen,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	COLORREF clrTransparentIn,
	UINT nResourceBmpIn,
	UINT nResourceBmpOut,
	__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpIn, // = RT_BITMAP // default bitmap section
	__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpOut, // = RT_BITMAP // default bitmap section
	HINSTANCE hInstBmpIn, // = NULL // find it automatically
	HINSTANCE hInstBmpOut // = NULL // find it automatically
	)
{
	return
		CreateFromBitmapResources(
			rcScreen,
			eDockMarkerType,
			clrTransparentIn,
			__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceBmpIn) ),
			__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE(nResourceBmpOut) ),
			strResourceTypeBmpIn,
			strResourceTypeBmpOut,
			hInstBmpIn,
			hInstBmpOut
			);
}

bool CExtPaintManager::DockMarkerBase::CreateFromBitmapResources(
	CRect rcScreen,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	COLORREF clrTransparentIn,
	__EXT_MFC_SAFE_LPCTSTR strResourceBmpIn,
	__EXT_MFC_SAFE_LPCTSTR strResourceBmpOut, // = NULL
	__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpIn, // = RT_BITMAP // default bitmap section
	__EXT_MFC_SAFE_LPCTSTR strResourceTypeBmpOut, // = RT_BITMAP // default bitmap section
	HINSTANCE hInstBmpIn, // = NULL // find it automatically
	HINSTANCE hInstBmpOut // = NULL // find it automatically
	)
{
	ASSERT( IsEmpty() );
	ASSERT( eDockMarkerType != __EDMT_EMPTY );
	ASSERT( strResourceBmpIn != NULL );
	if( strResourceBmpOut == NULL )
	{
		 strResourceBmpOut = strResourceBmpIn;
		 strResourceTypeBmpOut = strResourceTypeBmpIn;
		 hInstBmpOut = hInstBmpIn;
	} // if( strResourceBmpOut == NULL )
CExtBitmap _bmpIn;
	if( ! _bmpIn.LoadBMP_Resource(
			strResourceBmpIn,
			strResourceTypeBmpIn,
			hInstBmpIn
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CExtBitmap _bmpOut;
	if( ! _bmpOut.LoadBMP_Resource(
			strResourceBmpOut,
			strResourceTypeBmpOut,
			hInstBmpOut
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return
		CreateFromBitmaps(
			rcScreen,
			eDockMarkerType,
			clrTransparentIn,
			_bmpIn,
			_bmpOut
			);
}

bool CExtPaintManager::DockMarkerBase::CreateFromBitmapData(
	CRect rcScreen,
	eDockMarkerType_t eDockMarkerType,
	COLORREF clrTransparentIn,
	const BYTE * pDataBmpIn,
	DWORD dwSizeDataBmpIn,
	const BYTE * pDataBmpOut, // = NULL
	DWORD dwSizeDataBmpOut // = 0
	)
{
	ASSERT( IsEmpty() );
	ASSERT( eDockMarkerType != __EDMT_EMPTY );
	ASSERT(
		pDataBmpIn != NULL && dwSizeDataBmpIn > 0
		);
	ASSERT(
			( pDataBmpOut != NULL && dwSizeDataBmpOut > 0 )
		||	pDataBmpOut == NULL && dwSizeDataBmpOut == 0
		);
	if( pDataBmpOut == NULL )
	{
		pDataBmpOut = pDataBmpIn;
		dwSizeDataBmpOut = dwSizeDataBmpIn;
	} // if( pDataBmpOut == NULL )
CExtBitmap _bmpIn;
	if( ! _bmpIn.LoadBMP_Buffer(
			pDataBmpIn,
			dwSizeDataBmpIn
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CExtBitmap _bmpOut;
	if( ! _bmpOut.LoadBMP_Buffer(
			pDataBmpOut,
			dwSizeDataBmpOut
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return
		CreateFromBitmaps(
			rcScreen,
			eDockMarkerType,
			clrTransparentIn,
			_bmpIn,
			_bmpOut
			);
}

bool CExtPaintManager::DockMarkerBase::CreateFromBitmaps(
	CRect rcScreen,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	COLORREF clrTransparentIn,
	const CExtBitmap & _bmpIn,
	const CExtBitmap & _bmpOut
	)
{
	ASSERT( IsEmpty() );
	ASSERT( eDockMarkerType != __EDMT_EMPTY );
	ASSERT( ! _bmpIn.IsEmpty() );
	ASSERT( ! _bmpOut.IsEmpty() );
CSize _sizeBmpInfoIn = _bmpIn.GetSize(), _sizeBmpInfoOut = _bmpOut.GetSize();
	ASSERT( _sizeBmpInfoIn.cx > 0 && _sizeBmpInfoIn.cy > 0 );
	ASSERT( _sizeBmpInfoOut.cx > 0 && _sizeBmpInfoOut.cy > 0 );
	if(		_sizeBmpInfoIn.cx != _sizeBmpInfoOut.cx
		||	_sizeBmpInfoIn.cy != _sizeBmpInfoOut.cy
		)
	{
		ASSERT( FALSE );
		return false;
	}
// HRGN hRgn =
// 		CExtPaintManager::stat_HBITMAPtoHRGN(
// 		hBmpIn,
// 		clrTransparentIn
// 		);
HRGN hRgn =
		_bmpIn.GenerateColorHRGN( false, clrTransparentIn );
	if( hRgn == NULL )
	{
		ASSERT( FALSE );
		return false;
	} // if( hRgn == NULL )
	m_size.cx = _sizeBmpInfoIn.cx;
	m_size.cy = _sizeBmpInfoIn.cy;
	m_bmpIn = _bmpIn;
	m_bmpOut = _bmpOut;
	m_rgn.Attach( hRgn );
	m_eDockMarkerType = eDockMarkerType;
	OnCalcScreenRect(
		eDockMarkerType,
		rcScreen
		);
	g_PaintManager->DockMarker_AdjustBmps(
		eDockMarkerType,
		m_bmpIn,
		clrTransparentIn,
		m_bmpOut,
		clrTransparentIn
		);
	return true;
}

void CExtPaintManager::DockMarkerBase::OnCalcScreenRect(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CRect rcScreen,
	DockMarkerBase * pCenterMarker // = NULL
	)
{
	pCenterMarker;
	m_rcScreen = rcScreen;
	switch( eDockMarkerType )
	{
	case __EDMT_2005_TOP:
		m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
		m_rcScreen.OffsetRect(
			( m_rcScreen.Width() - m_size.cx ) / 2,
			0
			);
		m_rcScreen.right = m_rcScreen.left + m_size.cx;
	break;
	case __EDMT_2005_BOTTOM:
		m_rcScreen.top = m_rcScreen.bottom - m_size.cy;
		m_rcScreen.OffsetRect(
			( m_rcScreen.Width() - m_size.cx ) / 2,
			0
			);
		m_rcScreen.right = m_rcScreen.left + m_size.cx;
	break;
	case __EDMT_2005_LEFT:
		m_rcScreen.right = m_rcScreen.left + m_size.cx;
		m_rcScreen.OffsetRect(
			0,
			( m_rcScreen.Height() - m_size.cy ) / 2
			);
		m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
	break;
	case __EDMT_2005_RIGHT:
		m_rcScreen.left = m_rcScreen.right - m_size.cx;
		m_rcScreen.OffsetRect(
			0,
			( m_rcScreen.Height() - m_size.cy ) / 2
			);
		m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
	break;
	case __EDMT_2005_CENTER:
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case __EDMT_2005_TAB:
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		m_rcScreen.OffsetRect(
			( m_rcScreen.Width() - m_size.cx ) / 2,
			( m_rcScreen.Height() - m_size.cy ) / 2
			);
		m_rcScreen.right = m_rcScreen.left + m_size.cx;
		m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eDockMarkerType )
}

void CExtPaintManager::DockMarkerBeta2::OnCalcScreenRect(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CRect rcScreen,
	DockMarkerBase * pCenterMarker // = NULL
	)
{
	DockMarkerBase::OnCalcScreenRect(
		eDockMarkerType,
		rcScreen,
		pCenterMarker
		);

	if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )
	{
		CRect rcOuterMargins = g_PaintManager->DockMarker_GetOuterDistance();
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_OUTER_BK_LEFT
						)
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_ptOffsetInner.x = 4;
					m_ptOffsetInner.y = ( m_size.cy - m_rcScreen.Height() ) / 2;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.top -= m_ptOffsetInner.y;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_ptOffsetInner.y ++;
					m_rcScreen.OffsetRect( rcOuterMargins.left, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(0,255,0) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_RIGHT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_OUTER_BK_RIGHT
						)
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_ptOffsetInner.x = 3;
					m_ptOffsetInner.y = ( m_size.cy - m_rcScreen.Height() ) / 2;
					m_rcScreen.left = m_rcScreen.right - m_size.cx;
					m_rcScreen.top -= m_ptOffsetInner.y;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_ptOffsetInner.y ++;
					m_rcScreen.OffsetRect( - rcOuterMargins.right, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(0,255,0) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_TOP:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_OUTER_BK_TOP
						)
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_ptOffsetInner.y = 4;
					m_ptOffsetInner.x = ( m_size.cx - m_rcScreen.Width() ) / 2;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.left -= m_ptOffsetInner.x;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_ptOffsetInner.x ++;
					m_rcScreen.OffsetRect( 0, rcOuterMargins.top );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(0,255,0) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_BOTTOM:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_OUTER_BK_BOTTOM
						)
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_ptOffsetInner.y = 3;
					m_ptOffsetInner.x = ( m_size.cx - m_rcScreen.Width() ) / 2;
					m_rcScreen.top = m_rcScreen.bottom - m_size.cy;
					m_rcScreen.left -= m_ptOffsetInner.x;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_ptOffsetInner.x ++;
					m_rcScreen.OffsetRect( 0, - rcOuterMargins.bottom );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(0,255,0) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		} // switch( pDM->m_eDockMarkerType )
	} // if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )

	if( pCenterMarker != NULL )
	{
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			m_rcScreen.OffsetRect( 10, 1 );
			break;
		case __EDMT_2005_RIGHT:
			m_rcScreen.OffsetRect( -8, 1 );
			break;
		case __EDMT_2005_TOP:
			m_rcScreen.OffsetRect( 1, 10 );
			break;
		case __EDMT_2005_BOTTOM:
			m_rcScreen.OffsetRect( 1, -8 );
			break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		case __EDMT_2005_TAB:
			m_rcScreen.OffsetRect( 1, 1 );
			break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		} // switch( pDM->m_eDockMarkerType )
	} // if( pCenterMarker != NULL )
	else
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		switch( eDockMarkerType )
		{
		case __EDMT_2005_TAB:
			m_rcScreen.OffsetRect( 1, 1 );
			break;
		} // switch( pDM->m_eDockMarkerType )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // else from if( pCenterMarker != NULL )
}

void CExtPaintManager::DockMarker2008XP::OnCalcScreenRect(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CRect rcScreen,
	DockMarkerBase * pCenterMarker // = NULL
	)
{
	DockMarkerBase::OnCalcScreenRect(
		eDockMarkerType,
		rcScreen,
		pCenterMarker
		);

	if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )
	{
		CRect rcOuterMargins = g_PaintManager->DockMarker_GetOuterDistance();
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_LEFT ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_LEFT ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.OffsetRect( rcOuterMargins.left, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_RIGHT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_RIGHT ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_RIGHT ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.left = m_rcScreen.right - m_size.cx;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.OffsetRect( - rcOuterMargins.right, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_TOP:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_TOP ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_TOP ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.OffsetRect( 0, rcOuterMargins.top );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_BOTTOM:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_BOTTOM ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_BOTTOM ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.top = m_rcScreen.bottom - m_size.cy;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.OffsetRect( 0, - rcOuterMargins.bottom );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		} // switch( pDM->m_eDockMarkerType )
	} // if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )

	if( pCenterMarker != NULL )
	{
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			m_rcScreen.OffsetRect( -18, 0 );
			break;
		case __EDMT_2005_RIGHT:
			m_rcScreen.OffsetRect( 18, 0 );
			break;
		case __EDMT_2005_TOP:
			m_rcScreen.OffsetRect( 0, -18 );
			break;
		case __EDMT_2005_BOTTOM:
			m_rcScreen.OffsetRect( 0, 18 );
			break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		case __EDMT_2005_TAB:
			break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		} // switch( pDM->m_eDockMarkerType )
	} // if( pCenterMarker != NULL )
	else
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		switch( eDockMarkerType )
		{
		case __EDMT_2005_TAB:
			break;
		} // switch( pDM->m_eDockMarkerType )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // else from if( pCenterMarker != NULL )
}

void CExtPaintManager::DockMarker2008Vista::OnCalcScreenRect(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CRect rcScreen,
	DockMarkerBase * pCenterMarker // = NULL
	)
{
	DockMarkerBase::OnCalcScreenRect(
		eDockMarkerType,
		rcScreen,
		pCenterMarker
		);

	if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )
	{
		CRect rcOuterMargins = g_PaintManager->DockMarker_GetOuterDistance();
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_LEFT ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_LEFT ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.OffsetRect( rcOuterMargins.left, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_RIGHT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_RIGHT ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_RIGHT ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.left = m_rcScreen.right - m_size.cx;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.OffsetRect( - rcOuterMargins.right, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_TOP:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_TOP ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_TOP ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.OffsetRect( 0, rcOuterMargins.top );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_BOTTOM:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_BOTTOM ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_BOTTOM ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.top = m_rcScreen.bottom - m_size.cy;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.OffsetRect( 0, - rcOuterMargins.bottom );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		} // switch( pDM->m_eDockMarkerType )
	} // if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )

	if( pCenterMarker != NULL )
	{
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			m_rcScreen.OffsetRect( -9, 0 );
			break;
		case __EDMT_2005_RIGHT:
			m_rcScreen.OffsetRect( 9, 0 );
			break;
		case __EDMT_2005_TOP:
			m_rcScreen.OffsetRect( 0, -9 );
			break;
		case __EDMT_2005_BOTTOM:
			m_rcScreen.OffsetRect( 0, 9 );
			break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		case __EDMT_2005_TAB:
			break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		} // switch( pDM->m_eDockMarkerType )
	} // if( pCenterMarker != NULL )
	else
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		switch( eDockMarkerType )
		{
		case __EDMT_2005_TAB:
			break;
		} // switch( pDM->m_eDockMarkerType )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // else from if( pCenterMarker != NULL )
}

void CExtPaintManager::DockMarker2010::OnCalcScreenRect(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CRect rcScreen,
	DockMarkerBase * pCenterMarker // = NULL
	)
{
	DockMarkerBase::OnCalcScreenRect(
		eDockMarkerType,
		rcScreen,
		pCenterMarker
		);

	if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )
	{
		CRect rcOuterMargins = g_PaintManager->DockMarker_GetOuterDistance();
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_LEFT ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_LEFT ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.OffsetRect( rcOuterMargins.left, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_RIGHT:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_RIGHT ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_RIGHT ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.left = m_rcScreen.right - m_size.cx;
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.OffsetRect( - rcOuterMargins.right, 0 );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_TOP:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_TOP ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_TOP ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.bottom = m_rcScreen.top + m_size.cy;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.OffsetRect( 0, rcOuterMargins.top );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		case __EDMT_2005_BOTTOM:
			{
				CExtBitmap _bitmap;
				_bitmap.LoadBMP_Resource(
					( m_bSparsedMode )
						? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_BOTTOM ) )
						: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_BOTTOM ) )
					);
				if( ! _bitmap.IsEmpty() )
				{
					m_size = _bitmap.GetSize();
					m_rcScreen.top = m_rcScreen.bottom - m_size.cy;
					m_rcScreen.right = m_rcScreen.left + m_size.cx;
					m_rcScreen.OffsetRect( 0, - rcOuterMargins.bottom );
					m_bmpOuterBk = _bitmap;
					HRGN hRgn = _bitmap.GenerateColorHRGN( false, RGB(255,0,255) );
					if( hRgn != NULL )
					{
						if( m_rgn.GetSafeHandle() != NULL )
							m_rgn.DeleteObject();
						m_rgn.Attach( hRgn );
					} // if( hRgn != NULL )
				} // if( ! _bitmap.IsEmpty() )
#ifdef _DEBUG
				else
				{
					ASSERT( FALSE );
				} // else from if( ! _bitmap.IsEmpty() )
#endif // _DEBUG
			}
			break;
		} // switch( pDM->m_eDockMarkerType )
	} // if( m_bSparsedMode && m_bmpOuterBk.IsEmpty() )

	if( pCenterMarker != NULL )
	{
		switch( eDockMarkerType )
		{
		case __EDMT_2005_LEFT:
			m_rcScreen.OffsetRect( 10, 0 );
			break;
		case __EDMT_2005_RIGHT:
			m_rcScreen.OffsetRect( -10, 0 );
			break;
		case __EDMT_2005_TOP:
			m_rcScreen.OffsetRect( 0, 10 );
			break;
		case __EDMT_2005_BOTTOM:
			m_rcScreen.OffsetRect( 0, -10 );
			break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		case __EDMT_2005_TAB:
			break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		} // switch( pDM->m_eDockMarkerType )
	} // if( pCenterMarker != NULL )
	else
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		switch( eDockMarkerType )
		{
		case __EDMT_2005_TAB:
			break;
		} // switch( pDM->m_eDockMarkerType )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // else from if( pCenterMarker != NULL )
}

void CExtPaintManager::DockMarkerBase::_EmptyChildren()
{
int nCount = (int)m_arrChildren.GetSize();
	for( int nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		DockMarkerBase * pDM = m_arrChildren[ nIndex ];
		ASSERT( pDM != NULL );
		delete pDM;
	} // for( int nIndex = 0; nIndex < nCount; nIndex ++ )
	m_arrChildren.RemoveAll();
}

void CExtPaintManager::DockMarkerBase::Empty()
{
	m_bmpIn.Empty();
	m_bmpOut.Empty();
	if( m_rgn.GetSafeHandle() != NULL )
		m_rgn.DeleteObject();
	m_eDockMarkerType = __EDMT_EMPTY;
	m_size.cx = m_size.cy = 0;
	m_rcScreen.SetRect( 0, 0, 0, 0 );
	_EmptyChildren();
}

void CExtPaintManager::DockMarkerBase::_DetachHandles()
{
	m_bmpIn.Empty();
	m_bmpOut.Empty();
	if( m_rgn.GetSafeHandle() != NULL )
		m_rgn.Detach();
}

bool CExtPaintManager::DockMarkerBase::Create(
	bool bCentralMode,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType, // = CExtPaintManager::DockMarkerBase::__EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
{
	Empty();
	if( eDockMarkerType == __EDMT_EMPTY )
		return true;
	switch( eDockMarkerType )
	{
	case __EDMT_2005_TOP:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B1_TOP
						)
					)
				);
	case __EDMT_2005_BOTTOM:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B1_BOTTOM
						)
					)
				);
	case __EDMT_2005_LEFT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B1_LEFT
						)
					)
				);
	case __EDMT_2005_RIGHT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B1_RIGHT
						)
					)
				);
	case __EDMT_2005_CENTER:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B1_CENTER
						)
					)
				);
	case __EDMT_2005_GROUP_SPARSED:
		{
			DockMarkerBase * pDmTop = NULL, * pDmBottom = NULL,
				* pDmLeft = NULL, * pDmRight = NULL;
			try
			{
				pDmTop = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmBottom = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmLeft = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmRight = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( ! bCentralMode )
				{
					pDmTop->m_bSparsedMode = true;
					pDmBottom->m_bSparsedMode = true;
					pDmLeft->m_bSparsedMode = true;
					pDmRight->m_bSparsedMode = true;
				} // if( ! bCentralMode )
				if(		pDmTop->Create( bCentralMode, __EDMT_2005_TOP, rcScreen )
					&&	pDmBottom->Create( bCentralMode, __EDMT_2005_BOTTOM, rcScreen )
					&&	pDmLeft->Create( bCentralMode, __EDMT_2005_LEFT, rcScreen )
					&&	pDmRight->Create( bCentralMode, __EDMT_2005_RIGHT, rcScreen )
					)
				{
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.Add( pDmTop );
					m_arrChildren.Add( pDmBottom );
					m_arrChildren.Add( pDmLeft );
					m_arrChildren.Add( pDmRight );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTop != NULL )
				delete pDmTop;
			if( pDmBottom != NULL )
				delete pDmBottom;
			if( pDmLeft != NULL )
				delete pDmLeft;
			if( pDmRight != NULL )
				delete pDmRight;
		}
		return false;
	case __EDMT_2005_GROUP_CENTRAL:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_SPARSED,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmCenter = NULL;
			try
			{
				pDmCenter = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmCenter->Create(
						true,
						__EDMT_2005_CENTER,
						rcScreen
						)
					)
				{
					CRect _rcScreen( rcScreen );
					CSize _size( pDmCenter->m_size );
					_size.cx += 12;
					_size.cy += 12;
					_rcScreen.OffsetRect(
						( _rcScreen.Width() - _size.cx ) / 2,
						( _rcScreen.Height() - _size.cy ) / 2
						);
					_rcScreen.right = _rcScreen.left + _size.cx;
					_rcScreen.bottom = _rcScreen.top + _size.cy;
					int nCount = (int)m_arrChildren.GetSize();
					for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					{
						DockMarkerBase * pDM = m_arrChildren[ nIndex ];
						ASSERT( pDM != NULL );
						pDM->m_bSparsedMode = false;
						pDM->OnCalcScreenRect(
							pDM->m_eDockMarkerType,
							_rcScreen,
							pDmCenter
							);
					} // for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.InsertAt( 0, pDmCenter );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmCenter != NULL )
				delete pDmCenter;
			Empty();
		}
		return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case __EDMT_2005_GROUP_TAB:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_CENTRAL,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmTab = NULL;
			try
			{
				pDmTab = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmTab->Create(
						true,
						__EDMT_2005_TAB,
						rcScreen
						)
					)
				{
					m_eDockMarkerType = eDockMarkerType;
//					m_arrChildren.Add( pDmTab );
					ASSERT( m_arrChildren.GetSize() >= 1 );
					m_arrChildren.InsertAt( 1, pDmTab );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTab != NULL )
				delete pDmTab;
			Empty();
		}
		return false;
	case __EDMT_2005_TAB:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B1_TAB
						)
					)
				);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // switch( eDockMarkerType )
	ASSERT( FALSE );
	return false;
}

bool CExtPaintManager::DockMarkerBeta2::Create(
	bool bCentralMode,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType, // = CExtPaintManager::DockMarkerBase::__EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
{
	Empty();
	if( eDockMarkerType == __EDMT_EMPTY )
		return true;
	switch( eDockMarkerType )
	{
	case __EDMT_2005_TOP:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_TOP
						)
					)
				);
	case __EDMT_2005_BOTTOM:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_BOTTOM
						)
					)
				);
	case __EDMT_2005_LEFT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_LEFT
						)
					)
				);
	case __EDMT_2005_RIGHT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_RIGHT
						)
					)
				);
	case __EDMT_2005_CENTER:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_CENTER
						)
					)
				);
	case __EDMT_2005_GROUP_SPARSED:
		{
			DockMarkerBase * pDmTop = NULL, * pDmBottom = NULL,
				* pDmLeft = NULL, * pDmRight = NULL;
			try
			{
				pDmTop = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmBottom = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmLeft = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmRight = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( ! bCentralMode )
				{
					pDmTop->m_bSparsedMode = true;
					pDmBottom->m_bSparsedMode = true;
					pDmLeft->m_bSparsedMode = true;
					pDmRight->m_bSparsedMode = true;
				} // if( ! bCentralMode )
				if(		pDmTop->Create( bCentralMode, __EDMT_2005_TOP, rcScreen )
					&&	pDmBottom->Create( bCentralMode, __EDMT_2005_BOTTOM, rcScreen )
					&&	pDmLeft->Create( bCentralMode, __EDMT_2005_LEFT, rcScreen )
					&&	pDmRight->Create( bCentralMode, __EDMT_2005_RIGHT, rcScreen )
					)
				{
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.Add( pDmTop );
					m_arrChildren.Add( pDmBottom );
					m_arrChildren.Add( pDmLeft );
					m_arrChildren.Add( pDmRight );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTop != NULL )
				delete pDmTop;
			if( pDmBottom != NULL )
				delete pDmBottom;
			if( pDmLeft != NULL )
				delete pDmLeft;
			if( pDmRight != NULL )
				delete pDmRight;
		}
		return false;
	case __EDMT_2005_GROUP_CENTRAL:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_SPARSED,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmCenter = NULL;
			try
			{
				pDmCenter = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmCenter->Create(
						true,
						__EDMT_2005_CENTER,
						rcScreen
						)
					)
				{
					CRect _rcScreen( rcScreen );
					CSize _size( pDmCenter->m_size );
					_size.cx += 12;
					_size.cy += 12;
					_rcScreen.OffsetRect(
						( _rcScreen.Width() - _size.cx ) / 2,
						( _rcScreen.Height() - _size.cy ) / 2
						);
					_rcScreen.right = _rcScreen.left + _size.cx;
					_rcScreen.bottom = _rcScreen.top + _size.cy;
					int nCount = (int)m_arrChildren.GetSize();
					for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					{
						DockMarkerBase * pDM = m_arrChildren[ nIndex ];
						ASSERT( pDM != NULL );
						pDM->m_bSparsedMode = false;
						pDM->OnCalcScreenRect(
							pDM->m_eDockMarkerType,
							_rcScreen,
							pDmCenter
							);
					} // for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.InsertAt( 0, pDmCenter );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmCenter != NULL )
				delete pDmCenter;
			Empty();
		}
		return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case __EDMT_2005_GROUP_TAB:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_CENTRAL,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmTab = NULL;
			try
			{
				pDmTab = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmTab->Create(
						true,
						__EDMT_2005_TAB,
						rcScreen
						)
					)
				{
					m_eDockMarkerType = eDockMarkerType;
//					m_arrChildren.Add( pDmTab );
					ASSERT( m_arrChildren.GetSize() >= 1 );
					m_arrChildren.InsertAt( 1, pDmTab );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTab != NULL )
				delete pDmTab;
			Empty();
		}
		return false;
	case __EDMT_2005_TAB:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(0,255,0),
				__EXT_MFC_SAFE_LPCTSTR(
					MAKEINTRESOURCE(
						IDB_EXT_RESIZABLE_DOCK_MARKER_2005B2_TAB
						)
					)
				);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // switch( eDockMarkerType )
	ASSERT( FALSE );
	return false;
}

bool CExtPaintManager::DockMarker2008XP::Create(
	bool bCentralMode,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType, // = CExtPaintManager::DockMarkerBase::__EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
{
	Empty();
	if( eDockMarkerType == __EDMT_EMPTY )
		return true;
	switch( eDockMarkerType )
	{
	case __EDMT_2005_TOP:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_TOP ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_TOP ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_OUT_TOP ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_OUT_TOP ) )
				);
	case __EDMT_2005_BOTTOM:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_BOTTOM ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_BOTTOM ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_OUT_BOTTOM ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_OUT_BOTTOM ) )
				);
	case __EDMT_2005_LEFT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_LEFT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_LEFT ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_OUT_LEFT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_OUT_LEFT ) )
				);
	case __EDMT_2005_RIGHT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_IN_RIGHT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_RIGHT ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_OUTER_OUT_RIGHT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_OUT_RIGHT ) )
				);
	case __EDMT_2005_CENTER:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_EMPTY ) ),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_OUT_EMPTY ) )
				);
	case __EDMT_2005_GROUP_SPARSED:
		{
			DockMarkerBase * pDmTop = NULL, * pDmBottom = NULL,
				* pDmLeft = NULL, * pDmRight = NULL;
			try
			{
				pDmTop = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmBottom = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmLeft = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmRight = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( ! bCentralMode )
				{
					pDmTop->m_bSparsedMode = true;
					pDmBottom->m_bSparsedMode = true;
					pDmLeft->m_bSparsedMode = true;
					pDmRight->m_bSparsedMode = true;
				} // if( ! bCentralMode )
				if(		pDmTop->Create( bCentralMode, __EDMT_2005_TOP, rcScreen )
					&&	pDmBottom->Create( bCentralMode, __EDMT_2005_BOTTOM, rcScreen )
					&&	pDmLeft->Create( bCentralMode, __EDMT_2005_LEFT, rcScreen )
					&&	pDmRight->Create( bCentralMode, __EDMT_2005_RIGHT, rcScreen )
					)
				{
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.Add( pDmTop );
					m_arrChildren.Add( pDmBottom );
					m_arrChildren.Add( pDmLeft );
					m_arrChildren.Add( pDmRight );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTop != NULL )
				delete pDmTop;
			if( pDmBottom != NULL )
				delete pDmBottom;
			if( pDmLeft != NULL )
				delete pDmLeft;
			if( pDmRight != NULL )
				delete pDmRight;
		}
		return false;
	case __EDMT_2005_GROUP_CENTRAL:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_SPARSED,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmCenter = NULL;
			try
			{
				pDmCenter = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmCenter->Create(
						true,
						__EDMT_2005_CENTER,
						rcScreen
						)
					)
				{
					CRect _rcScreen( rcScreen );
					CSize _size( pDmCenter->m_size );
					_size.cx += 12;
					_size.cy += 12;
					_rcScreen.OffsetRect(
						( _rcScreen.Width() - _size.cx ) / 2,
						( _rcScreen.Height() - _size.cy ) / 2
						);
					_rcScreen.right = _rcScreen.left + _size.cx;
					_rcScreen.bottom = _rcScreen.top + _size.cy;
					int nCount = (int)m_arrChildren.GetSize();
					for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					{
						DockMarkerBase * pDM = m_arrChildren[ nIndex ];
						ASSERT( pDM != NULL );
						pDM->m_bSparsedMode = false;
						pDM->OnCalcScreenRect(
							pDM->m_eDockMarkerType,
							_rcScreen,
							pDmCenter
							);
					} // for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.InsertAt( 0, pDmCenter );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmCenter != NULL )
				delete pDmCenter;
			Empty();
		}
		return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case __EDMT_2005_GROUP_TAB:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_CENTRAL,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmTab = NULL;
			try
			{
				pDmTab = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmTab->Create(
						true,
						__EDMT_2005_TAB,
						rcScreen
						)
					)
				{
					m_eDockMarkerType = eDockMarkerType;
//					m_arrChildren.Add( pDmTab );
					ASSERT( m_arrChildren.GetSize() >= 1 );
					m_arrChildren.InsertAt( 1, pDmTab );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTab != NULL )
				delete pDmTab;
			Empty();
		}
		return false;
	case __EDMT_2005_TAB:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_IN_TAB ) ),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_XP_INNER_OUT_TAB ) )
				);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // switch( eDockMarkerType )
	ASSERT( FALSE );
	return false;
}

bool CExtPaintManager::DockMarker2008Vista::Create(
	bool bCentralMode,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType, // = CExtPaintManager::DockMarkerBase::__EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
{
	Empty();
	if( eDockMarkerType == __EDMT_EMPTY )
		return true;
	switch( eDockMarkerType )
	{
	case __EDMT_2005_TOP:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_TOP ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_TOP ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_OUT_TOP ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_OUT_TOP ) )
				);
	case __EDMT_2005_BOTTOM:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_BOTTOM ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_BOTTOM ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_OUT_BOTTOM ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_OUT_BOTTOM ) )
				);
	case __EDMT_2005_LEFT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_LEFT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_LEFT ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_OUT_LEFT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_OUT_LEFT ) )
				);
	case __EDMT_2005_RIGHT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_IN_RIGHT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_RIGHT ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_OUTER_OUT_RIGHT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_OUT_RIGHT ) )
				);
	case __EDMT_2005_CENTER:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_EMPTY ) ),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_OUT_EMPTY ) )
				);
	case __EDMT_2005_GROUP_SPARSED:
		{
			DockMarkerBase * pDmTop = NULL, * pDmBottom = NULL,
				* pDmLeft = NULL, * pDmRight = NULL;
			try
			{
				pDmTop = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmBottom = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmLeft = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmRight = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( ! bCentralMode )
				{
					pDmTop->m_bSparsedMode = true;
					pDmBottom->m_bSparsedMode = true;
					pDmLeft->m_bSparsedMode = true;
					pDmRight->m_bSparsedMode = true;
				} // if( ! bCentralMode )
				if(		pDmTop->Create( bCentralMode, __EDMT_2005_TOP, rcScreen )
					&&	pDmBottom->Create( bCentralMode, __EDMT_2005_BOTTOM, rcScreen )
					&&	pDmLeft->Create( bCentralMode, __EDMT_2005_LEFT, rcScreen )
					&&	pDmRight->Create( bCentralMode, __EDMT_2005_RIGHT, rcScreen )
					)
				{
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.Add( pDmTop );
					m_arrChildren.Add( pDmBottom );
					m_arrChildren.Add( pDmLeft );
					m_arrChildren.Add( pDmRight );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTop != NULL )
				delete pDmTop;
			if( pDmBottom != NULL )
				delete pDmBottom;
			if( pDmLeft != NULL )
				delete pDmLeft;
			if( pDmRight != NULL )
				delete pDmRight;
		}
		return false;
	case __EDMT_2005_GROUP_CENTRAL:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_SPARSED,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmCenter = NULL;
			try
			{
				pDmCenter = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmCenter->Create(
						true,
						__EDMT_2005_CENTER,
						rcScreen
						)
					)
				{
					CRect _rcScreen( rcScreen );
					CSize _size( pDmCenter->m_size );
					_size.cx += 12;
					_size.cy += 12;
					_rcScreen.OffsetRect(
						( _rcScreen.Width() - _size.cx ) / 2,
						( _rcScreen.Height() - _size.cy ) / 2
						);
					_rcScreen.right = _rcScreen.left + _size.cx;
					_rcScreen.bottom = _rcScreen.top + _size.cy;
					int nCount = (int)m_arrChildren.GetSize();
					for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					{
						DockMarkerBase * pDM = m_arrChildren[ nIndex ];
						ASSERT( pDM != NULL );
						pDM->m_bSparsedMode = false;
						pDM->OnCalcScreenRect(
							pDM->m_eDockMarkerType,
							_rcScreen,
							pDmCenter
							);
					} // for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.InsertAt( 0, pDmCenter );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmCenter != NULL )
				delete pDmCenter;
			Empty();
		}
		return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case __EDMT_2005_GROUP_TAB:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_CENTRAL,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmTab = NULL;
			try
			{
				pDmTab = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmTab->Create(
						true,
						__EDMT_2005_TAB,
						rcScreen
						)
					)
				{
					m_eDockMarkerType = eDockMarkerType;
//					m_arrChildren.Add( pDmTab );
					ASSERT( m_arrChildren.GetSize() >= 1 );
					m_arrChildren.InsertAt( 1, pDmTab );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTab != NULL )
				delete pDmTab;
			Empty();
		}
		return false;
	case __EDMT_2005_TAB:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_IN_TAB ) ),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2008_VISTA_INNER_OUT_TAB ) )
				);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // switch( eDockMarkerType )
	ASSERT( FALSE );
	return false;
}

bool CExtPaintManager::DockMarker2010::Create(
	bool bCentralMode,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType, // = CExtPaintManager::DockMarkerBase::__EDMT_EMPTY
	CRect rcScreen // = CRect( 0, 0, 0, 0 )
	)
{
	Empty();
	if( eDockMarkerType == __EDMT_EMPTY )
		return true;
	switch( eDockMarkerType )
	{
	case __EDMT_2005_TOP:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_TOP ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_TOP ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_OUT_TOP ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_OUT_TOP ) )
				);
	case __EDMT_2005_BOTTOM:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_BOTTOM ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_BOTTOM ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_OUT_BOTTOM ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_OUT_BOTTOM ) )
				);
	case __EDMT_2005_LEFT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_LEFT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_LEFT ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_OUT_LEFT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_OUT_LEFT ) )
				);
	case __EDMT_2005_RIGHT:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_IN_RIGHT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_RIGHT ) )
					,
				( m_bSparsedMode )
					? __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_OUTER_OUT_RIGHT ) )
					: __EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_OUT_RIGHT ) )
				);
	case __EDMT_2005_CENTER:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_EMPTY ) ),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_OUT_EMPTY ) )
				);
	case __EDMT_2005_GROUP_SPARSED:
		{
			DockMarkerBase * pDmTop = NULL, * pDmBottom = NULL,
				* pDmLeft = NULL, * pDmRight = NULL;
			try
			{
				pDmTop = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmBottom = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmLeft = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				pDmRight = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( ! bCentralMode )
				{
					pDmTop->m_bSparsedMode = true;
					pDmBottom->m_bSparsedMode = true;
					pDmLeft->m_bSparsedMode = true;
					pDmRight->m_bSparsedMode = true;
				} // if( ! bCentralMode )
				if(		pDmTop->Create( bCentralMode, __EDMT_2005_TOP, rcScreen )
					&&	pDmBottom->Create( bCentralMode, __EDMT_2005_BOTTOM, rcScreen )
					&&	pDmLeft->Create( bCentralMode, __EDMT_2005_LEFT, rcScreen )
					&&	pDmRight->Create( bCentralMode, __EDMT_2005_RIGHT, rcScreen )
					)
				{
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.Add( pDmTop );
					m_arrChildren.Add( pDmBottom );
					m_arrChildren.Add( pDmLeft );
					m_arrChildren.Add( pDmRight );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTop != NULL )
				delete pDmTop;
			if( pDmBottom != NULL )
				delete pDmBottom;
			if( pDmLeft != NULL )
				delete pDmLeft;
			if( pDmRight != NULL )
				delete pDmRight;
		}
		return false;
	case __EDMT_2005_GROUP_CENTRAL:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_SPARSED,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmCenter = NULL;
			try
			{
				pDmCenter = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmCenter->Create(
						true,
						__EDMT_2005_CENTER,
						rcScreen
						)
					)
				{
					CRect _rcScreen( rcScreen );
					CSize _size( pDmCenter->m_size );
					_size.cx += 12;
					_size.cy += 12;
					_rcScreen.OffsetRect(
						( _rcScreen.Width() - _size.cx ) / 2,
						( _rcScreen.Height() - _size.cy ) / 2
						);
					_rcScreen.right = _rcScreen.left + _size.cx;
					_rcScreen.bottom = _rcScreen.top + _size.cy;
					int nCount = (int)m_arrChildren.GetSize();
					for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					{
						DockMarkerBase * pDM = m_arrChildren[ nIndex ];
						ASSERT( pDM != NULL );
						pDM->m_bSparsedMode = false;
						pDM->OnCalcScreenRect(
							pDM->m_eDockMarkerType,
							_rcScreen,
							pDmCenter
							);
					} // for( int nIndex = 0; nIndex < nCount; nIndex ++ )
					m_eDockMarkerType = eDockMarkerType;
					m_arrChildren.InsertAt( 0, pDmCenter );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmCenter != NULL )
				delete pDmCenter;
			Empty();
		}
		return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case __EDMT_2005_GROUP_TAB:
		{
			if( ! Create(
					true,
					__EDMT_2005_GROUP_CENTRAL,
					rcScreen
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			DockMarkerBase * pDmTab = NULL;
			try
			{
				pDmTab = m_pPM->DockMarker_CreateObject( m_eResizablePanelDockingType );
				if( pDmTab->Create(
						true,
						__EDMT_2005_TAB,
						rcScreen
						)
					)
				{
					m_eDockMarkerType = eDockMarkerType;
//					m_arrChildren.Add( pDmTab );
					ASSERT( m_arrChildren.GetSize() >= 1 );
					m_arrChildren.InsertAt( 1, pDmTab );
					return true;
				}
				ASSERT( FALSE );
			} // try
			catch( ... )
			{
				ASSERT( FALSE );
			} // catch( .... )
			if( pDmTab != NULL )
				delete pDmTab;
			Empty();
		}
		return false;
	case __EDMT_2005_TAB:
		return
			CreateFromBitmapResources(
				rcScreen,
				eDockMarkerType,
				RGB(255,0,255),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_IN_TAB ) ),
				__EXT_MFC_SAFE_LPCTSTR( MAKEINTRESOURCE( IDB_EXT_RESIZABLE_DOCK_MARKER_2010_INNER_OUT_TAB ) )
				);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // switch( eDockMarkerType )
	ASSERT( FALSE );
	return false;
}

bool CExtPaintManager::DockMarkerBase::DockMarker_CreateWnds(
	HWND hWndLastDynamicTarget,
	CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * > & arrDockMarkerWnds,
	CExtDynDockMarkerWnd * pWndParentMarker
	)
{
bool bRetVal = true;
	if( ! m_bmpIn.IsEmpty() )
	{
		ASSERT( ! m_bmpOut.IsEmpty() );
		ASSERT( m_rgn.GetSafeHandle() != NULL );
		CExtDynDockMarkerWnd * pDynDockMarkerWnd =
			new CExtDynDockMarkerWnd(
				pWndParentMarker,
				hWndLastDynamicTarget,
				int(m_eDockMarkerType),
				m_bmpIn,
				m_bmpOut,
				(HRGN)m_rgn.GetSafeHandle(),
				m_rcScreen,
				& m_bmpOuterBk,
				m_ptOffsetInner.x,
				m_ptOffsetInner.y,
				m_bSparsedMode
				);
		pWndParentMarker = pDynDockMarkerWnd;
		_DetachHandles();
		Empty();
		if( pDynDockMarkerWnd->GetSafeHwnd() != NULL )
			arrDockMarkerWnds.Add(
				pDynDockMarkerWnd
				);
		else
		{
			ASSERT( FALSE );
			bRetVal = false;
		} // else from if( pDynDockMarkerWnd->GetSafeHwnd() != NULL )
	} // if( ! m_bmpIn.IsEmpty() )
int nCount = (int)m_arrChildren.GetSize();
bool bChainMode = ( pWndParentMarker == NULL ) ? true : false;
	for( int nIndex = 0; nIndex < nCount; nIndex++ )
	{
		DockMarkerBase * pDM = m_arrChildren[ nIndex ];
		ASSERT( pDM != NULL );
		if ( bChainMode && nIndex > 0 && arrDockMarkerWnds.GetSize() > 0 )
			pWndParentMarker = arrDockMarkerWnds[ arrDockMarkerWnds.GetSize() - 1 ];
		bool bChildrenRetVal =
			pDM->DockMarker_CreateWnds(
				hWndLastDynamicTarget,
				arrDockMarkerWnds,
				pWndParentMarker
				);
		if( !bChildrenRetVal )
			bRetVal = false;
	} // for( int nIndex = 0; nIndex < nCount; nIndex++ )
	return bRetVal;
}

bool CExtPaintManager::DockMarker_CreateWnds(
	HWND hWndLastDynamicTarget,
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CRect rcScreen,
	CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * > & arrDockMarkerWnds,
	int eResizablePanelDockingType,
	CExtDynDockMarkerWnd * pWndParentMarker
	)
{
	ASSERT_VALID( this );
	ASSERT( eDockMarkerType != __EDMT_EMPTY );
	CExtDynDockMarkerWnd::DestroyMarkers( arrDockMarkerWnds );
DockMarkerBase _DM( this, eResizablePanelDockingType );
	if( ! _DM.Create( false, eDockMarkerType, rcScreen ) )
	{
		ASSERT( FALSE );
		return false;
	} // if( ! _DM.Create( eDockMarkerType, rcScreen ) )
	return
		_DM.DockMarker_CreateWnds(
			hWndLastDynamicTarget,
			arrDockMarkerWnds,
			pWndParentMarker
			);
}

void CExtPaintManager::DockMarker_PostRender(
	CDC & dc,
	CRect rcItem,
	CExtPaintManager::eDockMarkerType_t eMarkerType,
	bool bHover,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	lParam;
	if( pHelperSrc == NULL )
		return;
	ASSERT_VALID( pHelperSrc );
	if( ! pHelperSrc->IsKindOf( RUNTIME_CLASS(CExtControlBar) ) )
		return;
	if( INT(((CExtControlBar*)(pHelperSrc))->OnQueryDockingType()) != INT(CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
		return;
	switch( m_eStyle2005 )
	{
	case __ES2005_BETA1:
		break;
	case __ES2005_BETA2:
	case __ES2005_RC:
		if(  pHelperSrc != NULL )
		{
			ASSERT_VALID( pHelperSrc );
			CExtDynDockMarkerWnd * pDynDockMarkerWnd =
				DYNAMIC_DOWNCAST( CExtDynDockMarkerWnd, pHelperSrc );
			if( pDynDockMarkerWnd != NULL )
			{
				static const COLORREF stat_clrHoverBorderBeta2 = RGB(0,96,196);
				if( bHover && pDynDockMarkerWnd->m_bSparsedMode )
				{
					COLORREF clrHoverBorderBeta2 = dc.GetNearestColor( stat_clrHoverBorderBeta2 );
					switch( eMarkerType )
					{
					case __EDMT_2005_LEFT:
						dc.FillSolidRect(
							rcItem.left,
							rcItem.top,
							rcItem.Width(),
							1,
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.left,
							rcItem.bottom-1,
							rcItem.Width(),
							1,
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.left,
							rcItem.top,
							1,
							rcItem.Height(),
							clrHoverBorderBeta2
							);
					break;
					case __EDMT_2005_RIGHT:
						dc.FillSolidRect(
							rcItem.left,
							rcItem.top,
							rcItem.Width(),
							1,
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.left,
							rcItem.bottom-1,
							rcItem.Width(),
							1,
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.right-1,
							rcItem.top,
							1,
							rcItem.Height(),
							clrHoverBorderBeta2
							);
					break;
					case __EDMT_2005_TOP:
						dc.FillSolidRect(
							rcItem.left,
							rcItem.top,
							rcItem.Width(),
							1,
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.left,
							rcItem.top,
							1,
							rcItem.Height(),
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.right-1,
							rcItem.top,
							1,
							rcItem.Height(),
							clrHoverBorderBeta2
							);
					break;
					case __EDMT_2005_BOTTOM:
						dc.FillSolidRect(
							rcItem.left,
							rcItem.bottom-1,
							rcItem.Width(),
							1,
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.left,
							rcItem.top,
							1,
							rcItem.Height(),
							clrHoverBorderBeta2
							);
						dc.FillSolidRect(
							rcItem.right-1,
							rcItem.top,
							1,
							rcItem.Height(),
							clrHoverBorderBeta2
							);
					break;
					} // switch( eMarkerType )
				} // if( bHover && pDynDockMarkerWnd->m_bSparsedMode )
				else if( eMarkerType == __EDMT_2005_CENTER )
				{
					if(		pDynDockMarkerWnd->m_hWndLastHoverDepenentMarker != NULL
						&&	::IsWindow(
								pDynDockMarkerWnd->m_hWndLastHoverDepenentMarker
								)
						)
					{ // if hover dependent marker window is valid
						CWnd * pWndPermanent =
							CWnd::FromHandlePermanent(
								pDynDockMarkerWnd->m_hWndLastHoverDepenentMarker
								);
						if( pWndPermanent != NULL )
						{
							CExtDynDockMarkerWnd * pWndLastHoverDepenentMarker =
								DYNAMIC_DOWNCAST(
									CExtDynDockMarkerWnd,
									pWndPermanent
									);
							if( pWndLastHoverDepenentMarker != NULL )
							{
								switch( pWndLastHoverDepenentMarker->m_eMarkerType )
								{
								case int(__EDMT_2005_LEFT):
									{
										COLORREF clrHoverBorderBeta2 = dc.GetNearestColor( stat_clrHoverBorderBeta2 );
										CRect rc( 0, 29, 23, 58 );
										dc.FillSolidRect(
											rc.left,
											rc.top,
											rc.Width(),
											1,
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.left,
											rc.bottom-1,
											rc.Width(),
											1,
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.left,
											rc.top,
											1,
											rc.Height(),
											clrHoverBorderBeta2
											);
									}
								break;
								case int(__EDMT_2005_RIGHT):
									{
										COLORREF clrHoverBorderBeta2 = dc.GetNearestColor( stat_clrHoverBorderBeta2 );
										CRect rc( 64, 29, 88, 58 );
										dc.FillSolidRect(
											rc.left,
											rc.top,
											rc.Width(),
											1,
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.left,
											rc.bottom-1,
											rc.Width(),
											1,
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.right-1,
											rc.top,
											1,
											rc.Height(),
											clrHoverBorderBeta2
											);
									}
								break;
								case int(__EDMT_2005_TOP):
									{
										COLORREF clrHoverBorderBeta2 = dc.GetNearestColor( stat_clrHoverBorderBeta2 );
										CRect rc( 29, 0, 58, 23 );
										dc.FillSolidRect(
											rc.left,
											rc.top,
											rc.Width(),
											1,
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.left,
											rc.top,
											1,
											rc.Height(),
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.right-1,
											rc.top,
											1,
											rc.Height(),
											clrHoverBorderBeta2
											);
									}
								break;
								case int(__EDMT_2005_BOTTOM):
									{
										COLORREF clrHoverBorderBeta2 = dc.GetNearestColor( stat_clrHoverBorderBeta2 );
										CRect rc( 29, 64, 58, 88 );
										dc.FillSolidRect(
											rc.left,
											rc.bottom-1,
											rc.Width(),
											1,
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.left,
											rc.top,
											1,
											rc.Height(),
											clrHoverBorderBeta2
											);
										dc.FillSolidRect(
											rc.right-1,
											rc.top,
											1,
											rc.Height(),
											clrHoverBorderBeta2
											);
									}
								break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
								case int(__EDMT_2005_TAB):
									{
										COLORREF clrHoverBorderBeta2 = dc.GetNearestColor( stat_clrHoverBorderBeta2 );
										CPen _pen( PS_SOLID, 1, clrHoverBorderBeta2 );
										CPen * pOldPen = dc.SelectObject( &_pen );
										dc.MoveTo( 23, 28 );
										dc.LineTo( 29, 22 );
										dc.MoveTo( 58, 23 );
										dc.LineTo( 64, 29 );
										dc.MoveTo( 58, 63 );
										dc.LineTo( 64, 57 );
										dc.MoveTo( 23, 58 );
										dc.LineTo( 29, 64 );
										dc.SelectObject( pOldPen );
									}
								break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
								} // switch( pWndLastHoverDepenentMarker->m_eMarkerType )
							} // if( pWndLastHoverDepenentMarker != NULL )
						} // if( pWndPermanent != NULL )
					} // if hover dependent marker window is valid
				} // else if( eMarkerType == __EDMT_2005_CENTER )
			} // if( pDynDockMarkerWnd != NULL )
		} // if( pHelperSrc != NULL )
		break;
	default:
		{
			ASSERT( FALSE );
		}
	} // switch( m_eStyle2005 )
}

CRect CExtPaintManager::DockMarker_GetOuterDistance()
{
	ASSERT_VALID( this );
	switch( m_eStyle2005 )
	{
	case __ES2005_BETA1:
		return CRect( 0, 0, 0, 0 );
	case __ES2005_BETA2:
	case __ES2005_RC:
		return CRect( 5, 5, 5, 5 );
	default:
		{
			ASSERT( FALSE );
		}
		return CRect( 0, 0, 0, 0 );
	} // switch( m_eStyle2005 )
}

BYTE CExtPaintManager::DockMarker_GetAlpha(
	bool bHighlight,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	bHighlight;
	pHelperSrc;
	lParam;
	return BYTE(255);
}

bool CExtPaintManager::DockMarker_IsAdjustBmpsRequired(
	eDockMarkerType_t eDockMarkerType
	) const
{
	ASSERT_VALID( this );
	eDockMarkerType;
	switch( m_eStyle2005 )
	{
	case __ES2005_BETA1:
		return true;
	case __ES2005_BETA2:
	case __ES2005_RC:
		return false;
	default:
		{
			ASSERT( FALSE );
		}
		return false;
	} // switch( m_eStyle2005 )
}

void CExtPaintManager::DockMarker_AdjustBmps(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CExtBitmap & bmpIn,
	COLORREF clrTransparentIn,
	CExtBitmap & bmpOut,
	COLORREF clrTransparentOut
	)
{
	ASSERT_VALID( this );
	ASSERT( ! bmpIn.IsEmpty() );
	ASSERT( ! bmpOut.IsEmpty() );
	if( ! DockMarker_IsAdjustBmpsRequired( eDockMarkerType ) )
		return;
	if( eDockMarkerType == CExtPaintManager::__EDMT_2005_CENTER )
		return;
	if( stat_GetBPP() <= 8 )
	{
		VERIFY(
			bmpOut.AdjustHLS(
				clrTransparentOut,
				RGB(0,0,0),
				0.0,
				0.30,
				-1.0
				)
			);
		VERIFY(
			bmpIn.AdjustHLS(
				clrTransparentIn,
				RGB(0,0,0),
				0.0,
				-0.15,
				-1.0
				)
			);
	} // if( stat_GetBPP() <= 8 )
	else
	{
		VERIFY(
			bmpOut.AdjustHLS(
				clrTransparentOut,
				RGB(0,0,0),
				0.0,
				0.35,
				0.0
				)
			);
	} // else from if( stat_GetBPP() <= 8 )
}

void CExtPaintManagerXP::DockMarker_AdjustBmps(
	CExtPaintManager::eDockMarkerType_t eDockMarkerType,
	CExtBitmap & bmpIn,
	COLORREF clrTransparentIn,
	CExtBitmap & bmpOut,
	COLORREF clrTransparentOut
	)
{
	ASSERT_VALID( this );
	ASSERT( ! bmpIn.IsEmpty() );
	ASSERT( ! bmpOut.IsEmpty() );
	if( ! DockMarker_IsAdjustBmpsRequired( eDockMarkerType ) )
		return;
	CExtPaintManager::DockMarker_AdjustBmps(
		eDockMarkerType,
		bmpIn,
		clrTransparentIn,
		bmpOut,
		clrTransparentOut
		);
}

void CExtPaintManager::DockMarker_AdjustHighlightedArea(
	COLORREF * pClrSurface,
	int nClrSurfaceDX,
	int nClrSurfaceDY,
	bool bTabShape,
	bool bTabsAtTop,
	const CRect & rcTabMainArea,
	const CRect & rcTabBottomMiddleArea,
	COLORREF clrAdjustMain, // = COLORREF(-1L)
	COLORREF clrAdjustBorder, // = COLORREF(-1L)
	int nBorderMetric // = 2
	)
{
	ASSERT_VALID( this );
	ASSERT( nBorderMetric >= 0 );
	ASSERT( pClrSurface != NULL );
	ASSERT( nClrSurfaceDX > 0 );
	ASSERT( nClrSurfaceDY > 0 );
CRect rcMainArea( 0, 0, nClrSurfaceDX, nClrSurfaceDY );
	if( bTabShape )
		rcMainArea = rcTabMainArea;
	if( stat_GetBPP() > 8 )
	{
		if( clrAdjustMain == COLORREF(-1L) )
			clrAdjustMain = ::GetSysColor( COLOR_HIGHLIGHT );
		if(		m_eStyle2005 == __ES2005_BETA2
			||	m_eStyle2005 == __ES2005_RC
			)
			clrAdjustMain = RGB(0,64,196);
		if( nBorderMetric > 0 && clrAdjustBorder == COLORREF(-1L) )
			clrAdjustBorder = ::GetSysColor( COLOR_ACTIVEBORDER );
		unsigned nPixelIdx = 0;
		for( int nY = 0; nY < nClrSurfaceDY; nY++ )
		{
			for( int nX = 0; nX < nClrSurfaceDX; nX++, nPixelIdx++ )
			{
				if( bTabShape )
				{
					CPoint ptTest( nX, nClrSurfaceDY - nY - 1 );
					if(		(! rcTabMainArea.PtInRect( ptTest ) )
						&&	(! rcTabBottomMiddleArea.PtInRect( ptTest ) )
						)
						continue;
				} // if( bTabShape )
				COLORREF clrAdjustMix = clrAdjustMain;
				if( nBorderMetric > 0 )
				{
					CPoint ptTest( nX, nClrSurfaceDY - nY - 1 );
					if(			bTabShape
							&&	rcTabBottomMiddleArea.PtInRect( ptTest )
							&&	(
									( bTabsAtTop
										? ( ptTest.y <= ( rcTabBottomMiddleArea.top + nBorderMetric ) )
										: ( ptTest.y >= ( rcTabBottomMiddleArea.bottom - nBorderMetric ) )
										)
								||	ptTest.x < (rcTabBottomMiddleArea.left + nBorderMetric)
								||	nX >= (rcTabBottomMiddleArea.right - nBorderMetric)
								)
							)
							clrAdjustMix = clrAdjustBorder;
					else if(	rcMainArea.PtInRect( ptTest )
							&&	(
									( bTabsAtTop
										? ( ptTest.y > ( rcMainArea.bottom - nBorderMetric ) )
										: ( ptTest.y < ( rcMainArea.top + nBorderMetric ) )
										)
								||	ptTest.x < (rcMainArea.left + nBorderMetric)
								||	ptTest.x >= (rcMainArea.right - nBorderMetric)
								||	(
										( bTabsAtTop
											? ( ptTest.y <= ( rcMainArea.top + nBorderMetric ) )
											: ( ptTest.y >= ( rcMainArea.bottom - nBorderMetric ) )
											)
									&&	(	(!bTabShape)
										||	(	bTabShape
											&&	(	ptTest.x < (rcTabBottomMiddleArea.left + nBorderMetric )
												||	ptTest.x >= (rcTabBottomMiddleArea.right - nBorderMetric )
												)
											)
										)
									)
								)
							)
							clrAdjustMix = clrAdjustBorder;

				} // if( nBorderMetric > 0 )
				COLORREF clrTemp = pClrSurface[ nPixelIdx ];
				clrTemp =
					RGB(
						min(
							255,
							GetRValue( clrTemp )
								+ ( ( GetBValue( clrAdjustMix ) - GetRValue( clrTemp ) ) / 2 ) ),
						min(
							255,
							GetGValue( clrTemp )
								+ ( ( GetGValue( clrAdjustMix ) - GetGValue( clrTemp ) ) / 2 ) ),
						min(
							255,
							GetBValue( clrTemp )
								+ ( ( GetRValue( clrAdjustMix ) - GetBValue( clrTemp ) ) / 2 ) )
						);
				pClrSurface[ nPixelIdx ] =
					CExtBitmap::stat_HLS_Adjust(
						clrTemp,
						0.0,
						0.10,
						0.0
						);
			} // for( int nX = 0; nX < bih.biWidth; nX++, nPixelIdx++ )
		} // for( int nY = 0; nY < bih.biHeight; nY++ )
	} // if( stat_GetBPP() > 8 )
	else
	{
		COLORREF clrDark = GetColor( COLOR_3DSHADOW, this );
		unsigned nPixelIdx = 0;
		for( int nY = 0; nY < nClrSurfaceDY; nY++ )
		{
			for( int nX = 0; nX < nClrSurfaceDX; nX++, nPixelIdx++ )
			{
				if( bTabShape )
				{
					CPoint ptTest( nX, nClrSurfaceDY - nY );
					if(		(! rcTabMainArea.PtInRect( ptTest ) )
						&&	(! rcTabBottomMiddleArea.PtInRect( ptTest ) )
						)
						continue;
				} // if( bTabShape )
				if(		( (nY&0x01) != 0 && (nX&0x01) == 0 )
					||	( (nY&0x01) == 0 && (nX&0x01) != 0 )
					)
					pClrSurface[ nPixelIdx ] = clrDark;
			} // for( int nX = 0; nX < bih.biWidth; nX++, nPixelIdx++ )
		} // for( int nY = 0; nY < bih.biHeight; nY++ )
	} // else from if( stat_GetBPP() > 8 )
}

void CExtPaintManager::DockMarker_AdjustHighlightedLayer(
	HWND hWnd,
	int nDX,
	int nDY,
	bool bTabShape,
	bool bTabsAtTop,
	const CRect & rcTabMainArea,
	const CRect & rcTabBottomMiddleArea,
	COLORREF clrAdjustMain, // = COLORREF(-1L)
	COLORREF clrAdjustBorder, // = COLORREF(-1L)
	int nBorderMetric // = 2
	)
{
	ASSERT_VALID( this );
	ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
	ASSERT( nBorderMetric >= 0 );
	ASSERT( g_PaintManager.m_pfnUpdateLayeredWindow != NULL );
	ASSERT( nDX > 0 );
	ASSERT( nDY > 0 );

CWindowDC dcDesktop(NULL);
BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = nDX;
	bih.biHeight = nDY;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = bih.biWidth*bih.biHeight;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
COLORREF * pClrSurface = NULL;
HBITMAP hDIB =
		::CreateDIBSection(
			dcDesktop.GetSafeHdc(),
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)&pClrSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pClrSurface != NULL );
CBitmap _bmpLayerData;
	_bmpLayerData.Attach( hDIB );
CDC dc;
	if( ! dc.CreateCompatibleDC( &dcDesktop ) )
	{
		ASSERT( FALSE );
		return;
	}
HGDIOBJ hOld = dc.SelectObject( hDIB );

CRect rcMainArea( 0, 0, nDX, nDY );
	if( bTabShape )
		rcMainArea = rcTabMainArea;
	if( clrAdjustMain == COLORREF(-1L) )
		clrAdjustMain = ::GetSysColor( COLOR_HIGHLIGHT );
	if(		m_eStyle2005 == __ES2005_BETA2
		||	m_eStyle2005 == __ES2005_RC
		)
		clrAdjustMain = RGB(0,64,196);
	if( nBorderMetric > 0 && clrAdjustBorder == COLORREF(-1L) )
		clrAdjustBorder = ::GetSysColor( COLOR_ACTIVEBORDER );

unsigned nPixelIdx = 0;
	for( int nY = 0; nY < nDY; nY++ )
	{
		for( int nX = 0; nX < nDX; nX++, nPixelIdx++ )
		{
			if( bTabShape )
			{
				CPoint ptTest( nX, nDY - nY - 1 );
				if(		(! rcTabMainArea.PtInRect( ptTest ) )
					&&	(! rcTabBottomMiddleArea.PtInRect( ptTest ) )
					)
					continue;
			} // if( bTabShape )
			COLORREF clrAdjustMix = clrAdjustMain;
			if( nBorderMetric > 0 )
			{
				CPoint ptTest( nX, nDY - nY - 1 );
				if(			bTabShape
						&&	rcTabBottomMiddleArea.PtInRect( ptTest )
						&&	(
								( bTabsAtTop
									? ( ptTest.y <= ( rcTabBottomMiddleArea.top + nBorderMetric ) )
									: ( ptTest.y >= ( rcTabBottomMiddleArea.bottom - nBorderMetric ) )
									)
							||	ptTest.x < (rcTabBottomMiddleArea.left + nBorderMetric)
							||	nX >= (rcTabBottomMiddleArea.right - nBorderMetric)
							)
						)
						clrAdjustMix = clrAdjustBorder;
				else if(	rcMainArea.PtInRect( ptTest )
						&&	(
								( bTabsAtTop
									? ( ptTest.y > ( rcMainArea.bottom - nBorderMetric ) )
									: ( ptTest.y < ( rcMainArea.top + nBorderMetric ) )
									)
							||	ptTest.x < (rcMainArea.left + nBorderMetric)
							||	ptTest.x >= (rcMainArea.right - nBorderMetric)
							||	(
									( bTabsAtTop
										? ( ptTest.y <= ( rcMainArea.top + nBorderMetric ) )
										: ( ptTest.y >= ( rcMainArea.bottom - nBorderMetric ) )
										)
								&&	(	(!bTabShape)
									||	(	bTabShape
										&&	(	ptTest.x < (rcTabBottomMiddleArea.left + nBorderMetric )
											||	ptTest.x >= (rcTabBottomMiddleArea.right - nBorderMetric )
											)
										)
									)
								)
							)
						)
						clrAdjustMix = clrAdjustBorder;

			} // if( nBorderMetric > 0 )
			COLORREF clrTemp =
				//pClrSurface[ nPixelIdx ];
				0;
			clrTemp =
				RGB(
					min(
						255,
						GetRValue( clrTemp )
							+ ( ( GetBValue( clrAdjustMix ) - GetRValue( clrTemp ) ) / 2 ) ),
					min(
						255,
						GetGValue( clrTemp )
							+ ( ( GetGValue( clrAdjustMix ) - GetGValue( clrTemp ) ) / 2 ) ),
					min(
						255,
						GetBValue( clrTemp )
							+ ( ( GetRValue( clrAdjustMix ) - GetBValue( clrTemp ) ) / 2 ) )
					);
			clrTemp =
				__EXT_MFC_RGBA(
					GetRValue( clrTemp ),
					GetGValue( clrTemp ),
					GetBValue( clrTemp ),
					128
					);
			pClrSurface[ nPixelIdx ] = clrTemp;
		} // for( int nX = 0; nX < bih.biWidth; nX++, nPixelIdx++ )
	} // for( int nY = 0; nY < bih.biHeight; nY++ )

CExtPaintManager::CExtPaintManagerAutoPtr::__EXT_MFC_BLENDFUNCTION _bf =
{	__EXT_MFC_AC_SRC_OVER,
	0,
	255,
	__EXT_MFC_AC_SRC_ALPHA
};
CRect rcWnd;
	::GetWindowRect( hWnd, &rcWnd );
CPoint ptSrc( 0, 0 ), ptDst( rcWnd.TopLeft() );
CSize sizeDst = rcWnd.Size();
bool bRetVal =
		g_PaintManager.m_pfnUpdateLayeredWindow(
			hWnd,
			dcDesktop,
			&ptDst,
			&sizeDst,
			dc,
			&ptSrc,
			0,
			&_bf,
			__EXT_MFC_ULW_ALPHA
			) ? true : false;
//		ASSERT( bRetVal );
	bRetVal;

	dc.SelectObject( hOld );
}

#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

void CExtPaintManager::Header_GetButtonPadding(
	INT & nButtonPaddingBefore,
	INT & nButtonPaddingAfter,
	INT nColNo,
	INT nColCount,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nColNo; nColCount; pHelperSrc; lParam;
	nButtonPaddingBefore = nButtonPaddingAfter = 3;
}

INT CExtPaintManager::Header_GetDistanceBetweenIconAndSortArrow(
	INT nColNo,
	INT nColCount,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nColNo; nColCount; pHelperSrc; lParam;
	return 3;
}

INT CExtPaintManager::Header_GetDistanceBetweenIconAndText(
	INT nColNo,
	INT nColCount,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nColNo; nColCount; pHelperSrc; lParam;
	return 3;
}

INT CExtPaintManager::Header_GetDistanceBetweenTextAndButton(
	INT nColNo,
	INT nColCount,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nColNo; nColCount; pHelperSrc; lParam;
	return 3;
}

INT CExtPaintManager::Header_GetSortArrowWidth(
	INT nColNo,
	INT nColCount,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nColNo; nColCount; pHelperSrc; lParam;
	return 15;
}

CRect CExtPaintManager::Header_GetItemPadding(
	INT nColNo,
	INT nColCount,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	nColNo; nColCount; pHelperSrc; lParam;
	return CRect( 3, 2, 3, 2 );
}

void CExtPaintManager::Header_PaintBackground(
	CDC & dc,
	CRect rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
COLORREF clrHeaderBackground = GetColor( CLR_3DFACE_OUT, pHelperSrc, lParam );
	dc.FillSolidRect( rc, clrHeaderBackground );
}

void CExtPaintManagerXP::Header_PaintBackground(
	CDC & dc,
	CRect rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( IsHighContrast() )
	{
		CExtPaintManager::Header_PaintBackground( dc, rc, pHelperSrc, lParam );
		return;
	}
COLORREF clrHeaderBackground = GetColor( XPCLR_3DFACE_DARK, pHelperSrc, lParam );
	dc.FillSolidRect( rc, clrHeaderBackground );
}

bool CExtPaintManagerNativeXP::stat_PaintHeaderPartUsingUxTheme(
	CDC & dc,
	CRect rc,
	CObject * pHelperSrc,
	LPARAM lParam,
	bool bDrawItemLikeBackground, // = false
	bool bHover, // = false
	bool bPressed // = false
	)
{
	ASSERT( dc.GetSafeHdc() != NULL );
	lParam;
	if( pHelperSrc == NULL )
		return false;
	ASSERT_VALID( pHelperSrc );
	if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
		return false;
HWND hWnd = NULL;
	if( pHelperSrc->IsKindOf( RUNTIME_CLASS(CWnd) ) )
		hWnd = ((CWnd*)pHelperSrc)->GetSafeHwnd();
	if( hWnd == NULL )
		hWnd = ::GetDesktopWindow();
	if( hWnd == NULL )
		return false;
	if(	! g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_HEADER ) != NULL )
		return false;
bool bRetVal =
		( g_PaintManager.m_UxTheme.DrawThemeBackground(
			dc.GetSafeHdc(), 
			HP_HEADERITEMRIGHT, 
			HIRS_NORMAL, 
			&rc, 
			&rc
			) == S_OK ) ? true : false;
	g_PaintManager.m_UxTheme.CloseThemeData( true );
	if( ! bRetVal )
		return false;
	if( ! bDrawItemLikeBackground )
		return true;
	if(	! g_PaintManager.m_UxTheme.OpenThemeData( hWnd, VSCLASS_HEADER ) != NULL )
		return false;
INT nStateID = HIS_NORMAL;
	if( bPressed )
		nStateID = HIS_PRESSED;
	else if( bHover )
		nStateID = HIS_HOT;
	bRetVal =
		( g_PaintManager.m_UxTheme.DrawThemeBackground(
			dc.GetSafeHdc(), 
			HP_HEADERITEM, 
			nStateID, 
			&rc, 
			&rc
			) == S_OK ) ? true : false;
	g_PaintManager.m_UxTheme.CloseThemeData( true );
	return bRetVal;
}

void CExtPaintManagerNativeXP::Header_PaintBackground(
	CDC & dc,
	CRect rc,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( IsHighContrast() )
	{
		CExtPaintManager::Header_PaintBackground( dc, rc, pHelperSrc, lParam );
		return;
	}
	if( ! stat_PaintHeaderPartUsingUxTheme( dc, rc, pHelperSrc, lParam ) )
		CExtPaintManager::Header_PaintBackground( dc, rc, pHelperSrc, lParam );
}

void CExtPaintManager::Header_PaintItem(
	CDC & dc,
	CRect rcItemEntire,
	CRect rcItemData,
	CRect rcIcon,
	CRect rcText,
	CRect rcSortArrow,
	CRect rcButton,
	CRect rcButtonIcon,
	bool bSorted,
	bool bSortedAscending,
	INT nColNo,
	INT nColCount,
	const CExtCmdIcon & iconItem,
	const CExtCmdIcon & iconButton,
	bool bHover,
	bool bPressed,
	bool bButtonEvent,
	__EXT_MFC_SAFE_LPCTSTR strItemText,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	nColNo; nColCount; pHelperSrc; lParam;
	rcItemData;
COLORREF clrLT1 = GetColor( COLOR_3DHIGHLIGHT, pHelperSrc, lParam );
COLORREF clrRB1 = GetColor( COLOR_3DDKSHADOW,  pHelperSrc, lParam );
COLORREF clrLT2 = GetColor( COLOR_3DFACE,      pHelperSrc, lParam );
COLORREF clrRB2 = GetColor( CLR_3DSHADOW_OUT,  pHelperSrc, lParam );
CRect rcDraw = rcItemEntire;
	if( ! rcButton.IsRectEmpty() )
		rcDraw.right = rcButton.left;
bool bSwapColors = false;
	if( (!bButtonEvent) && ( bHover || bPressed ) )
	{
		bSwapColors = bPressed;
		if( bPressed && bHover )
			dc.FillRect( &rcDraw, &m_brushDarkerDefault );
		else if( bPressed )
			dc.FillRect( &rcDraw, &m_brushDarkDefault );
		else if( bHover )
			dc.FillRect( &rcDraw, &m_brushLightDefault );
	}
COLORREF _clrLT1 = bSwapColors ? clrRB1 : clrLT1;
COLORREF _clrRB1 = bSwapColors ? clrLT1 : clrRB1;
COLORREF _clrLT2 = bSwapColors ? clrRB2 : clrLT2;
COLORREF _clrRB2 = bSwapColors ? clrLT2 : clrRB2;
	dc.Draw3dRect( &rcDraw, _clrLT1, _clrRB1 );
	rcDraw.DeflateRect( 1, 1, 1, 1 );
	dc.Draw3dRect( &rcDraw, _clrLT2, _clrRB2 );
	if(		(! rcButton.IsRectEmpty() )
		&&	dc.RectVisible( &rcButton )
		)
	{
		bSwapColors = false;
		rcDraw = rcButton;
		if( bButtonEvent && ( bHover || bPressed ) )
		{
			bSwapColors = bPressed;
			if( bPressed && bHover )
				dc.FillRect( &rcDraw, &m_brushDarkerDefault );
			else if( bPressed )
				dc.FillRect( &rcDraw, &m_brushDarkDefault );
			else if( bHover )
				dc.FillRect( &rcDraw, &m_brushLightDefault );
		}
		_clrLT1 = bSwapColors ? clrRB1 : clrLT1;
		_clrRB1 = bSwapColors ? clrLT1 : clrRB1;
		_clrLT2 = bSwapColors ? clrRB2 : clrLT2;
		_clrRB2 = bSwapColors ? clrLT2 : clrRB2;
		dc.Draw3dRect( &rcDraw, _clrLT1, _clrRB1 );
		rcDraw.DeflateRect( 1, 1, 1, 1 );
		dc.Draw3dRect( &rcDraw, _clrLT2, _clrRB2 );
		if(		(! iconButton.IsEmpty() )
			&&	(! rcButtonIcon.IsRectEmpty() )
			&&	dc.RectVisible( &rcButtonIcon )
			)
			iconButton.Paint( this, dc, rcButtonIcon );
	}
	if(		(! iconItem.IsEmpty() )
		&&	(! rcIcon.IsRectEmpty() )
		&&	dc.RectVisible( &rcIcon )
		)
		iconItem.Paint( this, dc, rcIcon );
	if(		bSorted
		&&	(! rcSortArrow.IsRectEmpty() )
		&&	dc.RectVisible( &rcSortArrow )
		)
	{
		COLORREF ColorValues[4] =
		{
			RGB(0,0,0),
			GetColor( COLOR_3DSHADOW,    pHelperSrc, lParam ),
			GetColor( COLOR_3DHIGHLIGHT, pHelperSrc, lParam ),
			GetColor( COLOR_3DDKSHADOW,  pHelperSrc, lParam ),
		};
		PaintGlyphCentered(
			dc,
			rcSortArrow,
			*( g_DockingCaptionGlyphs[ bSortedAscending ? __DCBT_ARROW_SORT_UP : __DCBT_ARROW_SORT_DOWN ] ),
			ColorValues
			);	
	}
INT nTextLen = ( LPCTSTR(strItemText) != NULL ) ? INT( _tcslen(LPCTSTR(strItemText)) ) : 0;
	if(		nTextLen > 0
		&&	(! rcText.IsRectEmpty() )
		&&	dc.RectVisible( &rcText )
		)
	{
		COLORREF clrTextColor = GetColor( CLR_TEXT_OUT, pHelperSrc, lParam );
		CFont * pItemFont = & m_FontNormal;
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		COLORREF clrOldText = dc.SetTextColor( clrTextColor );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strItemText), nTextLen, (LPRECT)&rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS, 0 );
		dc.SelectObject( pOldFont );
		dc.SetTextColor( clrOldText );
		dc.SetBkMode( nOldBkMode );
	}
}

void CExtPaintManagerXP::Header_PaintItem(
	CDC & dc,
	CRect rcItemEntire,
	CRect rcItemData,
	CRect rcIcon,
	CRect rcText,
	CRect rcSortArrow,
	CRect rcButton,
	CRect rcButtonIcon,
	bool bSorted,
	bool bSortedAscending,
	INT nColNo,
	INT nColCount,
	const CExtCmdIcon & iconItem,
	const CExtCmdIcon & iconButton,
	bool bHover,
	bool bPressed,
	bool bButtonEvent,
	__EXT_MFC_SAFE_LPCTSTR strItemText,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( IsHighContrast() )
	{
		CExtPaintManager::Header_PaintItem(
			dc, rcItemEntire, rcItemData, rcIcon, rcText, rcSortArrow, rcButton, rcButtonIcon,
			bSorted, bSortedAscending, nColNo, nColCount, iconItem, iconButton, bHover, bPressed, bButtonEvent,
			strItemText, pHelperSrc, lParam
			);
		return;
	}
COLORREF clrLT1 = GetColor( COLOR_3DHIGHLIGHT, pHelperSrc, lParam );
COLORREF clrRB1 = GetColor( COLOR_3DSHADOW,  pHelperSrc, lParam );
CRect rcDraw = rcItemEntire;
	if( ! rcButton.IsRectEmpty() )
		rcDraw.right = rcButton.left;
bool bDrawBorder = true;
	if( (!bButtonEvent) && ( bHover || bPressed ) )
	{
		bDrawBorder = !bPressed;
		if( bPressed && bHover )
			dc.FillRect( &rcDraw, &m_brushDarkerDefault );
		else if( bPressed )
			dc.FillRect( &rcDraw, &m_brushDarkDefault );
		else if( bHover )
			dc.FillRect( &rcDraw, &m_brushLighterDefault );
	}
	if( bDrawBorder )
		dc.Draw3dRect( &rcDraw, clrLT1, clrRB1 );
	if(		(! rcButton.IsRectEmpty() )
		&&	dc.RectVisible( &rcButton )
		)
	{
		bDrawBorder = true;
		rcDraw = rcButton;
		if( bButtonEvent && ( bHover || bPressed ) )
		{
			bDrawBorder = !bPressed;
			if( bPressed && bHover )
				dc.FillRect( &rcDraw, &m_brushDarkerDefault );
			else if( bPressed )
				dc.FillRect( &rcDraw, &m_brushDarkDefault );
			else if( bHover )
				dc.FillRect( &rcDraw, &m_brushLighterDefault );
		}
		if( bDrawBorder )
			dc.Draw3dRect( &rcDraw, clrLT1, clrRB1 );
		if(		(! iconButton.IsEmpty() )
			&&	(! rcButtonIcon.IsRectEmpty() )
			&&	dc.RectVisible( &rcButtonIcon )
			)
			iconButton.Paint( this, dc, rcButtonIcon );
	}
	if(		(! iconItem.IsEmpty() )
		&&	(! rcIcon.IsRectEmpty() )
		&&	dc.RectVisible( &rcIcon )
		)
		iconItem.Paint( this, dc, rcIcon );
	if(		bSorted
		&&	(! rcSortArrow.IsRectEmpty() )
		&&	dc.RectVisible( &rcSortArrow )
		)
	{
		COLORREF clr =
			GetColor(
				( bPressed && (!bButtonEvent) ) ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW,
				pHelperSrc,
				lParam
				);
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			clr,
		};
		PaintGlyphCentered(
			dc,
			rcSortArrow,
			*( g_DockingCaptionGlyphs[ bSortedAscending ? __DCBT_ARROW_UP : __DCBT_ARROW_DOWN ] ),
			ColorValues
			);	
	}
INT nTextLen = ( LPCTSTR(strItemText) != NULL ) ? INT( _tcslen(LPCTSTR(strItemText)) ) : 0;
	if(		nTextLen > 0
		&&	(! rcText.IsRectEmpty() )
		&&	dc.RectVisible( &rcText )
		)
	{
		COLORREF clrTextColor = GetColor( CLR_TEXT_OUT, pHelperSrc, lParam );
		CFont * pItemFont = & m_FontNormal;
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		COLORREF clrOldText = dc.SetTextColor( clrTextColor );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strItemText), nTextLen, (LPRECT)&rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS, 0 );
		dc.SelectObject( pOldFont );
		dc.SetTextColor( clrOldText );
		dc.SetBkMode( nOldBkMode );
	}
}

void CExtPaintManagerNativeXP::Header_PaintItem(
	CDC & dc,
	CRect rcItemEntire,
	CRect rcItemData,
	CRect rcIcon,
	CRect rcText,
	CRect rcSortArrow,
	CRect rcButton,
	CRect rcButtonIcon,
	bool bSorted,
	bool bSortedAscending,
	INT nColNo,
	INT nColCount,
	const CExtCmdIcon & iconItem,
	const CExtCmdIcon & iconButton,
	bool bHover,
	bool bPressed,
	bool bButtonEvent,
	__EXT_MFC_SAFE_LPCTSTR strItemText,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( IsHighContrast() )
	{
		CExtPaintManager::Header_PaintItem(
			dc, rcItemEntire, rcItemData, rcIcon, rcText, rcSortArrow, rcButton, rcButtonIcon,
			bSorted, bSortedAscending, nColNo, nColCount, iconItem, iconButton, bHover, bPressed, bButtonEvent,
			strItemText, pHelperSrc, lParam
			);
		return;
	}
COLORREF clrLT1 = GetColor( COLOR_3DHIGHLIGHT, pHelperSrc, lParam );
COLORREF clrRB1 = GetColor( COLOR_3DSHADOW,  pHelperSrc, lParam );
CRect rcDraw = rcItemEntire;
	if( ! rcButton.IsRectEmpty() )
		rcDraw.right = rcButton.left;
	if( ! stat_PaintHeaderPartUsingUxTheme(
			dc, rcDraw, pHelperSrc, lParam, true,
			bHover && (!bButtonEvent),
			bPressed && (!bButtonEvent) )
			)
	{
		bool bDrawBorder = true;
		if( (!bButtonEvent) && ( bHover || bPressed ) )
		{
			bDrawBorder = !bPressed;
			if( bPressed && bHover )
				dc.FillRect( &rcDraw, &m_brushDarkerDefault );
			else if( bPressed )
				dc.FillRect( &rcDraw, &m_brushDarkDefault );
			else if( bHover )
				dc.FillRect( &rcDraw, &m_brushLighterDefault );
		}
		if( bDrawBorder )
			dc.Draw3dRect( &rcDraw, clrLT1, clrRB1 );
	}
	if(		(! rcButton.IsRectEmpty() )
		&&	dc.RectVisible( &rcButton )
		)
	{
		rcDraw = rcButton;
		if( ! stat_PaintHeaderPartUsingUxTheme(
				dc, rcDraw, pHelperSrc, lParam, true,
				bHover && bButtonEvent,
				bPressed && bButtonEvent
				)
			)
		{
			bool bDrawBorder = true;
			if( bButtonEvent && ( bHover || bPressed ) )
			{
				bDrawBorder = !bPressed;
				if( bPressed && bHover )
					dc.FillRect( &rcDraw, &m_brushDarkerDefault );
				else if( bPressed )
					dc.FillRect( &rcDraw, &m_brushDarkDefault );
				else if( bHover )
					dc.FillRect( &rcDraw, &m_brushLighterDefault );
			}
			if( bDrawBorder )
				dc.Draw3dRect( &rcDraw, clrLT1, clrRB1 );
		}
		if(		(! iconButton.IsEmpty() )
			&&	(! rcButtonIcon.IsRectEmpty() )
			&&	dc.RectVisible( &rcButtonIcon )
			)
			iconButton.Paint( this, dc, rcButtonIcon );
	}
	if(		(! iconItem.IsEmpty() )
		&&	(! rcIcon.IsRectEmpty() )
		&&	dc.RectVisible( &rcIcon )
		)
		iconItem.Paint( this, dc, rcIcon );
	if(		bSorted
		&&	(! rcSortArrow.IsRectEmpty() )
		&&	dc.RectVisible( &rcSortArrow )
		)
	{
		COLORREF ColorValues[2] =
		{
			RGB(0,0,0),
			GetColor( COLOR_3DSHADOW, pHelperSrc, lParam ),
		};
		PaintGlyphCentered(
			dc,
			rcSortArrow,
			*( g_DockingCaptionGlyphs[ bSortedAscending ? __DCBT_ARROW_UP : __DCBT_ARROW_DOWN ] ),
			ColorValues
			);	
	}
INT nTextLen = ( LPCTSTR(strItemText) != NULL ) ? INT( _tcslen(LPCTSTR(strItemText)) ) : 0;
	if(		nTextLen > 0
		&&	(! rcText.IsRectEmpty() )
		&&	dc.RectVisible( &rcText )
		)
	{
		COLORREF clrTextColor = GetColor( CLR_TEXT_OUT, pHelperSrc, lParam );
		CFont * pItemFont = & m_FontNormal;
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		COLORREF clrOldText = dc.SetTextColor( clrTextColor );
		CFont * pOldFont = dc.SelectObject( pItemFont );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strItemText), nTextLen, (LPRECT)&rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS, 0 );
		dc.SelectObject( pOldFont );
		dc.SetTextColor( clrOldText );
		dc.SetBkMode( nOldBkMode );
	}
}

bool CExtPaintManager::PaintGroupBoxLabel(
	CDC & dc,
	__EXT_MFC_SAFE_LPCTSTR strText,
	COLORREF clrText,
	HFONT hFont,
	DWORD dwDrawTextFlags,
	bool bEnabled,
	const RECT & rcText,
	CObject * pHelperSrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	lParam;
CWnd * pWnd = DYNAMIC_DOWNCAST( CWnd, pHelperSrc );
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );
HGDIOBJ hOldFont = NULL;
	if( hFont != NULL )
		hOldFont = ::SelectObject( dc, (HGDIOBJ)hFont );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	if( clrText == COLORREF(-1L) )
		clrText = QueryObjectTextColor( dc, bEnabled, false, false, false, pHelperSrc );
CRect _rcText = rcText;
	if(		(! g_PaintManager.m_UxTheme.IsControlsThemed()) 
		||	clrText != COLORREF(-1L)
		)
	{
		if( clrText == COLORREF(-1L) )
			clrText = GetColor( bEnabled ? COLOR_BTNTEXT : CLR_TEXT_DISABLED, pHelperSrc );
		COLORREF clrOldText = dc.SetTextColor( clrText );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strText), &_rcText, dwDrawTextFlags, 0 );
		dc.SetTextColor( clrOldText );
	} // if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	else
	{
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC, LPCTSTR(strText), &_rcText, dwDrawTextFlags, 0,
			pWnd->GetSafeHwnd(), (LPCVOID)VSCLASS_BUTTON, 0, int(BP_GROUPBOX), bEnabled ? int(GBS_NORMAL) : int(GBS_DISABLED)
			);
	} // else from if( ! g_PaintManager.m_UxTheme.IsControlsThemed() )
	dc.SetBkMode( nOldBkMode );
	if( hOldFont != NULL )
		::SelectObject( dc, hOldFont );
	return false;
}




