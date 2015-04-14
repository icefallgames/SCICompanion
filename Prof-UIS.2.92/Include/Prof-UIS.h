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

#if (!defined __PROF_UIS_H)
#define __PROF_UIS_H

//	#if (defined __PROF_UIS_IMPL__ )
//	
//		#pragma message(" *** ")
//		#pragma message(" *** You cannot compile Prof-UIS when using the evaluation version.")
//		#pragma message(" *** To buy the full version please visit http://www.prof-uis.com/purchase.aspx")
//		#pragma message(" *** ")
//	
//		#error "You cannot compile Prof-UIS when using the evaluation version."
//	
//	#else
//	
//		#if ( ( (!defined _MBCS) && (!defined _UNICODE) ) || (!defined _AFXDLL) || (defined __STATPROFUIS_WITH_DLLMFC__) )
//	
//			#pragma message(" *** ")
//			#pragma message(" *** You can compile only Win32 MBCS/Unicode Debug/Release configurations of any sample projects when using the evaluation version.")
//			#pragma message(" *** To buy the full version please visit http://www.prof-uis.com/purchase.aspx")
//			#pragma message(" *** ")
//	
//			#error "You can compile only Win32 MBCS/Unicode Debug/Release configurations of any sample projects when using the evaluation version."
//	
//		#endif
//	
//	#endif

#if (!defined _AFXDLL || defined __STATPROFUIS_WITH_DLLMFC__)
	#define __EXT_PROFUIS_STATIC_LINK
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif

#ifndef __AFXWIN_H__
	#include <AfxWin.h>         // MFC core and standard components
#endif

#ifndef __AFXEXT_H__
	#include <afxext.h>         // MFC extensions
#endif

#ifndef _AFX_NO_OLE_SUPPORT
	#ifndef __AFXOLE_H__
		#include <AfxOle.h>     // MFC OLE classes
	#endif
	#ifndef __AFXODLGS_H__
		#include <AfxOdlgs.h>   // MFC OLE dialog classes
	#endif
	#ifndef __AFXDISP_H__
		#include <AfxDisp.h>    // MFC Automation classes
	#endif
#endif // _AFX_NO_OLE_SUPPORT

#ifndef __AFXTEMPL_H__
	#include <AfxTempl.h>
#endif

#ifndef __AFXMT_H__
	#include <AfxMT.h>
#endif

#ifndef __AFXCMN_H__
	#include <AfxCmn.h>
#endif

#ifndef __AFXDTCTL_H__
	#include <AfxDtCtl.h>
#endif

#ifndef __AFXCONV_H__
	#include <AfxConv.h>
#endif

//trial/commercial features
#define __EXT_MFC_NO_TAB_CTRL
#define __EXT_MFC_NO_TABMDI_CTRL
#define __EXT_MFC_NO_TABFLAT_CTRL
#define __EXT_MFC_NO_TABBUTTONS_CTRL
#define __EXT_MFC_NO_TAB_ONENOTE_CTRL
#define __EXT_MFC_NO_TAB_WHIDBEY_CTRL
#define __EXT_MFC_NO_TAB_CONTROLBARS
#define __EXT_MFC_NO_TOOLBOX_CTRL
#define __EXT_MFC_NO_SHORTCUTLIST_CTRL
#define __EXT_MFC_NO_PAGECONTAINER_CTRL
#define __EXT_MFC_NO_GRIDBASEWND
#define __EXT_MFC_NO_GRIDWND
#define __EXT_MFC_NO_IMAGEEDITWND
#define __EXT_MFC_NO_COLORPALETTE
#define __EXT_MFC_NO_ICONEDITDLG
#define __EXT_MFC_NO_BUILTIN_TEXTFIELD
#define __EXT_MFC_NO_BUILTIN_DATEFIELD
#define __EXT_MFC_NO_CUSTOMIZE
#define __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL
#define __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL
#define __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL
#define __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL
#define __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL
#define __EXT_MFC_NO_DATE_BROWSER
#define __EXT_MFC_NO_DATE_PICKER
#define __EXT_MFC_NO_DATE_PICKER_POPUP
#define __EXT_MFC_NO_PAGE_NAVIGATOR
#define __EXT_MFC_NO_UNDO_REDO_POPUP
#define __EXT_MFC_NO_DURATIONWND
#define __EXT_MFC_NO_DATETIMEWND
#define __EXT_MFC_NO_TREEGRIDWND
#define __EXT_MFC_NO_FORMULA_GRID
#define __EXT_MFC_NO_PROPERTYGRIDWND
#define __EXT_MFC_NO_REPORTGRIDWND
#define __EXT_MFC_NO_TABBED_TOOLBAR
#define __EXT_MFC_NO_TABBED_TOOLBAR_FLAT
#define __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS
#define __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE
#define __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY
#define __EXT_MFC_NO_RIBBON_BAR
#define __EXT_MFC_NO_PRINT
#define __EXT_MFC_NO_SHELL
#define __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL
#define __EXT_MFC_NO_SHELL_LIST_VIEW_CTRL
#define __EXT_MFC_NO_SHELL_COMBO_BOX
#define __EXT_MFC_NO_SHELL_EXTENSIONS_COMBO_BOX
#define __EXT_MFC_NO_SHELL_DIALOG_BFF
#define __EXT_MFC_NO_SHELL_DIALOG_FILE
#define __EXT_MFC_NO_GEO_CONTROLS
#define __EXT_MFC_NO_BACKSTAGEVIEWWND

