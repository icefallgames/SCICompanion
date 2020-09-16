/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/

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
#include "AudioPreferencesDialog.h"
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
#include "Message.h"
#include "MessageDoc.h"
#include "Sound.h"
#include "PaletteDoc.h"
#include "PaletteOperations.h"
#include "NewRasterResourceDocument.h"
#include "AppState.h"
#include "CObjectWrap.h"
#include "format.h"
#include "ExtractAllDialog.h"
#include "DecompileDialog.h"
#include "ResourceContainer.h"
#include "AudioMap.h"
#include <regex>
#include "ResourceBlob.h"
#include "GenerateDocsDialog.h"
#include "DependencyTracker.h"
#include "MessageSource.h"
#include "ValidateSaid.h"
#include "ValidateSelectors.h"
#include "OutputScriptStrings.h"
#include "CompiledScript.h"
#include <filesystem>
#include <regex>
#include "NearestColors.h"

using namespace std::tr2::sys;

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
    { "&Message", TAB_MESSAGE },
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
    ON_COMMAND(ID_NEW_MESSAGE, OnFileNewMessage)
    ON_COMMAND(ID_NEW_PALETTE, OnFileNewPalette)
    ON_COMMAND(ID_FILE_OPENRESOURCE, OnFileOpenResource)
    ON_COMMAND(ID_FILE_ADDRESOURCE, OnFileAddResource)
    ON_COMMAND(ID_EDIT_FINDINFILES, OnFindInFiles)
    ON_COMMAND(ID_PREFERENCES, OnShowPreferences)
    ON_COMMAND(ID_TOOLS_AUDIOPREFERENCES, OnShowAudioPreferences)
    ON_COMMAND(ID_TOOLS_EXTRACTALLRESOURCES, OnExtractAllResources)
    ON_COMMAND(ID_TOOLS_REBUILDRESOURCES, OnRebuildResources)
    ON_COMMAND(ID_TOOLS_REPACKAGEAUDIO, OnRepackageAudio)
    ON_COMMAND(ID_TOOLS_REBUILDCLASSTABLE, OnRebuildClassTable)
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
    ON_UPDATE_COMMAND_UI(ID_PREFERENCES, OnUpdateAlwaysEnabled)
    ON_UPDATE_COMMAND_UI(ID_NEW_MESSAGE, OnUpdateNewMessage)
    ON_UPDATE_COMMAND_UI(ID_NEW_PALETTE, OnUpdateNewPalette)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_EXTRACTALLRESOURCES, OnUpdateShowIfGameLoaded)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_REBUILDRESOURCES, OnUpdateShowIfGameLoaded)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_REPACKAGEAUDIO, OnUpdateShowIfSupportsAudio)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_REBUILDCLASSTABLE, OnUpdateShowIfGameLoaded)
    ON_COMMAND(ID_BACK, OnGoBack)
    ON_COMMAND(ID_FORWARD, OnGoForward)
    ON_COMMAND(ID_CLASSBROWSER, OnClassBrowser)
    ON_COMMAND(ID_SCRIPT_MANAGEDECOMPILATION, OnManageDecompilation)
    ON_COMMAND(ID_MAIN_SHOWNEARESTCOLORS, OnShowNearestColors)
    ON_UPDATE_COMMAND_UI(ID_CLASSBROWSER, OnUpdateClassBrowser)
    ON_UPDATE_COMMAND_UI(ID_BACK, OnUpdateBackForward)
    ON_UPDATE_COMMAND_UI(ID_FORWARD, OnUpdateBackForward)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_CLASSCOMBO, OnUpdateScriptCombo)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_FUNCTIONCOMBO, OnUpdateScriptCombo)
    ON_UPDATE_COMMAND_UI(ID_CLASSBROWSER, OnUpdateShowIfGameLoaded)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_MANAGEDECOMPILATION, OnUpdateShowIfGameLoaded)
    ON_COMMAND(ID_STOPDEBUG, OnStopDebugging)
    ON_UPDATE_COMMAND_UI(ID_STOPDEBUG, OnUpdateStopDebugging)

    ON_COMMAND(ID_SCRIPT_VALIDATEALLSAIDS, OnValidateAllSaids)
    ON_COMMAND(ID_SCRIPT_VALIDATESELECTORS, OnValidateSelectors)
    
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_VALIDATEALLSAIDS, OnUpdateValidateAllSaids)

    ON_COMMAND(ID_SCRIPT_EXTRACTALLSCRIPTSTRINGS, ExtractAllScriptStrings)

    // ON_COMMAND(ID_SELFTEST, OnSelfTest)
    // ON_WM_TIMER()

    ON_WM_WINDOWPOSCHANGED()

    // Bar states
    // ID_BAR_GAME and ID_BAR_TOOLBOX can never be turned off, so no need
    //ON_COMMAND_EX(ID_BAR_GAME, OnBarCheck)
    //ON_UPDATE_COMMAND_UI(ID_BAR_GAME, OnUpdateControlBarMenu)
    //ON_COMMAND_EX(ID_BAR_TOOLBOX, OnBarCheck)
    //ON_UPDATE_COMMAND_UI(ID_BAR_TOOLBOX, OnUpdateControlBarMenu)
    ON_COMMAND_EX(ID_BAR_OUTPUT, OnBarCheck)
    ON_UPDATE_COMMAND_UI(ID_BAR_OUTPUT, OnUpdateControlBarMenu)
    ON_MESSAGE(UWM_RESULTS, OnOutputPaneResults)
    ON_REGISTERED_MESSAGE(CExtPopupMenuWnd::g_nMsgPrepareMenu, OnExtMenuPrepare)

    ON_COMMAND_RANGE(ID_PLUGINS_PLUGIN1, ID_PLUGINS_PLUGIN10, OnRunPlugin)
#ifdef DOCSUPPORT
    ON_COMMAND(ID_COMPILEDOCS, OnGenerateDocs)
#endif
END_MESSAGE_MAP()

LRESULT CMainFrame::OnOutputPaneResults(WPARAM wParam, LPARAM lParam)
{
    std::vector<CompileResult> *results = reinterpret_cast<std::vector<CompileResult>*>(lParam);
    GetOutputPane().AddBatch((OutputPaneType)wParam, *results);
    //GetOutputPane().FinishAdd((OutputPaneType)wParam);
    delete results;
    return 0;
}

CMainFrame::CMainFrame() : m_dlgForPanelDialogPic(false, false, false), m_dlgForPanelDialogPicVGA(true, true, false), m_dlgForPanelDialogPicEGAPoly(false, true, true), _fFindInAll(1), _fMatchWholeWord(0), _fMatchCase(0)
{
    _pActiveFrame = nullptr;
    _fDidntGetDocYet = false;
    _fSelfTest = false;

    // Prof-UIS GUI persistence
    ::memset( &m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT) );
    m_dataFrameWP.length = sizeof(WINDOWPLACEMENT);
    m_dataFrameWP.showCmd = SW_HIDE;
    g_ResourceManager->SetLangLayout( LAYOUT_LTR );

    /*
    for (double lum = -0.5; lum < 0.5; lum += 0.25)
    {
        OutputDebugString("byte[] stepDown = {\n");
        // REMOVE THIS
        for (int i = 0; i < 256; i++)
        {
            RGBQUAD orig = g_egaColorsMixed[i];
            COLORREF crOld = RGB(orig.rgbRed, orig.rgbGreen, orig.rgbBlue);
            COLORREF crNew = CExtBitmap::stat_HLS_Adjust(crOld, 0.0, lum, 0.0);
            // Find new match:
            EGACOLOR closestEGA = GetClosestEGAColor(1, true, 1, crNew);

            // TODO potentially swap colors if one is the same as prev
            OutputDebugString(fmt::format("    0x{0:02x},\n", (uint16_t)closestEGA.ToByte()).c_str());
        }
        OutputDebugString("};\n");
    }
    */
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

