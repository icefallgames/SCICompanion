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

template<typename _TFunc>
void ProcessClipboardDataIfAvailable(UINT clipboardFormat, CWnd *wnd, _TFunc func)
{
    if (IsClipboardFormatAvailable(clipboardFormat))
    {
        OpenClipboardGuard clipBoard(wnd);
        if (clipBoard.IsOpen())
        {
            HGLOBAL hMem = GetClipboardData(clipboardFormat);
            if (hMem)   // No need to free?
            {
                GlobalLockGuard<uint8_t*> globalLock(hMem);
                uint8_t *data = globalLock.Object;
                if (data)
                {
                    sci::istream byteStream(data, GlobalSize(hMem));
                    func(byteStream);
                }
            }
        }
    }
}

void OpenAndSetClipboardDataFromStream(CWnd *wnd, UINT clipboardFormat, const sci::ostream &stream);
void SetClipboardDataFromStream(UINT clipboardFormat, const sci::ostream &stream);
