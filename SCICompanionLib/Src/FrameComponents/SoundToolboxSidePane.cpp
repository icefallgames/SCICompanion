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

// SoundToolboxSidePane.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SoundToolboxSidePane.h"
#include "SoundDoc.h"
#include "SoundUtil.h"

std::vector<SoundEvent> g_emptyEvents;

SoundToolboxSidePane::SoundToolboxSidePane(CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(SoundToolboxSidePane::IDD, pParent), m_wndList(_accumulatedTicksPerEvent)
{
    _pDoc = nullptr;
    _fAttached = FALSE;
    _iUserSelectedPos = LB_ERR;
    _iCurrentPicPos = LB_ERR;
    // Load our accelerators
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS), RT_ACCELERATOR);
	_hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS));
}

SoundToolboxSidePane::~SoundToolboxSidePane()
{
}

const SoundComponent *SoundToolboxSidePane::GetSound()
{
    const SoundComponent *pComponent = nullptr;
    if (_pDoc)
    {
        const ResourceEntity *pResource = _pDoc->GetResource();
        if (pResource)
        {
            pComponent = _pDoc->GetResource()->TryGetComponent<SoundComponent>();
        }
    }
    return pComponent;
}

//
// To properly handle commands we do two things here:
// 1) Process our own accelerators
//      - The main frame's accelerator table doesn't get used in dialogs in control bars,
//        so we need to do our own.  So, e.g. Ctrl-Z becomes ID_EDIT_UNDO
// 2) Fwd any commands we receive to our frame
//      if we don't handle them ourselves.
//      - they don't bubble up naturally in dialogs in control bars.
//      - we do this by inheriting from CExtDialogFwdCmd
//
BOOL SoundToolboxSidePane::PreTranslateMessage(MSG* pMsg)
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

void SoundToolboxSidePane::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    // Visuals
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
    DDX_Control(pDX, IDC_STATIC3, m_wndStatic3);
    DDX_Control(pDX, IDC_STATIC4, m_wndStatic4);
    DDX_Control(pDX, IDC_STATIC5, m_wndStatic5);

    DDX_Control(pDX, IDC_EDIT_TIME, m_wndEditTime);
    DDX_Control(pDX, IDC_EDIT_VALUE, m_wndEditValue);
    DDX_Control(pDX, IDC_BUTTON_ADDCUE, m_wndAdd);
    DDX_Control(pDX, IDC_BUTTON_DELETECUE, m_wndDelete);
    DDX_Control(pDX, IDC_LISTCOMMANDS, m_wndList);
    DDX_Control(pDX, IDC_COMBODEVICE, m_wndDevices);
    DDX_Control(pDX, IDC_CHECK_ALLCHANNELS, m_wndCheckAllChannels);
    DDX_Control(pDX, IDC_CHECK_ENABLELOOPPOINT, m_wndCheckEnableLoopPoint);
    DDX_Control(pDX, IDC_LIST_CUES, m_wndCues);
    DDX_Control(pDX, IDC_CHECK_CUMULATIVE, m_wndCumulative);

    AddAnchor(IDC_EDIT_TIME, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_EDIT_VALUE, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_LIST_CUES, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_COMBODEVICE, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC1, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_CHECK_ALLCHANNELS, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_CHECK_ENABLELOOPPOINT, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_STATIC4, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_BUTTON_ADDCUE, CPoint(0, 0), CPoint(50, 0));
    AddAnchor(IDC_BUTTON_DELETECUE, CPoint(50, 0), CPoint(100, 0));
}


BEGIN_MESSAGE_MAP(SoundToolboxSidePane, CExtDialogFwdCmd)
    ON_WM_DRAWITEM()
    ON_CBN_SELCHANGE(IDC_COMBODEVICE, OnSelChange)
    ON_LBN_SELCHANGE(IDC_LIST_CUES, OnCueChange)
    ON_BN_CLICKED(IDC_CHECK_ALLCHANNELS, OnAllChannels)
    ON_BN_CLICKED(IDC_CHECK_ENABLELOOPPOINT, OnEnableLoopPoint)
    ON_BN_CLICKED(IDC_BUTTON_ADDCUE, OnAddCue)
    ON_BN_CLICKED(IDC_BUTTON_DELETECUE, OnDeleteCue)
    ON_EN_KILLFOCUS(IDC_EDIT_VALUE, OnEditValue)
    ON_EN_KILLFOCUS(IDC_EDIT_TIME, OnEditTime)
    ON_BN_CLICKED(IDC_CHECK_CUMULATIVE, OnCheckCumulative)
    ON_WM_CREATE()