// optional features (you can also exclude some resources in the resource.rc file)
//#define __EXT_MFC_NO_PROF_UIS_ABOUT_DIALOG
//#define __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG
//#define __EXT_MFC_NO_SPLITTER_WND
//#define __EXT_MFC_NO_DOCK_MARKERS
//#define __EXT_MFC_NO_EXTCONTENTEXPANDWND_H
//#define __EXT_MFC_NO_DYNAMIC_BAR_SITE
//#define __EXT_MFC_NO_CHECK_LIST
//#define __EXT_MFC_NO_SCROLLWND
//#define __EXT_MFC_NO_SCROLLCONTAINERWND
//#define __EXT_MFC_NO_SCROLLITEMWND
//#define __EXT_MFC_NO_SCROLLCONAINERWND
//#define __EXT_MFC_NO_SCROLLRICHCONTENTWND
//#define __EXT_MFC_NO_RICHCONTENTGENERATORWND
//#define __EXT_MFC_NO_SPIN
//#define __EXT_MFC_NO_STATUSBAR
//#define __EXT_MFC_NO_NC_FRAME
//#define __EXT_MFC_NO_PROGRESS_WND
//#define __EXT_MFC_NO_THEME_SWITCHER_TOOLBAR
//#define __EXT_MFC_NO_TREE_VIEW_CTRL
//#define __EXT_MFC_NO_LIST_VIEW_CTRL
//#define __EXT_MFC_NO_MSG_BOX

