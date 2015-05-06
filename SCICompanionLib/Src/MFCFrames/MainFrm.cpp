// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AppState.h"
#include "Pic.h"
#include "PicDoc.h"
#include "PicView.h"
#include "TextDoc.h"
#include "SoundDoc.h"
#include "VocabDoc.h"
#include "Vocab000.h"
#include "ResourceListDoc.h"
#include "PreferencesDialog.h"
#include "SaveResourceDialog.h"
#include "NewGameDialog.h"
#include "NewRoomDialog.h"
#include "NewScriptDialog.h"
#include "ScriptDocument.h"
#include "NewCompileDialog.h"
#include "editcmd.h"
#include "CCrystalTextBuffer.h"
#include "ScriptView.h"
#include "FindAllDialog.h"
#include "ClassBrowserDialog.h"
#include "SoundView.h"
#include "MainFrm.h"
#include "ResourceEntity.h"
#include "View.h"
#include "Cursor.h"
#include "FontOperations.h"
#include "Text.h"
#include "Sound.h"
#include "NewRasterResourceDocument.h"
#include "AppState.h"
#include "CObjectWrap.h"
#include "format.h"
#include "ExtractAllDialog.h"
#include "DecompileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_SELFTEST 1234

//
// Override this to hide certain menus based on the currently active frame.
// We copy the entire chunk of CExtMenuControlBar::_UpdateMenuBar, and adjust to our needs.
//
#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

BOOL CExtMenuControlBarHideShow::_UpdateMenuBar(
	BOOL bDoRecalcLayout // = TRUE
	)
{
	SetButtons(); // remove all buttons

	VERIFY(
		g_CmdManager->CmdRemoveByMask(
			g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
			(DWORD)CExtCmdItem::STATE_MENUBAR_TMP
			)
		);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	MenuInfoUpdate();
CExtCustomizeSite::CCmdMenuInfo * pMenuInfo = MenuInfoGet();
	if( pMenuInfo != nullptr )
	{
		ASSERT_VALID( pMenuInfo->GetNode() );
		SetButtons( pMenuInfo->GetNode() );
	} // if( pMenuInfo != nullptr )
	else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	{
		CMenu * pMenu = GetMenu();
		if( pMenu != nullptr && pMenu->GetSafeHmenu() != nullptr )
		{
			ASSERT( ::IsMenu(pMenu->GetSafeHmenu()) );
			bool bRevertRTL = OnQueryRevertRTL();
			UINT nMenuItemCount = pMenu->GetMenuItemCount();
            UINT nMenuItemInsertIndex = 0;
			for( UINT nMenuItemIndex = 0; nMenuItemIndex < nMenuItemCount; nMenuItemIndex++ )
			{
				UINT nInsertButtonLocation =
					bRevertRTL
						? 0
						: nMenuItemInsertIndex
						;
				MENUITEMINFO mii;
				::memset( &mii, 0, sizeof(MENUITEMINFO) );
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask =
					MIIM_CHECKMARKS
					|MIIM_DATA
					|MIIM_ID
					|MIIM_STATE
					|MIIM_SUBMENU
					|MIIM_TYPE
					;
				mii.cch = __MAX_UI_ITEM_TEXT;
				CExtSafeString sText;
				mii.dwTypeData =
					sText.GetBuffer( __MAX_UI_ITEM_TEXT );
				ASSERT( mii.dwTypeData != nullptr );
				if( mii.dwTypeData == nullptr )
				{
					ASSERT( FALSE );
					return FALSE;
				}
				if( ! pMenu->GetMenuItemInfo(
						nMenuItemIndex,
						&mii,
						TRUE
						)
					)
				{
					sText.ReleaseBuffer();
					ASSERT( FALSE );
					return false;
				}
				sText.ReleaseBuffer();

				BOOL bAppendMdiWindowsMenu = FALSE;
				UINT nCmdID = 0;
				CExtCmdItem * pCmdItem = nullptr;
				if( mii.hSubMenu == nullptr )
				{
					nCmdID = mii.wID;
					if( nCmdID == ID_SEPARATOR )
					{
						if( ! InsertButton(
								nInsertButtonLocation,
								nCmdID,
								FALSE
								)
							)
						{
							ASSERT( FALSE );
							return FALSE;
						}
						continue;
					} // if( nCmdID == ID_SEPARATOR )
					ASSERT( CExtCmdManager::IsCommand(nCmdID) );
					pCmdItem =
						g_CmdManager->CmdGetPtr(
							g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() ),
							nCmdID
							);
					ASSERT( pCmdItem != nullptr );
				} // if( mii.hSubMenu == nullptr )
				else
				{
					pCmdItem =
						g_CmdManager->CmdAllocPtr(
							g_CmdManager->ProfileNameFromWnd( GetSafeHwnd() )
							);
					if( pCmdItem == nullptr )
					{
						ASSERT( FALSE );
						return FALSE;
					} // if( pCmdItem == nullptr )
					nCmdID = pCmdItem->m_nCmdID;
					ASSERT( CExtCmdManager::IsCommand(nCmdID) );
					pCmdItem->StateSetMenubarTemp();
					pCmdItem->StateSetBasic();

					if( _IsMdiApp() && (! m_sMdiWindowPopupName.IsEmpty() ) )
					{
						CExtSafeString _sText(sText);
						_sText.TrimLeft();
						_sText.TrimRight();
						while( _sText.Replace(_T("&"),_T("")) > 0 )
						{
							_sText.TrimLeft();
							_sText.TrimRight();
						} // while( _sText.Replace(_T("&"),_T("")) > 0 )
						if( _sText == m_sMdiWindowPopupName )
							bAppendMdiWindowsMenu = TRUE;
					} // if( _IsMdiApp() && (! m_sMdiWindowPopupName.IsEmpty() ) )
				} // else from if( mii.hSubMenu == nullptr )
				ASSERT( pCmdItem != nullptr );
				if( pCmdItem->m_sToolbarText.IsEmpty() )
					pCmdItem->m_sToolbarText = sText;
				if( pCmdItem->m_sMenuText.IsEmpty() )
					pCmdItem->m_sMenuText = sText;

                if (!_ExcludeMenu(sText))
                {
				    if( ! InsertButton(
						    nInsertButtonLocation,
						    nCmdID,
						    FALSE
						    )
					    )
				    {
					    ASSERT( FALSE );
					    return FALSE;
				    }
                    nMenuItemInsertIndex++;
				    if( mii.hSubMenu != nullptr )
				    {
					    ASSERT( ::IsMenu(mii.hSubMenu) );
					    SetButtonMenu(
						    nInsertButtonLocation,
						    mii.hSubMenu,
						    FALSE,
						    FALSE,
						    FALSE
						    );
				    } // if( mii.hSubMenu != nullptr )
				    if( bAppendMdiWindowsMenu )
				    {
					    VERIFY(
						    MarkButtonAsMdiWindowsMenu(
							    nInsertButtonLocation,
							    TRUE
							    )
						    );
				    } // if( bAppendMdiWindowsMenu )
                } // if (!_ExcludeMenu)
			} // for( UINT nMenuItemIndex = 0; nMenuItemIndex < nMenuItemCount; nMenuItemIndex++ )

			ASSERT( m_pRightBtn == nullptr );
			m_pRightBtn = OnCreateBarRightBtn();
			if( m_pRightBtn != nullptr )
			{
				ASSERT_VALID( m_pRightBtn );
				ASSERT_KINDOF( CExtBarContentExpandButton, m_pRightBtn );
				m_buttons.Add( m_pRightBtn );
			} // if( m_pRightBtn != nullptr )
		} // if( pMenu != nullptr && pMenu->GetSafeHmenu() != nullptr )
	} // else from if( pMenuInfo != nullptr )

	if( _IsMdiApp() )
	{
		if( !IsOleIpObjActive() )
			if( _InstallMdiDocButtons( FALSE ) )
				bDoRecalcLayout = TRUE;
		VERIFY( _SyncActiveMdiChild() );
	}
	
	if( bDoRecalcLayout )
	{
		Invalidate();
		_RecalcLayoutImpl();
		UpdateWindow();
		if( m_pDockSite != nullptr )
		{
			CFrameWnd * pFrame = GetParentFrame();
			ASSERT_VALID( pFrame );
			if( pFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd)) )
				pFrame->SetWindowPos(
					nullptr, 0, 0, 0, 0,
					SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
						|SWP_NOZORDER|SWP_NOOWNERZORDER
						|SWP_FRAMECHANGED
					);
		} // if( m_pDockSite != nullptr )
	} // if( bDoRecalcLayout )
	return TRUE;
}
// End prof-uis stuff...



// REVIEW: this is weak, tying it to the text of the menu header resources.
const key_value_pair<PCSTR, MDITabType> c_ResourceSpecificMenus[] =
{
    { "&Pic", TAB_PIC},
    { "Fo&nt", TAB_FONT},
    { "Voca&b", TAB_VOCAB},
    { "&View", TAB_VIEW},
};

