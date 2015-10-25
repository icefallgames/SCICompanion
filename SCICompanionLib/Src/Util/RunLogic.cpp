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
#include "RunLogic.h"
#include "format.h"

static const char c_szExeProfileKey[] = "ExeProfile";    // Defaults to Other

// When starting a fan-made game via ScummVM, ScummVM needs an id in order to associate
// with a known game. The SCI0 template game is sci-fanmade. The SCI1.1 is based on the
// SQ5 code, but it is modified to use the lsl6 interpreter, so we'll use lsl6-cd.
static const char c_szScummIdKey[] = "ScummId";

static const char c_szDOSBox[] = "DOSBox";
static const char c_szScummVM[] = "ScummVM";
static const char c_szOther[] = "Other";

// Other
static const char c_szOtherExecutableString[] = "Executable";
static const char c_szOtherExeParametersString[] = "ExeCmdLineParameters";
static const char c_szOtherDefaultParametersString[] = "";
static const char c_szDefaultExe[] = "sciv.exe";

// DOSBox
static const char c_szDOSBoxExecutableString[] = "DOSBox";
static const char c_szDOSBoxExeParametersString[] = "DosBoxCmdLineParameters";

// ScummVM
static const char c_szScummVMExecutableString[] = "ScummVM";
static const char c_szScummVMExeParametersString[] = "ScummVMCmdLineParameters";

static const char c_szGameSection[] = "Game";

static const char* c_szCommonGameExes[] =
{
    "SIERRA.EXE", "SCIV.EXE", "SCIDHUV.EXE", "SCIDUV.EXE", "SCIDHV.EXE", "SCIWV.EXE"
};

RunLogic::RunLogic() :
_profileToExeKey({ { c_szDOSBox, c_szDOSBoxExecutableString }, { c_szScummVM, c_szScummVMExecutableString }, { c_szOther, c_szOtherExecutableString } }),
_profileToParamsKey({ { c_szDOSBox, c_szDOSBoxExeParametersString }, { c_szScummVM, c_szScummVMExeParametersString }, { c_szOther, c_szOtherExeParametersString } })
{
    // Come up with runtime versions of the default DOSBox and Scumm paths

    // A version of DOSBox comes with SCICompanion. Let's use this by default
    std::string DOSBoxFolder = GetExeSubFolder("Tools\\DOSBox");
    _profileToDefaultExe[c_szDOSBox] = DOSBoxFolder + "\\" + "DOSBox.exe";
    
    // Default ScummVM exe. ScummVM and SCICompanion are both 32-bit processes, so %ProgramFiles% will resolve correctly.
    char szScummPath[MAX_PATH] = {};
    ExpandEnvironmentStrings("%ProgramFiles%\\ScummVM\\scummvm.exe", szScummPath, ARRAYSIZE(szScummPath));
    _profileToDefaultExe[c_szScummVM] = szScummPath;
}

void RunLogic::SetGameFolder(const std::string &folder)
{
    _gameFolder = folder;
    _gameIni = fmt::format("{0}\\game.ini", _gameFolder);
    _defaultProfile = c_szDOSBox;

    // What should our default profile be if non is specifed? DOSBox, unless we find an Executable key in [game], then we'll
    // switch it to "Other". This ensures compatibility with old games.
    if (!_ReadProfileString(c_szOtherExecutableString, "").empty())
    {
        _defaultProfile = c_szOther;
    }

    // Set up the DOSBox and Scumm cmd line params
    _profileToDefaultParams.clear();

    // Look for an exe
    std::string foundExe;
    for (const char *commonExe : c_szCommonGameExes)
    {
        std::string fullPath = _gameFolder + "\\" + commonExe;
        if (PathFileExists(fullPath.c_str()))
        {
            foundExe = commonExe;
            break;
        }
    }
    if (!foundExe.empty())
    {
        _profileToDefaultParams[c_szDOSBox] = fmt::format("{0} -exit -conf dosbox.conf -noconsole", foundExe);
    }

    std::string scummId = _ReadProfileString(c_szScummIdKey, "");
    _profileToDefaultParams[c_szScummVM] = fmt::format("--no-aspect-ratio --path=\"{0}\" {1}", _gameFolder, scummId);
}

bool RunLogic::RunGame(std::string &error, HANDLE &hProcess)
{
    std::string option = GetExecutableProfile();
    std::string exe = GetExe(option);
    std::string params = GetParams(option);

    bool success = false;
    hProcess = INVALID_HANDLE_VALUE;

    if (exe.empty())
    {
        error = "No executable specified. Open the Game Properties and specify an executable, or choose a different profile.";
    }
    else
    {
        SHELLEXECUTEINFO ei = {};
        ei.fMask = SEE_MASK_DEFAULT | SEE_MASK_NOCLOSEPROCESS;
        ei.cbSize = sizeof(ei);
        ei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
        ei.lpFile = exe.c_str();
        ei.lpVerb = "Open";
        ei.lpParameters = params.c_str();
        ei.lpDirectory = _gameFolder.c_str();
        ei.nShow = SW_SHOWNORMAL;

        // ShellExecuteEx throws an RPC exception, but I guess that's ok.
        if (!ShellExecuteEx(&ei))
        {
            INT_PTR iResult = GetLastError();

            TCHAR szReason[MAX_PATH];
            szReason[0] = 0;
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, (DWORD)iResult, 0, szReason, ARRAYSIZE(szReason), NULL);

            TCHAR szError[MAX_PATH];
            StringCchPrintf(szError, ARRAYSIZE(szError), TEXT("Failed to start %s: %s"), exe.c_str(), szReason);
            error = szError;
        }
        else
        {
            success = true;
            hProcess = ei.hProcess;
        }
    }
    return success;
}

bool RunLogic::_WriteProfileString(const std::string &key, const std::string &value)
{
    return !!WritePrivateProfileString(c_szGameSection, key.c_str(), value.c_str(), _gameIni.c_str());
}

std::string RunLogic::_ReadProfileString(const std::string &key, const std::string &defaultValue)
{
    std::string result;
    char szBuffer[500]; // Hope that's big enough.
    if (GetPrivateProfileString(c_szGameSection, key.c_str(), defaultValue.c_str(), szBuffer, ARRAYSIZE(szBuffer), _gameIni.c_str()))
    {
        result = szBuffer;
    }
    return result;
}

std::vector<std::string> RunLogic::GetExecutableProfiles()
{
    std::vector<std::string> options = { c_szDOSBox, c_szScummVM, c_szOther };
    return options;
}

void RunLogic::SetExecutableProfile(const std::string &profile)
{
    _WriteProfileString(c_szExeProfileKey, profile);
}

std::string RunLogic::GetExecutableProfile()
{
    return _ReadProfileString(c_szExeProfileKey, _defaultProfile);
}

std::string RunLogic::GetExe(const std::string &profile)
{
    return _ReadProfileString(_profileToExeKey[profile], _profileToDefaultExe[profile]);
}
std::string RunLogic::GetParams(const std::string &profile)
{
    return _ReadProfileString(_profileToParamsKey[profile], _profileToDefaultParams[profile]);
}
void RunLogic::SetExe(const std::string &profile, const std::string &exe)
{
    _WriteProfileString(_profileToExeKey[profile], exe);
}

void RunLogic::SetParams(const std::string &profile, const std::string &params)
{
    _WriteProfileString(_profileToParamsKey[profile], params);
}
