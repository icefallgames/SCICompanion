// RasterSidePane.cpp : implementation file
//

#include "stdafx.h"
#include "RasterSidePane.h"
#include "resource.h"
#include "ChoosePriConDialog.h"
#include "Components.h"
#include "NewRasterResourceDocument.h"
#include "Components.h"
#include "AppState.h"
#include "FontOperations.h"
#include "PaletteOperations.h"
#include "format.h"
#include "ChoosePenStyleDialog.h"

// RasterSidePane

RasterSidePane::RasterSidePane(CWnd* pParent /*=nullptr*/)
	: CExtDialogFwdCmd(0, pParent) // 0 for template, since we always use Create (since we use different templates)
{
    _pDoc = nullptr;
    _cRows = 0;
    _cColumns = 0;
    _fTransparency = false;
    _fSupportsPlacement = false;
    _fSupportsSize = false;
    _fSupportsFonts = false;
    _fSupportsLoops = false;
    _fSupportsApplyAll = false;
    _fRebuildingTree = false;

    // Load our accelerators
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORRASTERBAR), RT_ACCELERATOR);
	_hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORRASTERBAR));
}

RasterSidePane::~RasterSidePane()
{
    m_font.DeleteObject();
}

BOOL RasterSidePane::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

BEGIN_MESSAGE_MAP(RasterSidePane, CExtDialogFwdCmd)
    ON_CBN_SELCHANGE(IDC_COMBO_MIRROR, OnMirrorSelection)
    ON_CBN_SELCHANGE(IDC_COMBO_PALETTE, OnPaletteSelection)
    ON_EN_KILLFOCUS(IDC_EDIT_CELWIDTH, OnEditCelWidth)
    ON_EN_KILLFOCUS(IDC_EDIT_CELHEIGHT, OnEditCelHeight)
    ON_EN_KILLFOCUS(IDC_EDIT_CELX, OnEditCelX)
    ON_EN_KILLFOCUS(IDC_EDIT_CELY, OnEditCelY)
    ON_EN_KILLFOCUS(IDC_EDIT_LINEHEIGHT, OnEditLineHeight)
    ON_EN_KILLFOCUS(IDC_EDITNUMCHARS, OnEditNumChars)
    ON_EN_UPDATE(IDC_EDIT_SAMPLE, OnEditSampleText)
    ON_COMMAND(IDC_BUTTON_ADDCELBEFORE, OnAddCelBefore)
    ON_COMMAND(IDC_BUTTON_ADDCELAFTER, OnAddCelAfter)
    ON_COMMAND(IDC_BUTTON_DELETECEL, OnDeleteCel)
    ON_COMMAND(IDC_BUTTON_ADDLOOPBEFORE, OnAddLoopBefore)
    ON_COMMAND(IDC_BUTTON_ADDLOOPAFTER, OnAddLoopAfter)
    ON_COMMAND(IDC_BUTTON_DELETELOOP, OnDeleteLoop)
    ON_COMMAND(IDC_CHECKAPPLYTOALL, OnApplyToAll)
    ON_COMMAND(IDC_CHECKISSCALEABLE, OnIsScalable)
    ON_WM_DRAWITEM()
    ON_COMMAND(IDC_BUTTONUP, OnUp)
    ON_COMMAND(IDC_BUTTONDOWN, OnDown)
    ON_COMMAND(IDC_BUTTONRIGHT, OnRight)
    ON_COMMAND(IDC_BUTTONLEFT, OnLeft)
    ON_COMMAND(ID_MAKEFONT, OnMakeFont)
    ON_COMMAND(IDC_BUTTONPENSTYLE, OnPenStyle)
END_MESSAGE_MAP()

void RasterSidePane::_OnUpdateCommandUIs()
{
    // Direction buttons:
    if (_pDoc)
    {
        int nCel = _pDoc->GetSelectedCel();
        int nLoop = _pDoc->GetSelectedLoop();

        const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();

        if (m_wndUp.GetSafeHwnd())
        {
            m_wndUp.EnableWindow(nLoop > 0);
        }
        if (m_wndDown.GetSafeHwnd())
        {
            m_wndDown.EnableWindow(nLoop < (raster.LoopCount() - 1));
        }
        if (m_wndLeft.GetSafeHwnd())
        {
            m_wndLeft.EnableWindow(nCel > 0);
        }
        if (m_wndRight.GetSafeHwnd())
        {
            m_wndRight.EnableWindow(nCel < (raster.CelCount(nLoop) - 1));
        }
    }

    // Apply to all
    if (m_wndApplyToAll.GetSafeHwnd() && _pDoc)
    {
        m_wndApplyToAll.SetCheck(_pDoc->GetApplyToAllCels());
    }

    if (m_wndIsScalable.GetSafeHwnd() && _pDoc)
    {
        const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();
        m_wndIsScalable.EnableWindow(raster.Traits.SupportsScaling);
        m_wndIsScalable.SetCheck(IsScalable(raster));
    }

    if (_pDoc)
    {
        const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();
        if (m_wndDeleteLoop.GetSafeHwnd())
        {
            // We can only delete the loop if there is more than one.
            // REVIEW: this knowledge should belong in the doc (e.g. bool CanDeleteLoop())
            m_wndDeleteLoop.EnableWindow(raster.LoopCount() > 1);
        }

        if (m_wndDeleteCel.GetSafeHwnd())
        {
            int nLoop = _pDoc->GetSelectedLoop();
            m_wndDeleteCel.EnableWindow((raster.CelCount(nLoop) > 1) || (raster.LoopCount() > 1));
        }
    }
}

