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

#if (!defined __EXT_LOCALIZATION_H)
#define __EXT_LOCALIZATION_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

class __PROF_UIS_API CExtLocalResourceHelper
{
protected:
	HINSTANCE m_hInstPrevResource;
	bool m_bFree:1;
public:
	static bool g_bUseResourceHandle;
protected:
	
	void Init(
		HINSTANCE hInstResourceModule
		)
	{
		ASSERT( m_bFree );
		ASSERT( hInstResourceModule != NULL );
		m_hInstPrevResource =
			::AfxGetResourceHandle();
		::AfxSetResourceHandle(
			hInstResourceModule
			);
		m_bFree = false;
	}

	void Init(
		__EXT_MFC_SAFE_LPCTSTR sResourceModulePath
		)
	{
		HINSTANCE hInstResourceModule =
			::GetModuleHandle(
				sResourceModulePath
				);
		Init( hInstResourceModule );
	}

public:

	CExtLocalResourceHelper()
		: m_hInstPrevResource( NULL )
		, m_bFree( true )
	{
#if (!defined __EXT_PROFUIS_STATIC_LINK)
		Init( __PROF_UIS_MODULE_NAME );
#else
		//Init( ::AfxGetInstanceHandle() );
		Init( g_bUseResourceHandle ? ::AfxGetResourceHandle() : ::AfxGetInstanceHandle() );
#endif // #if (!defined __EXT_PROFUIS_STATIC_LINK)
	}

	CExtLocalResourceHelper(
		HINSTANCE hInstResourceModule
		)
		: m_hInstPrevResource( NULL )
		, m_bFree( true )
	{
		Init( hInstResourceModule );
	}
	
	CExtLocalResourceHelper(
		__EXT_MFC_SAFE_LPCTSTR sResourceModulePath
		)
		: m_hInstPrevResource( NULL )
		, m_bFree( true )
	{
		Init( sResourceModulePath );
	}

	virtual ~CExtLocalResourceHelper()
	{
		Free();
	}
	
	void Free()
	{
		if( m_bFree )
			return;
		::AfxSetResourceHandle(
			m_hInstPrevResource
			);
		m_hInstPrevResource = NULL;
		m_bFree = true;
	}

}; // class CExtLocalResourceHelper

#endif // __EXT_LOCALIZATION_H
