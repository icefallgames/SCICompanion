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

#if (!defined __EXT_PROF_UIS_ABOUT_DIALOG_H)
#define __EXT_PROF_UIS_ABOUT_DIALOG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_MFC_NO_PROF_UIS_ABOUT_DIALOG)

BOOL __PROF_UIS_API ProfUISAbout( 
	HWND hWndParent = NULL,
	__EXT_MFC_SAFE_LPCTSTR szApp = NULL,
	__EXT_MFC_SAFE_LPCTSTR szOtherStuff = NULL,
	HICON hIcon = NULL,
	bool bAutoDetectNullParms = true
	);

#endif // (!defined __EXT_MFC_NO_PROF_UIS_ABOUT_DIALOG)

#endif // !defined(__EXT_PROF_UIS_ABOUT_DIALOG_H)