void RasterSidePane::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (IDC_STATIC_CHOSENCOLORS == nIDCtl)
    {
        // Draw two rectangles.
        if (_pDoc)
        {
            CRect rect(&lpDrawItemStruct->rcItem);

            uint8_t color = _pDoc->GetViewColor();
            uint8_t altColor = _pDoc->GetAlternateViewColor();

            HDC hdc = lpDrawItemStruct->hDC;

            // Erase the background
            CDC dc;
            dc.Attach(hdc);
            m_wndChosenColors.OnEraseBackground(dc, rect);
            dc.Detach();

            CBrush brushFore;
            brushFore.CreateSolidBrush(_pDoc->SCIColorToCOLORREF(color));
            CBrush brushBack;
            brushBack.CreateSolidBrush(_pDoc->SCIColorToCOLORREF(altColor));
            CBrush brushEmpty;
            brushEmpty.CreateSolidBrush(g_PaintManager->GetColor(COLOR_3DFACE));

            int cxThirds = rect.Width() / 3;
            int cyThirds = rect.Height() / 3;
            const CBrush* brushes[3][3] = { { &brushFore, &brushFore, nullptr /*&brushEmpty*/ },
                                            { &brushFore, &brushFore, &brushBack },
                                            { nullptr /*&brushEmpty*/, &brushBack, &brushBack } };

            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (brushes[x][y])
                    {
                        int xx = x * cxThirds;
                        int yy = y * cyThirds;
                        CRect rectThird(rect.left + xx, rect.top + yy, rect.left + xx + cxThirds, rect.top + yy + cyThirds);
                        FillRect(hdc, &rectThird, (HBRUSH)(*(brushes[x][y])));
                    }
                }
            }
        }
    }
    else
    {
        __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
    }
}

void RasterSidePane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CNewRasterResourceDocument*>(pDoc);

    if (_pDoc)
    {
        _pDoc->AddNonViewClient(this);

        _SyncPalette();
        _SyncSampleText();
        _OnPenStyleChanged();
        _SyncCelPane();
        _SyncLoopPane();
        _OnUpdateCommandUIs();
        _UpdatePaletteChoices();

        // It's a bit of a hack to put this here, but let's use this as a clue that the user is interested in this view
        if (_pDoc->GetResource()->GetType() == ResourceType::View)
        {
            appState->SetRecentlyInteractedView(_pDoc->GetResource()->ResourceNumber);
        }
    }
}

void RasterSidePane::_SyncPalette()
{
    if (_pDoc)
    {
        const RasterComponent &raster = _pDoc->GetComponent<RasterComponent>();
        const RGBQUAD *paletteColors = raster.Traits.Palette;
        bool dithered = false;
        switch (raster.Traits.PaletteType)
        {
        case PaletteType::EGA_Two:
            _cRows = 1; _cColumns = 2;
            dithered = false;
            break;
        case PaletteType::EGA_Four:
            _cRows = 2; _cColumns = 2;
            dithered = false;
            break;
        case PaletteType::EGA_Sixteen:
            _cRows = 4; _cColumns = 4;
            dithered = false;
            break;
        case PaletteType::VGA_256:
            _cRows = 16; _cColumns = 16;
            dithered = false;
            paletteColors = _pDoc->GetPaletteVGA();
            break;
        }

        m_wndPalette.SetPalette(_cRows, _cColumns, reinterpret_cast<const EGACOLOR *>(raster.Traits.PaletteMapping), 256, paletteColors, dithered);
        m_wndPalette.OnPaletteUpdated();
        m_wndChosenColors.Invalidate(FALSE);
        m_wndPalette.SetSelection(_pDoc->GetViewColor());
        m_wndPalette.SetAuxSelection(_pDoc->GetAlternateViewColor());
        _UpdatePaletteChoices();
    }
}

const PCSTR pszEmbedded = "Embedded";
const PCSTR pszEmbeddedOnly = "Embedded only";

void RasterSidePane::_UpdatePaletteChoices()
{
    if (_pDoc)
    {
        if (m_wndPaletteChoice.m_hWnd)
        {
            const RasterComponent &raster = _pDoc->GetComponent<RasterComponent>();
            if (raster.Traits.PaletteType == PaletteType::VGA_256)
            {
                m_wndPaletteChoice.SetRedraw(FALSE);
                m_wndPaletteChoice.ResetContent();
                std::vector<int> &paletteChoices = _pDoc->GetPaletteChoices();
                for (int index = 0; index < (int)paletteChoices.size(); index++)
                {
                    m_wndPaletteChoice.AddString(_pDoc->GetPaletteChoiceName(index).c_str());
                }
                m_wndPaletteChoice.SetCurSel(_pDoc->GetPaletteChoice());
                m_wndPaletteChoice.ShowWindow(SW_SHOW);
                m_wndStatic9.ShowWindow(SW_SHOW);
                m_wndEditPaletteButton.ShowWindow(SW_SHOW);
                m_wndPaletteChoice.SetRedraw(TRUE);
            }
            else
            {
                m_wndPaletteChoice.ShowWindow(SW_HIDE);
                m_wndStatic9.ShowWindow(SW_HIDE);
                m_wndEditPaletteButton.ShowWindow(SW_HIDE);
            }

            OnPaletteSelection();
        }
    }
}

