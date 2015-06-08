#include "stdafx.h"
#include "PaletteEditorDialog.h"
#include "ExtColorDlg.h"
#include "PaletteOperations.h"
#include "RasterOperations.h"
#include "ImageUtil.h"
#include "ResourceDocument.h"
#include "ResourceEntity.h"

#define PALETTE_TIMER 6789

PaletteEditorDialog::PaletteEditorDialog(IVGAPaletteDefinitionCallback *callback, PaletteComponent &palette, const std::vector<const Cel*> &cels)
    : PaletteEditorCommon<CExtResizableDialog>(callback, PaletteEditorDialog::IDD, nullptr), _cycleForward(true), _cycling(false), _initialized(false)
{
    Init(palette, cels);
}

// INonViewClient
void PaletteEditorDialog::UpdateNonView(CObject *pObject)
{
    UpdateCommon(pObject);
}

void PaletteEditorDialog::_SyncSelectionCycle()
{
    std::vector<std::pair<uint8_t, uint8_t>> ranges = GetSelectedRanges();
    bool canCycle = _IsSingleRangeSelected(ranges);
    if (canCycle)
    {
        // If the user changed the selection while we were cycling, then act as if we toggled it off and on.
        // Otherwise, we'll cycle the wrong things.
        if (_cycling)
        {
            OnBnClickedCheckpreviewcycling();
            OnBnClickedCheckpreviewcycling();
        }
    }
    else
    {
        if (_cycling)
        {
            OnBnClickedCheckpreviewcycling();
        }
    }
    m_wndPreviewCycling.EnableWindow(canCycle);
    m_wndCycleLeft.EnableWindow(canCycle);
    m_wndCycleRight.EnableWindow(canCycle);

    _SyncCheckState();
}

void PaletteEditorDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    __super::OnColorClick(bIndex, nID, fLeftClick);
    _SyncSelectionCycle();
}

void PaletteEditorDialog::OnCancel()
{
    if (_cycling)
    {
        OnBnClickedCheckpreviewcycling();
    }
    __super::OnCancel();
}

void PaletteEditorDialog::OnOK()
{
    if (_cycling)
    {
        OnBnClickedCheckpreviewcycling();
    }
    __super::OnOK();
}

void PaletteEditorDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    if (!_initialized)
    {
        DDX_Control(pDX, IDC_CHECKPREVIEWCYCLING, m_wndPreviewCycling);
        DDX_Control(pDX, IDC_BUTTONCYCLELEFT, m_wndCycleLeft);
        DDX_Control(pDX, IDC_BUTTONCYCLERIGHT, m_wndCycleRight);
        _UpdateCycleUI();

        //SetTimer(PALETTE_TIMER, 17, nullptr); // 16.666 (60 FPS). This seems to match the game clock.
        SetTimer(PALETTE_TIMER, 8, nullptr);    // 8 (120 FPS). This seems to match the game clock. Or am I doing something wrong?

        _SyncSelectionCycle();
    }
}

void PaletteEditorDialog::_SyncCheckState()
{
    m_wndPreviewCycling.SetCheck(_cycling);
}

void PaletteEditorDialog::_UpdateCycleUI()
{
    m_wndCycleLeft.SetCheck(!_cycleForward);
    m_wndCycleRight.SetCheck(_cycleForward);
}

BEGIN_MESSAGE_MAP(PaletteEditorDialog, PaletteEditorCommon)
    ON_BN_CLICKED(IDC_CHECKPREVIEWCYCLING, &PaletteEditorDialog::OnBnClickedCheckpreviewcycling)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTONCYCLELEFT, &PaletteEditorDialog::OnBnClickedButtoncycleleft)
    ON_BN_CLICKED(IDC_BUTTONCYCLERIGHT, &PaletteEditorDialog::OnBnClickedButtoncycleright)
END_MESSAGE_MAP()

void PaletteEditorDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == PALETTE_TIMER)
    {
        if (_cycling)
        {
            std::vector<std::pair<uint8_t, uint8_t>> ranges = GetSelectedRanges();
            if (_IsSingleRangeSelected(ranges))
            {
                int offset = _cycleForward ? 1 : -1;
                PaletteComponent prev = *_palette;
                for (int i = ranges[0].first; i <= (int)ranges[0].second; i++)
                {
                    int oldIndex = i + offset;
                    if (oldIndex < (int)ranges[0].first)
                    {
                        oldIndex = ranges[0].second;
                    }
                    if (oldIndex > (int)ranges[0].second)
                    {
                        oldIndex = ranges[0].first;
                    }
                    _palette->Colors[i] = prev.Colors[oldIndex];
                }

                if (_callback)
                {
                    _callback->OnVGAPaletteChanged();
                }
            }
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void PaletteEditorDialog::OnBnClickedCheckpreviewcycling()
{
    _cycling = !_cycling;
    if (_cycling)
    {
        // Make a copy of the palette so we can restore it
        _cyclePaletteCopy = std::make_unique<PaletteComponent>(*_palette);
    }
    else
    {
        // Restore the palette
        *_palette = *_cyclePaletteCopy;
    }
    if (_callback)
    {
        _callback->OnVGAPaletteChanged();
    }
}


void PaletteEditorDialog::OnBnClickedButtoncycleleft()
{
    _cycleForward = false;
}


void PaletteEditorDialog::OnBnClickedButtoncycleright()
{
    _cycleForward = true;
}
