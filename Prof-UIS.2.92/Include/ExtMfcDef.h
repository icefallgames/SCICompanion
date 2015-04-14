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

#if (!defined __EXT_MFC_DEF_H)
#define __EXT_MFC_DEF_H

#define __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES (0x0000FFFFL)

// disable warning 4201
#pragma warning( push )
#pragma warning ( disable : 4201 )
	#include <mmsystem.h>
// rollback warning 4201
#pragma warning( pop )

#include <stdlib.h>
#include <math.h>

// __PROF_UIS_VERSION_DWORD is required for static library builds
#define __PROF_UIS_VERSION_DWORD    DWORD( 0x02090200 )
#define __PROF_UIS_VERSION                 0x02090200

// the multiple monitor stub can be included into the library
// builds (used in ExtPaintManager.cpp to define
// COMPILE_MULTIMON_STUBS symbol before include multimon.h)
#if ( (defined _AFXDLL) || ( _MFC_VER >= 0x0A00 ) )
	#define __PROF_UIS_BUILTIN_MULTIMON_STUB
#endif

#if _MFC_VER <= 0x710
	#pragma message("   Prof-UIS multiple monitor support:")
	#if (defined __PROF_UIS_BUILTIN_MULTIMON_STUB)
		#pragma message("      built-in")
	#else
		#pragma message("      external")
	#endif // __PROF_UIS_BUILTIN_MULTIMON_STUB
#endif

// Prof-UIS basic library
#if (defined _AFXDLL)
	#if (defined __STATPROFUIS_WITH_DLLMFC__)

		#if (defined __PROF_UIS_FOR_REGULAR_DLL)
			#define __PROF_UIS_PERFORMS_STATE_MANAGING
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292yud-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ymd-RDE.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ynd-RDE.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292yu-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ym-RDE.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292yn-RDE.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#else
			#define __PROF_UIS_MANAGE_STATE AFX_MANAGE_STATE( ::AfxGetAppModuleState() )
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292yud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ymd.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ynd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292yu.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ym.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292yn.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#endif // else from (defined __PROF_UIS_FOR_REGULAR_DLL)

	#else

		#define __PROF_UIS_PERFORMS_STATE_MANAGING

#if (defined __PROF_UIS_IMPL__)
		#define __PROF_UIS_STATE_MANAGER_DECL _declspec(dllexport)
#else
		#define __PROF_UIS_STATE_MANAGER_DECL _declspec(dllimport)
#endif // (defined __PROF_UIS_IMPL__)

		#if (defined __PROF_UIS_FOR_REGULAR_DLL)
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292ud-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ud-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292md-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292md-RDE.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292nd-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292nd-RDE.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292u-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292u-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292m-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292m-RDE.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292n-RDE.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292n-RDE.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#else
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292ud.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292ud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292md.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292md.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292nd.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292nd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292u.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292u.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292m.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292m.lib"
				#else
					#define __PROF_UIS_MODULE_NAME	_T("ProfUIS292n.dll")
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292n.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#endif // else from (defined __PROF_UIS_FOR_REGULAR_DLL)
	#endif // __STATPROFUIS_WITH_DLLMFC__
#else
		#if (defined __PROF_UIS_FOR_REGULAR_DLL)
			#define __PROF_UIS_PERFORMS_STATE_MANAGING
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292sud-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292smd-RDE.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292snd-RDE.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292su-RDE.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292sm-RDE.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292sn-RDE.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#else
			#define __PROF_UIS_MANAGE_STATE AFX_MANAGE_STATE( ::AfxGetAppModuleState() )
			#if (defined _DEBUG)
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292sud.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292smd.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292snd.lib"
				#endif // _UNICODE
			#else
				#if (defined _UNICODE)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292su.lib"
				#elif (defined _MBCS)
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292sm.lib"
				#else
					#define __PROF_UIS_LIB_NAME		   "ProfUIS292sn.lib"
				#endif // _UNICODE
			#endif // _DEBUG
		#endif // else from (defined __PROF_UIS_FOR_REGULAR_DLL)