// exclude unneded classes for ProfUIS-AX distributives
//#define __EXT_PROFUIS_AX_DISTRIBUTE_BUILD
#ifdef __EXT_PROFUIS_AX_DISTRIBUTE_BUILD
	#define __EXT_MFC_NO_TAB_CTRL
	#define __EXT_MFC_NO_TABMDI_CTRL
	#define __EXT_MFC_NO_TABFLAT_CTRL
	#define __EXT_MFC_NO_TABBUTTONS_CTRL
	#define __EXT_MFC_NO_TAB_ONENOTE_CTRL
	#define __EXT_MFC_NO_TAB_WHIDBEY_CTRL
	#define __EXT_MFC_NO_TAB_CONTROLBARS
	#define __EXT_MFC_NO_SHORTCUTLIST_CTRL
	#define __EXT_MFC_NO_PAGECONTAINER_CTRL
	#define __EXT_MFC_NO_SCROLLITEMWND
	#define __EXT_MFC_NO_SCROLLRICHCONTENTWND
	#define __EXT_MFC_NO_RICHCONTENTGENERATORWND
	#define __EXT_MFC_NO_BACKSTAGEVIEWWND
	#define __EXT_MFC_NO_SCROLLCONAINERWND
	#define __EXT_MFC_NO_GRIDBASEWND
	#define __EXT_MFC_NO_GRIDWND
	#define __EXT_MFC_NO_IMAGEEDITWND
	#define __EXT_MFC_NO_COLORPALETTE
	#define __EXT_MFC_NO_ICONEDITDLG
	#define __EXT_MFC_NO_BUILTIN_TEXTFIELD
	#define __EXT_MFC_NO_BUILTIN_DATEFIELD
	#define __EXT_MFC_NO_CUSTOMIZE
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL
	#define __EXT_MFC_NO_DATE_BROWSER
	#define __EXT_MFC_NO_DATE_PICKER_POPUP
	#define __EXT_MFC_NO_PAGE_NAVIGATOR
	#define __EXT_MFC_NO_UNDO_REDO_POPUP
	#define __EXT_MFC_NO_DURATIONWND
	#define __EXT_MFC_NO_DATETIMEWND
	#define __EXT_MFC_NO_TREEGRIDWND
	#define __EXT_MFC_NO_FORMULA_GRID
	#define __EXT_MFC_NO_PROPERTYGRIDWND
	#define __EXT_MFC_NO_REPORTGRIDWND
	#define __EXT_MFC_NO_TABBED_TOOLBAR
	#define __EXT_MFC_NO_TABBED_TOOLBAR_FLAT
	#define __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS
	#define __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE
	#define __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY

	#define __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG
	#define __EXT_MFC_NO_SPLITTER_WND
	#define __EXT_MFC_NO_DOCK_MARKERS
	#define __EXT_MFC_NO_EXTCONTENTEXPANDWND_H
	#define __EXT_MFC_NO_DYNAMIC_BAR_SITE
	#define __EXT_MFC_NO_CHECK_LIST
	#define __EXT_MFC_NO_SPIN
	#define __EXT_MFC_NO_STATUSBAR
#endif

// exclude unneded classes for Frame Features distributives
//#define __EXT_FRAME_FEATURES_DISTRIBUTE_BUILD
#ifdef __EXT_FRAME_FEATURES_DISTRIBUTE_BUILD
	#define __EXT_MFC_NO_TABFLAT_CTRL
	#define __EXT_MFC_NO_TABBUTTONS_CTRL
	#define __EXT_MFC_NO_TAB_ONENOTE_CTRL
	#define __EXT_MFC_NO_TAB_WHIDBEY_CTRL
	#define __EXT_MFC_NO_TAB_CONTROLBARS
	#define __EXT_MFC_NO_TOOLBOX_CTRL
	#define __EXT_MFC_NO_SHORTCUTLIST_CTRL
	#define __EXT_MFC_NO_PAGECONTAINER_CTRL
	#define __EXT_MFC_NO_SCROLLWND
	#define __EXT_MFC_NO_SCROLLCONTAINERWND
	#define __EXT_MFC_NO_SCROLLITEMWND
	#define __EXT_MFC_NO_SCROLLCONAINERWND
	#define __EXT_MFC_NO_GRIDBASEWND
	#define __EXT_MFC_NO_GRIDWND
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL
	#define __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL
	#define __EXT_MFC_NO_DATE_BROWSER
	#define __EXT_MFC_NO_DATE_PICKER
	#define __EXT_MFC_NO_DATE_PICKER_POPUP
	#define __EXT_MFC_NO_PAGE_NAVIGATOR
	#define __EXT_MFC_NO_UNDO_REDO_POPUP
	#define __EXT_MFC_NO_BUILTIN_DATEFIELD
	#define __EXT_MFC_NO_EXTCONTENTEXPANDWND_H
	#define __EXT_MFC_NO_DURATIONWND
	#define __EXT_MFC_NO_DATETIMEWND
	#define __EXT_MFC_NO_TREEGRIDWND
	#define __EXT_MFC_NO_FORMULA_GRID
	#define __EXT_MFC_NO_PROPERTYGRIDWND
	#define __EXT_MFC_NO_REPORTGRIDWND
	#define __EXT_MFC_NO_TABBED_TOOLBAR
	#define __EXT_MFC_NO_TABBED_TOOLBAR_FLAT
	#define __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS
	#define __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE
	#define __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY

	#define __EXT_MFC_NO_PROF_UIS_ABOUT_DIALOG
	#define __EXT_MFC_NO_SPLITTER_WND
	#define __EXT_MFC_NO_DOCK_MARKERS
	#define __EXT_MFC_NO_DYNAMIC_BAR_SITE
	#define __EXT_MFC_NO_SPIN