HHOOK s_BrowseInfoStatusMH = nullptr;
HWND hwndStatusPane = nullptr;
CBrowseInfoStatusPane *hwndBrowseInfoStatus = nullptr;
LRESULT CALLBACK s_BrowseInfoStatusMouseHook(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    )
{
    bool callNext = true;
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_LBUTTONDBLCLK)
        {
            MOUSEHOOKSTRUCT *msh = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam);
            if (hwndBrowseInfoStatus && (msh->hwnd == hwndStatusPane))
            {
                CRect rect;
                GetWindowRect(hwndBrowseInfoStatus->GetSafeHwnd(), &rect);
                if (rect.PtInRect(msh->pt))
                {
                    hwndBrowseInfoStatus->OnLButtonDblClk();
                    callNext = false;
                }
            }
        }
    }

    if (callNext)
    {
        return CallNextHookEx(s_BrowseInfoStatusMH, nCode, wParam, lParam);
    }
    else
    {
        return 1;
    }
}

BOOL CMainFrame::DestroyWindow()
{
    // Unhook
    if (s_BrowseInfoStatusMH)
    {
        UnhookWindowsHookEx(s_BrowseInfoStatusMH);
        s_BrowseInfoStatusMH = nullptr;
        hwndStatusPane = nullptr;
        hwndBrowseInfoStatus = nullptr;
    }

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
    { ID_SHOW_MESSAGES, ResourceType::Message, "Messages", "Show messages", "Show message resources", IDI_MESSAGE },
    { ID_SHOW_HEAPS, ResourceType::Heap, "Heaps", "Show heaps", "Show heap resources", IDI_SCRIPT },
    { ID_SHOW_AUDIO, ResourceType::Audio, "Audio", "Show audio", "Show audio resources", IDI_AUDIO },
    { ID_SHOW_MAPS, ResourceType::AudioMap, "Maps", "Show maps", "Show audio maps", IDI_AUDIOMAP },
};

std::string ResourceDisplayNameFromType(ResourceType type)
{
    for (const auto &typeInfo : c_ShowResourceCommands)
    {
        if (typeInfo.iType == type)
        {
            return typeInfo.pszShortText;
        }
    }
    return "";
}

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

    // The fake ego commands
    for (UINT id = ID_FAKEEGO0; id <= ID_FAKEEGO12; id++)
    {
        g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, id);
    }
}

void CMainFrame::OnDestroy()
{
    g_CmdManager->ProfileWndRemove(GetSafeHwnd());
    __super::OnDestroy();
}

void CMainFrame::RefreshExplorerTools()
{
    // Reload the toolbar from scratch, since we need to refresh all the buttons.
    m_wndExplorerTools.LoadToolBar(IDR_TOOLBARVIEWS);

    if (!appState->GetResourceMap().GetGameFolder().empty())
    {
        SCIVersion version = appState->GetVersion();
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_VIEWS);
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_PICS);
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_SCRIPTS);
        if (version.SupportsMessages)
        {
            m_wndExplorerTools.InsertButton(-1, ID_SHOW_MESSAGES);
        }
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_TEXTS);
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_SOUNDS);
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_VOCABS);
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_FONTS);
        if (!version.SeparateHeapResources)
        {
            m_wndExplorerTools.InsertButton(-1, ID_SHOW_CURSORS);
        }
        m_wndExplorerTools.InsertButton(-1, ID_SHOW_PATCHES);
        if (version.HasPalette)
        {
            m_wndExplorerTools.InsertButton(-1, ID_SHOW_PALETTES);
        }
        if (version.AudioVolumeName != AudioVolumeName::None)
        {
            m_wndExplorerTools.InsertButton(-1, ID_SHOW_AUDIO);
        }
        if (version.SeparateHeapResources)
        {
            m_wndExplorerTools.InsertButton(-1, ID_SHOW_HEAPS);
        }
        if (version.AudioVolumeName != AudioVolumeName::None)
        {
            m_wndExplorerTools.InsertButton(-1, ID_SHOW_MAPS);
        }

        m_wndExplorerTools.InitContentExpandButton(); // Overflow...
    }
}


void CMainFrame::OnRunPlugin(UINT nID)
{
    UINT index = nID - ID_PLUGINS_PLUGIN1;
    if (index < _pluginExes.size())
    {
        std::string exePath = _pluginExes[index];
        std::string gameFolderQuoted = fmt::format("\"{}\"", appState->GetResourceMap().GetGameFolder());
        ShellExecute(nullptr, "open", exePath.c_str(), gameFolderQuoted.c_str(), "", SW_SHOWNORMAL);
    }
}