void RasterSidePane::OnPaletteSelection()
{
    if (_pDoc)
    {
        int curSel = m_wndPaletteChoice.GetCurSel();
        if (curSel != CB_ERR)
        {
            _pDoc->SetPaletteChoice(curSel, false);
        }
    }
}

void RasterSidePane::OnColorClick(BYTE bIndex, int nID, BOOL fLeft)
{
    if (_pDoc)
    {
        assert((int)bIndex < (_cRows * _cColumns)); // Don't overflow our array.
        const RasterComponent &raster = _pDoc->GetComponent<RasterComponent>();
        uint8_t mappedColorIndex = raster.Traits.PaletteMapping[bIndex];

        if (fLeft)
        {
            // Foreground colour.
            _pDoc->SetViewColor(mappedColorIndex);
        }
        else
        {
            // Background colour.
            _pDoc->SetAlternateViewColor(mappedColorIndex);
        }
    }
}

// IDialogFactory
void RasterSidePane::InvokeDialog(UINT nID, RECT *prcTarget)
{
    assert(nID == IDC_BUTTON_TRANSPARENCY);
    if (_pDoc)
    {
        const RasterComponent &raster = _pDoc->GetComponent<RasterComponent>();
        const Cel &cel = raster.GetCel(_pDoc->GetSelectedIndex());
        uint8_t bColor = cel.TransparentColor;
        CChoosePriConDialog dialog(_pDoc->GetCurrentPaletteComponent());
        dialog.SetCaptionID(IDS_CHOOSEATRANSPARENCY);
        dialog.SetColor(bColor);
        dialog.SetTrackRect(prcTarget);
        if (IDOK == dialog.DoModal())
        {
            bColor = dialog.GetColor();

            CelIndex rgdwIndices[128];
            int cCels = _pDoc->GetSelectedGroup(rgdwIndices, ARRAYSIZE(rgdwIndices));
            if (cCels > 0)
            {
                ResourceEntityDocument *pred = static_cast<ResourceEntityDocument*>(_pDoc);
                pred->ApplyChanges<RasterComponent>(
                    [&](RasterComponent &raster)
                    {
                        return WrapRasterChange(SetGroupTransparentColor(raster, cCels, rgdwIndices, bColor));
                    }
                    );
            }
        }
    }
}

void RasterSidePane::OnMirrorSelection()
{
    ASSERT(_fSupportsLoops);
    int iSel = m_wndMirrorOf.GetCurSel();
    if (_pDoc)
    {
        int iMirrorOf = (int)m_wndMirrorOf.GetItemData(iSel);
        int nLoop = _pDoc->GetSelectedLoop();

        _pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
            {
                return WrapRasterChange(MakeMirrorOf(raster, nLoop, iMirrorOf));
            }
            );
    }
}


int _ValidateSize2(UINT nID, int nSize)
{
    if (nSize < 1)
    {
        return 1;
    }
    else if ((nID == IDC_EDIT_CELWIDTH) && (nSize > 320))
    {
        return 320;
    }
    else if ((nID == IDC_EDIT_CELHEIGHT) && (nSize > 190))
    {
        return 190;
    }
    else
    {
        return nSize;
    }

}