#endif

#if (defined __EXT_MFC_NO_TAB_CTRL)
	#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
		#define   __EXT_MFC_NO_TABMDI_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TABFLAT_CTRL)
		#define   __EXT_MFC_NO_TABFLAT_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TABBUTTONS_CTRL)
		#define   __EXT_MFC_NO_TABBUTTONS_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_ONENOTE_CTRL)
		#define   __EXT_MFC_NO_TAB_ONENOTE_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_WHIDBEY_CTRL)
		#define   __EXT_MFC_NO_TAB_WHIDBEY_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		#define   __EXT_MFC_NO_TAB_CONTROLBARS
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
		#define   __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)
		#define   __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL)
		#define   __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL)
		#define   __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL)
		#define   __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR
	#endif
#endif // (defined __EXT_MFC_NO_TAB_CTRL)

#if (defined __EXT_MFC_NO_TABFLAT_CTRL)
	#if (!defined __EXT_MFC_NO_TABBUTTONS_CTRL)
		#define   __EXT_MFC_NO_TABBUTTONS_CTRL
	#endif
#endif

#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR)
	#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
		#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR)
			#define   __EXT_MFC_NO_TABBED_TOOLBAR
		#endif
	#endif
#endif

#if (defined __EXT_MFC_NO_NC_FRAME)
	#if (!defined __EXT_MFC_NO_MSG_BOX)
		#define __EXT_MFC_NO_MSG_BOX
	#endif
	#if (!defined __EXT_MFC_NO_PRINT)
		#define __EXT_MFC_NO_PRINT
	#endif
#endif

#if (defined __EXT_MFC_NO_CHECK_LIST)
	#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		#define __EXT_MFC_NO_CUSTOMIZE
	#endif
#endif

#if (defined __EXT_MFC_NO_CUSTOMIZE) || (defined __EXT_MFC_NO_TOOLBOX_CTRL)
	#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		#define __EXT_MFC_NO_RIBBON_BAR
	#endif
#endif

#if (defined __EXT_MFC_NO_TABBED_TOOLBAR) || (defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_FLAT)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_FLAT
	#endif
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS
	#endif
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE
	#endif
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_FLAT)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_FLAT
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL)
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL)
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_BUTTONS
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL)
		#define   __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL)
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_ONENOTE
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL)
	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY)
		#define   __EXT_MFC_NO_TABBED_TOOLBAR_WHIDBEY
	#endif
#endif

#if (defined __EXT_MFC_NO_SCROLLWND)
	#if (!defined __EXT_MFC_NO_SCROLLCONTAINERWND)
		#define __EXT_MFC_NO_SCROLLCONTAINERWND
	#endif
	#if (!defined __EXT_MFC_NO_IMAGEEDITWND)
		#define   __EXT_MFC_NO_IMAGEEDITWND
	#endif
	#if (!defined __EXT_MFC_NO_COLORPALETTE)
		#define   __EXT_MFC_NO_COLORPALETTE
	#endif
	#if (!defined __EXT_MFC_NO_SCROLLITEMWND)
		#define   __EXT_MFC_NO_SCROLLITEMWND
	#endif
	#if (!defined __EXT_MFC_NO_SCROLLCONAINERWND)
		#define   __EXT_MFC_NO_SCROLLCONAINERWND
	#endif
	#if (!defined __EXT_MFC_NO_SCROLLRICHCONTENTWND)
		#define   __EXT_MFC_NO_SCROLLRICHCONTENTWND
	#endif
	#if (!defined __EXT_MFC_NO_TOOLBOX_CTRL)
		#define   __EXT_MFC_NO_TOOLBOX_CTRL
	#endif
#endif // (defined __EXT_MFC_NO_SCROLLWND)

#if (defined __EXT_MFC_NO_SCROLLITEMWND)
	#if (!defined __EXT_MFC_NO_GRIDBASEWND)
		#define  __EXT_MFC_NO_GRIDBASEWND
	#endif
#endif


#if (defined __EXT_MFC_NO_GRIDBASEWND)
	#if (!defined __EXT_MFC_NO_GRIDWND)
		#define  __EXT_MFC_NO_GRIDWND
	#endif