#endif // _AFXDLL

#if (defined __PROF_UIS_IMPL__)
	#if (!defined __EXT_PROFUIS_STATIC_LINK)
		#define __PROF_UIS_API _declspec(dllexport)
	#else
		#define __PROF_UIS_API
	#endif // _AFXDLL
#else 
	#if (!defined __EXT_PROFUIS_STATIC_LINK)
		#define __PROF_UIS_API _declspec(dllimport)
	#else
		#define __PROF_UIS_API
	#endif // _AFXDLL

	#pragma message("   Automatically linking with Prof-UIS library: " __PROF_UIS_LIB_NAME )
	#pragma message("      (Professional User Interface Suite)")
	#pragma comment( lib, __PROF_UIS_LIB_NAME ) 
#endif

#if (defined __PROF_UIS_PERFORMS_STATE_MANAGING)

	#if (! defined __PROF_UIS_STATE_MANAGER_DECL)
		#define __PROF_UIS_STATE_MANAGER_DECL
	#endif

	struct AFX_MAINTAIN_STATE2;
	class AFX_MODULE_STATE;

	class __PROF_UIS_STATE_MANAGER_DECL CExt_ProfUIS_ModuleState
	{
		static AFX_MODULE_STATE * g_pExternalModuleState;
		AFX_MAINTAIN_STATE2 * m_pAfxState;
	public:
		CExt_ProfUIS_ModuleState();
		virtual ~CExt_ProfUIS_ModuleState();
		static void InitExtension(
			AFX_MODULE_STATE * pExternalModuleState = NULL
			);
	}; // class CExt_ProfUIS_ModuleState
	#define __PROF_UIS_MANAGE_STATE CExt_ProfUIS_ModuleState the_ProfUIS_ModuleState

#endif // __PROF_UIS_PERFORMS_STATE_MANAGING

// Prof-UIS registry names
#define __PROF_UIS_REG_SECTION							_T("ProfUIS")
#define __PROF_UIS_REG_PROFILES							_T("Profiles")
#define __PROF_UIS_REG_COMMAND_MANAGER					_T("CommandManager")
#define __PROF_UIS_REG_CONTROL_BAR						_T("ControlBar")
#define __PROF_UIS_REG_DBS								_T("DynamicBarSite")
#define __PROF_UIS_REG_CUSTOMIZATION					_T("Customization")
#define __PROF_UIS_REG_REPORT_GRID						_T("ReportGrid")
#define __PROF_UIS_REG_PAINT_MANAGER					_T("PaintManager")
#define __PROF_UIS_REG_GRID_FILTER						_T("GridFilter")
#define __PROF_UIS_REG_REPORT_GRID_FILTER				_T("ReportGridFilter")
#define __PROF_UIS_REG_SHELL_ITEM_ID					_T("ShellItem")
#define __PROF_UIS_REG_SHELL_FILE_DIALOG_INTERRIOR		_T("FileDialogInterrior")

/////////////////////////////////////////////////////////////////////////
// Workaround for the 4996 warning ( 'function' was declared deprecated )
// in MFC version 8.00
/////////////////////////////////////////////////////////////////////////

inline void __EXT_MFC_MEMCPY(
	void * dest,
	size_t sizeInBytes,
	const void * src,
	size_t count 
	) 
{
	#if _MFC_VER >= 0x0800
		memcpy_s( dest, sizeInBytes, src, count );
	#else
		memcpy( dest, src, count );
		sizeInBytes;
	#endif
}

inline void __EXT_MFC_MEMMOVE(
	void * dest,
	size_t sizeInBytes,
	const void * src,
	size_t count 
	) 
{
	#if _MFC_VER >= 0x0800
		memmove_s( dest, sizeInBytes, src, count );
	#else
		memmove( dest, src, count );
		sizeInBytes;
	#endif
}

