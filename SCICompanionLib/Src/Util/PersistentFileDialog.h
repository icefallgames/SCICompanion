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
#pragma once

// File-open/save dialogs weren't remembering the last folder browsed to
// under Wine -- within the same session, not just across restarts, and
// on every single dialog in the app. Not caused by OFN_NOCHANGEDIR
// (removed from every CFileDialog call site in this codebase, no
// effect) or an explicit SetCurrentDirectory call anywhere (there isn't
// one), so Wine's own "remember last folder" mechanism appears to just
// not persist here regardless of what flags are passed. Rather than
// depend on that OS-level behavior at all, this class takes over the
// responsibility itself: sets the dialog's initial directory from our
// own persisted setting on construction, and updates that setting after
// a successful pick. Drop-in replacement for CFileDialog -- same
// constructor signature.
CString GetLastUsedFolder();
void SetLastUsedFolder(const CString &folder);

class CPersistentFileDialog : public CFileDialog
{
public:
    CPersistentFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = nullptr, LPCTSTR lpszFileName = nullptr,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = nullptr, CWnd *pParentWnd = nullptr);
    INT_PTR DoModal();

private:
    // Kept alive for the lifetime of the dialog -- m_ofn.lpstrInitialDir
    // just points at this buffer's data, so it can't be a temporary.
    CString _initialDir;
};