bool CExtMenuControlBarHideShow::_ExcludeMenu(PCSTR pszName)
{
    bool fRet = false;
    CMainFrame *pMainFrame = static_cast<CMainFrame*>(GetParent());
    CFrameWnd *pActiveFrameTemp = pMainFrame->GetActiveFrame();
    MDITabType iCurrentTab = TAB_NONE;
    if (pActiveFrameTemp && pActiveFrameTemp->IsKindOf(RUNTIME_CLASS(CMDITabChildWnd)))
    {
        CMDITabChildWnd *pActiveFrame = static_cast<CMDITabChildWnd*>(pMainFrame->GetActiveFrame());
        iCurrentTab = pActiveFrame->GetTabType();
    }
    int iTabMenu = LookupStringValue(c_ResourceSpecificMenus, ARRAYSIZE(c_ResourceSpecificMenus), pszName, TAB_NONE);
    if (TAB_NONE != iTabMenu)
    {
        // This is a menu we might exclude.  We'll exclude it if the current type doesn't match the
        // tab type for this menu.
        fRet = (iTabMenu != iCurrentTab);
    }
    // else we don't care about this menu -> allow it
    return fRet;
}


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
    ON_COMMAND(ID_FILE_NEWPIC, OnFileNewPic)
    ON_COMMAND(ID_FILE_NEWGAME, OnFileNewGame)
    ON_COMMAND(ID_FILE_NEWVIEW, OnFileNewView)
    ON_COMMAND(ID_FILE_NEWFONT, OnFileNewFont)
    ON_COMMAND(ID_FILE_NEWCURSOR, OnFileNewCursor)
    ON_COMMAND(ID_FILE_NEWTEXT, OnFileNewText)
    ON_COMMAND(ID_FILE_NEWSOUND, OnFileNewSound)
    ON_COMMAND(ID_FILE_OPENRESOURCE, OnFileOpenResource)
    ON_COMMAND(ID_FILE_ADDRESOURCE, OnFileAddResource)
    ON_COMMAND(ID_EDIT_FINDINFILES, OnFindInFiles)
    ON_COMMAND(ID_PREFERENCES, OnShowPreferences)
    ON_COMMAND(ID_TOOLS_EXTRACTALLRESOURCES, OnExtractAllResources)
    ON_COMMAND(ID_TOOLS_REBUILDRESOURCES, OnRebuildResources)
    ON_COMMAND(ID_NEWROOM, OnNewRoom)
    ON_COMMAND(ID_NEWSCRIPT, OnNewScript)
    ON_COMMAND(ID_COMPILEALL, OnCompileAll)
    ON_COMMAND_EX(ID_SHOW_VIEWS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_PICS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_SCRIPTS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_TEXTS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_SOUNDS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_MEMORY, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_VOCABS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_FONTS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_CURSORS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_PATCHES, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_PALETTES, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_MESSAGES, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_HEAPS, OnShowResource)
    ON_COMMAND_EX(ID_SHOW_AUDIO, OnShowResource)
    ON_UPDATE_COMMAND_UI(ID_FILE_NEWPIC, OnUpdateNewPic)
    ON_UPDATE_COMMAND_UI(ID_FILE_NEWPIC, OnUpdateNewPic)
    ON_UPDATE_COMMAND_UI(ID_PREFERENCES, OnUpdateAlwaysEnabled)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_EXTRACTALLRESOURCES, OnUpdateShowIfGameLoaded)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_REBUILDRESOURCES, OnUpdateAlwaysEnabled)
    ON_COMMAND(ID_BACK, OnGoBack)
    ON_COMMAND(ID_FORWARD, OnGoForward)
    ON_COMMAND(ID_CLASSBROWSER, OnClassBrowser)
    ON_COMMAND(ID_SCRIPT_MANAGEDECOMPILATION, OnManageDecompilation)
    ON_UPDATE_COMMAND_UI(ID_CLASSBROWSER, OnUpdateClassBrowser)
    ON_UPDATE_COMMAND_UI(ID_BACK, OnUpdateBackForward)
    ON_UPDATE_COMMAND_UI(ID_FORWARD, OnUpdateBackForward)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_CLASSCOMBO, OnUpdateScriptCombo)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_FUNCTIONCOMBO, OnUpdateScriptCombo)
    ON_UPDATE_COMMAND_UI(ID_CLASSBROWSER, OnUpdateShowIfGameLoaded)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_MANAGEDECOMPILATION, OnUpdateShowIfGameLoaded)

    ON_COMMAND(ID_SELFTEST, OnSelfTest)
    ON_WM_TIMER()

    // Bar states
    // ID_BAR_GAME and ID_BAR_TOOLBOX can never be turned off, so no need
    //ON_COMMAND_EX(ID_BAR_GAME, OnBarCheck)
    //ON_UPDATE_COMMAND_UI(ID_BAR_GAME, OnUpdateControlBarMenu)
    //ON_COMMAND_EX(ID_BAR_TOOLBOX, OnBarCheck)
    //ON_UPDATE_COMMAND_UI(ID_BAR_TOOLBOX, OnUpdateControlBarMenu)
    ON_COMMAND_EX(ID_BAR_OUTPUT, OnBarCheck)
    ON_UPDATE_COMMAND_UI(ID_BAR_OUTPUT, OnUpdateControlBarMenu)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_dlgForPanelDialogPic(false), m_dlgForPanelDialogPicVGA(true)
{
    _pActiveFrame = nullptr;
    _fDidntGetDocYet = false;
    _fSelfTest = false;

    // Prof-UIS GUI persistence
    ::memset( &m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT) );
    m_dataFrameWP.length = sizeof(WINDOWPLACEMENT);
    m_dataFrameWP.showCmd = SW_HIDE;
	g_ResourceManager->SetLangLayout( LAYOUT_LTR );
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
    // Prof-UIS GUI persistence
    if (m_dataFrameWP.showCmd != SW_HIDE)
    {
        SetWindowPlacement(&m_dataFrameWP);
        __super::ActivateFrame(m_dataFrameWP.showCmd);
        m_dataFrameWP.showCmd = SW_HIDE;
        return;
    }
    __super::ActivateFrame(nCmdShow);
}

BOOL CMainFrame::DestroyWindow()
{
    // Prof-UIS GUI persistence
    CWinApp *pApp = ::AfxGetApp();
    ASSERT(pApp != nullptr);
    ASSERT(pApp->m_pszRegistryKey != nullptr);
    ASSERT(pApp->m_pszRegistryKey[0] != _T('\0'));
    ASSERT(pApp->m_pszProfileName != nullptr);
    ASSERT(pApp->m_pszProfileName[0] != _T('\0'));
    VERIFY(CExtControlBar::ProfileBarStateSave(this, pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName));
    return __super::DestroyWindow();
}

CMainFrame::~CMainFrame()
{
}

const struct
{
    UINT nCommandId;
    ResourceType iType;
    PCSTR pszShortText;
    PCSTR pszLongText;
    PCSTR pszTipText;
    UINT iconResource;
}
c_ShowResourceCommands[] =
{
    { ID_SHOW_VIEWS, ResourceType::View, "Views", "Show views", "Show view resources", IDI_VIEW },
    { ID_SHOW_PICS, ResourceType::Pic, "Pics", "Show pics", "Show pic resources", IDI_PIC },
    { ID_SHOW_SCRIPTS, ResourceType::Script, "Scripts", "Show scripts", "Show script resources", IDI_SCRIPT },
    { ID_SHOW_TEXTS, ResourceType::Text, "Texts", "Show texts", "Show text resources", IDI_TEXT },
    { ID_SHOW_SOUNDS, ResourceType::Sound, "Sounds", "Show sounds", "Show sound resources", IDI_SOUND },
    { ID_SHOW_VOCABS, ResourceType::Vocab, "Vocabs", "Show vocabs", "Show vocab resources", IDI_VOCAB },
    { ID_SHOW_FONTS, ResourceType::Font, "Fonts", "Show fonts", "Show font resources", IDI_FONT },
    { ID_SHOW_CURSORS, ResourceType::Cursor, "Cursors", "Show cursors", "Show cursor resources", IDI_CURSOR },
    { ID_SHOW_PATCHES, ResourceType::Patch, "Patches", "Show patches", "Show patch resources", IDI_PATCH },
    { ID_SHOW_PALETTES, ResourceType::Palette, "Palettes", "Show palettes", "Show palette resources", IDI_PALETTEVGA },
    { ID_SHOW_MESSAGES, ResourceType::Message, "Messages", "Show messages", "Show message resources", IDI_TEXT },
    { ID_SHOW_HEAPS, ResourceType::Heap, "Heaps", "Show heaps", "Show heap resources", IDI_SCRIPT },
    { ID_SHOW_AUDIO, ResourceType::Audio, "Audio", "Show audio", "Show audio resources", IDI_SOUND },
};

void _AssignIcons(const key_value_pair<UINT, int> *rg, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        // Set the 24x24 icons used in the toolbars
        HICON hicon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(rg[i].value), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);        
        g_CmdManager->CmdSetIcon(appState->_pszCommandProfile,
            rg[i].key,
            hicon,
            true);
        DeleteObject(hicon);
    }
};

