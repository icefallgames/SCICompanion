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

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif

#ifndef __AFXSTATE_H__
	#include <AfxStat_.h>
#endif


#if (defined __PROF_UIS_PERFORMS_STATE_MANAGING)

#if _MFC_VER < 0x800
	#ifndef _AFXDLL

		class _AFX_THREAD_STATE;

		struct AFX_MAINTAIN_STATE2
		{
			AFX_MAINTAIN_STATE2(AFX_MODULE_STATE* pModuleState);
			~AFX_MAINTAIN_STATE2();

		protected:
			AFX_MODULE_STATE* m_pPrevModuleState;
			_AFX_THREAD_STATE* m_pThreadState;
		};

		AFX_MAINTAIN_STATE2::AFX_MAINTAIN_STATE2(AFX_MODULE_STATE* pNewState)
		{
			m_pThreadState = _afxThreadState;
			m_pPrevModuleState = m_pThreadState->m_pModuleState;
			m_pThreadState->m_pModuleState = pNewState;
		}

		AFX_MAINTAIN_STATE2::~AFX_MAINTAIN_STATE2()
		{
			m_pThreadState->m_pModuleState = m_pPrevModuleState;
		}

	#endif //_AFXDLL
#endif // _MFC_VER < 0x800

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
//	new CDynLinkLibrary( the_ProfUIS_ExtensionModule );
	g_pExternalModuleState = pExternalModuleState;
}

#endif // (defined __PROF_UIS_PERFORMS_STATE_MANAGING)