void RasterSidePane::_OnEditSize(UINT nID, CWnd *pwnd)
{
    if (_pDoc)
    {
        const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();
        const Cel &cel = raster.GetCel(_pDoc->GetSelectedIndex());

        CSize size = SizeToCSize(cel.size);
        CPoint point = PointToCPoint(cel.placement);

        CString strNumber;
        pwnd->GetWindowText(strNumber);
        int iNew = StrToInt(strNumber);
        int iNewValidated = iNew;
        if ((nID == IDC_EDIT_CELWIDTH) || (nID == IDC_EDIT_CELHEIGHT))
        {
            iNewValidated = _ValidateSize2(nID, iNew);
        }
        // Placement is validated at the data level, since it's different for different guys

        if (iNew != iNewValidated)
        {
            iNew = iNewValidated;
            std::stringstream ss;
            ss << iNew;
            pwnd->SetWindowText(ss.str().c_str());
        }

        ResourceEntityDocument *pred = static_cast<ResourceEntityDocument*>(_pDoc);
        CelIndex rgdwIndices[128];
        int cCels = _pDoc->GetSelectedGroup(rgdwIndices, ARRAYSIZE(rgdwIndices));
        if (cCels > 0)
        {
            bool fDoIt = false;
            size16 sizes[128];
            switch(nID)
            {
            case IDC_EDIT_CELWIDTH:
                if (size.cx != iNew)
                {
                    fDoIt = true;
                    sizes[0] = size16((uint16_t)iNew, (uint16_t)size.cy);
                }
                break;
            case IDC_EDIT_CELHEIGHT:
                if (size.cy != iNew)
                {
                    fDoIt = true;
                    sizes[0] = size16((uint16_t)size.cx, (uint16_t)iNew);
                }
                break;
            case IDC_EDIT_CELX:
            case IDC_EDIT_CELY:
                if (((point.x != iNew) && (nID == IDC_EDIT_CELX)) ||
                    ((point.y != iNew) && (nID == IDC_EDIT_CELY)))
                {
                    pred->ApplyChanges<RasterComponent>(
                        [&](RasterComponent &raster)
                    {
                        if (nID == IDC_EDIT_CELY)
                        {
                            return WrapRasterChange(SetGroupPlacement(raster, cCels, rgdwIndices, (int16_t)point.x, (int16_t)iNew));
                        }
                        else
                        {
                            return WrapRasterChange(SetGroupPlacement(raster, cCels, rgdwIndices, (int16_t)iNew, (int16_t)point.y));
                        }
                    }
                    );
                }
                break;
            }

            if (fDoIt)
            {
                for (int i = 1; i < cCels; i++)
                {
                    sizes[i] = sizes[0];
                }

                pred->ApplyChanges<RasterComponent>(
                    [&](RasterComponent &raster)
                {
                    return WrapRasterChange(SetGroupSize(raster, cCels, rgdwIndices, sizes, RasterResizeFlags::Normal));
                }
                );
            }
        }
    }
}

void RasterSidePane::OnEditCelWidth() { _OnEditSize(IDC_EDIT_CELWIDTH, &m_wndCelWidth); }
void RasterSidePane::OnEditCelHeight() { _OnEditSize(IDC_EDIT_CELHEIGHT, &m_wndCelHeight); }
void RasterSidePane::OnEditCelX() { _OnEditSize(IDC_EDIT_CELX, &m_wndCelX); }
void RasterSidePane::OnEditCelY() { _OnEditSize(IDC_EDIT_CELY, &m_wndCelY); }

void RasterSidePane::OnEditNumChars()
{
    if (_pDoc && _fSupportsFonts)
    {
        CString strNumber;
        m_wndNumChars.GetWindowText(strNumber);
        uint16_t wNew = (uint16_t)StrToInt(strNumber);

        // Clip to between 1 and 255 (do all SCI interpreters handle extended char sets?)
        const FontComponent &fontComponent = static_cast<CNewRasterResourceDocument*>(_pDoc)->GetResource()->GetComponent<FontComponent>();
        wNew = fontComponent.Traits.ValidateCharCount(wNew);

        _SyncCelPane(); // In case we changed wNew

        _pDoc->ApplyChanges<RasterComponent>(
            [wNew](RasterComponent &raster)
        {
            Loop &loop = raster.Loops[0];
            RasterChangeHint hint = RasterChangeHint::None;
            while (loop.Cels.size() != wNew)
            {
                hint = RasterChangeHint::NewView;
                if (loop.Cels.size() > wNew)
                {
                    RemoveCel(raster, CelIndex(0, loop.Cels.size() - 1));
                }
                else
                {
                    InsertCel(raster, CelIndex(0, loop.Cels.size() - 1), false, false);
                }
            }
            return WrapHint(hint);
        }
        );
    }
}

void RasterSidePane::OnEditLineHeight()
{
    if (_pDoc && _fSupportsFonts)
    {
        CString strNumber;
        m_wndLineHeight.GetWindowText(strNumber);
        uint16_t wNew = (uint16_t)StrToInt(strNumber);
        // Clip to between 1 and 127 (don't know the real limits, but those seem more than reasonable)
        wNew = max(wNew, 1);
        wNew = min(wNew, 127);

        _pDoc->ApplyChanges<FontComponent>(
            [&](FontComponent &font)
        {
            font.LineHeight = wNew;
            return WrapHint(RasterChangeHint::NewView);
        }
        );
    }
}

void RasterSidePane::OnEditSampleText()
{
    if (_pDoc && _fSupportsFonts)
    {
        CString strSample;
        m_wndSampleText.GetWindowText(strSample);
        std::string temp = (PCSTR)strSample;
        _pDoc->SetPreviewLetters(temp);
    }
}