void RegisterCommands()
{
    CExtCmdItem *pCmdItem;
    // For some commands that don't exist in toolbar or menu resources.

    // Screens toolbar - this exists, but is per-pic.  So assign icons here.
    pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, ID_SHOWVISUALSCREEN);
    pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, ID_SHOWPRIORITYSCREEN);
    pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, ID_SHOWCONTROLSCREEN);
    static const key_value_pair<UINT, int> c_screenIcons[] =
    {
        { ID_SHOWVISUALSCREEN, IDI_SCREENVISUAL },
        { ID_SHOWPRIORITYSCREEN, IDI_SCREENPRIORITY },
        { ID_SHOWCONTROLSCREEN, IDI_SCREENCONTROL },
    };
    _AssignIcons(c_screenIcons, ARRAYSIZE(c_screenIcons));

    // Zoom slider
	pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, ID_ZOOMSLIDER);
	pCmdItem->m_sToolbarText = "Zoom:";
	pCmdItem->m_sMenuText = "Zoom slider";
	pCmdItem->m_sTipTool = pCmdItem->m_sTipStatus = "Zoom slider";

    // Alpha slider
	pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, ID_ALPHASLIDER);
	pCmdItem->m_sToolbarText = "Opacity:";
	pCmdItem->m_sMenuText = "Trace opacity";
	pCmdItem->m_sTipTool = pCmdItem->m_sTipStatus = "Trace image opacity";

    // Tempo slider
	pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, ID_TEMPOSLIDER);
	pCmdItem->m_sToolbarText = "Speed:";
	pCmdItem->m_sMenuText = "Speed";
	pCmdItem->m_sTipTool = pCmdItem->m_sTipStatus = "Speed (bpm)";

    // The generic "show resources" commands.
    for (int i = 0; i < ARRAYSIZE(c_ShowResourceCommands); i++)
    {
        pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, c_ShowResourceCommands[i].nCommandId);
        pCmdItem->m_sToolbarText = c_ShowResourceCommands[i].pszShortText;
	    pCmdItem->m_sMenuText = c_ShowResourceCommands[i].pszLongText;
	    pCmdItem->m_sTipTool = pCmdItem->m_sTipStatus = c_ShowResourceCommands[i].pszTipText;
        HICON hicon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(c_ShowResourceCommands[i].iconResource), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);
        g_CmdManager->CmdSetIcon(appState->_pszCommandProfile,
            c_ShowResourceCommands[i].nCommandId,
            hicon,
            true);
        DeleteObject(hicon);
    }

    HICON hicon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_COMPILEALL), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);
    g_CmdManager->CmdSetIcon(appState->_pszCommandProfile,
        ID_COMPILEALL,
        hicon,
        true);
    DeleteObject(hicon);
}

void CMainFrame::OnDestroy()
{
    g_CmdManager->ProfileWndRemove(GetSafeHwnd());
    __super::OnDestroy();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    
    g_CmdManager->ProfileSetup(appState->_pszCommandProfile, GetSafeHwnd());
    g_CmdManager->UpdateFromMenu(appState->_pszCommandProfile, IDR_MAINFRAME);
    RegisterCommands();

    CExtPopupColorMenuWnd::g_bMenuExpanding = false;
    CExtPopupColorMenuWnd::g_bMenuHighlightRarely = false;

    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    m_wndMenuBar.SetMdiWindowPopupName("&Window");
    if(!m_wndMenuBar.Create(nullptr, this, IDR_MAINFRAME))
    {
        TRACE0("Failed to create menubar\n");
        return -1;
    }

    if( !m_wndToolBar.Create("Main toolbar", this, AFX_IDW_TOOLBAR) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
   

    // We don't actually show this, but we use its m_nMenuMarkerID member variable.
    if (!m_ThemeSwitcher.Create("theme swtich toolbar", this, AFX_IDW_TOOLBAR))
    {
        return -1;
    }
    m_ThemeSwitcher.m_bAppearInDockSiteControlBarPopupMenu = false; // Don't show in prof-uis menus
	ASSERT( m_ThemeSwitcher.GetSafeHwnd() != nullptr );
	if(	! m_ThemeSwitcher.ThemeSwitcherInit() )
	{
		ASSERT( FALSE );
		return -1;
	}
    m_ThemeSwitcher.ShowWindow(SW_HIDE);



    // These are just placeholders...
    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    if (!m_BrowseInfoStatus.Create("Browse info status", WS_CHILD|WS_VISIBLE|WS_TABSTOP|SS_CENTER|SS_NOTIFY,
        CRect(0, 0, 0, 0), &m_wndStatusBar, 0))
    {
        TRACE0("Failed to create browse status pane\n");
        return -1;
    }
    if (!m_wndStatusBar.AddPane(ID_BROWSEINFOPANE, 0)) // position zero?
    {
        TRACE0("Failed to add browse status pane\n");
        return -1;
    }
	int nIndex = m_wndStatusBar.CommandToIndex(ID_BROWSEINFOPANE);
    if (nIndex != -1)
    {
	    m_wndStatusBar.SetPaneWidth(nIndex, 170);
	    m_wndStatusBar.SetPaneControl(&m_BrowseInfoStatus, ID_BROWSEINFOPANE, true);
    }
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    {
        ClassBrowserLock lock(browser);
        lock.Lock();
        browser.SetClassBrowserEvents(&m_BrowseInfoStatus);
    }

    // The tab control at the top
    m_wndTab.Create(WS_VISIBLE | WS_CHILD | TCS_OWNERDRAWFIXED, CRect(0, 0, 0, 0), this, AFX_IDW_DIALOGBAR);

    // Seems like we need to set this desired size prior to creating the control.
    m_wndResizableBarGeneral.SetInitDesiredSizeVertical(CSize(180, 100));
    if (!m_wndResizableBarGeneral.Create("Toolbox", this, ID_BAR_TOOLBOX))
    {
	    TRACE0("Failed to create m_wndResizableBar_View\n");
	    return -1;		// fail to create
    }

    if (!m_dlgEmpty.Create(CBarContainerDialog::IDD, &m_wndResizableBarGeneral))
    {
	    TRACE0("Failed to create empty dialog\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogFont.Create(IDD_DIALOGCELFONT2, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_wndResizableBar_View\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogView.Create(IDD_DIALOGCELVIEW2, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_wndResizableBar_View\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogCursor.Create(IDD_DIALOGCELCURSOR2, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_wndResizableBar_Cursor\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogPic.Create(IDD_PICCOMMANDS, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_wndResizableBar_Pic\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogPicVGA.Create(IDD_PICCOMMANDS_VGA, &m_dlgEmpty))
    {
        TRACE0("Failed to create m_wndResizableBar_Pic\n");
        return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogScript.Create(IDD_QUICKSCRIPTS, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_dlgForPanelDialogScript\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogGame.Create(IDD_SAMPLES, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_dlgForPanelDialogGame\n");
	    return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogSound.Create(IDD_SOUNDTOOLBOX, &m_dlgEmpty))
    {
	    TRACE0("Failed to create m_dlgForPanelDialogSound\n");
	    return -1;		// fail to create
    }

    // The output area at the bottom (which only appears when necessary)
    if (!m_wndResizableBarOutput.Create("Output", this, ID_BAR_OUTPUT))
    {
	    TRACE0("Failed to create m_wndResizableBarOutput\n");
	    return -1;		// fail to create
    }
    m_wndOutput.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, CRect(0, 0, 0, 0),
        &m_wndResizableBarOutput, ID_CONTROL_OUTPUTPANE);

    //
    // Now the resource-specific toolbars, all but one of which will be hidden.
    //

    // Game explorer
    if (!m_wndExplorerTools.Create("explorer", this, ID_BAR_TOOLEXPLORER) ||
        !m_wndExplorerTools.LoadToolBar(IDR_TOOLBARVIEWS))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    _PrepareExplorerCommands();
    m_wndExplorerTools.InsertButton();
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_VIEWS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_PICS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_SCRIPTS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_TEXTS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_SOUNDS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_VOCABS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_FONTS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_CURSORS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_PATCHES);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_PALETTES);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_MESSAGES);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_HEAPS);
    m_wndExplorerTools.InsertButton(-1, ID_SHOW_AUDIO);
    m_wndExplorerTools.InitContentExpandButton(); // Overflow...

    // Pics
    if( !m_wndPicTools.Create("pic", this, ID_BAR_TOOLPIC) ||
        !m_wndPicTools.LoadToolBar(IDR_TOOLBARPIC))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    _PreparePicCommands();
    ShowControlBar(&m_wndPicTools, FALSE, FALSE);
    ASSERT(g_pPicAlphaSlider == nullptr);
	g_pPicAlphaSlider = new CExtAlphaSlider(&m_wndPicTools, ID_ALPHASLIDER);
	VERIFY(m_wndPicTools.InsertSpecButton(-1, g_pPicAlphaSlider, FALSE));


    // Scripts
    if( !m_wndScriptTools.Create("scripts", this, ID_BAR_TOOLSCRIPT) ||
        !m_wndScriptTools.LoadToolBar(IDR_TOOLBARSCRIPT))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    ShowControlBar(&m_wndScriptTools, FALSE, FALSE);
    _PrepareScriptCommands();
#ifndef DOCSUPPORT
    for (int i = 0; i < m_wndScriptTools.GetButtonsCount(); i++)
    {
        if (m_wndScriptTools.GetButtonID(i) == ID_COMPILEDOCS)
        {
            m_wndScriptTools.RemoveButton(i, true);
            break;
        }
    }
#endif

    // Add two comboboxes to the script toolbar
    // Class combo
    m_wndScriptTools.InsertButton(); // Separator
    if (!m_wndScriptToolComboBoxClass.Create(
            WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS | CBS_OWNERDRAWFIXED,
            CRect(0, 0, 200, 100),
            &m_wndScriptTools,
            ID_SCRIPT_CLASSCOMBO))
    {
        TRACE0("Failed to create script class combo");
        return -1;
    }
	m_wndScriptTools.InsertButton(-1, ID_SCRIPT_CLASSCOMBO);
	VERIFY(m_wndScriptTools.SetButtonCtrl(m_wndScriptTools.CommandToIndex(ID_SCRIPT_CLASSCOMBO), &m_wndScriptToolComboBoxClass));
	m_wndScriptToolComboBoxClass.SetFont(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

	m_wndScriptToolComboBoxClass.AddString("test class");
	m_wndScriptToolComboBoxClass.SetCurSel( 0 );

    /*
    // Function combo
    m_wndScriptTools.InsertButton(); // Separator
    if (!m_wndScriptToolComboBoxFunction.Create(
            WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS,
            CRect(0, 0, 200, 100),
            &m_wndScriptTools,
            ID_SCRIPT_FUNCTIONCOMBO))
    {
        TRACE0("Failed to create script function combo");
        return -1;
    }
	m_wndScriptTools.InsertButton(-1, ID_SCRIPT_FUNCTIONCOMBO);
	VERIFY(m_wndScriptTools.SetButtonCtrl(m_wndScriptTools.CommandToIndex(ID_SCRIPT_FUNCTIONCOMBO), &m_wndScriptToolComboBoxFunction));
    m_wndScriptToolComboBoxFunction.SetFont(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
*/


    // Text resources
    if( !m_wndTextTools.Create("text", this, ID_BAR_TOOLTEXT) ||
        !m_wndTextTools.LoadToolBar(IDR_TOOLBARTEXT))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    _PrepareTextCommands();
    ShowControlBar(&m_wndTextTools, FALSE, FALSE);

    // Sound resources
    if( !m_wndSoundTools.Create("sound", this, ID_BAR_TOOLSOUND) ||
        !m_wndSoundTools.LoadToolBar(IDR_TOOLBARSOUND))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    _PrepareSoundCommands();
    ShowControlBar(&m_wndSoundTools, FALSE, FALSE);
    ASSERT(g_pSoundTempoSlider == nullptr);
	g_pSoundTempoSlider = new CExtTempoSlider(&m_wndSoundTools, ID_TEMPOSLIDER);
	VERIFY(m_wndSoundTools.InsertSpecButton(-1, g_pSoundTempoSlider, FALSE));


    // Vocab
    if( !m_wndVocabTools.Create("vocab", this, ID_BAR_TOOLVOCAB) ||
        !m_wndVocabTools.LoadToolBar(IDR_TOOLBARVOCAB))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    _PrepareVocabCommands();
    ShowControlBar(&m_wndVocabTools, FALSE, FALSE);

    // Raster (view, font, cursor)
    if( !m_wndViewTools.Create("view", this, ID_BAR_TOOLRASTER) ||
        !m_wndViewTools.LoadToolBar(IDR_TOOLBARDRAWVIEW))
    {
        // If you assert in the above function, it means that placeholder IDR_TOOLBARDRAWVIEW doesn't have 
        // the right number of icons.
        TRACE0( "Failed to create toolbar" );
        return -1;
    }
    ShowControlBar(&m_wndViewTools, FALSE, FALSE);
    _PrepareRasterCommands();

    // Prof-UIS gui persistence
    CWinApp *pApp = ::AfxGetApp();
    /*if  (!CExtControlBar::ProfileBarStateLoad(this,
            pApp->m_pszRegistryKey,
            pApp->m_pszProfileName,
            pApp->m_pszProfileName,
            &m_dataFrameWP))*/
    // REVIEW: crash in Prof-UIS when trying to restore state.
    if (TRUE)
    {
	    m_wndResizableBarGeneral.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
        m_wndResizableBarOutput.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);

        EnableDocking(CBRS_ALIGN_ANY);

        if (!CExtControlBar::FrameEnableDocking(this))
        {
            ASSERT( FALSE );
            return -1;
        }

        m_wndResizableBarGeneral.DockControlBarInnerOuter(AFX_IDW_DOCKBAR_LEFT, true, this, true);
        m_wndResizableBarOutput.DockControlBarInnerOuter(AFX_IDW_DOCKBAR_BOTTOM, true, this, true);
        // Hidden by default:
        ShowControlBar(&m_wndResizableBarOutput, FALSE, FALSE);
    }

    _PrepareMainCommands();

    return 0;
}