// From the MSDN sample.
// Win32 API is such a tragedy. All this to get a file description.
std::string GetFileDescription(uint8_t *pBlock)
{
    std::string result;

    // Structure used to store enumerated languages and code pages.

    HRESULT hr;

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    // Read the list of languages and code pages.

    UINT cbTranslate;
    VerQueryValue(pBlock,
        TEXT("\\VarFileInfo\\Translation"),
        (LPVOID*)&lpTranslate,
        &cbTranslate);

    // Read the file description for each language and code page.
    char SubBlock[200];
    for (UINT i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
    {
        hr = StringCchPrintf(SubBlock, 50,
            TEXT("\\StringFileInfo\\%04x%04x\\FileDescription"),
            lpTranslate[i].wLanguage,
            lpTranslate[i].wCodePage);
        if (FAILED(hr))
        {
            // TODO: write error handler.
        }

        DWORD_PTR pointerToString;
        UINT stringSizeInBytes;
        // Retrieve file description for language and code page "i". 
        if (VerQueryValue(pBlock,
            SubBlock,
            (LPVOID*)&pointerToString,
            &stringSizeInBytes
            ))
        {
            result = std::string((PCSTR)pointerToString, stringSizeInBytes);
            // First language is good enough, I can't be bothered...
            break;
        }
    }

    return result;
}

void CMainFrame::_EnumeratePlugins(CExtPopupMenuWnd &menu)
{
    // Clear out the plugin list and remove any old menu items
    _pluginExes.clear();
    for (UINT id = ID_PLUGINS_PLUGIN1; id <= ID_PLUGINS_PLUGIN10; id++)
    {
        INT itemPos = menu.ItemFindPosForCmdID(id);
        if (itemPos != -1)
        {
            menu.ItemRemove(itemPos);
        }
    }

    std::vector<path> pluginDirectories;
    // First get the plugin folders.
    path pluginFolder = GetExeSubFolder("Plugins");
    for (auto it = directory_iterator(pluginFolder); it != directory_iterator(); ++it)
    {
        const auto &folder = it->path();
        if (is_directory(folder))
        {
            pluginDirectories.push_back(folder);
        }
    }

    UINT maxItems = ID_PLUGINS_PLUGIN10 - ID_PLUGINS_PLUGIN1 + 1;

    // Then look for exes in each of those. This isn't recursive, we only look at top level folder
    auto matchEXERegex = std::regex("(\\w+)\\.exe", std::regex_constants::icase);
    for (path pluginSubDirectory : pluginDirectories)
    {
        for (auto it = directory_iterator(pluginSubDirectory); it != directory_iterator(); ++it)
        {
            const auto &file = it->path();
            std::smatch sm;
            std::string temp = file.filename().string();
            if (!is_directory(file) && std::regex_search(temp, sm, matchEXERegex) && (sm.size() > 1))
            {
                _pluginExes.push_back(file.string());
            }
            if (_pluginExes.size() >= maxItems)
            {
                break;
            }
        }
        if (_pluginExes.size() >= maxItems)
        {
            break;
        }
    }

    // Done. Now make menu items for each of those.
    UINT idToAdd = ID_PLUGINS_PLUGIN1;
    int index = 0;
    for (path exePath : _pluginExes)
    {
        // Default to the exe name
        std::string description = exePath.stem().string();
        // But try to get a better one:
        DWORD size = GetFileVersionInfoSize(exePath.string().c_str(), nullptr);
        if (size)
        {
            std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
            if (GetFileVersionInfo(exePath.string().c_str(), 0, size, data.get()))
            {
                description = GetFileDescription(data.get());
            }
        }

        CExtCmdItem *pCmdItem = g_CmdManager->CmdGetPtr(appState->_pszCommandProfile, idToAdd);
        // Allocate this command if not used yet.
if (pCmdItem == nullptr)
{
    pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, idToAdd);
}
pCmdItem->m_sMenuText = description.c_str();
menu.ItemInsert(idToAdd, index);
index++;
idToAdd++;
    }
}

// Prepare fake ego menu, or the plugins menu
LRESULT CMainFrame::OnExtMenuPrepare(WPARAM wParam, LPARAM)
{
    CExtPopupMenuWnd::MsgPrepareMenuData_t *pData =
        reinterpret_cast<CExtPopupMenuWnd::MsgPrepareMenuData_t*>(wParam);
    CExtPopupMenuWnd * pPopup = pData->m_pPopup;
    INT nItemPos = pPopup->ItemFindPosForCmdID(ID_FAKEEGONUMBER);
    if (nItemPos >= 0)
    {
        int index = 0;
        for (int viewNumber : appState->GetRecentViews())
        {
            UINT commandId = ID_FAKEEGO0 + index;
            std::string text = fmt::format("{0}", viewNumber);
            // Get a bitmap for it and assign it to the command.
            try
            {
                std::unique_ptr<ResourceBlob> blob = appState->GetResourceMap().MostRecentResource(ResourceType::View, viewNumber, true);
                if (blob)
                {
                    text = blob->GetName();

                    auto resource = CreateResourceFromResourceData(*blob);

                    // Incorporate the global palette if necessary
                    auto &raster = resource->GetComponent<RasterComponent>();
                    PaletteComponent *palette = nullptr;
                    std::unique_ptr<PaletteComponent> temp;
                    if (raster.Traits.PaletteType == PaletteType::VGA_256)
                    {
                        temp = appState->GetResourceMap().GetMergedPalette(*resource, 999);
                        palette = temp.get();
                    }

                    // Note: if the index is out of bounds, it will return a NULL HBITMAP
                    CBitmap bitmap;
                    bitmap.Attach(GetBitmap(raster, palette, CelIndex(0, 0), 32, 32, BitmapScaleOptions::AllowMag | BitmapScaleOptions::AllowMin));

                    if ((HBITMAP)bitmap)
                    {
                        CExtBitmap extBitmap;
                        extBitmap.FromBitmap((HBITMAP)bitmap);
                        g_CmdManager->CmdSetIcon(appState->_pszCommandProfile, commandId, extBitmap, RGB(255, 255, 255), CRect(0, 0, 32, 32));
                    }
                }
            }
            catch (std::exception) {}

            CExtCmdItem *pCmdItem = g_CmdManager->CmdGetPtr(appState->_pszCommandProfile, commandId);
            assert(pCmdItem);
            pCmdItem->m_sMenuText = text.c_str();
            pPopup->ItemInsert(commandId, index);
            index++;
        }
    }

    nItemPos = pPopup->ItemFindPosForCmdID(ID_PLUGINS_OPENPLUGINSFOLDER);
    if (nItemPos >= 0)
    {
        // Found the plugins menu
        _EnumeratePlugins(*pPopup);
    }

    return 0;
}


void CMainFrame::OnWindowPosChanged(WINDOWPOS *wp)
{
    if ((wp->flags & SWP_NOMOVE) == 0)
    {
        if (wp->hwnd == this->GetSafeHwnd())
        {
            appState->HideTipWindows();
        }
    }
    __super::OnWindowPosChanged(wp);
}

struct ColorAndDistance
{
    int a;
    int b;
    int distance;

