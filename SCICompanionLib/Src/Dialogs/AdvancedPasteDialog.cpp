/***************************************************************************
Copyright (c) 2019 Philip Fortier

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
#include "AppState.h"
#include "AdvancedPasteDialog.h"
#include "Components.h"
#include "RasterOperations.h"
#include "NewRasterResourceDocument.h"
#include <format.h>

using namespace fmt;
using namespace std;

AdvancedRasterCopyInfo g_copyInfo =
{
    AdvancedRasterOp::Replace,
    TRUE,       // Random cel
    0,
    0,
    TRUE,       // Random offset
    TRUE,       // wraps
    TRUE,
    TRUE,       // Greyscale (?)
    FALSE,      // honor tx
    0, 0, 0, 0,  // Margins
    0, 0,        // frame offsets
    FALSE,
    0, 360
};

// AdvancedPasteDialog dialog

AdvancedPasteDialog::AdvancedPasteDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(AdvancedPasteDialog::IDD, pParent), _initialized(false), _pDoc(nullptr), _destLoop(0), _sourceLoop(0), _fillAllCels(FALSE)
{
    _blendOp = static_cast<int>(g_copyInfo.blendOp);
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPALETTE), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPALETTE));
}

BOOL AdvancedPasteDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        /*
        if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_DELETE))
        {
            if (GetFocus() == &m_wndClips)
            {
                fRet = TRUE;
                DeleteAnItem();
            }
        }*/
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

void AdvancedPasteDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECKRANDOMCEL, g_copyInfo.randomCel);
    DDX_Check(pDX, IDC_CHECKRANDOMOFFSET, g_copyInfo.randomOffset);
    DDX_Check(pDX, IDC_CHECKWRAPX, g_copyInfo.xWrap);
    DDX_Check(pDX, IDC_CHECKWRAPY, g_copyInfo.yWrap);
    DDX_Check(pDX, IDC_CHECKHONORTRANSPARENCY, g_copyInfo.honorTx);
    DDX_Check(pDX, IDC_CHECKALLDESTCELS, _fillAllCels);

    DDX_CBIndex(pDX, IDC_COMBOBLENDOP, _blendOp);
    DDX_Text(pDX, IDC_EDITDESTLOOP, _destLoop);
    DDX_Text(pDX, IDC_SOURCELOOP, _sourceLoop);

    DDX_Text(pDX, IDC_EDITMARGINLEFT, g_copyInfo.xMarginLeft);
    DDX_Text(pDX, IDC_EDITMARGINRIGHT, g_copyInfo.xMarginRight);
    DDX_Text(pDX, IDC_EDITMARGINTOP, g_copyInfo.yMarginTop);
    DDX_Text(pDX, IDC_EDITMARGINBOTTOM, g_copyInfo.yMarginBottom);
    DDX_Text(pDX, IDC_EDITFRAMEOFFSETX, g_copyInfo.xFrameOffset);
    DDX_Text(pDX, IDC_EDITFRAMEOFFSETY, g_copyInfo.yFrameOffset);

    DDX_Check(pDX, IDC_CHECKRANDOMOFFSETANGLE, g_copyInfo.randomFrameOffset);
    DDX_Text(pDX, IDC_EDITANGLEMIN, g_copyInfo.minAngle);
    DDX_Text(pDX, IDC_EDITANGLEMAX, g_copyInfo.maxAngle);

    if (!_initialized)
    {
        _initialized = true;

        // Visuals
        DDX_Control(pDX, ID_OK, m_wndOk);
        // Spin control
        DDX_Control(pDX, IDC_SPINSOURCELOOP, m_wndSourceLoop);
    }

}


BEGIN_MESSAGE_MAP(AdvancedPasteDialog, CExtResizableDialog)
    ON_BN_CLICKED(ID_OK, &AdvancedPasteDialog::OnBnClickedDoit)
END_MESSAGE_MAP()

void AdvancedPasteDialog::SetRasterDocument(CNewRasterResourceDocument *pDoc)
{
    _pDoc = pDoc;

    // Update our range
    m_wndSourceLoop.SetRange(_pDoc->GetResource()->GetComponent<RasterComponent>().LoopCount() - 1, 0);
}

void AdvancedPasteDialog::OnBnClickedDoit()
{
    UpdateData(TRUE);

    // Can't cast 
    g_copyInfo.blendOp = static_cast<AdvancedRasterOp>(_blendOp);

    _pDoc->ApplyChanges<RasterComponent>(
        [&](RasterComponent &raster)
    {
        int cCels = _fillAllCels ? (int)raster.Loops[_destLoop].Cels.size() : (int)raster.Loops[_sourceLoop].Cels.size();
        RasterChange rasterChange = AdvancedRasterCopy(g_copyInfo, raster, CelIndex(_sourceLoop, 0), cCels, raster, CelIndex(_destLoop, 0));
        return WrapRasterChange(rasterChange);
    }
    );

}