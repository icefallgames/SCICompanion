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
#include "ClipboardUtil.h"

void SetClipboardDataFromStream(UINT clipboardFormat, const sci::ostream &serial)
{
    GlobalAllocGuard globalAlloc(GMEM_MOVEABLE, serial.tellp());
    if (globalAlloc.Global)
    {
        GlobalLockGuard<uint8_t*> globalLock(globalAlloc);
        if (globalLock.Object)
        {
            memcpy(globalLock.Object, serial.GetInternalPointer(), serial.tellp());
            globalLock.Unlock();
        }
        else
        {
            // Didn't work
            globalAlloc.Free();
        }
        if (SetClipboardData(clipboardFormat, globalAlloc.Global))
        {
            // Success. Clipboard now owns the data.
            globalAlloc.RelinquishOwnership();
        }
    }
}

void OpenAndSetClipboardDataFromStream(CWnd *wnd, UINT clipboardFormat, const sci::ostream &stream)
{
    OpenClipboardGuard clipBoard(wnd);
    if (clipBoard.IsOpen())
    {
        if (EmptyClipboard())
        {
            SetClipboardDataFromStream(clipboardFormat, stream);
        }
    }
}