inline void __EXT_MFC_LTOA(
	LONG nValue,
	TCHAR * strBuffer, 
	size_t nSizeOfStr, 
	INT nRadix
	) 
{
	#if _MFC_VER >= 0x0800
		_ltot_s( nValue, strBuffer, nSizeOfStr, nRadix );
	#else
		_ltot( nValue, strBuffer, nRadix );
		nSizeOfStr;
	#endif
}

inline void __EXT_MFC_ULTOA(
	ULONG nValue,
	TCHAR * strBuffer, 
	size_t nSizeOfStr, 
	INT nRadix
	) 
{
	#if _MFC_VER >= 0x0800
		_ultot_s( nValue, strBuffer, nSizeOfStr, nRadix );
	#else
		_ultot( nValue, strBuffer, nRadix );
		nSizeOfStr;
	#endif
}

inline void __EXT_MFC_ITOA(
	INT nValue,
	TCHAR * strBuffer, 
	size_t nSizeInCharacters, 
	INT nRadix
	) 
{
	#if _MFC_VER >= 0x0800
		_itot_s( nValue, strBuffer, nSizeInCharacters, nRadix );
	#else
		_itot( nValue, strBuffer, nRadix );
		nSizeInCharacters;
	#endif
}

inline void __EXT_MFC_I64TOA(
	__int64 nValue,
	TCHAR * strBuffer, 
	size_t nSizeInCharacters, 
	INT nRadix
	) 
{
	#if _MFC_VER >= 0x0800
		_i64tot_s( nValue, strBuffer, nSizeInCharacters, nRadix );
	#else
		_i64tot( nValue, strBuffer, nRadix );
		nSizeInCharacters;
	#endif
}

inline void __EXT_MFC_UI64TOA(
	__int64 nValue,
	TCHAR * strBuffer, 
	size_t nSizeInCharacters, 
	INT nRadix
	) 
{
	#if _MFC_VER >= 0x0800
		_ui64tot_s( nValue, strBuffer, nSizeInCharacters, nRadix );
	#else
		_ui64tot( nValue, strBuffer, nRadix );
		nSizeInCharacters;
	#endif
}

inline void __EXT_MFC_STRCPY(
	TCHAR * strDest, 
	size_t nSize, 
	const TCHAR * strSource
	) 
{
	#if _MFC_VER >= 0x0800
		_tcscpy_s( strDest, nSize, strSource );
	#else
		_tcscpy( strDest, strSource );
		nSize;
	#endif
}

inline void __EXT_MFC_STRCAT(
	TCHAR * strDest, 
	size_t nSize, 
	const TCHAR * strSource
	) 
{
	#if _MFC_VER >= 0x0800
		_tcscat_s( strDest, nSize, strSource );
	#else
		_tcscat( strDest, strSource );
		nSize;
	#endif
}

inline void __EXT_MFC_STRLWR(
	TCHAR * str, 
	size_t nSize
	) 
{
	#if _MFC_VER >= 0x0800
		_tcslwr_s( str, nSize );
	#else
		_tcslwr( str );
		nSize;
	#endif
}

inline void __EXT_MFC_STRUPR(
	TCHAR * str, 
	size_t nSize
	) 
{
	#if _MFC_VER >= 0x0800
		_tcsupr_s( str, nSize );
	#else
		_tcsupr( str );
		nSize;
	#endif
}

#if _MFC_VER >= 0x0800
	#define __EXT_MFC_STSCANF _stscanf_s
	#define __EXT_MFC_STSCANF_PARAM( buffer, sizeOfBuffer ) buffer, sizeOfBuffer 
#else
	#define __EXT_MFC_STSCANF _stscanf
	#define __EXT_MFC_STSCANF_PARAM( buffer, sizeOfBuffer ) buffer
#endif