    bool operator<(const ColorAndDistance &cad) const
    {
        return cad.distance > distance;
    }
};

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    std::vector<ColorAndDistance> boop;

    // phil quick test
    for (int a = 0; a < 256; a++)
    {
        for (int b = a; b < 256; b++)
        {
            if (_ColorRefFromRGBQuad(g_egaColorsExtended[a]) != _ColorRefFromRGBQuad(g_egaColorsExtended[b]))
            {
                int distance = GetColorDistanceRGB(g_egaColorsExtended[a], g_egaColorsExtended[b]);
                boop.push_back({a, b, distance});
            }
        }
    }
    std::sort(boop.begin(), boop.end());

    for (int i = 0; i < 10; i++)
    {
        OutputDebugString(fmt::format("color: {0:4x} and {1:4x}: dist: {2}\n", boop[i].a, boop[i].b, boop[i].distance).c_str());
    }


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

    // Hook for browse info status double click.
    s_BrowseInfoStatusMH = SetWindowsHookEx(WH_MOUSE, s_BrowseInfoStatusMouseHook, nullptr, GetCurrentThreadId());
    hwndStatusPane = m_wndStatusBar.GetSafeHwnd();
    hwndBrowseInfoStatus = &m_BrowseInfoStatus;

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
    SCIClassBrowser &browser = appState->GetClassBrowser();
    {
        ClassBrowserLock lock(browser);
        lock.Lock();
        browser.SetClassBrowserEvents(&m_BrowseInfoStatus);
    }

    // The tab control at the top
    m_wndTab.Create(WS_VISIBLE | WS_CHILD | TCS_OWNERDRAWFIXED, CRect(0, 0, 0, 0), this, AFX_IDW_DIALOGBAR);

    // Seems like we need to set this desired size prior to creating the control.
    //m_wndResizableBarGeneral.SetInitDesiredSizeVertical(CSize(180, 100));
    m_wndResizableBarGeneral.SetInitDesiredSizeVertical(CSize(200, 100));
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
    if (!m_dlgForPanelDialogViewEGAFixed.Create(IDD_DIALOGCELVIEW3, &m_dlgEmpty))
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
    if (!m_dlgForPanelDialogPicVGA.Create(IDD_PICCOMMANDS_VGA1, &m_dlgEmpty))
    {
        TRACE0("Failed to create m_wndResizableBar_Pic\n");
        return -1;		// fail to create
    }
    if (!m_dlgForPanelDialogPicEGAPoly.Create(IDD_PICCOMMANDS_EGAPOLY, &m_dlgEmpty))
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
    if (!m_dlgForPanelDialogMessage.Create(IDD_MESSAGETOOLBOX, &m_dlgEmpty))
    {
        TRACE0("Failed to create m_dlgForPanelDialogMessage\n");
        return -1;		// fail to create
    }

    

    // The output area at the bottom (which only appears when necessary)
    if (!m_wndResizableBarOutput.Create("Output", this, ID_BAR_OUTPUT))
    {
        TRACE0("Failed to create m_wndResizableBarOutput\n");
        return -1;		// fail to create
    }

    m_wndNewOutput.Create(NewOutputPane::IDD, &m_wndResizableBarOutput);

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

    RefreshExplorerTools();

    // Pics
    if( !m_wndPicTools.Create("pic", this, ID_BAR_TOOLPIC) ||
        !m_wndPicTools.LoadToolBar(IDR_TOOLBARPIC))
    {
        TRACE0( "Failed to create toolbar" );
        return -1;
    }

    // phil testing here
    int buttonIndex = m_wndPicTools.CommandToIndex(ID_TOGGLEEGO);
    CExtBarButton *pEgo = m_wndPicTools.GetButton(buttonIndex);
    CMenu menuTemp;
    menuTemp.LoadMenu(IDR_MENUEGOS);
    m_wndPicTools.SetButtonMenu(buttonIndex, menuTemp.Detach());
    pEgo->SetSeparatedDropDown(true);


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
#if !defined(DOCSUPPORT) || !defined(DEBUG)
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
            WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS | CBS_OWNERDRAWFIXED | WS_VSCROLL,
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

    if (!m_wndMessageTools.Create("text", this, ID_BAR_TOOLMESSAGE) ||
        !m_wndMessageTools.LoadToolBar(IDR_TOOLBARMESSAGE))
    {
        TRACE0("Failed to create toolbar");
        return -1;
    }
    _PrepareMessageCommands();
    ShowControlBar(&m_wndMessageTools, FALSE, FALSE);

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
        { ID_IMPORT_IMAGE, IDI_IMAGE },
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
        { ID_UNDITHERPIC, IDI_UNDITHERPIC },
    };
    _AssignIcons(c_rasterIcons, ARRAYSIZE(c_rasterIcons));
}

void CMainFrame::_PreparePicCommands()
{
    static const key_value_pair<UINT, int> c_picIcons[] =
    {
        { ID_HISTORY, IDI_HISTORY },
        { ID_SHOWTRACEIMAGE, IDI_SHOWTRACEIMAGE },
        { ID_UNDITHERPIC, IDI_UNDITHERPIC },
        { ID_TRACEBITMAP, IDI_LOADTRACEIMAGE },
        { ID_DEFINEPALETTES, IDI_PALETTE },
        { ID_CIRCLE, IDI_ELLIPSE },
        { ID_PICCLIPS, IDI_STAMP },
        { ID_TOGGLEEGO, IDI_FAKEEGO },
        { ID_POLYPATH, IDI_POLYPATH },
        { ID_NAMEDPOSITIONS, IDI_PLACEMENT },
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
        { ID_OPEN_EXPLORERWINDOW, IDI_OPENGAMEFOLDER },
        { ID_COMPILE, IDI_COMPILE },
        { ID_INSERTOBJECT, IDI_SCRIPT_INSERT_OBJECT },
        //{ ID_CONVERTSCRIPT, IDI_SCRIPT_CONVERT },
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

void CMainFrame::_PrepareMessageCommands()
{
    static const key_value_pair<UINT, int> c_textIcons[] =
    {
        { IDC_BUTTONADDNEW, IDI_NEWMESSAGEENTRY },
        { IDC_BUTTONCLONE, IDI_CLONEMESSAGEENTRY },
        { IDC_BUTTONADDSEQ, IDI_NEWMESSAGESEQ },
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
        { ID_IMPORT_AUDIO, IDI_AUDIO },
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
        { ID_TOOLS_REPACKAGEAUDIO, IDI_CD },
        { ID_RUNGAME, IDI_RUNGAME },
        { ID_DEBUGGAME, IDI_DEBUG },
        { ID_STOPDEBUG, IDI_STOPDEBUG },
        { ID_DEBUGROOM, IDI_DEBUGROOM },
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
        { ID_NEW_MESSAGE, IDI_NEWMESSAGE },
        { ID_NEW_PALETTE, IDI_NEWPALETTE },
        { ID_CONVERTSCRIPT, IDI_CONVERT }
    };
    _AssignIcons(c_mainIcons, ARRAYSIZE(c_mainIcons));
}

void CMainFrame::ShowOutputPane(OutputPaneType type)
{
    if (!m_wndResizableBarOutput.IsVisible())
    {
        ShowControlBar(&m_wndResizableBarOutput, TRUE, FALSE);
    }
    m_wndNewOutput.ShowPage(type);
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

const char c_szDefaultPaletteSample[] = "\\palettes\\Default.bin";

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
            if (appState->GetVersion().PicFormat >= PicFormat::VGA1_1)
            {
                // Let's add a palette. 
                const PaletteComponent *globalPalette = appState->GetResourceMap().GetPalette999();
                if (globalPalette)
                {
                    pEditPic->AddComponent<PaletteComponent>(make_unique<PaletteComponent>(*globalPalette));
                }
                else
                {
                    // No global palette, use the default one
                    std::string palettePath = appState->GetResourceMap().GetSamplesFolder() + c_szDefaultPaletteSample;
                    ResourceBlob blob;
                    if (SUCCEEDED(blob.CreateFromFile(nullptr, palettePath.c_str(), appState->GetVersion(), appState->GetResourceMap().GetDefaultResourceSaveLocation(), - 1, -1)))
                    {
                        unique_ptr<ResourceEntity> paletteEntity = CreateResourceFromResourceData(blob);
                        if (paletteEntity)
                        {
                            unique_ptr<PaletteComponent> palette(static_cast<PaletteComponent*>((paletteEntity->GetComponent<PaletteComponent>().Clone())));
                            pEditPic->AddComponent<PaletteComponent>(move(palette));
                        }
                    }
                }
            }

            pDocument->SetEditPic(appState->GetDependencyTracker(), move(pEditPic));
            pDocument->SetModifiedFlag(TRUE);
        }
    }
}