END_MESSAGE_MAP()


// SoundToolboxSidePane message handlers

BOOL SoundToolboxSidePane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    // Set up anchoring for resize
    AddAnchor(IDC_LISTCOMMANDS, CPoint(0, 0), CPoint(100, 100));
    // Hide the sizing grip
    ShowSizeGrip(FALSE);
    return fRet;
}

void SoundToolboxSidePane::OnSelChange()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->SetDevice(GetDeviceFromComboHelper(appState->GetVersion(), m_wndDevices));
    }
    // Whoa, hit a crash here once, when loading up a game. REVIEW
    // pDoc was non null, but invalid.
}

void SoundToolboxSidePane::OnCueChange()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->SetActiveCue(m_wndCues.GetCurSel());
        _UpdateOnSelectedCue();
    }
}

void SoundToolboxSidePane::OnAllChannels()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        bool turnOn = false;
        if (m_wndCheckAllChannels.GetCheck() == BST_CHECKED)
        {
            turnOn = true;
        }

        pDoc->ApplyChanges<SoundComponent>(
            [&](SoundComponent &sound)
            {
                // If there are duplicate channels (e.g. SCI1+), this is fine. SetChannelId
                // will ignore any subsequent adds of the same channel.
                SoundChangeHint hint = SoundChangeHint::None;
                for (auto &channel : sound.GetChannelInfos())
                {
                    hint |= sound.SetChannelId(pDoc->GetDevice(), channel.Id, turnOn);

                }
                return WrapHint(hint);
            }
            );
    }
}


void SoundToolboxSidePane::OnEnableLoopPoint()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        DWORD loopPoint = SoundComponent::LoopPointNone;
        if (m_wndCheckEnableLoopPoint.GetCheck() == BST_CHECKED)
        {
            loopPoint = 0; // TODO: use current pos of thing.. that would cause problems when we update though.
        }

        pDoc->ApplyChanges<SoundComponent>(
            [loopPoint](SoundComponent &sound)
        {
            return WrapHint(sound.SetLoopPoint(loopPoint));
        }
        );
    }
}

const uint8_t CueInc = 10;

void SoundToolboxSidePane::OnAddCue()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        // Use a higher value than the previous cue.
        uint8_t value = 0;
        const SoundComponent *pSound = GetSound();
        if (pSound && !pSound->GetCuePoints().empty())
        {
            value = pSound->GetCuePoints().back().GetValue() + CueInc;
        }

        CuePoint cue(CuePoint::NonCumulative, 0, value);
        pDoc->ApplyChanges<SoundComponent>(
            [cue](SoundComponent &sound)
            {
                return WrapHint(sound.AddCuePoint(cue));
            }
            );
        // The cue should have been added now - set the selection to the last item:
        m_wndCues.SetCurSel(m_wndCues.GetCount() - 1);
        OnCueChange(); // Need to call this manually...
    }    
}

void SoundToolboxSidePane::OnDeleteCue()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->ApplyChanges<SoundComponent>(
            [&](SoundComponent &sound)
        {
            return WrapHint(sound.DeleteCue(m_wndCues.GetCurSel()));
        }
        );
    }    
}

void SoundToolboxSidePane::OnEditTime()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const SoundComponent *pSound = GetSound();
        if (pSound)
        {
            CString strNumber;
            m_wndEditTime.GetWindowText(strNumber);
            int iNew = StrToInt(strNumber);
            iNew = min((int)pSound->GetTotalTicks(), iNew);
            iNew = max(0, iNew);
            size_t index = m_wndCues.GetCurSel();
            CuePoint cue = pSound->GetCuePoints()[index];
            cue.SetTickPos(iNew);

            pDoc->ApplyChanges<SoundComponent>(
                [index, cue](SoundComponent &sound)
            {
                return WrapHint(sound.SetCue(index, cue));
            }
            );
        }
    }    
}

void SoundToolboxSidePane::OnEditValue()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const SoundComponent *pSound = GetSound();
        if (pSound)
        {
            CString strNumber;
            m_wndEditValue.GetWindowText(strNumber);
            int iNew = StrToInt(strNumber);
            iNew = min(255, iNew);
            iNew = max(0, iNew);
            size_t index = m_wndCues.GetCurSel();
            CuePoint cue = pSound->GetCuePoints()[index];
            cue.SetValue((BYTE)iNew);

            pDoc->ApplyChanges<SoundComponent>(
                [index, cue](SoundComponent &sound)
            {
                return WrapHint(sound.SetCue(index, cue));
            }
            );
        }
    }    
}

