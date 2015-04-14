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

#if (!defined __EXT_HOOK_H)
#define __EXT_HOOK_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

class __PROF_UIS_API CExtHookSink
{
public:

	struct HookSinkArray_t;
	struct HookChains_t;
	friend struct HookChains_t;


	bool m_bEnableDetailedWndHooks:1, m_bEatNcDestroy:1;

	typedef
		CList < HWND, HWND >
		HookedWndList_t;

	CExtHookSink(
		bool bEnableDetailedWndHooks = true
		);
	virtual ~CExtHookSink();

	virtual bool IsAutoDeleteHookWndSink();

	virtual LRESULT OnHookWndMsgNextProcCurrent(
		WPARAM wParam,
		LPARAM lParam
		);
	virtual LRESULT OnHookWndMsgNextProcInvoke(
		UINT nMessage,
		WPARAM wParam,
		LPARAM lParam
		);
	virtual LRESULT OnHookWndMsgDefault();
	virtual bool OnHookWndMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM & wParam,
		LPARAM & lParam
		);
	virtual void OnPostHookWndMsg(
		LRESULT lResult,
		HWND hWndHooked,
		UINT nMessage,
		WPARAM wParam,
		LPARAM lParam
		);
	virtual bool OnHookCmdMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		WORD wNotifyCode,
		WORD wID,
		HWND hWndCtrl
		);
	virtual bool OnHookNotifyMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		INT nIdCtrl,
		LPNMHDR lpnmhdr
		);
	virtual bool OnHookPaintMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		HDC hDC
		);
	virtual bool OnHookEraseBackgroundMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		HDC hDC
		);
	virtual bool OnHookPrintMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		HDC hDC
		);
	virtual bool OnHookNcPaintMsg(
		LRESULT & lResult,
		HWND hWndHooked,
		HRGN hRgnUpdate
		);

	virtual void OnHookWndNcDestroy();

	virtual void OnHookWndAttach( HWND hWnd );
	virtual void OnHookWndDetach( HWND hWnd );

	virtual bool SetupHookWndSink(
		HWND hWnd,
		bool bRemove = false,
		bool bAddToHead = false
		);

	ULONG SetupHookWndSinkToChilds(
		HWND hWnd,
		UINT * pDlgCtrlIDs = NULL,
		ULONG nCountOfDlgCtrlIDs = 0,
		bool bDeep = false
		);

	virtual void RemoveAllWndHooks();

	void GetHookedWindows( HookedWndList_t & _list );
	bool IsHookedWindow( HWND hWnd );

}; // class CExtHookSink

#define __EHSEF_MOUSE_MOVE					0x00000001
#define __EHSEF_MOUSE_MOVE_NC				0x00000002
#define __EHSEF_MOUSE_CLICK					0x00000004
#define __EHSEF_MOUSE_CLICK_NC				0x00000008
#define __EHSEF_MOUSE_WHEEL					0x00000010
#define __EHSEF_MOUSE_ALL_WITHOUT_WHEEL		(__EHSEF_MOUSE_MOVE|__EHSEF_MOUSE_MOVE_NC|__EHSEF_MOUSE_CLICK|__EHSEF_MOUSE_CLICK_NC)
#define __EHSEF_MOUSE_ALL					(__EHSEF_MOUSE_ALL_WITHOUT_WHEEL|__EHSEF_MOUSE_WHEEL)
#define __EHSEF_KEYBOARD					0x00000020
#define __EHSEF_ALL_MOUSE_AND_KEYBOARD		(__EHSEF_MOUSE_ALL|__EHSEF_KEYBOARD)
#define __EHSEF_GET_MSG						0x00000040
#define __EHSEF_SYS_MSG						0x00000080
#define __EHSEF_ALL_MSG						(__EHSEF_GET_MSG|__EHSEF_SYS_MSG)
#define __EHSEF_WND_PROC_IN					0x00000100
#define __EHSEF_WND_PROC_OUT				0x00000200
#define __EHSEF_WND_PROC_ALL				(__EHSEF_WND_PROC_IN|__EHSEF_WND_PROC_OUT)
#define __EHSEF_PRE_TRANSLATION_LL			0x20000000
#define __EHSEF_PRE_TRANSLATION				0x40000000
#define __EHSEF_POST_PROCESSING				0x80000000
#define __EHSEF_ADDITIONAL_PROCESSING		(__EHSEF_PRE_TRANSLATION_LL|__EHSEF_PRE_TRANSLATION|__EHSEF_POST_PROCESSING)
#define __EHSEF_BASIC_HOOK_FILTERING		(__EHSEF_ALL_MOUSE_AND_KEYBOARD|__EHSEF_ALL_MSG|__EHSEF_WND_PROC_ALL)
#define __EHSEF_ALL							0xFFFFFFFF

#define __EHS_IDX_MOUSE						0
#define __EHS_IDX_KEYBOARD					1
#define __EHS_IDX_GET_MSG					2
#define __EHS_IDX_SYS_MSG					3
#define __EHS_IDX_WND_PROC_IN				4
#define __EHS_IDX_WND_PROC_OUT				5
#define __EHS_IDX_COUNT						6