#endif

#if (defined __EXT_MFC_NO_GRIDWND)
	#if (!defined __EXT_MFC_NO_TREEGRIDWND)
		#define  __EXT_MFC_NO_TREEGRIDWND
	#endif
	#if (!defined __EXT_MFC_NO_FORMULA_GRID)
		#define __EXT_MFC_NO_FORMULA_GRID
	#endif
#endif

#if (defined __EXT_MFC_NO_TREEGRIDWND)
	#if (!defined __EXT_MFC_NO_PROPERTYGRIDWND)
		#define  __EXT_MFC_NO_PROPERTYGRIDWND
	#endif
	#if (!defined __EXT_MFC_NO_REPORTGRIDWND)
		#define  __EXT_MFC_NO_REPORTGRIDWND
	#endif
#endif

#if (defined __EXT_MFC_NO_IMAGEEDITWND) || (defined __EXT_MFC_NO_COLORPALETTE)
	#if (!defined __EXT_MFC_NO_ICONEDITDLG)
		#define   __EXT_MFC_NO_ICONEDITDLG
	#endif
#endif // (defined __EXT_MFC_NO_IMAGEEDITWND) || (defined __EXT_MFC_NO_COLORPALETTE)

#if (defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_BUTTONS_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_TABFLAT_CTRL)
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_FLAT_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_ONENOTE_CTRL)
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_TAB_WHIDBEY_CTRL)
	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL)
		#define  __EXT_MFC_NO_TAB_PAGECONTAINER_WHIDBEY_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_DATE_PICKER)
	#if (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)
		#define  __EXT_MFC_NO_DATE_PICKER_POPUP
	#endif
	#if (!defined __EXT_MFC_NO_BUILTIN_DATEFIELD)
		#define  __EXT_MFC_NO_BUILTIN_DATEFIELD
	#endif
#endif

#if (defined __EXT_MFC_NO_DATE_PICKER_POPUP)
	#if (!defined __EXT_MFC_NO_BUILTIN_DATEFIELD)
		#define  __EXT_MFC_NO_BUILTIN_DATEFIELD
	#endif
#endif

#if (defined __EXT_MFC_NO_DURATIONWND)
	#if (!defined __EXT_MFC_NO_DATETIMEWND)
		#define  __EXT_MFC_NO_DATETIMEWND
	#endif
#endif

#if (defined __EXT_MFC_NO_DATE_PICKER_POPUP)
	#if (!defined __EXT_MFC_NO_DATETIMEWND)
		#define  __EXT_MFC_NO_DATETIMEWND
	#endif
#endif

#if (defined __EXT_MFC_NO_SHELL)
	#if (!defined __EXT_MFC_NO_SHELL_COMBO_BOX)
		#define __EXT_MFC_NO_SHELL_COMBO_BOX
	#endif
	#if (!defined __EXT_MFC_NO_SHELL_EXTENSIONS_COMBO_BOX)
		#define __EXT_MFC_NO_SHELL_EXTENSIONS_COMBO_BOX
	#endif
	#if (!defined __EXT_MFC_NO_SHELL_LIST_VIEW_CTRL)
		#define __EXT_MFC_NO_SHELL_LIST_VIEW_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL)
		#define __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL
	#endif
	#if (!defined __EXT_MFC_NO_SHELL_DIALOG_BFF)
		#define __EXT_MFC_NO_SHELL_DIALOG_BFF
	#endif
	#if (!defined __EXT_MFC_NO_SHELL_DIALOG_FILE)
		#define __EXT_MFC_NO_SHELL_DIALOG_FILE
	#endif
#endif

#if (defined __EXT_MFC_NO_LIST_VIEW_CTRL)
	#if (!defined __EXT_MFC_NO_SHELL_LIST_VIEW_CTRL)
		#define __EXT_MFC_NO_SHELL_LIST_VIEW_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_TREE_VIEW_CTRL)
	#if (!defined __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL)
		#define __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL
	#endif
#endif

#if (defined __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL)
	#if (!defined __EXT_MFC_NO_SHELL_DIALOG_BFF)
		#define __EXT_MFC_NO_SHELL_DIALOG_BFF
	#endif
#endif