void SoundToolboxSidePane::OnCheckCumulative()
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const SoundComponent *pSound = GetSound();
        if (pSound)
        {
            CuePoint::Type type = (m_wndCumulative.GetCheck() == BST_CHECKED) ?
                CuePoint::Cumulative :
                CuePoint::NonCumulative;
            size_t index = m_wndCues.GetCurSel();
            CuePoint cue = pSound->GetCuePoints()[index];
            cue.SetType(type);

            pDoc->ApplyChanges<SoundComponent>(
                [index, cue](SoundComponent &sound)
            {
                return WrapHint(sound.SetCue(index, cue));
            }
            );
        }
    }    
}

const char *c_szNoteFormat[] =
{
    "%02d,C%1d ",
    "%02d,C%1d#",
    "%02d,D%1d ",
    "%02d,D%1d#",
    "%02d,E%1d ",
    "%02d,F%1d ",
    "%02d,F%1d#",
    "%02d,G%1d ",
    "%02d,G%1d#",
    "%02d,A%1d ",
    "%02d,A%1d#",
    "%02d,B%1d ",
};

std::string _GetNoteString(const SoundEvent &event)
{
    BYTE bNote = event.bParam1 % 12;
    BYTE bOctave = (event.bParam1 / 12) + 1;
    char sz[7];
    ASSERT(ARRAYSIZE(c_szNoteFormat) > bNote);
    StringCchPrintf(sz, ARRAYSIZE(sz), c_szNoteFormat[bNote], event.GetChannel(), bOctave);
    return sz;
}

std::string _GetEventString(const SoundEvent &event, DWORD dwTime)
{
    std::string desc;
    char szTime[10];
    StringCchPrintf(szTime, ARRAYSIZE(szTime), " %03d:", dwTime);
    desc += szTime;
    switch (event.GetCommand())
    {
    case SoundEvent::NoteOn:
        if (event.bParam2)
        {
            desc += "NoteOn   ";
            desc += _GetNoteString(event);
            char sz[10];
            StringCchPrintf(sz, ARRAYSIZE(sz), " v:%03d", event.bParam2);
            desc += sz;
            break;
        }
        // else fall through
    case SoundEvent::NoteOff:
        desc += "NoteOff  ";
        desc += _GetNoteString(event);
        break;
    case SoundEvent::KeyPressure:
        desc += "KPressure";
        break;
    case SoundEvent::Pressure:
        desc += "Pressure ";
        break;
    case SoundEvent::PitchWheel:
        desc += "PitchWheel";
        break;
    case SoundEvent::Special:
        desc += "Special";
        break;
    case SoundEvent::ProgramChange:
        {
            char sz[40];
            StringCchPrintf(sz, ARRAYSIZE(sz), "ProgChange ch:%02d $%02x (%d)", event.GetChannel(), event.bParam1, event.bParam1);
            desc += sz;
        }
        break;
    case SoundEvent::Control:
        desc += "Control ";
        char sz[20];
        switch (event.bParam1)
        {
        // Non-standard MIDI controls:
        case 0x4B:
            StringCchPrintf(sz, ARRAYSIZE(sz), "using %d voices", event.bParam2);
            desc += sz;
            break;
        case 0x4C:
            StringCchPrintf(sz, ARRAYSIZE(sz), "reset-on-pause: %s", event.bParam2 ? "Off" : "On");
            desc += sz;
            break;
        case 0x4E:
            desc += "unknown";
            break;
        case 0x50:
            StringCchPrintf(sz, ARRAYSIZE(sz), "reverb %d", event.bParam2);
            desc += sz;
            break;
        case 0x60:
            desc += "cummulative cue";
            break;
        default:
            const char *pszType;
            char szOther[20];
            switch (event.bParam1)
            {
            case 1:
                pszType = "Mod wheel";
                break;
            case 4:
                pszType = "Foot pedal";
                break;
            case 7:
            case 39:
                pszType = "Volume";
                break;
            case 8:
            case 40:
                pszType = "Balance";
                break;
            case 10:
            case 42:
                pszType = "Pan position";
                break;
            default:
                pszType = szOther;
                StringCchPrintf(szOther, ARRAYSIZE(szOther), "%d:", event.bParam1);
                break;
            }
            char szMsg[40];
            StringCchPrintf(szMsg, ARRAYSIZE(szMsg), "%s %d", pszType, event.bParam2);
            desc += szMsg;
            break;
        }
        break;

    }
    return desc;
}