inline void __EXT_MFC_STRNCPY(
	TCHAR * strDest, 
	size_t nSize, 
	const TCHAR * strSource, 
	size_t count
	) 
{
	#if _MFC_VER >= 0x0800
		_tcsncpy_s( strDest, nSize, strSource, count );
	#else
		_tcsncpy( strDest, strSource, count );
		nSize;
	#endif
}

inline wchar_t * __EXT_MFC_WCSLWR(
	wchar_t * str, 
	size_t nSize
	) 
{
	#if _MFC_VER >= 0x0800
		_wcslwr_s( str, nSize );
		return str;
	#else
		nSize;
		return _wcslwr( str );
	#endif
}

inline wchar_t * __EXT_MFC_WCSUPR(
	wchar_t * str, 
	size_t nSize
	) 
{
	#if _MFC_VER >= 0x0800
		_wcsupr_s( str, nSize );
		return str;
	#else
		nSize;
		return _wcsupr( str );
	#endif
}

inline wchar_t * __EXT_MFC_WCSCPY(
	wchar_t * strDest, 
	size_t nSize,
	const wchar_t * strSource 
	) 
{
	#if _MFC_VER >= 0x0800
		wcscpy_s( strDest, nSize, strSource );
		return strDest;
	#else
		nSize;
		return wcscpy( strDest, strSource );
	#endif
}

inline void __EXT_MFC_SPLITPATH(
	const TCHAR * path,
	TCHAR * drive,
	size_t driveSizeInCharacters,
	TCHAR * dir,
	size_t dirSizeInCharacters,
	TCHAR * fname,
	size_t nameSizeInCharacters,
	TCHAR * ext, 
	size_t extSizeInBytes	
	) 
{
	#if _MFC_VER >= 0x0800
		_tsplitpath_s( 
			path, 
			drive, driveSizeInCharacters, 
			dir, dirSizeInCharacters,
			fname, nameSizeInCharacters,
			ext, extSizeInBytes 
			);
	#else
		_tsplitpath( 
			path, 
			drive, 
			dir, 
			fname, 
			ext 
			);
		driveSizeInCharacters;
		dirSizeInCharacters;
		nameSizeInCharacters;
		extSizeInBytes;
	#endif
}

inline void __EXT_MFC_ITOA( int nVal, LPTSTR str, int nCountOfChars, int nRadix )
{
	#if _MFC_VER >= 0x0800
		_itot_s( nVal, str, nCountOfChars, nRadix );
	#else
		nCountOfChars;
		_itot( nVal, str, nRadix );
	#endif
}

inline void __EXT_MFC_SwapLeftRight( LPRECT pRect )
{
	ASSERT( pRect != NULL );
LONG nTmp = pRect->left;
	pRect->left = pRect->right;
	pRect->right = nTmp;
}

inline void __EXT_MFC_SwapLeftRight( RECT & _rect )
{
	__EXT_MFC_SwapLeftRight( &_rect );
}

#if _MFC_VER >= 0x0800
	#define __EXT_MFC_SPRINTF _stprintf_s
	#define __EXT_MFC_SPRINTF_PARAM( buffer, sizeOfBuffer ) buffer, sizeOfBuffer 
#else
	#define __EXT_MFC_SPRINTF _stprintf
	#define __EXT_MFC_SPRINTF_PARAM( buffer, sizeOfBuffer ) buffer
#endif

#if _MFC_VER >= 0x0800
	#define __EXT_MFC_VSPRINTF _vstprintf_s
	#define __EXT_MFC_VSPRINTF_PARAM( buffer, sizeOfBuffer ) buffer, sizeOfBuffer 
#else
	#define __EXT_MFC_VSPRINTF _vstprintf
	#define __EXT_MFC_VSPRINTF_PARAM( buffer, sizeOfBuffer ) buffer
#endif

// Prof-UIS native Unicode character type support for VC++ .NET 2002 or later
#if (!defined __ExtMfcSafeNativeTCHAR_H)
	#include <ExtMfcSafeNativeTCHAR.h>
#endif

