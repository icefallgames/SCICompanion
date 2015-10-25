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
#include "CustomMessageBox.h"

static HHOOK messageBoxHook;
static std::vector<std::pair<int, std::string>> *g_Substitutions = nullptr;

BOOL CALLBACK MessageBoxEnumProc(HWND hwnd, LPARAM lParam)
{
    char szClassName[200];
    GetClassName(hwnd, szClassName, ARRAYSIZE(szClassName) - 1);
    if (0 == lstrcmp(szClassName, "Button"))
    {
        int ctlId = GetDlgCtrlID(hwnd);
        for (auto &sub : (*g_Substitutions))
        {
            if (sub.first == ctlId)
            {
                SetWindowText(hwnd, sub.second.c_str());
                break;
            }
        }
    }
    return true;
}

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code < 0)
    {
        return CallNextHookEx(messageBoxHook, code, wParam, lParam);
    }

    CWPRETSTRUCT *msg = reinterpret_cast<CWPRETSTRUCT *>(lParam);
    HHOOK hook = messageBoxHook;
    if (msg->message == WM_INITDIALOG)
    {
        char szClassName[200];
        GetClassName(msg->hwnd, szClassName, ARRAYSIZE(szClassName) - 1);
        if (0 == lstrcmp(szClassName, "#32770"))
        {
            EnumChildWindows(msg->hwnd, MessageBoxEnumProc, 0);
        }
    }
    return CallNextHookEx(hook, code, wParam, lParam);
}

MessageBoxCustomization::MessageBoxCustomization(const std::vector<std::pair<int, std::string>> &subs) : _unhook(false)
{
    for (auto &sub : subs)
    {
        Substitutions.push_back(sub);
        g_Substitutions = &Substitutions;
    }

    if (messageBoxHook == nullptr)
    {
        _unhook = true;
        messageBoxHook = SetWindowsHookEx(WH_CALLWNDPROCRET, hookProc, nullptr, GetCurrentThreadId());
    }
    // otherwise do nothing.
}

MessageBoxCustomization::~MessageBoxCustomization()
{
    if (_unhook)
    {
        UnhookWindowsHookEx(messageBoxHook);
        messageBoxHook = nullptr;
    }
}

