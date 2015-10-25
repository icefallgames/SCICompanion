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

#include "resource.h"
#include "PhonemeEditor.h"
#include "PhonemeMap.h"
#include "Task.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"

class alignment_result;
struct SyncComponent;

class PhonemeDialog : public AudioPlaybackUI<CExtResizableDialog>, public IPhonemeMapNotify
{
public:
    PhonemeDialog(int nView, int nLoop, PhonemeMap &map, CWnd* pParent = nullptr);

    // Dialog Data
    enum { IDD = IDD_PHONEMEEDITOR };

    // IPhonemeMapNotify
    void OnMapUpdated() override;

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    LRESULT _OnLipSyncDone(WPARAM wParam, LPARAM lParam);
    BOOL OnInitDialog() override;
    void _UpdateLipSync();

    DECLARE_MESSAGE_MAP()

    std::unique_ptr<ResourceEntity> _viewResource;

    int _nLoop;
    int _nView;

    PhonemeEditor m_wndMap;

    // Visuals
    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    ViewUIElement m_wndMouth;
    CExtEdit m_wndSampleText;
    CExtEdit m_wndDescription;

    std::unique_ptr<SyncComponent> _syncComponent;
    PhonemeMap &_map;

    std::unique_ptr<CWndTaskSink<std::vector<alignment_result>>> _taskSink;
};