void CMainFrame::OnFileNewGame()
{
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
            if (SUCCEEDED(hr) && pEVR) \
                                    { \
                pDocument->__resourceSetter(std::move(pEVR)); \
                pDocument->SetModifiedFlag(TRUE); \
                        } \
                        else \
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

void CMainFrame::OnFileNewPalette()
{
    // A little bit different. We need interesting data to populate the default palette, so use a sample.
    std::string palettePath = appState->GetResourceMap().GetSamplesFolder() + c_szDefaultPaletteSample;
    ResourceBlob blob;
    if (SUCCEEDED(blob.CreateFromFile(nullptr, palettePath.c_str(), appState->GetVersion(), appState->GetResourceMap().GetDefaultResourceSaveLocation(), - 1, -1)))
    {
        OpenResource(&blob, true);
    }
}

void CMainFrame::OnFileNewMessage()
{
    // Grok the current messages to figure out a message version.
    auto messageContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Message, ResourceEnumFlags::AddInDefaultEnumFlags);
    uint16_t maxMessageVersion = 0;
    vector<int> existingResources;
    for (auto &messageBlob : *messageContainer)
    {
        existingResources.push_back(messageBlob->GetNumber());
        sci::istream byteStream = messageBlob->GetReadStream();
        uint16_t msgVersion = CheckMessageVersion(byteStream);
        maxMessageVersion = max(maxMessageVersion, msgVersion);
    }

    // Ask the user for resource number. We need this so we can support adding nouns and such.
    SaveResourceDialog saveResourceDialog(false, ResourceType::Message);
    int suggestedNumber = appState->GetResourceMap().SuggestResourceNumber(ResourceType::Message);
    saveResourceDialog.Init(appState->GetVersion().DefaultVolumeFile, suggestedNumber);
    if (IDOK == saveResourceDialog.DoModal())
    {
        bool goAhead = true;
        if (find(existingResources.begin(), existingResources.end(), saveResourceDialog.GetResourceNumber()) != existingResources.end())
        {
            goAhead = (IDYES == AfxMessageBox("A message resource already exists for this number. Continue anyway?", MB_OK | MB_YESNO));
        }

        if (goAhead)
        {
            std::unique_ptr<ResourceEntity> resource(CreateNewMessageResource(appState->GetVersion(), maxMessageVersion));
            if (resource)
            {
                resource->ResourceNumber = saveResourceDialog.GetResourceNumber();
                resource->PackageNumber = saveResourceDialog.GetPackageNumber();
                CDocTemplate *pDocTemplate = appState->GetMessageTemplate();
                if (pDocTemplate)
                {
                    CMessageDoc *pDocument = (CMessageDoc*)pDocTemplate->OpenDocumentFile(nullptr, TRUE);
                    if (pDocument)
                    {
                        pDocument->SetMessageResource(std::move(resource));
                        appState->_resourceRecency.AddResourceToRecency(pDocument);
                    }
                }
            }
        }
    }
}

void CMainFrame::OnFileNewSound()
{
    DeviceType device = DeviceType::NewGM;
    if (appState->GetVersion().SoundFormat == SoundFormat::SCI1)
    {
        device = DeviceType::SCI1_GM;
    }
    
    std::unique_ptr<ResourceEntity> pSound(CreateSoundResource(appState->GetVersion()));
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
                pDocument->SetDevice(device, false); // Don't notify, since we have no sound yet.
                pDocument->SetSoundResource(std::move(pSound));
                pDocument->SetModifiedFlag(TRUE);
            }
        }
    }
}

#define PATCH_FILE_VIEW "view.*;*.v56"
#define PATCH_FILE_PIC "pic.*;*.p56"
#define PATCH_FILE_SCRIPT "script.*;*.scr"
#define PATCH_FILE_TEXT "text.*;*.tex"
#define PATCH_FILE_SOUND "sound.*;*.snd"
#define PATCH_FILE_MEMORY ""
#define PATCH_FILE_VOCAB "vocab.*;*.voc"
#define PATCH_FILE_FONT "font.*;*.fon"
#define PATCH_FILE_CURSOR "cursor.*;*.cur"
#define PATCH_FILE_PATCH "patch.*;*.pat"
#define PATCH_FILE_BITMAP "*.bit"
#define PATCH_FILE_PALETTE "palette.*;*.pal"
#define PATCH_FILE_CDAUDIO "*.cda"
#define PATCH_FILE_AUDIO "*.aud"
#define PATCH_FILE_SYNC "*.syn"
#define PATCH_FILE_MESSAGE "*.msg"
#define PATCH_FILE_AUDIOMAP "*.map"
#define PATCH_FILE_HEAP "*.hep"

//#define PATCH_FILE_TYPES "pic.*;view.*;vocab.*;font.*;cursor.*;text.*;sound.*;patch.*;script.*;*.v56;*.p56;*.scr;*.tex;*.snd;*.voc;*.fon;*.cur;*.pat;*.bit;*.pal;*.cda;*.aud;*.syn;*.msg;*.hep;*.map"

#define PATCH_FILE_TYPES PATCH_FILE_VIEW ";"\
 PATCH_FILE_PIC ";" \
PATCH_FILE_SCRIPT ";" \
PATCH_FILE_TEXT ";" \
PATCH_FILE_SOUND ";" \
PATCH_FILE_MEMORY ";" \
PATCH_FILE_VOCAB ";" \
PATCH_FILE_FONT ";" \
PATCH_FILE_CURSOR ";" \
PATCH_FILE_PATCH ";" \
PATCH_FILE_BITMAP ";" \
PATCH_FILE_PALETTE ";" \
PATCH_FILE_CDAUDIO ";" \
PATCH_FILE_AUDIO ";" \
PATCH_FILE_SYNC ";"  \
PATCH_FILE_MESSAGE ";" \
PATCH_FILE_AUDIOMAP ";" \
PATCH_FILE_HEAP

const TCHAR g_szResourceSpec[] = PATCH_FILE_TYPES;

const TCHAR* g_szResourceSpecByType[ResourceType::Max] =
{
    PATCH_FILE_VIEW,
    PATCH_FILE_PIC,
    PATCH_FILE_SCRIPT,
    PATCH_FILE_TEXT,
    PATCH_FILE_SOUND,
    PATCH_FILE_MEMORY,
    PATCH_FILE_VOCAB,
    PATCH_FILE_FONT,
    PATCH_FILE_CURSOR,
    PATCH_FILE_PATCH,
    PATCH_FILE_BITMAP,
    PATCH_FILE_PALETTE,
    PATCH_FILE_CDAUDIO,
    PATCH_FILE_AUDIO,
    PATCH_FILE_SYNC,
    PATCH_FILE_MESSAGE,
    PATCH_FILE_AUDIOMAP,
    PATCH_FILE_HEAP
};

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
    "Palettes (*.pal)|palette.*;*.pal|"
    "Audio (*.aud)|*.aud|"
    "CD Audio (*.cda)|*.cda|"
    "All files (*.*)|*.*|");

