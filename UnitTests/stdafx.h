// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#include <afxcview.h>       // CListView

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

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#ifdef DEBUG
#define DOCSUPPORT
#endif

// Commonly included or large header files:
#include "sci.h"
#include "Stream.h"
#include "StlUtil.h"
#include "ResourceBlob.h"
#include "PicCommands.h"

// Headers for CppUnitTest
// This must come after all the AFX stuff, because:
// "The CRT libraries use weak external linkage for the new, delete, and DllMain functions.The MFC libraries also contain new, delete,
// and DllMain functions.These functions require the MFC libraries to be linked before the CRT library is linked."
#include "CppUnitTest.h"
