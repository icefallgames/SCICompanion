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

#include "stdafx.h"
#include <afxdllx.h>

#ifndef __AFXSTATE_H__
	#include <AfxStat_.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE the_ProfUIS_ExtensionModule =
{
	NULL,
	NULL
};

extern "C" int APIENTRY DllMain(
	HINSTANCE hInstance,
	DWORD dwReason,
	LPVOID lpReserved
	)
{
	lpReserved;
	if( dwReason == DLL_PROCESS_ATTACH )
	{
		TRACE3(
			"   * * * INITIALIZING DYNAMIC LIBRARY: ProfUIS version %d.%d%d * * * \n",
			((__PROF_UIS_VERSION_DWORD >> 24) & 0XFF),
			((__PROF_UIS_VERSION_DWORD >> 16) & 0XFF),
			((__PROF_UIS_VERSION_DWORD >>  8) & 0XFF)
			);
		if( ! AfxInitExtensionModule(
				the_ProfUIS_ExtensionModule,
				hInstance
				)
			)
			return 0;
#if (!defined __PROF_UIS_FOR_REGULAR_DLL)
//		new CDynLinkLibrary( the_ProfUIS_ExtensionModule );
		CExt_ProfUIS_ModuleState::InitExtension();
#endif // (!defined __PROF_UIS_FOR_REGULAR_DLL)
	} // if( dwReason == DLL_PROCESS_ATTACH )
	else if( dwReason == DLL_PROCESS_DETACH )
	{
		TRACE3(
			"   * * * TERMINATING DYNAMIC LIBRARY: ProfUIS version %d.%d%d * * * \n",
			((__PROF_UIS_VERSION_DWORD >> 24) & 0XFF),
			((__PROF_UIS_VERSION_DWORD >> 16) & 0XFF),
			((__PROF_UIS_VERSION_DWORD >>  8) & 0XFF)
			);
		AfxTermExtensionModule( the_ProfUIS_ExtensionModule );
	} // else if( dwReason == DLL_PROCESS_DETACH )
	return 1;   // ok
}

#if (defined __PROF_UIS_PERFORMS_STATE_MANAGING)

AFX_MODULE_STATE * CExt_ProfUIS_ModuleState::g_pExternalModuleState = NULL;

CExt_ProfUIS_ModuleState::CExt_ProfUIS_ModuleState()
{
	m_pAfxState = NULL;
	if( g_pExternalModuleState != NULL )
		m_pAfxState = new AFX_MAINTAIN_STATE2( g_pExternalModuleState );
}

CExt_ProfUIS_ModuleState::~CExt_ProfUIS_ModuleState()
{
	if( m_pAfxState != NULL )
		delete m_pAfxState;
}

void CExt_ProfUIS_ModuleState::InitExtension(
	AFX_MODULE_STATE * pExternalModuleState // = NULL
	)
{
	new CDynLinkLibrary( the_ProfUIS_ExtensionModule );
	g_pExternalModuleState = pExternalModuleState;
}

#endif // (defined __PROF_UIS_PERFORMS_STATE_MANAGING)