//
// Opens a resource from a file
//
void CMainFrame::OnFileOpenResource()
{
    CFileDialog fileDialog(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, g_szResourceFilter);
    if (IDOK == fileDialog.DoModal())
    {
        CString strFileName = fileDialog.GetPathName();
        ResourceBlob data;
        HRESULT hr = data.CreateFromFile(nullptr, (PCSTR)strFileName, appState->GetVersion(), appState->GetResourceMap().GetDefaultResourceSaveLocation(), appState->GetVersion().DefaultVolumeFile);
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
    CFileDialog fileDialog(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, g_szResourceFilter);
    if (IDOK == fileDialog.DoModal())
    {
        CString strFileName = fileDialog.GetPathName();

        int number = -1;
        std::string resNameFromFilename;
        MatchesResourceFilenameFormat((PCSTR)strFileName, appState->GetVersion(), &number, resNameFromFilename);

        // Get a resource number and package
        SaveResourceDialog srd(false, ResourceType::None);
        srd.Init(appState->GetVersion().DefaultVolumeFile, number, resNameFromFilename);
        if (IDOK == srd.DoModal())
        {
            int iResourceNumber = srd.GetResourceNumber();
            int iPackageNumber = srd.GetPackageNumber();
            ResourceBlob data;
            HRESULT hr = data.CreateFromFile(nullptr, (PCSTR)strFileName, appState->GetVersion(), appState->GetResourceMap().GetDefaultResourceSaveLocation(), iPackageNumber, iResourceNumber);
            if (!srd.GetName().empty())
            {
                data.SetName(srd.GetName().c_str());
            }
            else
            {
                data.SetName(nullptr);
            }
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

void CMainFrame::OnUpdateNewMessage(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->GetResourceMap().IsGameLoaded() && appState->GetVersion().SupportsMessages);
}

void CMainFrame::OnUpdateNewPalette(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->GetResourceMap().IsGameLoaded() && appState->GetVersion().HasPalette);
}

void CMainFrame::OnUpdateShowIfGameLoaded(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->GetResourceMap().IsGameLoaded());
}

void CMainFrame::OnUpdateShowIfSupportsAudio(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->GetVersion().MainAudioMapVersion != AudioMapVersion::None);
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

void CMainFrame::OnUpdateStopDebugging(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->IsProcessBeingDebugged());
}

void CMainFrame::OnStopDebugging()
{
    appState->TerminateDebuggedProcess();
}

void CMainFrame::OnShowPreferences()
{
    CPreferencesDialog dialog;
    if (IDOK == dialog.DoModal())
    {
    }
}

void CMainFrame::OnShowAudioPreferences()
{
    AudioPreferencesDialog dialog;
    if (IDOK == dialog.DoModal())
    {
    }
}

HRESULT RebuildResources(const GameFolderHelper &helper, SCIVersion version, BOOL fShowUI, ResourceSaveLocation saveLocation, std::map<ResourceType, RebuildStats> &stats);

void PurgeUnnecessaryResources()
{
    std::map<ResourceType, RebuildStats> stats;
    const GameFolderHelper &helper = appState->GetResourceMap().Helper();
    HRESULT hr = RebuildResources(helper, helper.Version, TRUE, helper.GetResourceSaveLocation(ResourceSaveLocation::Default), stats);
    if (SUCCEEDED(hr))
    {
        size_t totalSize = 0;
        for (const auto &stat : stats)
        {
            totalSize += stat.second.TotalSize;
        }
        vector<CompileResult> statResults;
        statResults.emplace_back(fmt::format("Total package size: {0:5}KB", totalSize / 1024), CompileResult::CompileResultType::CRT_Message);

        for (const auto &stat : stats)
        {
            std::string result = fmt::format("{0:3} {1:>10}: Total size: {2:4}KB ({3:4.2f}% of total)",
                stat.second.ItemCount,
                ResourceDisplayNameFromType(stat.first),
                stat.second.TotalSize / 1024,
                ((float)stat.second.TotalSize / (float)totalSize) * 100.0f
            );
            statResults.emplace_back(result, CompileResult::CompileResultType::CRT_Message);
        }
        appState->OutputResults(OutputPaneType::Compile, statResults);

        appState->GetResourceMap().StartPostBuildThread();
        appState->GetResourceMap().PokeResourceMapReloaded();
    }
}

void CMainFrame::OnRebuildResources()
{
    PurgeUnnecessaryResources();
}

void CMainFrame::OnRepackageAudio()
{
    appState->GetResourceMap().RepackageAudio();
}

void CMainFrame::OnRebuildClassTable()
{
    if (IDYES == AfxMessageBox("Rebuilding the class table will purge old deleted classes from the class table.\nAfterwards, you will need to compile all. Go ahead?", MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO))
    {
        SpeciesTable speciesTable;
        if (speciesTable.Load(appState->GetResourceMap().Helper()))
        {
            speciesTable.PurgeOldClasses(appState->GetResourceMap().Helper());
        }
    }
}

void CMainFrame::OnExtractAllResources()
{
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        ConvertViewsToVGA();
    }
    else
    {
        ExtractAllDialog extractAll;
        extractAll.DoModal();
    }
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
                pDoc->SetDependencyTracker(appState->GetDependencyTracker());
                pDoc->SetNameAndContent(script, dialog.GetNumber(), strBuffer);
                // Compile it so it is added to the resource map.
                pDoc->Compile();

                m_dlgForPanelDialogScript.Refresh();
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

// If dependencyTracker is null, all are compiled.
bool CompileABunchOfScripts(AppState *appState, DependencyTracker *dependencyTracker)
{
    std::unordered_set<std::string> scriptsToRecompile;
    if (dependencyTracker)
    {
        dependencyTracker->GetScriptsToRecompile(scriptsToRecompile, true);
    }
    if (dependencyTracker && scriptsToRecompile.empty())
    {
        // Nothing to do...
        return true;
    }

    CPrecisionTimer timer;
    timer.Start();

    g_compileIOTimer.Reset();
    g_compileSyntaxParseTimer.Reset();
    g_compileCodeGenTimer.Reset();
    g_compileDebugSymbolTimer.Reset();
    g_compileObjFileTimer.Reset();
    g_compileAppendTimer.Reset();

    bool result = true;

    // Clear out results
    appState->ShowOutputPane(OutputPaneType::Compile);
    appState->OutputClearResults(OutputPaneType::Compile);
    {
        DeferResourceAppend defer(appState->GetResourceMap());
        CNewCompileDialog dialog(scriptsToRecompile);
        dialog.DoModal();
        result = !dialog.HasErrors();
        g_compileIOTimer.Start();
        g_compileAppendTimer.Start();
        defer.Commit();
        g_compileIOTimer.Stop();
        g_compileAppendTimer.Stop();
    }

    timer.Stop();

    CompileLog log;
    log.ReportResult(CompileResult("--------------------------------"));
    std::stringstream strMessage;
    strMessage << "Time elapsed: " << fmt::format("{0:.2f}", (float)timer.GetEllapsed()) << " seconds.";
    strMessage << " Parse: " << fmt::format("{0:.2f}", (float)g_compileSyntaxParseTimer.GetEllapsed()) << " seconds.";
    strMessage << " CodeGen: " << fmt::format("{0:.2f}", (float)g_compileCodeGenTimer.GetEllapsed()) << " seconds.";
    strMessage << " Total I/O : " << fmt::format("{0:.2f}", (float)g_compileIOTimer.GetEllapsed()) << " seconds.";
    strMessage << " Sym I/O: " << fmt::format("{0:.2f}", (float)g_compileDebugSymbolTimer.GetEllapsed()) << " seconds.";
    strMessage << " .sco I/O: " << fmt::format("{0:.2f}", (float)g_compileObjFileTimer.GetEllapsed()) << " seconds.";
    strMessage << " write I/O: " << fmt::format("{0:.2f}", (float)g_compileAppendTimer.GetEllapsed()) << " seconds.";
    log.ReportResult(CompileResult(strMessage.str()));
    log.CalculateErrors();

    // Even if we weren't completely successful, update the script resource view, since some
    // scripts may have been successful.
    appState->GetResourceMap().NotifyToReloadResourceType(ResourceType::Script);

    // Get the final success/fail message up there:
    appState->OutputAddBatch(OutputPaneType::Compile, log.Results());
    appState->OutputFinishAdd(OutputPaneType::Compile);

    return result;
}