void CMainFrame::_PrepareExplorerCommands()
{
    static const key_value_pair<UINT, int> c_explorerIcons[] =
    {
        { ID_VIEWTHUMBNAILS, IDI_VIEWICONS },
        { ID_VIEWDETAILS, IDI_VIEWDETAILS },
    };
    _AssignIcons(c_explorerIcons, ARRAYSIZE(c_explorerIcons));
}

void CMainFrame::_PrepareRasterCommands()
{
    static const key_value_pair<UINT, int> c_rasterIcons[] =
    {
        { ID_PENTOOL, IDI_BRUSH },
        { ID_LINE, IDI_LINE },
        { ID_FILL, IDI_FLOODFILL },
        { ID_EYEDROPPER, IDI_EYEDROPPER },
        { ID_SETTRANSPARENCY, IDI_TRANSPARENCY },
        { ID_REPLACE, IDI_REPLACECOLOUR },
        { ID_ZOOM, IDI_ZOOM },
        { ID_ANIMATE, IDI_ANIMATE },
        { ID_IMPORT_IMAGESEQUENCE, IDI_IMAGESEQUENCE },
        { ID_DRAWOVAL, IDI_ELLIPSE },
        { ID_DRAWOVALSOLID, IDI_ELLIPSE_SOLID },
        { ID_DRAWRECT, IDI_RECT },
        { ID_DRAWRECTSOLID, IDI_RECT_SOLID },
        { ID_DRAWROUNDEDRECT, IDI_ROUNDEDRECT },
        { ID_DRAWROUNDEDRECTSOLID, IDI_ROUNDEDRECT_SOLID },
        { ID_FLIPHORZ, IDI_FLIPHORZ },
        { ID_FLIPVERT, IDI_FLIPVERT },
        { ID_INVERT, IDI_INVERT },
        { ID_GREYSCALE, IDI_GREYSCALE },
        { ID_DITHER, IDI_DITHER },
        { ID_SETPLACEMENT, IDI_PLACEMENT },
    };
    _AssignIcons(c_rasterIcons, ARRAYSIZE(c_rasterIcons));
}

void CMainFrame::_PreparePicCommands()
{
    static const key_value_pair<UINT, int> c_picIcons[] =
    {
        { ID_HISTORY, IDI_HISTORY },
        { ID_SHOWTRACEIMAGE, IDI_SHOWTRACEIMAGE },
        { ID_TRACEBITMAP, IDI_LOADTRACEIMAGE },
        { ID_DEFINEPALETTES, IDI_PALETTE },
        { ID_CIRCLE, IDI_ELLIPSE },
        { ID_TOGGLEEGO, IDI_FAKEEGO },
        { ID_TOGGLEPRIORITYLINES, IDI_PRIORITYLINES },
        { ID_LIGHTUPCOORDS, IDI_LIGHTUP },
        { ID_SHOWPALETTE0, IDI_PALETTE0 },
        { ID_SHOWPALETTE1, IDI_PALETTE1 },
        { ID_SHOWPALETTE2, IDI_PALETTE2 },
        { ID_SHOWPALETTE3, IDI_PALETTE3 },
    };
    _AssignIcons(c_picIcons, ARRAYSIZE(c_picIcons));
}

void CMainFrame::_PrepareScriptCommands()
{
    static const key_value_pair<UINT, int> c_scriptIcons[] =
    {
        { ID_COMPILEALL, IDI_COMPILEALL },
        { ID_COMPILE, IDI_COMPILE },
        { ID_INSERTOBJECT, IDI_SCRIPT_INSERT_OBJECT },
        { ID_CONVERTSCRIPT, IDI_SCRIPT_CONVERT },
    };
    _AssignIcons(c_scriptIcons, ARRAYSIZE(c_scriptIcons));
}

void CMainFrame::_PrepareTextCommands()
{
    static const key_value_pair<UINT, int> c_textIcons[] =
    {
        { ID_MOVEUP, IDI_MOVEUP },
        { ID_MOVEDOWN, IDI_MOVEDOWN },
    };
    _AssignIcons(c_textIcons, ARRAYSIZE(c_textIcons));
}

void CMainFrame::_PrepareVocabCommands()
{
    static const key_value_pair<UINT, int> c_vocabIcons[] =
    {
        { ID_VOCAB_NEWWORD, IDI_NEWWORD },
    };
    _AssignIcons(c_vocabIcons, ARRAYSIZE(c_vocabIcons));
}

void CMainFrame::_PrepareSoundCommands()
{
    static const key_value_pair<UINT, int> c_soundIcons[] =
    {
        { ID_PLAY, IDI_PLAY },
        { ID_PAUSE, IDI_PAUSE },
        { ID_STOP, IDI_STOP },
    };
    _AssignIcons(c_soundIcons, ARRAYSIZE(c_soundIcons));
}

