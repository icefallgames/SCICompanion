// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0501	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxcview.h>       // CListView

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxpriv.h> // For WM_IDLEUPDATECMDUI, WM_SIZEPARENT
#include <afxole.h> // For crystal edit

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <shlwapi.h>

#include <gdiplus.h>
#include <Gdipluspixelformats.h>
#include <afxdlgs.h>

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <iostream>
#include <functional>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <stack>
#include <cassert>
#include <typeinfo>
#include <typeindex>
#include <thread>
#include <mutex>
#include <condition_variable>


// REVIEW: Things I need to add to make it compile with vs2013:
// zero fill everything after the vtbl pointer
#define AFX_ZERO_INIT_OBJECT(base_class) \
memset(((base_class*)this)+1, 0, sizeof(*this) - sizeof(class base_class));

// Deprecated warnings caused by shlwapi
#pragma warning(disable: 4995)

// decorated name length exceeded
#pragma warning(disable: 4503)

// for deleting values in a map:
struct delete_map_value
{
    template<typename TKEY, typename TVALUE>
    void operator()(const std::pair<TKEY, TVALUE> &ptr) const
    {
        delete ptr.second;
    }
};


#include <afxtempl.h>


#define BEGIN_TEMPLATE_MESSAGE_MAP_2(theClass, type_name1, type_name2, baseClass)			\
	PTM_WARNING_DISABLE														\
	template < typename type_name1, typename type_name2 >											\
	const AFX_MSGMAP* theClass< type_name1, type_name2 >::GetMessageMap() const			\
		{ return GetThisMessageMap(); }										\
	template < typename type_name1, typename type_name2 >											\
	const AFX_MSGMAP* PASCAL theClass< type_name1, type_name2 >::GetThisMessageMap()		\
	{																		\
		typedef theClass< type_name1, type_name2 > ThisClass;							\
		typedef baseClass TheBaseClass;										\
		static const AFX_MSGMAP_ENTRY _messageEntries[] =					\
		{


// Additional defines so we can use multi-value templates with BEGIN_TEMPLATE_MESSAGE_MAP
#define TEMPLATE_1(t1)                   t1
#define TEMPLATE_2(t1, t2)               t1, t2
#define TEMPLATE_3(t1 ,t2 ,t3)           t1, t2, t3
#define TCLASS_1(theClass, t1)           theClass<t1>
#define TCLASS_2(theClass, t1, t2)       theClass<t1, t2>
#define TCLASS_3(theClass, t1, t2, t3)   theClass<t1, t2, t3>


#include <Prof-UIS.h>

// Commonly included or large header files:
#include "sci.h"
#include "Version.h"
#include "Stream.h"
#include "StlUtil.h"
#include "CObjectWrap.h"