void CMainFrame::OnCompileAll()
{
    CompileABunchOfScripts(appState, nullptr);
}

// TODO: Attempt at making Find in Files faster. regex was way too slow. Just need to mimic line endings of crystal text buffer.
/*
void CMainFrame::_AddFindResults(vector<char> &buffer, ICompileLog &log, PCTSTR pszFullPath, PCTSTR pszFileName, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    // Start easy at first
    string searchString = fmt::format("({0})", pszWhat);
    ifstream is;
    is.open(pszFullPath, ios_base::in);
    if (is.is_open())
    {
        is.read(&buffer[0], buffer.size());
        PCSTR pszHere = &buffer[0];
        int nPos;
        while (-1 != (nPos = FindStringHelper(pszHere, pszWhat, fWholeWord)))
        {
            int x = 0;
            pszHere += (nPos + 1);
        }

    }
}
*/


void CMainFrame::_AddFindResults(vector<char> &dummyBuffer, ICompileLog &log, PCTSTR pszFullPath, PCTSTR pszFileName, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
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

void CMainFrame::_FindInFilesOfType(ICompileLog &log, const std::string &srcFolder, PCTSTR pszWildcard, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    vector<char> worker(5000);
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
            if (worker.size() < findData.nFileSizeLow)
            {
                worker.resize(findData.nFileSizeLow);
            }
            _AddFindResults(worker, log, strFullPath.c_str(), findData.cFileName, pszWhat, fMatchCase, fWholeWord);
            fOk = FindNextFile(hFFF, &findData);
        }
        FindClose(hFFF);
    }
}

const int TextRangeOutsideResultToShow = 35;

std::unordered_set<uint8_t> _GetSetOfMatchingNumbers(const std::vector<MessageDefine> &defines, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    std::unordered_set<uint8_t> numbers;
    for (auto &define : defines)
    {
        std::string text = define.first;
        if (!fMatchCase)
        {
            ToUpper(text);
            int pos = FindStringHelper(text.c_str(), pszWhat, fWholeWord);
            if (pos != -1)
            {
                numbers.insert((uint8_t)define.second);
            }
        }
    }
    return numbers;
}