void CMainFrame::_PrepareMainCommands()
{
    static const key_value_pair<UINT, int> c_mainIcons[] =
    {
        { ID_FILE_SAVE, IDI_FILE_SAVE },
        { ID_FILE_OPEN, IDI_FILE_OPEN },
        { ID_EDIT_CUT, IDI_EDIT_CUT },
        { ID_EDIT_PASTE, IDI_EDIT_PASTE },
        { ID_EDIT_COPY, IDI_EDIT_COPY },
        { ID_EDIT_DELETE, IDI_DELETE },
        { ID_EDIT_FIND, IDI_EDIT_FIND },
        { ID_TOOLS_REBUILDRESOURCES, IDI_REBUILDRESOURCES },
        { ID_RUNGAME, IDI_RUNGAME },
        { ID_GAME_PROPERTIES, IDI_GAMEPROPERTIES },
        { ID_BACK, IDI_BACK },
        { ID_FORWARD, IDI_FORWARD },
        { ID_FILE_NEWSOUND, IDI_NEWSOUND },
        { ID_FILE_NEWTEXT, IDI_NEWTEXT },
        { ID_FILE_NEWFONT, IDI_NEWFONT },
        { ID_FILE_NEWVIEW, IDI_NEWVIEW },
        { ID_FILE_NEWPIC, IDI_NEWPIC },
        { ID_NEWROOM, IDI_NEWROOM },
        { ID_NEWSCRIPT, IDI_NEWSCRIPT },
        { ID_FILE_NEWCURSOR, IDI_NEWCURSOR },
        { ID_PREFERENCES, IDI_PREFERENCES },
    };
    _AssignIcons(c_mainIcons, ARRAYSIZE(c_mainIcons));
}

void CMainFrame::ShowOutputPane()
{
    if (!m_wndResizableBarOutput.IsVisible())
    {
        ShowControlBar(&m_wndResizableBarOutput, TRUE, FALSE);
    }
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if(m_wndMenuBar.TranslateMainFrameMessage(pMsg))
    {
        return TRUE;
    }
    if(m_ThemeSwitcher.PreTranslateMessage(pMsg))
    {
        return TRUE;
    }
    return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

// Tabs

void CMainFrame::AddTab(CFrameWnd *pNewFrame, MDITabType iType)
{
    m_wndTab.AddTab(pNewFrame, iType);
}

void CMainFrame::RemoveTab(CFrameWnd *pNewFrame)
{
    m_wndTab.RemoveTab(pNewFrame);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}

void CMainFrame::OnFileNewPic()
{
    // Get the document template, so we can create a new CPicDoc.
    CDocTemplate *pDocTemplate = appState->GetPicTemplate();
    if (pDocTemplate)
    {
        HRESULT hr = S_OK;
        // and create the pic resource for it.
        CPicDoc *pDocument = (CPicDoc*)pDocTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> pEditPic(CreateDefaultPicResource(appState->GetVersion()));
            pDocument->SetEditPic(move(pEditPic));
        }
    }
}

void CMainFrame::OnFileNewGame()
{
    // Get the document template, so we can create a new CPicDoc.
    CDocTemplate *pDocTemplate = appState->GetPicTemplate();
    NewGameDialog dialog;
    dialog.DoModal();
}

#define DECLARE_NEWRESOURCE_AP2(__templateFunc, __documentClass, __resourceClass, __resourceCreate, __resourceSetter) \
    CDocTemplate *pDocTemplate = appState->__templateFunc(); \
    if (pDocTemplate) \
        { \
        HRESULT hr = S_OK; \
        __documentClass *pDocument = (__documentClass*)pDocTemplate->OpenDocumentFile(nullptr, TRUE); \
        if (pDocument) \
                { \
            std::unique_ptr<__resourceClass> pEVR(__resourceCreate(appState->GetVersion())); \
            if (SUCCEEDED(hr)) \
                        { \
                pDocument->__resourceSetter(std::move(pEVR)); \
                        } \
            if (FAILED(hr)) \
                        { \
                delete pDocument; \
                        } \
                } \
        }

void CMainFrame::OnFileNewView()
{
    DECLARE_NEWRESOURCE_AP2(GetViewTemplate, CNewRasterResourceDocument, ResourceEntity, CreateDefaultViewResource, SetNewResource)
}

void CMainFrame::OnFileNewFont()
{
    DECLARE_NEWRESOURCE_AP2(GetFontTemplate, CNewRasterResourceDocument, ResourceEntity, CreateDefaultFontResource, SetNewResource)
}

void CMainFrame::OnFileNewCursor()
{
    DECLARE_NEWRESOURCE_AP2(GetCursorTemplate, CNewRasterResourceDocument, ResourceEntity, CreateDefaultCursorResource, SetNewResource)
}

void CMainFrame::OnFileNewText()
{
    DECLARE_NEWRESOURCE_AP2(GetTextTemplate, CTextDoc, ResourceEntity, CreateDefaultTextResource, SetTextResource)
}

void CMainFrame::OnFileNewSound()
{
    std::unique_ptr<ResourceEntity> pSound(ImportMidi());
    if (pSound)
    {
        CDocTemplate *pDocTemplate = appState->GetSoundTemplate();
        if (pDocTemplate)
        {
            HRESULT hr = S_OK;
            CSoundDoc *pDocument = (CSoundDoc*)pDocTemplate->OpenDocumentFile(nullptr, TRUE);
            if (pDocument)
            {
                // Also set the device to GM, since that is what has channels enabled by default...
                pDocument->SetDevice(DeviceType::NewGM, false); // Don't notify, since we have no sound yet.
                pDocument->SetSoundResource(std::move(pSound));
                pDocument->SetModifiedFlag(TRUE);
            }
        }
    }
}

#define PATCH_FILE_TYPES "pic.*;view.*;vocab.*;font.*;cursor.*;text.*;sound.*;patch.*;script.*;*.v56;*.p56;*.scr;*.tex;*.snd;*.voc;*.fon;*.cur;*.pat;*.bit;*.pal;*.cda;*.aud;*.syn;*.msg;*.hep;*.map"

const TCHAR g_szResourceSpec[] = PATCH_FILE_TYPES;

const TCHAR g_szResourceFilter[] = TEXT("All resources|" PATCH_FILE_TYPES "|"
    "Pics (pic.*)|pic.*;*.p56|"
    "Views (view.*)|view.*;*.v56|"
    "Vocabs (vocab.*)|vocab.*;*.voc|"
    "Fonts (font.*)|font.*;*.fon|"
    "Cursors (cursor.*)|cursor.*;*.cur|"
    "Texts (test.*)|text.*;*.tex|"
    "Sounds (sound.*)|sound.*;*.snd|"
    "Patches (patch.*)|patch.*;*.pat|"
    "Scripts (script.*)|script.*;*.scr|"
    "Messages (*.msg)|*.msg|"
    "Palettes (*.pal)|*.pal|"
    "Audio (*.aud)|*.aud|"
    "CD Audio (*.cda)|*.cda|"
    "All files (*.*)|*.*|");

//
// Opens a resource from a file
//
void CMainFrame::OnFileOpenResource()
{
    CFileDialog fileDialog(TRUE, nullptr, nullptr, OFN_HIDEREADONLY, g_szResourceFilter);
    if (IDOK == fileDialog.DoModal())
    {
		CString strFileName = fileDialog.GetPathName();
        ResourceBlob data;
        HRESULT hr = data.CreateFromFile(nullptr, (PCSTR)strFileName, appState->GetVersion());
        if (SUCCEEDED(hr))
        {
            OpenResource(&data);
        }
        else
        {
            DisplayFileError(hr, TRUE, strFileName);
        }
    }
}

int ResourceNumberFromFileName(PCTSTR pszFileName)
{
    int iNumber = -1;
    PCTSTR pszExt = PathFindExtension(pszFileName);
    if (pszExt && *pszExt && *(pszExt + 1))
    {
        if (isdigit(*(pszExt + 1)))
        {
            iNumber = StrToInt(pszExt + 1);
        }
        else
        {
            PCTSTR pszJustFileName = PathFindFileName(pszFileName);
            if (pszJustFileName && *pszJustFileName && isdigit(*pszJustFileName))
            {
                iNumber = StrToInt(pszJustFileName);
            }
        }

    }
    return iNumber;
}

//
// Adds a file resource to the game directly, w/o opening it
//
void CMainFrame::OnFileAddResource()
{
    CFileDialog fileDialog(TRUE, nullptr, nullptr, OFN_HIDEREADONLY, g_szResourceFilter);
    if (IDOK == fileDialog.DoModal())
    {
		CString strFileName = fileDialog.GetPathName();
        // Get a resource number and package
        SaveResourceDialog srd;
        srd.Init(1, ResourceNumberFromFileName(strFileName));
        if (IDOK == srd.DoModal())
        {
            int iResourceNumber = srd.GetResourceNumber();
            int iPackageNumber = srd.GetPackageNumber();
            ResourceBlob data;
            HRESULT hr = data.CreateFromFile(nullptr, (PCSTR)strFileName, appState->GetVersion(), iPackageNumber, iResourceNumber);
            data.SetName(nullptr);
            if (SUCCEEDED(hr))
            {
                appState->GetResourceMap().AppendResource(data);
            }
            else
            {
                DisplayFileError(hr, TRUE, strFileName);
            }
        }
    }
}