#if (!defined _AFX_NO_OLE_SUPPORT)
	#if (!defined __AFXCTL_H__)
		#include <AfxCtl.h>     // MFC OLE Control support
	#endif
	#define DECLARE_PROF_UIS_WINDOW_METHODS \
		virtual bool OnQueryWindowEnabledState() const \
		{ \
			ASSERT_VALID( this ); \
			ASSERT_KINDOF( CWnd, this ); \
			ASSERT( GetSafeHwnd() != NULL ); \
			bool bEnabled = IsWindowEnabled() ? true : false; \
			CWnd * pWndParent = GetParent(); \
			if( pWndParent != NULL ) \
			{ \
				COleControl * pOleControl = \
					DYNAMIC_DOWNCAST( COleControl, pWndParent ); \
				if( pOleControl != NULL ) \
				{ \
					if( pOleControl->AmbientUserMode() == FALSE ) \
						return true; \
				} \
			} \
			return bEnabled; \
		}
#else
	#define DECLARE_PROF_UIS_WINDOW_METHODS \
		virtual bool OnQueryWindowEnabledState() const \
		{ \
			ASSERT_VALID( this ); \
			ASSERT_KINDOF( CWnd, this ); \
			ASSERT( GetSafeHwnd() != NULL ); \
			bool bEnabled = IsWindowEnabled() ? true : false; \
			return bEnabled; \
		}
#endif // _AFX_NO_OLE_SUPPORT

class CExtPaintManager;

class __PROF_UIS_API CExtPmBridge
{
public:
	CExtPmBridge()
	{
	}
	virtual ~CExtPmBridge()
	{
	}

	virtual void PmBridge_Install(
		bool bAddTail = true
		) = 0;
	virtual void PmBridge_Uninstall() = 0;
	virtual HWND PmBridge_GetSafeHwnd() const = 0;
	virtual CExtPaintManager * PmBridge_GetPM() const = 0;
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		)
	{
		CExtPaintManager * pLocalPM =
			PmBridge_GetPM();
		if( LPVOID(pGlobalPM) != LPVOID(pLocalPM) )
			return;
		HWND hWndOwn = PmBridge_GetSafeHwnd();
		if( hWndOwn == NULL )
			return;
		ASSERT( ::IsWindow( hWndOwn ) );
		::SetWindowPos(
			hWndOwn, NULL, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_FRAMECHANGED
			);
		::RedrawWindow(
			hWndOwn,
			NULL,
			NULL,
			RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN
			);
	}
	virtual void PmBridge_OnSysColorChange(
		CExtPaintManager * pGlobalPM,
		CWnd * pWndNotifySrc
		)
	{
		pGlobalPM;
		pWndNotifySrc;
	}
	virtual void PmBridge_OnSettingChange(
		CExtPaintManager * pGlobalPM,
		CWnd * pWndNotifySrc,
		UINT uFlags,
		__EXT_MFC_SAFE_LPCTSTR lpszSection
		)
	{
		pGlobalPM;
		pWndNotifySrc;
		uFlags;
		lpszSection;
	}
	virtual void PmBridge_OnDisplayChange(
		CExtPaintManager * pGlobalPM,
		CWnd * pWndNotifySrc,
		INT nDepthBPP,
		CPoint ptSizes
		)
	{
		pGlobalPM;
		pWndNotifySrc;
		nDepthBPP;
		ptSizes;
	}
	virtual void PmBridge_OnThemeChanged(
		CExtPaintManager * pGlobalPM,
		CWnd * pWndNotifySrc,
		WPARAM wParam,
		LPARAM lParam
		)
	{
		pGlobalPM;
		pWndNotifySrc;
		wParam;
		lParam;
	}
}; // class CExtPmBridge

#define DECLARE_CExtPmBridge_MEMBERS_GENERIC( __class_name__ ) \
	virtual void PmBridge_Install( \
		bool bAddTail = true \
		); \
	virtual void PmBridge_Uninstall(); \
	virtual CExtPaintManager * PmBridge_GetPM() const;