void CMainFrame::_FindInTexts(ICompileLog &log, PCTSTR pszWhat, BOOL fMatchCase, BOOL fWholeWord)
{
    std::unordered_set<uint8_t> matchingTalkerNumbers;
    std::unordered_set<uint8_t> matchingVerbNumbers;
    MessageSource *verbsMessageSource = appState->GetResourceMap().GetVerbsMessageSource(false);
    if (verbsMessageSource)
    {
        matchingVerbNumbers = _GetSetOfMatchingNumbers(verbsMessageSource->GetDefines(), pszWhat, fMatchCase, fWholeWord);
    }
    MessageSource *talkers = appState->GetResourceMap().GetTalkersMessageSource();
    if (talkers)
    {
        matchingTalkerNumbers = _GetSetOfMatchingNumbers(talkers->GetDefines(), pszWhat, fMatchCase, fWholeWord);
    }

    auto container = appState->GetResourceMap().Resources(ResourceTypeFlags::Text | ResourceTypeFlags::Message, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::AddInDefaultEnumFlags);
    for (auto &blob : *container)
    {
        auto resource = CreateResourceFromResourceData(*blob);
        TextComponent &textComponent = resource->GetComponent<TextComponent>();
        for (size_t i = 0; i < textComponent.Texts.size(); i++)
        {
            auto &entry = textComponent.Texts[i];
            std::string text = entry.Text;
            if (!fMatchCase)
            {
                ToUpper(text);
            }
            int pos = FindStringHelper(text.c_str(), pszWhat, fWholeWord);
            std::string resultText;
            if (pos != -1)
            {
                text = textComponent.Texts[i].Text; // since we ToUpper'd it.
                int rangeStart = max(0, pos - TextRangeOutsideResultToShow);
                int rangeEnd = min(pos + TextRangeOutsideResultToShow + lstrlen(pszWhat), (int)text.size());
                resultText = fmt::format("{0}{1}{2}",
                    (rangeStart > 0) ? "" : "...",
                    text.substr(rangeStart, rangeEnd - rangeStart),
                    (rangeEnd < (int)text.size()) ? "" : "..."
                    );
            }
            else
            {
                if (entry.Talker && (matchingTalkerNumbers.find(entry.Talker) != matchingTalkerNumbers.end()))
                {
                    resultText = fmt::format("{0} - {1}...", talkers->ValueToName(entry.Talker), textComponent.Texts[i].Text.substr(0, TextRangeOutsideResultToShow));
                }
                else if (entry.Verb && (matchingVerbNumbers.find(entry.Verb) != matchingVerbNumbers.end()))
                {
                    resultText = fmt::format("{0} - {1}...", verbsMessageSource->ValueToName(entry.Verb), textComponent.Texts[i].Text.substr(0, TextRangeOutsideResultToShow));
                }
            }
            if (!resultText.empty())
            {
                std::string finalText = fmt::format("{0} ({1}, {2}): {3}",
                    GetResourceInfo(blob->GetType()).pszTitleDefault,
                    blob->GetNumber(),
                    i,
                    resultText);
                log.ReportResult(CompileResult(finalText, blob->GetType(), blob->GetNumber(), (int)i));
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
            std::string srcFolder = appState->GetResourceMap().Helper().GetSrcFolder();
            std::string polyFolder = appState->GetResourceMap().Helper().GetPolyFolder();
            std::string messageFolder = appState->GetResourceMap().Helper().GetMsgFolder();
            std::string includeFolder = appState->GetResourceMap().Helper().GetIncludeFolder();

            _FindInFilesOfType(log, srcFolder, TEXT("\\*.sc"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, srcFolder, TEXT("\\*.sh"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, polyFolder, TEXT("\\*.shp"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, messageFolder, TEXT("\\*.shm"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            _FindInFilesOfType(log, includeFolder, TEXT("\\*.sh"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            // We don't support c++ syntax ATM
            //_FindInFilesOfType(log, TEXT("\\*.scp"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));
            //_FindInFilesOfType(log, TEXT("\\*.shp"), strFindWhat, (_fMatchCase != 0), (_fMatchWholeWord != 0));

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
        appState->OutputResults(OutputPaneType::Find, log.Results());
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
            m_dlgForPanelDialogViewEGAFixed.SetDocument(nullptr);
            m_dlgForPanelDialogFont.SetDocument(nullptr);
            m_dlgForPanelDialogCursor.SetDocument(nullptr);
            m_dlgForPanelDialogPic.SetDocument(nullptr);
            m_dlgForPanelDialogPicVGA.SetDocument(nullptr);
            m_dlgForPanelDialogPicEGAPoly.SetDocument(nullptr);
            m_dlgForPanelDialogScript.SetDocument(nullptr);
            m_wndScriptToolComboBoxClass.SetDocument(nullptr);
            m_dlgForPanelDialogGame.SetDocument(nullptr);
            m_dlgForPanelDialogSound.SetDocument(nullptr);
            m_dlgForPanelDialogMessage.SetDocument(nullptr);
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

StatusIndicatorInfo textIndicators[] =
{
    { ID_INDICATOR_RESSIZE, 150 },
};

StatusIndicatorInfo messageIndicators[] =
{
    { ID_INDICATOR_RESSIZE, 150 },
    { ID_INDICATOR_AUDIOSIZE, 150 },
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
        indicators = textIndicators;
        count = ARRAYSIZE(textIndicators);
        break;

    case TAB_MESSAGE:
        indicators = messageIndicators;
        count = ARRAYSIZE(messageIndicators);
        break;

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
                if (appState->GetVersion().FixedPalette)
                {
                    pWndToShow = &m_dlgForPanelDialogViewEGAFixed;
                    m_dlgForPanelDialogViewEGAFixed.SetDocument(pDoc);
                }
                else
                {
                    pWndToShow = &m_dlgForPanelDialogView;
                    m_dlgForPanelDialogView.SetDocument(pDoc);
                }
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
                    if (appState->GetVersion().UsesPolygons)
                    {
                        pWndToShow = &m_dlgForPanelDialogPicEGAPoly;
                        m_dlgForPanelDialogPicEGAPoly.SetDocument(pDoc);
                    }
                    else
                    {
                        pWndToShow = &m_dlgForPanelDialogPic;
                        m_dlgForPanelDialogPic.SetDocument(pDoc);
                    }
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

            case TAB_MESSAGE:
                pWndToShow = &m_dlgForPanelDialogMessage;
                m_dlgForPanelDialogMessage.SetDocument(pDoc);
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
        _HideTabIfNot(iTabType, TAB_MESSAGE, m_wndMessageTools);
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
        case TAB_MESSAGE:
            ShowControlBar(&m_wndMessageTools, TRUE, TRUE);
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
    CClassBrowserDialog dialog(&appState->GetClassBrowser());
    dialog.DoModal();
}

#ifdef DOCSUPPORT
void CMainFrame::OnGenerateDocs()
{
    _docsDialog = std::make_unique<GenerateDocsDialog>(appState->GetResourceMap().Helper());
    _docsDialog->Create(GenerateDocsDialog::IDD);
    _docsDialog->ShowWindow(SW_SHOW);
}
#endif

BOOL createNearestColorsDialogResult = FALSE;
NearestColorsDialog nearestColorsDialog;

void CMainFrame::OnShowNearestColors()
{
    if (!createNearestColorsDialogResult)
    {
        createNearestColorsDialogResult = nearestColorsDialog.Create(NearestColorsDialog::IDD, this);
    }
    if (createNearestColorsDialogResult)
    {
        nearestColorsDialog.ShowWindow(SW_SHOW);
    }
}


void CMainFrame::OnManageDecompilation()
{
    unique_ptr<DecompileDialog> decompileDialog = make_unique<DecompileDialog>();
    decompileDialog->DoModal();
    appState->GetResourceMap().NotifyToReloadResourceType(ResourceType::Script);
}

void CMainFrame::OnUpdateClassBrowser(CCmdUI *pCmdUI)
{
    SCIClassBrowser &browser = appState->GetClassBrowser();
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

void CMainFrame::OnUpdateValidateAllSaids(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(appState->GetVersion().HasSaidVocab);
}

void CMainFrame::OnValidateAllSaids()
{
    const Vocab000 *vocab = appState->GetResourceMap().GetVocab000();
    if (vocab)
    {
        CompileLog log;
        ValidateSaids(log, *vocab);
        appState->OutputResults(OutputPaneType::Compile, log.Results());
    }
}

void CMainFrame::OnValidateSelectors()
{
    SelectorTable &selectors = appState->GetResourceMap().GetCompiledScriptLookups()->GetSelectorTable();
    CompileLog log;
    ValidateSelectors(log, selectors);
    appState->OutputResults(OutputPaneType::Compile, log.Results());
}


void CMainFrame::ExtractAllScriptStrings()
{
    std::vector<ScriptId> scripts;
    appState->GetResourceMap().GetAllScripts(scripts);
    CompileLog log;
    std::vector<std::string> allStrings;
    for (ScriptId &script : scripts)
    {
        ExtractScriptStrings(log, script, allStrings);
    }
    // TODO: show compile errors
    std::stringstream ss;
    for (const std::string &line : allStrings)
    {
        ss << line;
        ss << "\n";
    }
    ShowTextFile(ss.str().c_str(), "Script strings.txt");
}

/*
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
*/

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
    ON_MESSAGE(UWM_STATUSREADY, _OnStatusReady)
END_MESSAGE_MAP()

LRESULT CBrowseInfoStatusPane::_OnStatusReady(WPARAM wParam, LPARAM lParam)
{
    string textToPost;
    BrowseInfoStatus status;
    {
        
        std::lock_guard<std::mutex> lock(_csTextPosting);
        textToPost = _textToPost;
        status = _status;
    }
    SetWindowText(textToPost.c_str());
    if (status == Errors)
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

void CBrowseInfoStatusPane::OnLButtonDblClk()
{
    if (_status == Errors)
    {
        SCIClassBrowser &browser = appState->GetClassBrowser();
        appState->OutputResults(OutputPaneType::Compile, browser.GetErrors());
    }
}

// This method may  be called on a different thread, so care should be taken
// as to what gets done here... generally just Invalidate()
void CBrowseInfoStatusPane::NotifyClassBrowserStatus(BrowseInfoStatus status, int iPercent)
{
    // We may end up calling NotifyClassBrowserStatus thousands of times. Don't bother doing that (and taking critsec)
    // unless necessary.
    if ((_status != status) || ((status == InProgress) && (iPercent != _lastPercent)))
    {
        std::lock_guard<std::mutex> lock(_csTextPosting);
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
        _lastPercent = iPercent;
        PostMessage(UWM_STATUSREADY, 0, 0);
    }
}

CBrowseInfoStatusPane::CBrowseInfoStatusPane() : _lastPercent(-1) {}
CBrowseInfoStatusPane::~CBrowseInfoStatusPane() {}

