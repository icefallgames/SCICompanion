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

#if (!defined __EXT_PMSYNCHRONIZESINK_H)
#define __EXT_PMSYNCHRONIZESINK_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __AFXTEMPL_H__)
	#include <AfxTempl.h>
#endif

struct __PROF_UIS_API CExtPmSynchronizeSink
{
protected:
	bool m_bPmSynchronizeSinkIsSynchronizing:1;
	DWORD m_dwPmSynchronizeSinkThreadID;
public:
	CExtPmSynchronizeSink();
	virtual ~CExtPmSynchronizeSink();
	virtual DWORD PmSynchronizeSink_GetThreadID();
	virtual bool PmSynchronizeSink_IsSynchronizing();
	virtual bool PmSynchronizeSink_IsEqual(
		CExtPmSynchronizeSink * pOther
		);
	virtual bool PmSynchronizeSink_PerformSynchronizationWith(
		CExtPmSynchronizeSink * pPmSynchronizeSink,
		bool bRegister = true,
		bool bSynchronizeItNow = true
		) = 0;
	virtual LPSTREAM PmSynchronizeSink_GetData(
		HGLOBAL hGlobal = NULL
		) = 0;
	virtual void PmSynchronizeSink_SynchronizeFrom(
		LPSTREAM pStream
		) = 0;
protected:
	CTypedPtrArray < CPtrArray, CExtPmSynchronizeSink * > m_arrPmSynchronizeSink;
	INT GetIndexOfPmSynchronizeSink(
		CExtPmSynchronizeSink * pPmSynchronizeSink
		);
public:
	virtual void PmSynchronizeSink_SynchronizeOthers(
		HGLOBAL hGlobal = NULL
		);
}; // struct CExtPmSynchronizeSink

#endif // __EXT_PMSYNCHRONIZESINK_H