#if (defined __EXT_MFC_NO_SHELL_TREE_VIEW_CTRL) || (defined __EXT_MFC_NO_SHELL_LIST_VIEW_CTRL) || (defined __EXT_MFC_NO_SHELL_COMBO_BOX) || (defined __EXT_MFC_NO_SHELL_EXTENSIONS_COMBO_BOX)
	#if (!defined __EXT_MFC_NO_SHELL_DIALOG_FILE)
		#define __EXT_MFC_NO_SHELL_DIALOG_FILE
	#endif
#endif

#if (!defined __EXT_MFC_NO_GEO_CONTROLS)
	#if (defined __EXT_MFC_NO_DURATIONWND)
		#define __EXT_MFC_NO_GEO_CONTROLS
	#endif
#endif

#if (defined __EXT_MFC_NO_SCROLLRICHCONTENTWND)
	#if (!defined __EXT_MFC_NO_RICHCONTENTGENERATORWND)
		#define __EXT_MFC_NO_RICHCONTENTGENERATORWND
	#endif
#endif

#if (defined __EXT_MFC_NO_RICHCONTENTGENERATORWND) || (defined __EXT_MFC_NO_RIBBON_BAR)
	#if (!defined __EXT_MFC_NO_BACKSTAGEVIEWWND)
		#define __EXT_MFC_NO_BACKSTAGEVIEWWND
	#endif
#endif

