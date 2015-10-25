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
#include "ExtDialogFwdCmd.h"

BOOL CExtDialogFwdCmd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    BOOL fRet = __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    if (!fRet)
    {
        CFrameWnd *pFrame = GetTopLevelFrame();
        if (pFrame)
        {
            CFrameWnd *pFrameActive = pFrame->GetActiveFrame();
            if (pFrameActive)
            {
                fRet = pFrameActive->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
            }
        }
    }
    return fRet;
}