void RasterSidePane::UpdateNonView(CObject *pObject)
{
    RasterChangeHint hint = GetHint<RasterChangeHint>(pObject);

    // Take care of color first, since that is a lightweight operation.
    if (IsFlagSet(hint, RasterChangeHint::Color))
    {
        m_wndChosenColors.Invalidate(FALSE);
        if (_pDoc)
        {
            m_wndPalette.SetSelection(_pDoc->GetViewColor());
            m_wndPalette.SetAuxSelection(_pDoc->GetAlternateViewColor());
        }
        ClearFlag(hint, RasterChangeHint::Color);
    }

    if (IsFlagSet(hint, RasterChangeHint::PaletteChoice))
    {
        _SyncPalette();
        ClearFlag(hint, RasterChangeHint::PaletteChoice);
    }

    if (IsFlagSet(hint, RasterChangeHint::PenStyle))
    {
        _OnPenStyleChanged();
        ClearFlag(hint, RasterChangeHint::PenStyle);
    }

    if (IsFlagSet(hint, RasterChangeHint::SampleText))
    {
        _SyncSampleText();
        ClearFlag(hint, RasterChangeHint::SampleText);
    }

    if (IsFlagSet(hint, RasterChangeHint::NewView | RasterChangeHint::Loop))
    {
        // Don't remove RasterChangeHint::NewView, because we still want
        // to do other things if its present, below.
        // Yeah, this too:
        _UpdatePaletteChoices();
    }

    if (hint != RasterChangeHint::None)
    {
        _OnPenStyleChanged();
        _SyncCelPane();
        _SyncLoopPane();
        _OnUpdateCommandUIs(); // TODO: could be more efficient..
    }
}


void RasterSidePane::_SyncLoopPane()
{
    _SyncMirror();

    if (_pDoc)
    {
        const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();
        int nLoop = _pDoc->GetSelectedLoop();
        TCHAR szMsg[MAX_PATH];
        StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("Loop %d/%d    (%d cels)"), nLoop, raster.LoopCount() - 1, raster.CelCount(nLoop));
        SetDlgItemText(IDC_STATIC_LOOPGROUP, szMsg);
    }
}

void RasterSidePane::_SyncMirror()
{
    if (_fSupportsLoops)
    {
        if (_pDoc)
        {
            const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();
            m_wndMirrorOf.SetRedraw(FALSE);
            int nLoop = _pDoc->GetSelectedLoop();
            // Redo the combobox.
            m_wndMirrorOf.ResetContent();
            int iNoneIndex = m_wndMirrorOf.AddString(TEXT("None"));
            m_wndMirrorOf.SetItemData(iNoneIndex, -1);
            const Loop &loop = raster.Loops[nLoop];
            int iCurrentMirrorOf = loop.GetMirrorOf();
            int cLoops = raster.LoopCount();
            BOOL fEnableCombo = TRUE;
            for (int i = 0; i < cLoops; i++)
            {
                int iMirrorOf = raster.Loops[i].GetMirrorOf();
                int iIndex = 0;
                if ((iMirrorOf == -1) && (i != nLoop))
                {
                    // This one is not a mirror, and it's not ourselves.  Thus, it is a candidate for being listed in the combobox.
                    std::stringstream ss;
                    ss << "Loop " << i;
                    iIndex = m_wndMirrorOf.AddString(ss.str().c_str());
                    m_wndMirrorOf.SetItemData(iIndex, i);
                }

                if (iMirrorOf == nLoop)
                {
                    // Someone is a mirror of us.  So we should not offer a choice to make us a mirror of anyone.
                    fEnableCombo = FALSE;
                }

                if (iCurrentMirrorOf == i)
                {
                    // Select this one.
                    m_wndMirrorOf.SetCurSel(iIndex);
                }
            }
            if (iCurrentMirrorOf == -1)
            {
                m_wndMirrorOf.SetCurSel(iNoneIndex);
            }
            m_wndMirrorOf.EnableWindow(fEnableCombo);
            m_wndStaticMirrorOf.EnableWindow(fEnableCombo);
            m_wndMirrorOf.SetRedraw(TRUE);
            m_wndMirrorOf.Invalidate();
        }
    }
}

void RasterSidePane::_SyncSampleText()
{
    // Don't set the text if its the same
    if (_pDoc && _fSupportsFonts)
    {
        CString strSampleCurrent;
        m_wndSampleText.GetWindowText(strSampleCurrent);
        CString strSampleNew = static_cast<CNewRasterResourceDocument*>(_pDoc)->GetPreviewLetters().c_str();
        if (0 != strSampleNew.Compare(strSampleCurrent))
        {
            m_wndSampleText.SetWindowText(strSampleNew);
        }
    }
}