void CMainFrame::OnUpdateNewPic(CCmdUI *pCmdUI)
{
    // Always available.
    pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateShowIfGameLoaded(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->GetResourceMap().IsGameLoaded());
}


void CMainFrame::_HideTabIfNot(MDITabType iTabTypeCurrent, MDITabType iTabTypeCompare, CExtControlBar &bar)
{
    if ((iTabTypeCurrent &  iTabTypeCompare) == 0)
    {
        // Hide it if it's visible.
        if (bar.IsWindowVisible())
        {
            ShowControlBar(&bar, FALSE, TRUE);
        }
    }
}

void CMainFrame::OnIdleUpdateCmdUI()
{
    // Update the titles of tabs.
    m_wndTab.OnUpdateTitles();

    _RefreshToolboxPanel(GetActiveFrame());

    __super::OnIdleUpdateCmdUI();
}

void CMainFrame::OnShowPreferences()
{
    CPreferencesDialog dialog;
    if (IDOK == dialog.DoModal())
    {
        // This code doesn't work because no view is ever active when the dialog is up.
        // It's also flawed because we're sending a pic-specific message to any old document.
        // A better solution would be to call some method on the "current" document.
        /*
        CDocument* pDocument = GetActiveDocument();
        if (pDocument)
        {
            pDocument->UpdateAllViews(nullptr, 0, &WrapHint(PicChangeHint::Preferences));
        }*/
    }
}

void CMainFrame::OnRebuildResources()
{
    appState->GetResourceMap().PurgeUnnecessaryResources();
}

void CMainFrame::OnExtractAllResources()
{
    ExtractAllDialog extractAll;
    extractAll.DoModal();
}

void CMainFrame::_OnNewScriptDialog(CNewScriptDialog &dialog)
{
    if (IDOK == dialog.DoModal())
    {
        ScriptId script = dialog.GetScript();
        std::string &strBuffer = dialog.GetScriptBuffer();
        CMultiDocTemplate *pScriptTemplate = appState->GetScriptTemplate();
        if (pScriptTemplate)
        {
            CScriptDocument *pDoc = static_cast<CScriptDocument*>(pScriptTemplate->OpenDocumentFile(nullptr));
            if (pDoc)
            {
                pDoc->SetNameAndContent(script, dialog.GetNumber(), strBuffer);
            }
        }
    }    
}

void CMainFrame::OnNewRoom()
{
    CNewRoomDialog dialog;
    _OnNewScriptDialog(dialog);
}

void CMainFrame::OnNewScript()
{
    CNewScriptDialog dialog;
    _OnNewScriptDialog(dialog);
}

void CMainFrame::OnCompileAll()
{
    CPrecisionTimer timer;
    timer.Start();

    // Clear out results
    appState->ShowOutputPane();
    appState->OutputClearResults();
    {
        DeferResourceAppend defer(appState->GetResourceMap());
        CNewCompileDialog dialog;
        dialog.DoModal();
        dialog.GetResult();
        defer.Commit();
    }

    timer.Stop();

    CompileLog log;
    log.ReportResult(CompileResult("--------------------------------"));
    std::stringstream strMessage;
    strMessage << "Time elapsed: " << (int)timer.Stop() << " seconds.";
    log.ReportResult(CompileResult(strMessage.str()));
    log.CalculateErrors();

    // Even if we weren't completely successful, update the script resource view, since some
    // scripts may have been successful.
    appState->GetResourceMap().NotifyToResourceResourceType(ResourceType::Script);

    // Get the final success/fail message up there:
    appState->OutputAddBatch(log.Results());
    appState->OutputFinishAdd();
}



void CMainFrame::_AddFindResults(ICompileLog &log, PCTSTR pszFullPath, PCTSTR pszFileName, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(pszFullPath))
    {
        int cLines = buffer.GetLineCount();
        for (int nLine = 0; nLine < cLines; nLine++)
        {
            int nLineLength = buffer.GetLineLength(nLine);
            if (nLineLength > 0)
            {
                CString strWhat = pszWhat;

                PTSTR pszLine = buffer.GetLineChars(nLine);
				CString line;
				lstrcpyn(line.GetBuffer(nLineLength + 1), pszLine, nLineLength + 1);
				line.ReleaseBuffer();
				if (!fMatchCase)
                {
				    line.MakeUpper();
                    strWhat.MakeUpper();
                }
                int nPos = FindStringHelper((LPCTSTR)line, (LPCTSTR)strWhat, fWholeWord);
				if (nPos >= 0)
                {
                    // Remove tabs from the line (they don't look good in a listbox)
				    CString lineCleansed;
				    lstrcpyn(lineCleansed.GetBuffer(nLineLength + 1), pszLine, nLineLength + 1);
				    lineCleansed.ReleaseBuffer();
                    lineCleansed.Remove(TEXT('\t'));

                    TCHAR szDescription[MAX_PATH];
                    StringCchPrintf(szDescription, ARRAYSIZE(szDescription), TEXT("%s: %s"), pszFileName, (PCTSTR)lineCleansed);
                    log.ReportResult(CompileResult(szDescription, ScriptId(pszFullPath), nLine + 1));
                }
            }
        }
    }
    buffer.FreeAll();
}

void CMainFrame::_FindInFilesOfType(ICompileLog &log, PCTSTR pszWildcard, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    std::string srcFolder = appState->GetResourceMap().Helper().GetSrcFolder();
    std::string wildcard = (srcFolder + pszWildcard);
    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFFF = FindFirstFile(wildcard.c_str(), &findData);
    if (hFFF != INVALID_HANDLE_VALUE)
    {
        BOOL fOk = TRUE;
        while(fOk)
        {
            std::string strFullPath = srcFolder;
            strFullPath += "\\";
            strFullPath += (PCTSTR)findData.cFileName;
            _AddFindResults(log, strFullPath.c_str(), findData.cFileName, pszWhat, fMatchCase, fWholeWord);
            fOk = FindNextFile(hFFF, &findData);
        }
        FindClose(hFFF);
    }
}

const int TextRangeOutsideResultToShow = 35;

void CMainFrame::_FindInTexts(ICompileLog &log, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    auto container = appState->GetResourceMap().Resources(ResourceTypeFlags::Text | ResourceTypeFlags::Message, ResourceEnumFlags::MostRecentOnly);
    for (auto &blob : *container)
    {
        auto resource = CreateResourceFromResourceData(*blob);
        TextComponent &textComponent = resource->GetComponent<TextComponent>();
        for (size_t i = 0; i < textComponent.Texts.size(); i++)
        {
            std::string text = textComponent.Texts[i].Text;
            if (!fMatchCase)
            {
                ToUpper(text);
            }
            int pos = FindStringHelper(text.c_str(), pszWhat, fWholeWord);
            if (pos != -1)
            {
                int rangeStart = max(0, pos - TextRangeOutsideResultToShow);
                int rangeEnd = min(pos + TextRangeOutsideResultToShow + lstrlen(pszWhat), (int)text.size());
                fmt::MemoryWriter writer;
                writer << GetResourceInfo(blob->GetType()).pszTitleDefault << " (" << blob->GetNumber() << ", " << i << "): ";
                if (rangeStart > 0)
                {
                    writer << "...";
                }
                writer << text.substr(rangeStart, rangeEnd - rangeStart);
                if (rangeEnd < (int)text.size())
                {
                    writer << "...";
                }
                log.ReportResult(CompileResult(writer.str(), blob->GetType(), blob->GetNumber(), (int)i));
            }
        }
    }
}

void CMainFrame::_FindInVocab000(ICompileLog &log, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    const Vocab000 *pVocab = appState->GetResourceMap().GetVocab000();
    if (pVocab)
    {
        int iItem = 0;
        Vocab000::groups_iterator position = pVocab->GroupsBegin();
        while (position != pVocab->GroupsEnd())
        {
            Vocab000::WordGroup dwGroup;
            WordClass dwClass;
            std::string strWords;
            pVocab->EnumGroups(position, dwGroup, dwClass, strWords);
            if (!fMatchCase)
            {
                ToUpper(strWords);
            }
            int pos = FindStringHelper(strWords.c_str(), pszWhat, fWholeWord);
            if (pos != -1)
            {
                fmt::MemoryWriter writer;
                writer << "Vocab (" << 0 << ", " << iItem << "): ";
                writer << strWords;
                log.ReportResult(CompileResult(writer.str(), ResourceType::Vocab, 0, (int)InfoFromClassAndGroup(dwClass, dwGroup)));
            }
            iItem++;
        }
    }
}

