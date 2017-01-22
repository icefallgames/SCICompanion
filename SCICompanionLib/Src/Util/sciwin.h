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


class OpenClipboardGuard
{
public:
    OpenClipboardGuard(HWND hwnd)
    {
        _open = OpenClipboard(hwnd);
    }
    OpenClipboardGuard(CWnd *pWnd)
    {
        _open = pWnd->OpenClipboard();
    }

    BOOL IsOpen() { return _open; }

    ~OpenClipboardGuard()
    {
        Close();
    }

    void Close()
    {
        if (_open)
        {
            CloseClipboard();
            _open = FALSE;
        }
    }

private:
    BOOL _open;
};


BOOL _GetMenuItem(PCTSTR pszText, CMenu *pMenu, UINT *pnID);

size16 CSizeToSize(CSize size);
CSize SizeToCSize(size16 size);

BOOL CreateDCCompatiblePattern(RGBQUAD color1, RGBQUAD color2, int width, int height, CDC *pDC, CBitmap *pbm);
void *_GetBitsPtrFromBITMAPINFO(const BITMAPINFO *pbmi);

bool Save8BitBmp(const std::string &filename, const BITMAPINFO &info, BYTE *pBits, DWORD id = 0);

point16 CPointToPoint(CPoint pt);
CPoint PointToCPoint(point16 pt);
point16 CPointToPointClamp(CPoint pt);