void RasterSidePane::_SyncCelPane()
{
    if (_pDoc)
    {
        if (GetDlgItem(IDC_STATIC_CELGROUP))
        {
            int nCel = _pDoc->GetSelectedCel();
            std::stringstream ss;
            ss << "Cel: " << nCel;
            SetDlgItemText(IDC_STATIC_CELGROUP, ss.str().c_str());
        }

        const RasterComponent &raster = _pDoc->GetResource()->GetComponent<RasterComponent>();
        const Cel &cel = raster.GetCel(_pDoc->GetSelectedIndex());

        if (_fSupportsSize)
        {
            CSize size = SizeToCSize(cel.size);
            std::stringstream ss;
            ss << size.cx;
            m_wndCelWidth.SetWindowText(ss.str().c_str());
            std::stringstream ss2;
            ss2 << size.cy;
            m_wndCelHeight.SetWindowText(ss2.str().c_str());
        }

        if (_fSupportsPlacement)
        {
            CPoint pt = PointToCPoint(cel.placement);
            std::stringstream ss;
            ss << pt.x;
            m_wndCelX.SetWindowText(ss.str().c_str());
            std::stringstream ss2;
            ss2 << pt.y;
            m_wndCelY.SetWindowText(ss2.str().c_str());
        }

        if (_fTransparency)
        {
            uint8_t bTransparent = cel.TransparentColor;
            const PaletteComponent *optionalPalette = _pDoc->GetCurrentPaletteComponent();
            if (optionalPalette)
            {
                m_wndTransparent.SetColorAndState(optionalPalette->Colors[bTransparent], TRUE);
            }
            else
            {
                EGACOLOR color = { bTransparent, bTransparent };
                m_wndTransparent.SetColorAndState(color, TRUE);
            }
            m_wndPalette.SetTransparentIndex(bTransparent);
        }

        // Font line height and number of chars
        const FontComponent *fontComponent = static_cast<CNewRasterResourceDocument*>(_pDoc)->GetResource()->TryGetComponent<FontComponent>();
        if (fontComponent)
        {
            assert(_fSupportsFonts);
            m_wndLineHeight.SetWindowText(fmt::format("{0}", (int)fontComponent->LineHeight).c_str());
            m_wndNumChars.SetWindowText(fmt::format("{0}", (int)raster.Loops[0].Cels.size()).c_str());
        }
    }
}

void RasterSidePane::OnApplyToAll()
{
    if (_pDoc)
    {
        _pDoc->SetApplyToAllCels(m_wndApplyToAll.GetCheck());
    }
}

void RasterSidePane::OnIsScalable()
{
    if (_pDoc)
    {
        _pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
        {
            return WrapRasterChange(SetScalable(raster, m_wndIsScalable.GetCheck() != 0));
        }
        );
    }
}

void RasterSidePane::_OnPenStyleChanged()
{
    if (_pDoc)
    {
        m_wndButtonPenStyle.SetPenStyle(_pDoc->GetPenStyle());
    }
}

// RasterSidePane message handlers