void CMainFrame::OnFindInFiles()
{
    // If there is anything currently selected, use it as the default text.
    CString strFindWhat;
    CMDITabChildWnd *pActiveFrame = static_cast<CMDITabChildWnd*>(GetActiveFrame());
    if (pActiveFrame)
    {
        CView *pView = pActiveFrame->GetActiveView();
        if (pView->IsKindOf(RUNTIME_CLASS(CScriptView)))
        {
            CScriptView *pScriptView = static_cast<CScriptView*>(pView);
            pScriptView->GetSelectedText(strFindWhat);
        }
    }
    CFindAllDialog dialog(_fMatchWholeWord, _fMatchCase, _fFindInAll, strFindWhat);
    if (IDOK == dialog.DoModal())
    {
        CompileLog log;
        // Say what we're doing:
        TCHAR szBuf[MAX_PATH];
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("Find \"%s\", in %s, %s, %s:"),
                        strFindWhat,
                        _fFindInAll ? TEXT("all project files") : TEXT("all open files"),
                        _fMatchCase ? TEXT("matching case") : TEXT("not matching case"),
                        _fMatchWholeWord ? TEXT("matching whole word") : TEXT("not matching whole word"));
        log.ReportResult(CompileResult(szBuf));

        if (_fFindInAll)
        {
            _FindInFilesOfType(log, TEXT("\\*.sc"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, TEXT("\\*.sh"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, TEXT("\\*.scp"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, TEXT("\\*.shp"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));

            CString stdFindWhat2 = strFindWhat;
            if (_fMatchCase == 0)
            {
                stdFindWhat2.MakeUpper();
            }
            _FindInTexts(log, stdFindWhat2, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInVocab000(log, stdFindWhat2, (_fMatchCase != 0), (_fMatchWholeWord != 0));
        }
        else
        {
            // Find in only in the open files
            // TODO - re-implement this.
        }

        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("Total occurrences found: %d"), (log.Results().size() - 1));
        log.ReportResult(CompileResult(szBuf));
        appState->OutputResults(log.Results());
    }
}

void CMainFrame::OnUpdateBackForward(CCmdUI *pCmdUI)
{
    pCmdUI->Enable((pCmdUI->m_nID == ID_BACK) ? m_wndTab.CanGoBack() : m_wndTab.CanGoForward());
}
void CMainFrame::OnGoForward()
{
    m_wndTab.GoForward();
}
void CMainFrame::OnGoBack()
{
    m_wndTab.GoBack();
}

void CMainFrame::OnUpdateScriptCombo(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->_fBrowseInfo);
}

ResourceType ResourceCommandToType(UINT nId)
{
    for (int i = 0; i < ARRAYSIZE(c_ShowResourceCommands); i++)
    {
        if (c_ShowResourceCommands[i].nCommandId == nId)
        {
            return c_ShowResourceCommands[i].iType;
        }
    }
    return ResourceType::None;
}

BOOL CMainFrame::OnShowResource(UINT nId)
{
    ResourceType i = ResourceCommandToType(nId);
    if (i == ResourceType::None)
    {
        return FALSE;
    }
    else
    {
        appState->ShowResourceType(i);
        return TRUE; // Handled
    }
}

//
// Prof-UIS by default puts up right-click context menus everywhere.  We need to suppress these
// by listening to one of its registered messages.
//
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == CExtControlBar::g_nMsgConstructPopupMenu)
    {
        return OnConstructPopupMenuCB(wParam, lParam);
    }
    else
    {
        return __super::WindowProc(message, wParam, lParam);
    }
}

LRESULT CMainFrame::OnConstructPopupMenuCB(WPARAM wParam, LPARAM lParam)
{
    ASSERT_VALID(this);
    CExtControlBar::POPUP_MENU_EVENT_DATA *p_pmed = CExtControlBar::POPUP_MENU_EVENT_DATA::FromWParam(wParam);
    ASSERT(p_pmed != nullptr);
    ASSERT_VALID(p_pmed->m_pPopupMenuWnd);
    ASSERT_VALID(p_pmed->m_pWndEventSrc);
    ASSERT(p_pmed->m_pWndEventSrc->GetSafeHwnd() != nullptr);
    ASSERT(::IsWindow(p_pmed->m_pWndEventSrc->GetSafeHwnd()));

    // Suppress context menus in the client areas - we have our own in several cases.
    if ((!p_pmed->m_bPostNotification) &&
         p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MDICLIAREA_CTX)
    {
        return (!0);
    }
    return 0;
}

// This is important for closing windows. What an awful hack.
void CMainFrame::_RefreshToolboxPanelOnDeactivate(CFrameWnd *pWnd)
{
    if (pWnd->IsKindOf(RUNTIME_CLASS(CMDITabChildWnd)))
    {
        CMDITabChildWnd *pActiveFrame = static_cast<CMDITabChildWnd*>(pWnd);
        CDocument *pDoc = pActiveFrame->GetActiveDocument();
        if (pDoc)
        {
            // If anyone 
            // Clear out all documents first.
            // REVIEW: This code really needs reworking.
            m_dlgForPanelDialogView.SetDocument(nullptr);
            m_dlgForPanelDialogFont.SetDocument(nullptr);
            m_dlgForPanelDialogCursor.SetDocument(nullptr);
            m_dlgForPanelDialogPic.SetDocument(nullptr);
            m_dlgForPanelDialogPicVGA.SetDocument(nullptr);
            m_dlgForPanelDialogScript.SetDocument(nullptr);
            m_wndScriptToolComboBoxClass.SetDocument(nullptr);
            m_dlgForPanelDialogGame.SetDocument(nullptr);
            m_dlgForPanelDialogSound.SetDocument(nullptr);
        }
    }
}

struct StatusIndicatorInfo
{
int IndicatorId;
int Width;
};

StatusIndicatorInfo rasterIndicators[] =
{
    { ID_INDICATOR_RESSIZE, 100 },
    { ID_INDICATOR_COORDS, 100 },
    { ID_INDICATOR_PIXELCOLOR, 190 },
    { ID_INDICATOR_RASTERSIZE, 140 },
};

StatusIndicatorInfo picIndicators[] =
{
    { ID_INDICATOR_RESSIZE, 100 },
    { ID_INDICATOR_COORDS, 100 },
    { ID_INDICATOR_PIXELCOLOR, 190 },
    { ID_INDICATOR_PRI, 100 },
    { ID_INDICATOR_CONTROLCOLOR, 100 },
    { ID_INDICATOR_PRICOLOR, 100 },
};

StatusIndicatorInfo vocabIndicators[] =
{
    { ID_INDICATOR_RESSIZE, 100 },
    { ID_INDICATOR_WORD, 200 },
};

StatusIndicatorInfo soundIndicators[] =
{
    { ID_INDICATOR_RESSIZE, 100 },
};

StatusIndicatorInfo gameIndicators[] =
{
    { ID_INDICATOR_RESCOUNT, 200 },
};

StatusIndicatorInfo scriptIndicators[] =
{
    { ID_EDIT_INDICATOR_READ, 200 },
    { ID_INDICATOR_OVR, 80 },
    { ID_EDIT_INDICATOR_COL, 80 },
    { ID_INDICATOR_LINECOUNT, 100 },
};

const int NumberOfFixedPanes = 1;

void CMainFrame::_ApplyIndicators(MDITabType tabType)
{
    size_t count = 0;
    StatusIndicatorInfo *indicators = nullptr;
    switch (tabType)
    {
    case TAB_VIEW:
    case TAB_FONT:
    case TAB_CURSOR:
        indicators = rasterIndicators;
        count = ARRAYSIZE(rasterIndicators);
        break;

    case TAB_SOUND:
        indicators = soundIndicators;
        count = ARRAYSIZE(soundIndicators);
        break;

    case TAB_PIC:
        indicators = picIndicators;
        count = ARRAYSIZE(picIndicators);
        break;

    case TAB_GAME:
        indicators = gameIndicators;
        count = ARRAYSIZE(gameIndicators);
        break;

    case TAB_VOCAB:
        indicators = vocabIndicators;
        count = ARRAYSIZE(vocabIndicators);
        break;

    case TAB_SCRIPT:
        indicators = scriptIndicators;
        count = ARRAYSIZE(scriptIndicators);
        break;

    case TAB_TEXT:
    case TAB_PALETTE:
    case TAB_ROOMEXPLORER:
    case TAB_NONE:
        count = 0;
        break;

    }

    // Adjust the number of panes in the status bar
    int currentPaneCount = m_wndStatusBar.GetPaneCount();
    if (currentPaneCount > (int)(NumberOfFixedPanes + count))
    {
        m_wndStatusBar.RemovePanes(NumberOfFixedPanes + count, (currentPaneCount - (NumberOfFixedPanes + count)));
    }

    for (int i = 0; i < (int)count; i++)
    {
        int paneIndex = i + NumberOfFixedPanes;
        if (paneIndex >= m_wndStatusBar.GetPaneCount())
        {
            // Add any needed ones.
            m_wndStatusBar.AddPane(indicators[i].IndicatorId, paneIndex);
        }
        m_wndStatusBar.SetPaneInfo(paneIndex, indicators[i].IndicatorId, SBPS_NORMAL, indicators[i].Width);
    }
}

