// SCICompanionLib.h : main header file for the SCICompanionLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSCICompanionLibApp
// See SCICompanionLib.cpp for the implementation of this class
//

class CSCICompanionLibApp : public CWinApp
{
public:
	CSCICompanionLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