#define DECLARE_CExtPmBridge_MEMBERS( __class_name__ ) \
	DECLARE_CExtPmBridge_MEMBERS_GENERIC( __class_name__ ) \
	virtual HWND PmBridge_GetSafeHwnd() const;

#define IMPLEMENT_CExtPmBridge_MEMBERS_INSTALL_UNINSTALL( __class_name__ ) \
	void __class_name__ :: PmBridge_Install( \
		bool bAddTail /* = true */ \
		) \
	{ \
		g_PaintManager.Advise( \
			static_cast < CExtPmBridge * > ( this ), \
			bAddTail \
			); \
	} \
	void __class_name__ :: PmBridge_Uninstall() \
	{ \
		g_PaintManager.Unadvise( \
			static_cast < CExtPmBridge * > ( this ) \
			); \
	}

#define IMPLEMENT_CExtPmBridge_MEMBERS_GENERIC( __class_name__ ) \
	IMPLEMENT_CExtPmBridge_MEMBERS_INSTALL_UNINSTALL( __class_name__ ); \
	CExtPaintManager * __class_name__ :: PmBridge_GetPM() const \
	{ \
		return g_PaintManager.GetPM(); \
	}

#define IMPLEMENT_CExtPmBridge_MEMBERS( __class_name__ ) \
	IMPLEMENT_CExtPmBridge_MEMBERS_GENERIC( __class_name__ ); \
	HWND __class_name__ :: PmBridge_GetSafeHwnd() const \
	{ \
		return NULL; \
	}

#if (!defined LAYOUT_RTL)
	#define LAYOUT_RTL								0x00000001
#endif // (!defined LAYOUT_RTL)

#if (!defined LAYOUT_LTR)
	#define LAYOUT_LTR								0x00000000
#endif // (!defined LAYOUT_LTR)

#if (!defined NOMIRRORBITMAP)
	#define NOMIRRORBITMAP							0x80000000
#endif // (!defined NOMIRRORBITMAP)

#if (!defined LAYOUT_BITMAPORIENTATIONPRESERVED)
	#define LAYOUT_BITMAPORIENTATIONPRESERVED		0x80000000
#endif // (!defined LAYOUT_BITMAPORIENTATIONPRESERVED)

#if (!defined WS_EX_LAYOUTRTL)
	#define WS_EX_LAYOUTRTL							0x00400000L
#endif // (!defined WS_EX_LAYOUTRTL)

#if (!defined WS_EX_LAYOUT_RTL)
	#define WS_EX_LAYOUT_RTL						WS_EX_LAYOUTRTL
#endif // (!defined WS_EX_LAYOUT_RTL)

#if (!defined WS_EX_NOINHERITLAYOUT)
	#define WS_EX_NOINHERITLAYOUT					0x00100000L
#endif // (!defined WS_EX_NOINHERITLAYOUT)

#if (!defined WS_EX_NOINHERIT_LAYOUT)
	#define WS_EX_NOINHERIT_LAYOUT					WS_EX_NOINHERITLAYOUT
#endif // (!defined WS_EX_NOINHERIT_LAYOUT)

#if _MFC_VER <= 0x710
	#define __EXT_MFC_ON_WM_NCHITTEST() ON_WM_NCHITTEST()
#else
	#define __EXT_MFC_ON_WM_NCHITTEST() \
		{ WM_NCHITTEST, 0, 0, 0, AfxSig_l_p, \
			(AFX_PMSG)(AFX_PMSGW) \
			(static_cast < UINT (AFX_MSG_CALL CWnd::*)(CPoint) > (&ThisClass :: OnNcHitTest)) },
#endif