void MidiCommandListBox::DrawItem(DRAWITEMSTRUCT *pDrawItemStruct)
{
    SoundToolboxSidePane *pParent = static_cast<SoundToolboxSidePane*>(GetParent());
    CSoundDoc* pDoc = pParent->GetDocument();
    if (pDoc)
    {
        const SoundComponent *pSound = pDoc->GetResource()->TryGetComponent<SoundComponent>();
        if (pSound)
        {
            const std::vector<SoundEvent> *events = &g_emptyEvents;
            int selectedChannelId = pDoc->GetChannelId();
            if (selectedChannelId != -1)
            {
                events = &pSound->GetChannelInfos()[selectedChannelId].Events;
            }

            assert(pDrawItemStruct->CtlType == ODT_LISTBOX);
            CDC dc;
            dc.Attach(pDrawItemStruct->hDC);

            // REVIEW: We need to use this font when measuring too.
            HGDIOBJ hFontOld = dc.SelectObject(&g_PaintManager->m_FontNormalBC);

            // Save these values to restore them when done drawing.
            COLORREF crOldTextColor = dc.GetTextColor();
            COLORREF crOldBkColor = dc.GetBkColor();
            int iMode = dc.GetBkMode();

            dc.FillSolidRect(&pDrawItemStruct->rcItem, crOldBkColor);

            // If this item has the focus, draw a greenbackground around the
            // item's rect - unless it was selected.
            if ((pDrawItemStruct->itemAction | ODA_FOCUS) &&
                (pDrawItemStruct->itemState & ODS_FOCUS))
            {
                CBrush br(RGB(200, 255, 200));
                dc.FrameRect(&pDrawItemStruct->rcItem, &br);
            }

            if (pDrawItemStruct->itemID != -1)
            {
                const SoundEvent &event = (*events)[pDrawItemStruct->itemID];
                std::string foo = _GetEventString(event, _accumulatedTicksPerEvent[pDrawItemStruct->itemID]);

                dc.SetTextColor(RGB(0, 0, 0));
                dc.SetBkMode(TRANSPARENT);
                // Draw the text.
                dc.DrawText(
                    foo.c_str(),
                    -1,
                    &pDrawItemStruct->rcItem,
                    DT_SINGLELINE | DT_VCENTER);
            }

            // Reset the background color and the text color back to their
            // original values.
            dc.SelectObject(hFontOld);
            dc.SetTextColor(crOldTextColor);
            dc.SetBkColor(crOldBkColor);
            dc.SetBkMode(iMode);
            dc.Detach();
        }
    }
}

void _CalculateAccumulatedTicks(const std::vector<SoundEvent> &events, std::vector<DWORD> &accumulatedTicks)
{
    accumulatedTicks.resize(events.size(), 0);
    DWORD accumulatedTime = 0;
    for (size_t i = 0; i < accumulatedTicks.size(); i++)
    {
        accumulatedTime += events[i].wTimeDelta;
        accumulatedTicks[i] = accumulatedTime;
    }
}

const std::vector<SoundEvent> &SoundToolboxSidePane::_GetCurrentChannelEvents()
{
    // For now, just use channel zero. But we'll need a notion of currently selected channel.
    const SoundComponent *pSound = GetSound();
    if (pSound)
    {
        const std::vector<SoundEvent> *events = &g_emptyEvents;
        int selectedChannelId = GetDocument()->GetChannelId();
        if (selectedChannelId != -1)
        {
            return pSound->GetChannelInfos()[selectedChannelId].Events;
        }
    }
    return g_emptyEvents;
}

void SoundToolboxSidePane::_UpdateItemCount()
{
    const SoundComponent *pSound = GetSound();
    if (pSound)
    {
        _CalculateAccumulatedTicks(_GetCurrentChannelEvents(), _accumulatedTicksPerEvent);

        // Store off the top index
        m_wndList.SetRedraw(FALSE);
        int iTopIndex = (int)m_wndList.SendMessage(LB_GETTOPINDEX, 0, 0);
        m_wndList.SendMessage(LB_SETCOUNT, (WPARAM)_GetCurrentChannelEvents().size(), 0);
        // Restore the top index.
        m_wndList.SendMessage(LB_SETTOPINDEX, iTopIndex, 0);
        m_wndList.SetRedraw(TRUE);
    }
}