//
// REVIEW: This code really needs reworking.  Currently we don't have a good way to
// know when the documents that the panels refer to are deleted.  So every time a frame
// is activated, we set all the documents to nullptr.  This isn't a very scalable solution though.
// We could have all documents inherit from a base class that has some kind of registration mechanism
// for "listeners" on the documents (e.g. sort of like views!).  And then the panels could be listeners.
// When the documents are destroyed, they would tell the listeners that they no longer exist.
//
void CMainFrame::_RefreshToolboxPanel(CFrameWnd *pWnd)
{
    // A couple ways we could get here:
    // if pWnd is non-null, then get the document from its active document.  The active document
    // may be still null if the frame was just created and being made active.
    // if pWnd is null, then we assume the document is the active document for the current frame.
    CMDITabChildWnd *pActiveFrame;
    if (pWnd->IsKindOf(RUNTIME_CLASS(CMDITabChildWnd)))
    {
        pActiveFrame = static_cast<CMDITabChildWnd*>(pWnd);
    }
    else
    {
        // This is a valid scenario -> when no game is loaded, the active frame will be
        // the main frame.
        pActiveFrame = nullptr;
    }

    if ((pActiveFrame != _pActiveFrame) || (_fDidntGetDocYet))
    {
        // Update the "toolbox"
        _pActiveFrame = pActiveFrame;
        CDocument *pDoc = pActiveFrame ? pActiveFrame->GetActiveDocument() : nullptr;
        // If this is a newly created frame, it won't have an active document just yet.
        // Go ahead and show the appropriate panel (and set the doc to null).  We'll come around
        // when the UI is idle and update the document.
        _fDidntGetDocYet = (pDoc == nullptr);
        CWnd *pWndToShow = nullptr;

        if (_pActiveFrame)
        {
            _ApplyIndicators(_pActiveFrame->GetTabType());

            switch (_pActiveFrame->GetTabType())
            {
            case TAB_VIEW:
                pWndToShow = &m_dlgForPanelDialogView;
                m_dlgForPanelDialogView.SetDocument(pDoc);
                break;
            case TAB_FONT:
                pWndToShow = &m_dlgForPanelDialogFont;
                m_dlgForPanelDialogFont.SetDocument(pDoc);
                break;
            case TAB_CURSOR:
                pWndToShow = &m_dlgForPanelDialogCursor;
                m_dlgForPanelDialogCursor.SetDocument(pDoc);
                break;
            case TAB_PIC:
                if (appState->GetVersion().PicFormat == PicFormat::EGA)
                {
                    pWndToShow = &m_dlgForPanelDialogPic;
                    m_dlgForPanelDialogPic.SetDocument(pDoc);
                }
                else
                {
                    pWndToShow = &m_dlgForPanelDialogPicVGA;
                    m_dlgForPanelDialogPicVGA.SetDocument(pDoc);
                }
                break;
            case TAB_SCRIPT:
                pWndToShow = &m_dlgForPanelDialogScript;
                m_dlgForPanelDialogScript.Initialize();
                m_dlgForPanelDialogScript.SetDocument(pDoc);
                m_wndScriptToolComboBoxClass.SetDocument(pDoc);
                //m_wndScriptToolComboBoxFunction.SetDocument(pDoc);
                break;
            case TAB_GAME:
                pWndToShow = &m_dlgForPanelDialogGame;
                m_dlgForPanelDialogGame.SetDocument(pDoc);
                break;
            case TAB_SOUND:
                pWndToShow = &m_dlgForPanelDialogSound;
                m_dlgForPanelDialogSound.SetDocument(pDoc);
                break;
            }
            m_dlgEmpty.SelectChild(pWndToShow, _pActiveFrame->GetTabType());
        }
        else
        {
            m_dlgEmpty.SelectChild(nullptr, TAB_NONE);
        }

        // Update the resource-specific toolbar
        MDITabType iTabType = _pActiveFrame ? _pActiveFrame->GetTabType() : TAB_NONE;
        _HideTabIfNot(iTabType, TAB_GAME, m_wndExplorerTools);
        _HideTabIfNot(iTabType, (MDITabType)(TAB_VIEW | TAB_FONT | TAB_CURSOR), m_wndViewTools);
        _HideTabIfNot(iTabType, TAB_PIC, m_wndPicTools);
        _HideTabIfNot(iTabType, TAB_SCRIPT, m_wndScriptTools);
        _HideTabIfNot(iTabType, TAB_VOCAB, m_wndVocabTools);
        _HideTabIfNot(iTabType, TAB_TEXT, m_wndTextTools);
        _HideTabIfNot(iTabType, TAB_SOUND, m_wndSoundTools);
        switch (iTabType)
        {
        case TAB_GAME:
            ShowControlBar(&m_wndExplorerTools, TRUE, TRUE);
            break;
        case TAB_VIEW:
        case TAB_FONT:
        case TAB_CURSOR:
            ShowControlBar(&m_wndViewTools, TRUE, TRUE);
            break;
        case TAB_PIC:
            ShowControlBar(&m_wndPicTools, TRUE, TRUE);
            break;
        case TAB_SCRIPT:
            ShowControlBar(&m_wndScriptTools, TRUE, TRUE);
            break;
        case TAB_VOCAB:
            ShowControlBar(&m_wndVocabTools, TRUE, TRUE);
            break;
        case TAB_TEXT:
            ShowControlBar(&m_wndTextTools, TRUE, TRUE);
            break;
        case TAB_SOUND:
            ShowControlBar(&m_wndSoundTools, TRUE, TRUE);
            break;
        default:
            break;
        }
    }
}

void CMainFrame::OnActivateTab(CFrameWnd *pWnd)
{
    ASSERT(GetActiveFrame() == pWnd);
    Tabs().OnActivateTab(pWnd);

    _RefreshToolboxPanel(pWnd);
}

void CMainFrame::OnDeactivateTab(CFrameWnd *pWnd)
{
    _RefreshToolboxPanelOnDeactivate(pWnd);
}

void CMainFrame::OnClassBrowser()
{
    CClassBrowserDialog dialog(appState->GetResourceMap().GetClassBrowser());
    dialog.DoModal();
}

void CMainFrame::OnManageDecompilation()
{
    unique_ptr<DecompileDialog> decompileDialog = make_unique<DecompileDialog>();
    decompileDialog->DoModal();
}

void CMainFrame::OnUpdateClassBrowser(CCmdUI *pCmdUI)
{
	SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
    ClassBrowserLock lock(browser);
    if (lock.TryLock())
    {
        // Enable this if the classbrowser is done.
        pCmdUI->Enable();
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}


void CMainFrame::OnSelfTest()
{
    if (!_fSelfTest)
    {
        SetTimer(TIMER_SELFTEST, 30000, nullptr);
        _fSelfTest = true;
        OnTimer(TIMER_SELFTEST); // Start it off right away
    }
    else
    {
        KillTimer(TIMER_SELFTEST);
        _fSelfTest = false;
    }
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_SELFTEST)
    {
        // Continue the self-test.
        int n = rand() % 20;
        if (n < 5)
        {
            OnRebuildResources();
        }
        else if (n < 10)
        {
            OnCompileAll();
        }
        else if (n < 15)
        {
            // Open a random resource
        }
        else if (n < 20)
        {
            // Close one of the open resources.
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CBrowseInfoStatusPane message handlers

#define UWM_STATUSREADY      (WM_APP + 0)

BEGIN_MESSAGE_MAP(CBrowseInfoStatusPane, CExtLabel)
    ON_WM_LBUTTONDBLCLK()
    ON_MESSAGE(UWM_STATUSREADY, _OnStatusReady)
END_MESSAGE_MAP()

LRESULT CBrowseInfoStatusPane::_OnStatusReady(WPARAM wParam, LPARAM lParam)
{
    CGuard guard(&_csTextPosting);
    SetWindowText(_textToPost.c_str());
    if (_status == Errors)
    {
        // TODO: set background to yellow
        SetBkColor(RGB(255, 255, 0));
    }
    else
    {
        SetBkColor((COLORREF)-1);
    }
    return TRUE;
}

void CBrowseInfoStatusPane::OnDrawLabelText(CDC &dc, const RECT &rcText, __EXT_MFC_SAFE_LPCTSTR strText, DWORD dwDrawTextFlags, bool bEnabled)
{
    __super::OnDrawLabelText(dc, rcText, strText, dwDrawTextFlags, bEnabled);
}

void CBrowseInfoStatusPane::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (_status == Errors)
    {
		SCIClassBrowser &browser = *appState->GetResourceMap().GetClassBrowser();
        appState->OutputResults(browser.GetErrors());
    }
    __super::OnLButtonDblClk(nFlags, point);
}

// This method may  be called on a different thread, so care should be taken
// as to what gets done here... generally just Invalidate()
void CBrowseInfoStatusPane::NotifyClassBrowserStatus(BrowseInfoStatus status, int iPercent)
{
    CGuard guard(&_csTextPosting);
    if ((_status != status) || (status == InProgress))
    {
        _status = status;
        if (status == Ok)
        {
            _textToPost = "Browse info complete";
        }
        else if (status == Errors)
        {
            _textToPost = "Double-click here to see errors";
        }
        else if (status == InProgress)
        {
            std::stringstream ss;
            ss << "Browse info " << iPercent << "% complete";
            _textToPost = ss.str();
        }
    }
    PostMessage(UWM_STATUSREADY, 0, 0);
}

CBrowseInfoStatusPane::CBrowseInfoStatusPane()
{
    InitializeCriticalSection(&_csTextPosting);
}

CBrowseInfoStatusPane::~CBrowseInfoStatusPane()
{
    DeleteCriticalSection(&_csTextPosting);
}