#if (!defined __EXT_MFC_INT_PTR)
	#if _MFC_VER < 0x700
		#define __EXT_MFC_INT_PTR					INT
		#define __EXT_MFC_UINT_PTR					UINT
		#define __EXT_MFC_LONG_PTR					LONG
		#define __EXT_MFC_ULONG_PTR					ULONG
		#define __EXT_MFC_ULONG_SIZE_T				ULONG
		#define __EXT_MFC_GetClassLong				GetClassLong
		#define __EXT_MFC_SetClassLong				SetClassLong
		#define __EXT_MFC_GetWindowLong				GetWindowLong
		#define __EXT_MFC_SetWindowLong				SetWindowLong
	#else
		#define __EXT_MFC_INT_PTR					INT_PTR
		#define __EXT_MFC_UINT_PTR					UINT_PTR
		#define __EXT_MFC_LONG_PTR					LONG_PTR
		#define __EXT_MFC_ULONG_PTR					ULONG_PTR
		#if defined(_WIN64)
			#define __EXT_MFC_ULONG_SIZE_T			SIZE_T
		#else
			#define __EXT_MFC_ULONG_SIZE_T			ULONG
		#endif
		#define __EXT_MFC_GetClassLong				GetClassLongPtr
		#define __EXT_MFC_SetClassLong				SetClassLongPtr
		#define __EXT_MFC_GetWindowLong				GetWindowLongPtr
		#define __EXT_MFC_SetWindowLong				SetWindowLongPtr
	#endif
	#define __EXT_MFC_DWORD_PTR						__EXT_MFC_ULONG_PTR
#endif

#if _MFC_VER >= 0x900
	#ifndef HRAWINPUT
		#define HRAWINPUT DWORD
	#endif
	struct tagMENUGETOBJECTINFO;
	typedef tagMENUGETOBJECTINFO MENUGETOBJECTINFO;
	typedef tagMENUGETOBJECTINFO * PMENUGETOBJECTINFO;
#endif

#if _MFC_VER < 0x900
	#define __EXT_MFC_IDC_STATIC					IDC_STATIC
#else
//	#define __EXT_MFC_IDC_STATIC					(1)
	#define __EXT_MFC_IDC_STATIC					(32766)
#endif

#if (!defined __EXT_MFC_GWL_WNDPROC)
	#define __EXT_MFC_GWL_WNDPROC					(-4)
	#define __EXT_MFC_GWL_HINSTANCE					(-6)
	#define __EXT_MFC_GWL_HWNDPARENT				(-8)
	#define __EXT_MFC_GWL_STYLE						(-16)
	#define __EXT_MFC_GWL_EXSTYLE					(-20)
	#define __EXT_MFC_GWL_USERDATA					(-21)
	#define __EXT_MFC_GWL_ID						(-12)
	#define __EXT_MFC_GCL_MENUNAME					(-8)
	#define __EXT_MFC_GCL_HBRBACKGROUND				(-10)
	#define __EXT_MFC_GCL_HCURSOR					(-12)
	#define __EXT_MFC_GCL_HICON						(-14)
	#define __EXT_MFC_GCL_HMODULE					(-16)
	#define __EXT_MFC_GCL_CBWNDEXTRA				(-18)
	#define __EXT_MFC_GCL_CBCLSEXTRA				(-20)
	#define __EXT_MFC_GCL_WNDPROC					(-24)
	#define __EXT_MFC_GCL_STYLE						(-26)
	#define __EXT_MFC_GCW_ATOM						(-32)
	#define __EXT_MFC_GCL_HICONSM					(-34)
#endif

#define __EXT_MFC_WM_MOUSEFIRST						0x0200
#define __EXT_MFC_WM_MOUSELAST						0x0209

#define __EXT_MFC_CX_BORDER							1
#define __EXT_MFC_CY_BORDER							1

#define __EXT_MFC_WM_NCUAHDRAWCAPTION				0xAE
#define __EXT_MFC_WM_NCUAHDRAWFRAME					0xAF