class __PROF_UIS_API CExtHookSpy
{
	struct __PROF_UIS_API HookSpyItem_t
	{
		CTypedPtrList < CPtrList, CExtHookSpy * > m_list;
		CMap < CExtHookSpy *, CExtHookSpy *, bool, bool > m_map;
		HHOOK m_hHook;
		bool m_bListChanged;
		DWORD m_dwEHSEF;
		HookSpyItem_t( DWORD dwEHSEF );
		void Hook( bool bHook );
		bool IsRegistered( CExtHookSpy * pHS, DWORD dwEHSEF );
		void Register( CExtHookSpy * pHS, DWORD dwEHSEF, bool bAddToHead );
		void Unregister( CExtHookSpy * pHS, DWORD dwEHSEF );
		static LRESULT CALLBACK SHS_HookMouseProc(
			int nCode,      // hook code
			WPARAM wParam,  // message identifier
			LPARAM lParam   // mouse coordinates
			);
		static LRESULT CALLBACK SHS_HookKeyboardProc(
			int nCode,      // hook code
			WPARAM wParam,  // virtual-key code
			LPARAM lParam   // keystroke-message information
			);
		static LRESULT CALLBACK SHS_HookGetMsgProc(
			int nCode,
			WPARAM wParam,
			LPARAM lParam
			);
		static LRESULT CALLBACK SHS_HookSysMsgProc(
			int nCode,
			WPARAM wParam,
			LPARAM lParam
			);
		static LRESULT CALLBACK SHS_HookCallWindowProcIn(
			int nCode,
			WPARAM wParam,
			LPARAM lParam
			);
		static LRESULT CALLBACK SHS_HookCallWindowProcOut(
			int nCode,
			WPARAM wParam,
			LPARAM lParam
			);
	};
	static HookSpyItem_t g_arr[ __EHS_IDX_COUNT ];

	DWORD m_dwEHSEF;
public:

	bool m_bHookConvertLocationsToClient:1;
	CExtHookSpy(
		DWORD dwEHSEF = 0 // auto-register flags, zero - unregister
		);
	virtual ~CExtHookSpy();
	void HookSpyRegister(
		DWORD dwEHSEF = __EHSEF_ALL,
		bool bAddToHead = true
		);
	static void HookSpyRegister(
		CExtHookSpy * pHS,
		DWORD dwEHSEF = __EHSEF_ALL,
		bool bAddToHead = true
		);
	void HookSpyUnregister(
		DWORD dwEHSEF = __EHSEF_ALL
		);
	static void HookSpyUnregister(
		CExtHookSpy * pHS,
		DWORD dwEHSEF = __EHSEF_ALL
		);
	DWORD HookSpyGetEHSEF() const
	{
		ASSERT( this != NULL );
		return m_dwEHSEF;
	}
	bool HookSpySetEHSEF( DWORD dwEHSEF )
	{
		ASSERT( this != NULL );
		if( m_dwEHSEF == dwEHSEF )
			return false;
		m_dwEHSEF = dwEHSEF;
		return true;
	}
	bool HookSpyModifyEHSEF(
		DWORD dwRemoveEHSEF,
		DWORD dwAddEHSEF
		)
	{
		ASSERT( this != NULL );
		DWORD dwOldEHSEF = HookSpyGetEHSEF();
		DWORD dwNewEHSEF = dwOldEHSEF & (~(dwRemoveEHSEF));
		dwNewEHSEF |= dwAddEHSEF;
		return HookSpySetEHSEF( dwNewEHSEF );
	}
	static bool HookSpyIsRegistered(
		const CExtHookSpy * pHS,
		DWORD dwEHSEF = __EHSEF_ALL
		);
	bool HookSpyIsRegistered(
		DWORD dwEHSEF = __EHSEF_ALL
		) const
	{
		return HookSpyIsRegistered( this, dwEHSEF );
	}
protected:
	virtual bool HSLL_OnMouseWheel(
		WPARAM wParam,
		LPARAM lParam
		);
	virtual bool HSLL_OnMouseMove(
		HWND hWnd,
		UINT nFlags,
		CPoint point
		);
	virtual bool HSLL_OnMouseClick(
		HWND hWnd,
		UINT nMessage,
		UINT nFlags,
		CPoint point
		);
	virtual bool HSLL_OnKey(
		UINT nChar,
		UINT nRepCnt,
		UINT nFlags
		);
	virtual bool HSLL_OnGetMsgProc(
		bool & bCallNextHook,
		LRESULT & lResult,
		int nCode,
		WPARAM wParam,
		MSG * pMSG
		);
	virtual bool HSLL_OnSysMsgProc(
		bool & bCallNextHook,
		LRESULT & lResult,
		int nCode,
		WPARAM wParam,
		LPARAM lParam
		);
	virtual bool HSLL_OnCallWindowProcIn(
		bool & bCallNextHook,
		LRESULT & lResult,
		int nCode,
		bool bMessageSentByCurrentThread,
		PCWPSTRUCT pMessageData
		);
	virtual bool HSLL_OnCallWindowProcOut(
		bool & bCallNextHook,
		LRESULT & lResult,
		int nCode,
		bool bMessageSentByCurrentThread,
		PCWPRETSTRUCT pMessageData
		);
	virtual bool HSLL_PreTranslateMessage(
		MSG * pMSG
		);
public:
	virtual bool OnHookSpyPreTranslateMessage(
		MSG * pMSG
		);
	virtual bool OnHookSpyPostProcessMessage(
		MSG * pMSG
		);
	virtual bool OnHookSpyMouseWheelMsg(
		MSG * pMSG
		);
	virtual bool OnHookSpyMouseMoveMsg(
		MSG * pMSG
		);
	virtual bool OnHookSpyMouseClickMsg(
		MSG * pMSG
		);
	virtual bool OnHookSpyKeyMsg(
		MSG * pMSG
		);
	virtual bool OnHookSpyGetMsgProc(
		MSG * pMSG
		);
//	virtual bool OnHookSpySysMsgProc(
//		MSG * pMSG
//		);
	virtual bool OnHookSpyCallWindowProcIn(
		MSG * pMSG
		);
	virtual bool OnHookSpyCallWindowProcOut(
		MSG * pMSG
		);

	friend struct HookSpyItem_t;
};

#endif // __EXT_HOOK_H
