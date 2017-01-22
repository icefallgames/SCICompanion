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
#include "stdafx.h"
#include "CppUnitTest.h"
#include "Helper.h"
#include "AppState.h"
#include "ClassBrowser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string GetTestFileDirectory(const std::string &subDirectory)
{
    char szPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szPath);
    std::string folder = szPath;
    folder += "\\TestFiles\\" + subDirectory;
    return folder;
}

std::string SetUpGame(const std::string &name)
{
    char szPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szPath);
    std::string srcGameFolder = szPath;
    srcGameFolder += name;

    std::string gameFolder = GetRandomTempFolder();
    Assert::IsFalse(gameFolder.empty());
    CopyFilesOver(nullptr, srcGameFolder, gameFolder);

    appState = new AppState(nullptr);
    appState->GetResourceMap().SetGameFolder(gameFolder);

    // Normally ResourceMap uses the module filename for this. But unit tests are run from another exe.
    std::string exeFolder = szPath;
    exeFolder += "\\";
    appState->GetResourceMap().SetIncludeFolderForTest(exeFolder);

    return gameFolder;
}

void CleanUpGame(const std::string &gameFolder)
{
    appState->GetClassBrowser().SetClassBrowserEvents(nullptr);

    appState->ResetClassBrowser();
    delete appState;
    appState = nullptr;

    char szPath[MAX_PATH];
    StringCchCopy(szPath, ARRAYSIZE(szPath), gameFolder.c_str());
    szPath[gameFolder.length() + 1] = 0;   // double null term

    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.hwnd = nullptr;
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = szPath;
    fileOp.pTo = nullptr;
    fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
    int result = SHFileOperation(&fileOp);

}

std::string SetUpGameSCI0()
{
    return SetUpGame("\\TemplateGame\\SCI0\\*");
}

std::string SetUpGameSCI11()
{
    return SetUpGame("\\TemplateGame\\SCI1.1\\*");
}