//			#if ( ! defined __EXT_MFC_MIIM_BITMAP )
//
//				#define __EXT_MFC_MIIM_BITMAP					0x00000080
//
//				#define __EXT_MFC_HBMMENU_CALLBACK              ((HBITMAP) -1)
//				#define __EXT_MFC_HBMMENU_SYSTEM                ((HBITMAP)  1)
//				#define __EXT_MFC_HBMMENU_MBAR_RESTORE          ((HBITMAP)  2)
//				#define __EXT_MFC_HBMMENU_MBAR_MINIMIZE         ((HBITMAP)  3)
//				#define __EXT_MFC_HBMMENU_MBAR_CLOSE            ((HBITMAP)  5)
//				#define __EXT_MFC_HBMMENU_MBAR_CLOSE_D          ((HBITMAP)  6)
//				#define __EXT_MFC_HBMMENU_MBAR_MINIMIZE_D       ((HBITMAP)  7)
//				#define __EXT_MFC_HBMMENU_POPUP_CLOSE           ((HBITMAP)  8)
//				#define __EXT_MFC_HBMMENU_POPUP_RESTORE         ((HBITMAP)  9)
//				#define __EXT_MFC_HBMMENU_POPUP_MAXIMIZE        ((HBITMAP) 10)
//				#define __EXT_MFC_HBMMENU_POPUP_MINIMIZE        ((HBITMAP) 11)
//
//				typedef struct __EXT_MFC_tagMENUITEMINFOA_V5
//				{
//					UINT    cbSize;
//					UINT    fMask;
//					UINT    fType;
//					UINT    fState;
//					UINT    wID;
//					HMENU   hSubMenu;
//					HBITMAP hbmpChecked;
//					HBITMAP hbmpUnchecked;
//					__EXT_MFC_ULONG_PTR   dwItemData;
//					LPSTR   dwTypeData;
//					UINT    cch;
//					HBITMAP hbmpItem;
//				}   __EXT_MFC_MENUITEMINFOA_V5, * __EXT_MFC_LPMENUITEMINFOA_V5;
//
//				typedef struct __EXT_MFC_tagMENUITEMINFOW_V5
//				{
//					UINT    cbSize;
//					UINT    fMask;
//					UINT    fType;
//					UINT    fState;
//					UINT    wID;
//					HMENU   hSubMenu;
//					HBITMAP hbmpChecked;
//					HBITMAP hbmpUnchecked;
//					__EXT_MFC_ULONG_PTR   dwItemData;
//					LPWSTR  dwTypeData;
//					UINT    cch;
//					HBITMAP hbmpItem;
//				}   __EXT_MFC_MENUITEMINFOW_V5, * __EXT_MFC_LPMENUITEMINFOW_V5;
//
//				#ifdef UNICODE
//					typedef __EXT_MFC_MENUITEMINFOW_V5       __EXT_MFC_MENUITEMINFO_V5;
//					typedef __EXT_MFC_LPMENUITEMINFOW_V5     __EXT_MFC_LPMENUITEMINFO_V5;
//				#else
//					typedef __EXT_MFC_MENUITEMINFOA_V5       __EXT_MFC_MENUITEMINFO_V5;
//					typedef __EXT_MFC_LPMENUITEMINFOA_V5     __EXT_MFC_LPMENUITEMINFO_V5;
//				#endif
//
//				typedef __EXT_MFC_MENUITEMINFOA_V5 CONST   * __EXT_MFC_LPCMENUITEMINFOA_V5;
//				typedef __EXT_MFC_MENUITEMINFOW_V5 CONST   * __EXT_MFC_LPCMENUITEMINFOW_V5;
//
//				#ifdef UNICODE
//					typedef __EXT_MFC_LPCMENUITEMINFOW_V5    __EXT_MFC_LPCMENUITEMINFO_V5;
//				#else
//					typedef __EXT_MFC_LPCMENUITEMINFOA_V5    __EXT_MFC_LPCMENUITEMINFO_V5;
//				#endif
//
//			#endif // ( ! defined __EXT_MFC_MIIM_BITMAP )

#endif // __EXT_MFC_DEF_H
