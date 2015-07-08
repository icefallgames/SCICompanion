#include "stdafx.h"
#include "CppUnitTest.h"
#include "Helper.h"
#include "AppState.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
    delete appState;

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