void SoundToolboxSidePane::_UpdateCues()
{
    m_wndCues.SetRedraw(FALSE);
    const SoundComponent *pSound = GetSound();
    int iCurSel = m_wndCues.GetCurSel();
    m_wndCues.ResetContent();
    if (pSound)
    {
        const std::vector<CuePoint> &cues = pSound->GetCuePoints();
        for (size_t i = 0; i < cues.size(); i++)
        {
            const CuePoint &cue = cues[i];
            char sz[100];
            StringCchPrintf(sz,
                ARRAYSIZE(sz),
                "%5d: %s: %d",
                cue.GetTickPos(),
                (cue.GetType() == CuePoint::Cumulative) ? "Cum." : "Non-cum.",
                cue.GetValue());
            m_wndCues.InsertString(-1, sz);
        }
        if (iCurSel != LB_ERR)
        {
            // Restore selection.
            m_wndCues.SetCurSel(iCurSel);
        }
    }
    m_wndCues.SetRedraw(TRUE);
    OnCueChange();
}

void SoundToolboxSidePane::_UpdateOnSelectedCue()
{
    // These controls are enabled depending on if a cue is selected.
    bool fEnable = (m_wndCues.GetCurSel() != LB_ERR);
    m_wndEditTime.EnableWindow(fEnable);
    m_wndEditValue.EnableWindow(fEnable);
    m_wndDelete.EnableWindow(fEnable);
    m_wndCumulative.EnableWindow(fEnable);

    const SoundComponent *pSound = GetSound();
    if (pSound && fEnable)
    {
        const CuePoint &cue = pSound->GetCuePoints()[m_wndCues.GetCurSel()];
        char sz[100];
        StringCchPrintf(sz, ARRAYSIZE(sz), "%d", cue.GetTickPos());
        m_wndEditTime.SetWindowText(sz);
        StringCchPrintf(sz, ARRAYSIZE(sz), "%d", cue.GetValue());
        m_wndEditValue.SetWindowText(sz);
        m_wndCumulative.SetCheck((cue.GetType() == CuePoint::Cumulative) ?
            BST_CHECKED : BST_UNCHECKED);
    }
    else
    {
        m_wndEditTime.SetWindowText("");
        m_wndEditValue.SetWindowText("");
        m_wndCumulative.SetCheck(BST_UNCHECKED);
    }
}

void SoundToolboxSidePane::UpdateNonView(CObject *pObject)
{
    SoundChangeHint hint = GetHint<SoundChangeHint>(pObject);
    if (IsFlagSet(hint, SoundChangeHint::Changed | SoundChangeHint::DeviceChanged))
    {
        const SoundComponent *pSound = GetSound();
        if (pSound)
        {
            WORD wChannelMask = pSound->CalculateChannelMask(GetDocument()->GetDevice());
            if (wChannelMask == 0x7FFF)
            {
                m_wndCheckAllChannels.SetCheck(BST_CHECKED);
            }
            else if (wChannelMask == 0)
            {
                m_wndCheckAllChannels.SetCheck(BST_UNCHECKED);
            }
            else
            {
                m_wndCheckAllChannels.SetCheck(BST_INDETERMINATE);
            }

            m_wndCheckEnableLoopPoint.SetCheck((pSound->GetLoopPoint() != SoundComponent::LoopPointNone) ? BST_CHECKED : BST_UNCHECKED);
        }
    }

    if (IsFlagSet(hint, SoundChangeHint::Changed | SoundChangeHint::CueChanged | SoundChangeHint::SelectedChannelChanged))
    {
        // Remove all items
        _UpdateItemCount();
        _UpdateCues();
    }

    if (IsFlagSet(hint, SoundChangeHint::DeviceChanged))
    {
        CSoundDoc *pDoc = GetDocument();
        if (pDoc)
        {
            SelectDeviceInComboHelper(appState->GetVersion(), m_wndDevices, pDoc->GetDevice());
        }
    }

    if (IsFlagSet(hint, SoundChangeHint::ActiveCueChanged))
    {
        CSoundDoc *pDoc = GetDocument();
        if (pDoc)
        {
            m_wndCues.SetCurSel(pDoc->GetActiveCue());
            OnCueChange();
        }
    }
}

void SoundToolboxSidePane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CSoundDoc*>(pDoc);
    if (_pDoc)
    {
        int prevSel = m_wndDevices.GetCurSel();
        m_wndDevices.ResetContent();
        PopulateComboWithDevicesHelper(appState->GetVersion(), m_wndDevices);
        if ((prevSel >= 0) && (prevSel < m_wndDevices.GetCount()))
        {
            m_wndDevices.SetCurSel(prevSel);
        }
        else
        {
            m_wndDevices.SetCurSel(0);
        }

        _pDoc->AddNonViewClient(this);
    }
    UpdateNonView(&WrapHint(SoundChangeHint::Changed | SoundChangeHint::DeviceChanged));

}