#if (!defined __PROF_UIS_IMPL__)

	#if (!defined __EXT_RICH_CONTENT_H)
		#include <ExtRichContent.h>
	#endif // (!defined __EXT_RICH_CONTENT_H)

	#if (!defined __EXT_LABEL_H)
		#include <ExtLabel.h>
	#endif

	#if (!defined __EXT_GROUPBOX_H)
		#include <ExtGroupBox.h>
	#endif

	#if (!defined __EXTBTNONFLAT_H)
		#include <ExtBtnOnFlat.h>
	#endif

	#if (!defined __EXT_BUTTON_H)
		#include <ExtButton.h>
	#endif

	#if (!defined __EXT_COMBO_BOX_H)
		#include <ExtComboBox.h>
	#endif

	#if (!defined __EXT_MFC_NO_EXTCONTENTEXPANDWND_H)
		#if (!defined __EXT_EXTCONTENTEXPANDWND_H)
			#include <ExtContentExpandWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_EXTCONTENTEXPANDWND_H)

	#if (!defined __EXT_EDIT_H)
		#include <ExtEdit.h>
	#endif

	#if (!defined __EXT_CMD_ICON_H)
		#include <ExtCmdIcon.h>
	#endif

	#if (!defined __ExtCmdManager_H)
		#include <ExtCmdManager.h>
	#endif

	#if (!defined __EXT_PAINT_MANAGER_H)
		#include <ExtPaintManager.h>
	#endif

	#if (!defined __EXT_CONTROLBAR_H)
		#include <ExtControlBar.h>
	#endif

	#if (!defined __EXT_TOOLCONTROLBAR_H)
		#include <ExtToolControlBar.h>
	#endif

	#if (!defined __EXT_MENUCONTROLBAR_H)
		#include <ExtMenuControlBar.h>
	#endif

	#if (!defined __EXT_MFC_NO_STATUSBAR)
		#if (!defined __EXT_STATUSCONTROLBAR_H)
			#include <ExtStatusControlBar.h>
		#endif
	#endif

	#if (!defined __EXT_POPUP_MENU_WND_H)
		#include <ExtPopupMenuWnd.h>
	#endif

	#if (!defined __EXT_RESIZABLE_DIALOG_H)
		#include <ExtResizableDialog.h>
	#endif

	#if (!defined __EXTCOLORCTRL_H)
		#include <ExtColorCtrl.h>
	#endif

	#if (!defined __EXTCOLORDLG_H)
		#include <ExtColorDlg.h>
	#endif

	#if (!defined __EXT_TEMPL_H)
		#include <ExtTempl.h>
	#endif

	#if (!defined __EXT_MFC_NO_TAB_CTRL)
		#if (!defined __EXT_TABWND_H)
			#include <ExtTabWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_TAB_CTRL)

	#if (!defined __EXT_MFC_NO_TABFLAT_CTRL)
		#if (! defined __EXT_TAB_FLAT_WND_H)
			#include <ExtTabFlatWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_TABFLAT_CTRL)

	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
		#if (! defined __EXT_TAB_PAGE_CONTAINER_WND_H)
			#include <ExtTabPageContainerWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

	#if (!defined __EXT_MFC_NO_TOOLBOX_CTRL)
		#if (!defined __EXTTOOLBOXWND_H)
			#include <ExtToolBoxWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_TOOLBOX_CTRL)

	#if (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
		#if (!defined __EXTSHORTCUTLISTWND_H)
			#include <ExtShortcutListWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_SHORTCUTLIST_CTRL)
	
	#if (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)
		#if (!defined __EXT_PAGECONTAINERWND_H)
			#include <ExtPageContainerWnd.h>
		#endif
	#endif //  (!defined __EXT_MFC_NO_PAGECONTAINER_CTRL)

	#if (!defined __EXT_MFC_NO_SCROLLWND)
		#if (!defined __EXT_SCROLLWND_H)
			#include <ExtScrollWnd.h>
		#endif 
	#endif // #if (!defined __EXT_MFC_NO_SCROLLWND)

	#if (!defined __EXT_MFC_NO_GRIDBASEWND)
		#if (!defined __EXT_GRIDWND_H)
			#include <ExtGridWnd.h>
		#endif 
	#endif // #if (!defined __EXT_MFC_NO_GRIDBASEWND)

	#if (!defined __EXT_MFC_NO_IMAGEEDITWND)
		#if (!defined __EXT_IMAGEEDITWND_H)
			#include <ExtImageEditWnd.h>
		#endif
	#endif // #if (!defined __EXT_MFC_NO_IMAGEEDITWND)

	#if (!defined __EXT_MFC_NO_COLORPALETTE)
		#if (!defined __EXT_COLORPALETTE_WND_H)
			#include <ExtColorPaletteWnd.h>
		#endif
	#endif // #if (!defined __EXT_MFC_NO_COLORPALETTE)

	#if (!defined __EXT_MFC_NO_ICONEDITDLG)
		#if (!defined __EXT_ICONEDITDLG_H)
			#include <ExtIconEditDlg.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_ICONEDITDLG)

	#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		#if (!defined __EXTCUSTOMIZE_H)
			#include <ExtCustomize.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	#if (!defined __EXT_MFC_NO_DATE_PICKER) || (!defined __EXT_MFC_NO_DATE_BROWSER)
		#if (!defined __EXT_DATE_PICKER_H)
			#include <ExtDatePicker.h>
		#endif // (!defined __EXT_DATE_PICKER_H)
	#endif

	#if (!defined __EXT_MFC_NO_PROF_UIS_ABOUT_DIALOG)
		#if (!defined __EXT_PROF_UIS_ABOUT_DIALOG_H)
			#include <ExtProfUISAboutDialog.h>
		#endif // (!defined __EXT_PROF_UIS_ABOUT_DIALOG_H)
	#endif // (!defined __EXT_MFC_NO_PROF_UIS_ABOUT_DIALOG)

	#if (!defined __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG)
		#if (!defined __EXT_MDI_WINDOWS_LIST_DIALOG_H)
			#include <ExtMdiWindowsListDlg.h>
		#endif // (!defined __EXT_MDI_WINDOWS_LIST_DIALOG_H)
	#endif // (!defined __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG)

	#if (!defined __EXT_MFC_NO_SPLITTER_WND)
		#if (!defined __EXT_SPLITTER_WND_H)
			#include <ExtSplitterWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_SPLITTER_WND)

	#if (!defined __EXT_MFC_NO_PAGE_NAVIGATOR)
		#if (!defined __EXT_PAGE_NAVIGATOR_H)
			#include <ExtPageNavigatorWnd.h>
		#endif // (!defined __EXT_PAGE_NAVIGATOR_H)
	#endif // (!defined __EXT_MFC_NO_PAGE_NAVIGATOR)

	#if (!defined __EXT_MFC_NO_CUSTOMIZE) || (!defined __EXT_MFC_NO_DATE_PICKER_POPUP) || (!defined __EXT_MFC_NO_UNDO_REDO_POPUP) || (!defined __EXT_MFC_NO_GRIDBASEWND)
		#if (!defined __EXT_POPUP_CTRL_MENU_H)
			#include <ExtPopupCtrlMenu.h>
		#endif
	#endif

	#if (!defined __EXT_MFC_NO_DURATIONWND)
		#if (!defined __EXT_DURATIONWND_H)
			#include <ExtDurationWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_DURATIONWND)

	#if (!defined __EXT_MFC_NO_TREEGRIDWND)
		#if (!defined __EXTTREEGRIDWND_H)
			#include <ExtTreeGridWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_TREEGRIDWND)

	#if (!defined __EXT_MFC_NO_PROPERTYGRIDWND)
		#if (!defined __EXTPROPERTYGRIDWND_H)
			#include <ExtPropertyGridWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_PROPERTYGRIDWND)

	#if (!defined __EXT_SLIDER_H)
		#include <ExtSliderWnd.h>
	#endif // (!defined __EXT_SLIDER_H)

	#if (!defined __EXT_MFC_NO_REPORTGRIDWND)
		#if (!defined __EXT_REPORT_GRID_WND_H)
			#include <ExtReportGridWnd.h>
		#endif // (!defined __EXT_REPORT_GRID_WND_H)
	#endif // (!defined __EXT_MFC_NO_REPORTGRIDWND)

	#if (!defined __EXT_MFC_NO_CHECK_LIST)
		#if (!defined __EXT_CHECK_LIST_H)
			#include <ExtCheckListWnd.h>
		#endif // (!defined __EXT_CHECK_LIST_H)
	#endif // (!defined __EXT_MFC_NO_CHECK_LIST)

	#if (!defined __EXT_MFC_NO_TABBED_TOOLBAR)
		#if (!defined __EXT_TABBEDTOOLCONTROLBAR_H)
			#include <ExtTabbedToolControlBar.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_TABBED_TOOLBAR)

	#if (!defined __EXT_MFC_NO_SPIN)
		#if (!defined __EXT_SPIN_H)
			#include <ExtSpinWnd.h>
		#endif // (!defined __EXT_SPIN_H)
	#endif // (!defined __EXT_MFC_NO_SPIN)

	#if (!defined __EXT_MFC_NO_NC_FRAME )
		#if (!defined __EXT_NC_FRAME_H)
			#include <ExtNcFrame.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_NC_FRAME )

	#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		#if (!defined __EXT_RIBBON_BAR_H)
			#include <ExtRibbonBar.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_NC_FRAME )

	#if (!defined __EXT_CONTROLS_COMMON_H)
		#include <ExtControlsCommon.h>
	#endif

	#if (!defined __EXT_MFC_NO_SHELL)
		#if (!defined __EXT_CONTROLS_SHELL_H)
			#include <ExtControlsShell.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_SHELL)

	#if (!defined __EXT_MFC_NO_PROGRESS_WND)
		#if (!defined __EXT_PROGRESS_WND_H)
			#include <ExtProgressWnd.h>
		#endif
	#endif // (!defined __EXT_MFC_NO_PROGRESS_WND )

	#if (!defined __EXT_CONTROLS_H)
		#include <ExtControls.h>
	#endif // (!defined __EXT_CONTROLS_H )

	#if (!defined __EXT_MFC_NO_PRINT)
		#if (!defined __EXT_PRINT_H)
			#include <ExtPrint.h>
		#endif // (!defined __EXT_PRINT_H )
	#endif // __EXT_MFC_NO_PRINT

	#if (!defined __EXT_MFC_NO_GEO_CONTROLS)
		#if (!defined __EXT_GEO_CONTROLS_H)
			#include <ExtGeoControls.h>
		#endif // (!defined __EXT_GEO_CONTROLS_H)
	#endif // (!defined __EXT_MFC_NO_GEO_CONTROLS)

	#if (!defined __EXT_MFC_NO_FORMULA_GRID)
		#if (!defined __EXT_FORMULAGRID_H)
			#include <ExtFormulaGrid.h>
		#endif // (!defined __EXT_FORMULAGRID_H)
	#endif // (!defined __EXT_MFC_NO_FORMULA_GRID)

#endif // !__PROF_UIS_IMPL__

#endif // __PROF_UIS_H