void RasterSidePane::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	__super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    if (GetDlgItem(IDC_EDIT_CELWIDTH))
    {
	    DDX_Control(pDX, IDC_EDIT_CELWIDTH, m_wndCelWidth);
        DDX_Control(pDX, IDC_EDIT_CELHEIGHT, m_wndCelHeight);
        AddAnchor(IDC_EDIT_CELWIDTH, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_EDIT_CELHEIGHT, CPoint(100, 0), CPoint(100, 0));
        _fSupportsSize = true;
    }
    if (GetDlgItem(IDC_EDIT_CELX))
    {
        DDX_Control(pDX, IDC_EDIT_CELX, m_wndCelX);
	    DDX_Control(pDX, IDC_EDIT_CELY, m_wndCelY);
        AddAnchor(IDC_EDIT_CELX, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_EDIT_CELY, CPoint(100, 0), CPoint(100, 0));
        _fSupportsPlacement = true;
    }
    if (GetDlgItem(IDC_BUTTON_TRANSPARENCY))
    {
        DDX_Control(pDX, IDC_BUTTON_TRANSPARENCY, m_wndTransparent);
        AddAnchor(IDC_BUTTON_TRANSPARENCY, CPoint(100, 0), CPoint(100, 0));
        m_wndTransparent.SetDialogFactory(this);
        _fTransparency = true;
    }
    if (GetDlgItem(IDC_EDIT_LINEHEIGHT))
    {
        DDX_Control(pDX, IDC_EDIT_LINEHEIGHT, m_wndLineHeight);
        DDX_Control(pDX, IDC_EDIT_SAMPLE, m_wndSampleText);
        DDX_Control(pDX, IDC_EDITNUMCHARS, m_wndNumChars);
        AddAnchor(IDC_EDIT_LINEHEIGHT, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_EDIT_SAMPLE, CPoint(0, 0), CPoint(100, 0));
        AddAnchor(IDC_EDITNUMCHARS, CPoint(100, 0), CPoint(100, 0));
        _fSupportsFonts = true;
    }
    if (GetDlgItem(IDC_COMBO_MIRROR))
    {
        DDX_Control(pDX, IDC_COMBO_MIRROR, m_wndMirrorOf);
        DDX_Control(pDX, IDC_STATIC_MIRROROF, m_wndStaticMirrorOf);
        AddAnchor(IDC_STATIC_MIRROROF, CPoint(0, 0), CPoint(0, 0));
        AddAnchor(IDC_COMBO_MIRROR, CPoint(0, 0), CPoint(100, 0));
        _fSupportsLoops = true;
    }
    if (GetDlgItem(IDC_CHECKAPPLYTOALL))
    {
        DDX_Control(pDX, IDC_CHECKAPPLYTOALL, m_wndApplyToAll);
        AddAnchor(IDC_CHECKAPPLYTOALL, CPoint(0, 0), CPoint(100, 0));
        _fSupportsApplyAll = true;
    }
    if (GetDlgItem(IDC_CHECKISSCALABLE))
    {
        DDX_Control(pDX, IDC_CHECKISSCALABLE, m_wndIsScalable);
        AddAnchor(IDC_CHECKISSCALABLE, CPoint(0, 0), CPoint(100, 0));
    }

    // Prepare the palette - a 4x4 grid of colours - we'll set this up later
    DDX_Control(pDX, IDC_STATIC_PALETTE, m_wndPalette);
    AddAnchor(IDC_STATIC_PALETTE, CPoint(0, 0), CPoint(100, 0));
    m_wndPalette.SetPalette(0, 0, nullptr, 0, nullptr);
    m_wndPalette.SetCallback(this);
    m_wndPalette.SetShowHover(false);
    m_wndPalette.ShowFocusBoxes(true);
    m_wndPalette.SetShowTransparentIndex(_fTransparency);

    DDX_Control(pDX, IDC_STATIC_CHOSENCOLORS, m_wndChosenColors);
    AddAnchor(IDC_STATIC_CHOSENCOLORS, CPoint(0, 0), CPoint(0, 0));

    // Prepare the VGA palette
    if (GetDlgItem(IDC_COMBO_PALETTE))
    {
        DDX_Control(pDX, IDC_COMBO_PALETTE, m_wndPaletteChoice);
        AddAnchor(IDC_COMBO_PALETTE, CPoint(0, 0), CPoint(100, 0));

        DDX_Control(pDX, ID_VIEW_EDITPALETTE, m_wndEditPaletteButton);
        m_wndEditPaletteButton.SetIcon(IDI_EDITPALETTE, 0, 0, 0, 24, 24);
        AddAnchor(ID_VIEW_EDITPALETTE, CPoint(100, 0), CPoint(100, 0));
    }

    // Prepare the left, right, up, down buttons, by applying a wingdings font.
    CFont *pFont = GetFont();
    LOGFONT logFont = { 0 };
    logFont.lfHeight = -18;
    logFont.lfCharSet = SYMBOL_CHARSET;
    logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
    logFont.lfWeight = FW_NORMAL;
    m_font;
    StringCchCopy(logFont.lfFaceName, ARRAYSIZE(logFont.lfFaceName), TEXT("WingDings"));
    m_font.CreateFontIndirect(&logFont);
    if (GetDlgItem(IDC_BUTTONUP))
    {
        DDX_Control(pDX, IDC_BUTTONUP, m_wndUp);
        DDX_Control(pDX, IDC_BUTTONDOWN, m_wndDown);
        AddAnchor(IDC_BUTTONUP, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_BUTTONDOWN, CPoint(100, 0), CPoint(100, 0));
        m_wndUp.SetFont(&m_font);
        m_wndDown.SetFont(&m_font);
        m_wndUp.SetDrawFocusRect(FALSE);
        m_wndDown.SetDrawFocusRect(FALSE);
    }
    if (GetDlgItem(IDC_BUTTONLEFT))
    {
        DDX_Control(pDX, IDC_BUTTONLEFT, m_wndLeft);
        DDX_Control(pDX, IDC_BUTTONRIGHT, m_wndRight);
        AddAnchor(IDC_BUTTONLEFT, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_BUTTONRIGHT, CPoint(100, 0), CPoint(100, 0));
        m_wndLeft.SetFont(&m_font);
        m_wndRight.SetFont(&m_font);
        m_wndLeft.SetDrawFocusRect(FALSE);
        m_wndRight.SetDrawFocusRect(FALSE);
    }

    // Other visuals...
    if (GetDlgItem(IDC_BUTTON_ADDCELBEFORE))
    {
        DDX_Control(pDX, IDC_BUTTON_ADDCELBEFORE, m_wndCelBefore);
        DDX_Control(pDX, IDC_BUTTON_ADDCELAFTER, m_wndCelAfter);
        DDX_Control(pDX, IDC_BUTTON_DELETECEL, m_wndDeleteCel);
        AddAnchor(IDC_BUTTON_ADDCELBEFORE, CPoint(0, 0), CPoint(50, 0));
        AddAnchor(IDC_BUTTON_ADDCELAFTER, CPoint(0, 0), CPoint(50, 0));
        AddAnchor(IDC_BUTTON_DELETECEL, CPoint(50, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_BUTTON_ADDLOOPBEFORE))
    {
        DDX_Control(pDX, IDC_BUTTON_ADDLOOPBEFORE, m_wndLoopBefore);
        DDX_Control(pDX, IDC_BUTTON_ADDLOOPAFTER, m_wndLoopAfter);
        DDX_Control(pDX, IDC_BUTTON_DELETELOOP, m_wndDeleteLoop);
        AddAnchor(IDC_BUTTON_ADDLOOPBEFORE, CPoint(0, 0), CPoint(50, 0));
        AddAnchor(IDC_BUTTON_ADDLOOPAFTER, CPoint(0, 0), CPoint(50, 0));
        AddAnchor(IDC_BUTTON_DELETELOOP, CPoint(50, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC1))
    {
        DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
        AddAnchor(IDC_STATIC1, CPoint(100, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC2))
    {
        DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
        AddAnchor(IDC_STATIC2, CPoint(0, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC3))
    {
        DDX_Control(pDX, IDC_STATIC3, m_wndStatic3);
        AddAnchor(IDC_STATIC3, CPoint(0, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC4))
    {
        DDX_Control(pDX, IDC_STATIC4, m_wndStatic4);
        AddAnchor(IDC_STATIC4, CPoint(0, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC5))
    {
        DDX_Control(pDX, IDC_STATIC5, m_wndStatic5);
        AddAnchor(IDC_STATIC5, CPoint(100, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC6))
    {
        DDX_Control(pDX, IDC_STATIC6, m_wndStatic6);
        AddAnchor(IDC_STATIC6, CPoint(100, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC7))
    {
        DDX_Control(pDX, IDC_STATIC7, m_wndStatic7);
        AddAnchor(IDC_STATIC7, CPoint(100, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC8))
    {
        DDX_Control(pDX, IDC_STATIC8, m_wndStatic8);
        AddAnchor(IDC_STATIC8, CPoint(0, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC9))
    {
        DDX_Control(pDX, IDC_STATIC9, m_wndStatic9);
        AddAnchor(IDC_STATIC9, CPoint(0, 0), CPoint(0, 0));
    }
    if (GetDlgItem(IDC_STATIC_CELGROUP))
    {
        DDX_Control(pDX, IDC_STATIC_CELGROUP, m_wndGroup1);
        AddAnchor(IDC_STATIC_CELGROUP, CPoint(0, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_STATIC_LOOPGROUP))
    {
        DDX_Control(pDX, IDC_STATIC_LOOPGROUP, m_wndGroup2);
        AddAnchor(IDC_STATIC_LOOPGROUP, CPoint(0, 0), CPoint(100, 0));
    }
    if (GetDlgItem(ID_MAKEFONT))
    {
        DDX_Control(pDX, ID_MAKEFONT, m_wndMakeFont);
        AddAnchor(ID_MAKEFONT, CPoint(100, 0), CPoint(100, 0));
    }
    if (GetDlgItem(IDC_BUTTONPENSTYLE))
    {
        DDX_Control(pDX, IDC_BUTTONPENSTYLE, m_wndButtonPenStyle);
        AddAnchor(IDC_BUTTONPENSTYLE, CPoint(100, 0), CPoint(100, 0));
    }
}

void RasterSidePane::_OnAddCel(bool before)
{
    if (_pDoc)
    {
        CelIndex curSel = _pDoc->GetSelectedIndex();
        _pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
        {
            return WrapRasterChange(InsertCel(raster, _pDoc->GetSelectedIndex(), before, !!appState->_fDupeNewCels));
        }
        );
        if (!before)
        {
            // Move selection to "after".
            _pDoc->SetSelectedCel(curSel.cel + 1);
        }
    }
}

void RasterSidePane::OnAddCelBefore()
{
    _OnAddCel(true);
}

void RasterSidePane::OnAddCelAfter()
{
    _OnAddCel(false);
}

void RasterSidePane::OnDeleteCel()
{
    if (_pDoc)
    {
        _pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
        {
            return WrapRasterChange(RemoveCel(raster, _pDoc->GetSelectedIndex()));
        }
        );
    }
}

void RasterSidePane::_OnAddLoop(bool before)
{
    if (_pDoc)
    {
        CelIndex curSel = _pDoc->GetSelectedIndex();
        _pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
        {
            return WrapRasterChange(InsertLoop(raster, _pDoc->GetSelectedLoop(), before));
        }
        );
        if (!before)
        {
            _pDoc->SetSelectedLoop(curSel.loop + 1);
        }
    }
}

void RasterSidePane::OnAddLoopBefore()
{
    _OnAddLoop(true);
}

void RasterSidePane::OnAddLoopAfter()
{
    _OnAddLoop(false);
}

void RasterSidePane::OnDeleteLoop()
{
    if (_pDoc)
    {
        _pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
        {
            return WrapRasterChange(RemoveLoop(raster, _pDoc->GetSelectedLoop()));
        }
        );
    }
}

void RasterSidePane::_OnDirectionButton(CPoint point)
{
    if (_pDoc)
    {
        if ((point.x != 0) || (point.y != 0))
        {
            _pDoc->MoveSelectedCel(point);
        }
    }
}

void RasterSidePane::OnUp()
{
    _OnDirectionButton(CPoint(0, -1));
}
void RasterSidePane::OnDown()
{
    _OnDirectionButton(CPoint(0, 1));
}
void RasterSidePane::OnRight()
{
    _OnDirectionButton(CPoint(1, 0));
}
void RasterSidePane::OnLeft()
{
    _OnDirectionButton(CPoint(-1, 0));
}
void RasterSidePane::OnMakeFont()
{
    if (_pDoc)
    {
        _pDoc->MakeFont();
    }
}
void RasterSidePane::OnPenStyle()
{
    if (_pDoc)
    {
        CChoosePenStyleDialog dialog;
        PenStyle penStyle = _pDoc->GetPenStyle();
        dialog.SetPenStyle(&penStyle);
        CRect rect;
        m_wndButtonPenStyle.GetWindowRect(&rect);
        dialog.SetTrackRect(&rect);
        if (IDOK == dialog.DoModal())
        {
            dialog.GetPenStyle(&penStyle);
            _pDoc->SetPenStyle(penStyle);
        }
    }
}

