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
// SCIPicEditor.h : main header file for the SCIPicEditor application
//
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols
#include "ResourceMap.h"
#include "ResourceRecency.h"
#include "IntellisenseListBox.h"
#include "ColoredToolTip.h"
#include "CompileInterfaces.h"

class AutoCompleteThread;
class CScriptView;

// SCICompanionApp:
// See SCIPicEditor.cpp for the implementation of this class
//

// FWD decl
class CScriptDoc;
class ResourceEntity;
class CResourceListDoc;
class AppState;

class SCICompanionApp : public CWinApp
{
public:
    SCICompanionApp();
    ~SCICompanionApp();

    // Overrides
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    BOOL _RegisterWindowClasses();
    void _LoadSettings(BOOL fReset = FALSE);
    void _SaveSettings();

    virtual void AddToRecentFileList(PCTSTR lpszPathName) override;
    virtual CDocument* OpenDocumentFile(PCTSTR lpszFileName) override;

    // Implementation
    afx_msg void OnAppAbout();
    afx_msg void OnTutorials();
    afx_msg void OnForums();
    afx_msg void OnWindowsExplorer();
    afx_msg void OnOpenPluginsFolder();
    afx_msg void OnDocumentation();
    afx_msg void OnEmail();
    afx_msg void GetGames();
    afx_msg void OnSCICompHelp();
    afx_msg void OnUpdates();
    afx_msg void OnRunGame();
    afx_msg void OnDebugGame();
    afx_msg void OnCloseGame();
    afx_msg void OnGameProperties();
    afx_msg void OnGameVersionDetection();
    afx_msg void OnRoomExplorer();
    afx_msg void OnUpdateGameLoaded(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()

private:

};


