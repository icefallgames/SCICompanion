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

#if (!defined __EXT_CONTROLS_H)
#define __EXT_CONTROLS_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_HOOK_H)
	#include "../Src/ExtHook.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define __EXT_SUBCLASS_PROFUISCTRL( hWnd, class_name ) \
{ \
	CWnd * pWnd = CWnd::FromHandlePermanent( hWnd ); \
	if( pWnd == NULL ) \
	{ \
		class class_name##Dynamic : public class_name \
		{ \
			void PostNcDestroy() \
			{ \
				delete this; \
			} \
		}; \
		pWnd = new class_name##Dynamic; \
		ASSERT_VALID( pWnd ); \
		pWnd->SubclassWindow( hWnd ); \
		if( pWnd->IsWindowVisible() ) \
			pWnd->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN|RDW_FRAME ); \
	} \
}

void __PROF_UIS_API SubclassChildControls( 
	HWND hWndParent 
	);
void __PROF_UIS_API SubclassChildControls( 
	const CWnd * pWndParent 
	);
void __PROF_UIS_API SubclassChildControls( 
	const CWnd & wndParent 
	);

/////////////////////////////////////////////////////////////////////////////
// CExtWSGripper for the CExtWS template

#define __EXT_SIZE_GRIPPER_CLASS_NAME _T("ProfUIS-SizeGripper") 

class __PROF_UIS_API CExtWSGripper : public CWnd
{
public:
	DECLARE_DYNCREATE( CExtWSGripper );
	CExtWSGripper();
	virtual CExtPaintManager * PmBridge_GetPM() const;
protected:
	virtual void DoPaint( CDC * pDC );
	virtual LRESULT WindowProc(
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);
}; // class CExtWSGripper

#if (!defined __EXT_MFC_NO_THEME_SWITCHER_TOOLBAR)

/////////////////////////////////////////////////////////////////////////////
// CExtThemeSwitcherToolButton

class __PROF_UIS_API CExtThemeSwitcherToolButton : public CExtBarButton
{
public:
	DECLARE_DYNCREATE( CExtThemeSwitcherToolButton );
	CExtThemeSwitcherToolButton(
		CExtToolControlBar * pBar = NULL,
		UINT nCmdID = ID_SEPARATOR,
		UINT nStyle = 0
		);
	virtual ~CExtThemeSwitcherToolButton();
	virtual void OnUpdateCmdUI(
		CWnd * pTarget,
		BOOL bDisableIfNoHndler,
		int nIndex
		);
	virtual void OnDeliverCmd();
	virtual BOOL PutToPopupMenu(
		CExtPopupMenuWnd * pPopup
		);
}; // class CExtThemeSwitcherToolButton

/////////////////////////////////////////////////////////////////////////////
// CExtThemeSwitcherToolControlBar

class __PROF_UIS_API CExtThemeSwitcherToolControlBar
	: public CExtToolControlBar
	, public CExtHookSink
{
public:
	DECLARE_DYNCREATE( CExtThemeSwitcherToolControlBar );
// Construction
public:
	bool m_bEnableOffice2007r3:1;
	CExtThemeSwitcherToolControlBar();
// Implementation
public:
	virtual ~CExtThemeSwitcherToolControlBar();
	virtual CExtBarButton * OnCreateBarCommandBtn(
		UINT nCmdID,
		UINT nStyle = 0
		);
	enum e_ThemeIconSize_t
	{
		__ETIS_16x16 = 0,
		__ETIS_24x24 = 1,
		__ETIS_32x32 = 2,
	};
	UINT m_nMenuMarkerID;
protected:
	CExtSafeString m_strCommandProfileName;
	e_ThemeIconSize_t m_eTIS_PopupMenu, m_eTIS_ToolBar;
	typedef
		CMap < UINT, UINT, CRuntimeClass *, CRuntimeClass * >
		MapCmd2RTC_t;
	MapCmd2RTC_t m_mapCmd2RTC;
	typedef
		CMap < UINT, UINT, UINT, UINT >
		MapCmdID2ThemeType_t;
	MapCmdID2ThemeType_t m_mapCmdID2ThemeType;
	typedef
		CMap < UINT, UINT, UINT, UINT >
		MapThemeType2CmdID_t;
	MapThemeType2CmdID_t m_arrMapsTT[3];
	bool m_bCommandProfileInitialized:1;
	typedef
		CList < UINT, UINT >
		ListCmdSequence_t;
	ListCmdSequence_t m_listCmdSequence;
public:
	virtual bool ThemeSwitcherInit(
		e_ThemeIconSize_t _eTIS_PopupMenu = __ETIS_16x16,
		e_ThemeIconSize_t _eTIS_ToolBar = __ETIS_24x24
		);
	virtual void ThemeSwitcher_OnButtonInvoke(
		CExtThemeSwitcherToolButton * pTBB
		);
	virtual void ThemeSwitcher_OnButtonUpdate(
		CExtThemeSwitcherToolButton * pTBB
		);
	virtual UINT ThemeSwitcher_GetMenuCmdIDByToolBarCmdID(
		UINT nCmdID
		);
	virtual UINT ThemeSwitcher_GetToolBarCmdIDByMenuCmdID(
		UINT nCmdID
		);
protected:
	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);
// Overrides
	//{{AFX_VIRTUAL(CExtThemeSwitcherToolControlBar)
	public:
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	virtual BOOL OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo );
	protected:
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(CExtThemeSwitcherToolControlBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};// class CExtThemeSwitcherToolControlBar

#endif // (!defined __EXT_MFC_NO_THEME_SWITCHER_TOOLBAR)

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // (!defined __EXT_CONTROLS_H)
