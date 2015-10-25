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
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"
#include "AudioWaveformUI.h"
#include "Task.h"
#include "ToolTipDialog.h"

class PhonemeMap;
struct AudioComponent;
struct SyncComponent;
class ResourceEntity;
class alignment_result;
struct LipSyncDialogTaskResult;

class ExtractLipSyncDialog : public AudioPlaybackUI<ToolTipDialog>
{
public:
    ExtractLipSyncDialog(const ResourceEntity &resource, uint8_t talker, const std::string &talkerName, const std::string &messageText, bool useText, CWnd* pParent = NULL);   // standard constructor
    virtual ~ExtractLipSyncDialog();
    std::unique_ptr<ResourceEntity> GetResult();

    // Dialog Data
    enum { IDD = IDD_LIPSYNCDIALOG };

protected:
    void _SyncViewLoop();
    void _UpdateSyncList();
    void _InitSyncListColumns();
    void _UpdateWords(const std::vector<alignment_result> &rawResults);
    void _ReloadPhonemeMap();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

    afx_msg void OnTimer(UINT_PTR nIDEvent);

    CProgressCtrl m_wndProgress;

    CExtLabel m_wndTalkerLabel;
    ViewUIElement m_wndMouth;
    AudioWaveformUI m_wndWaveform;
    CListCtrl m_wndSyncList;
    CExtEdit m_rawLipSyncWords;
    CExtEdit m_wndEditPhonemeMap;
    CExtEdit m_wndEditPhonemeMapStatus;
    CExtButton m_wndLipSyncButton;

    // Visuals
    CExtButton m_wndCancel;
    CExtButton m_wndOk;
    CExtButton m_wndReloadMapping;
    CExtButton m_wndOpenMapping;
    CExtButton m_wndResetMapping;
    CExtButton m_wndSetView;
    CExtButton m_wndDeleteSync;
    CExtButton m_wndImportSync;
    CExtButton m_wndExportSync;
    CExtButton m_wndButtonRaw;
    CExtButton m_wndEditAudio;
    CExtButton m_wndEditPhoneme;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtSpinWnd m_wndSpinLipSync;

    CExtGroupBox m_wndGroupViewLoop;
    CExtGroupBox m_wndGroupMessage;
    CExtGroupBox m_wndGroupTalker;

    CExtLabel m_wndStatic4;
    CExtCheckBox m_wndUseSample;
    CExtCheckBox m_wndUseText;

    std::unique_ptr<ResourceEntity> _audioResource;
    std::unique_ptr<ResourceEntity> _viewResource;
    bool _wantToUseSample;
    bool _actuallyUsingSample;
    int _nView, _nLoop;
    uint8_t _talker;
    bool _useText;
    std::string _talkerName;
    
    std::string _messageText;

    std::unique_ptr<CWndTaskSink<LipSyncDialogTaskResult>> _taskSink;

    std::unique_ptr<PhonemeMap> _phonemeMap;

    bool _initialized;

public:
    afx_msg LRESULT _OnLipSyncDone(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButtonResetmapping();
    afx_msg void OnBnClickedGeneratelipsync();
    afx_msg void OnBnClickedButtonReloadmapping();
    afx_msg void OnBnClickedButtonSetview();
    afx_msg void OnBnClickedCheckUsesample();
    afx_msg void OnBnClickedButtonDeleteSync();
    afx_msg void OnBnClickedButtonRaw();
    afx_msg void OnBnClickedButtonExportsync();
    afx_msg void OnBnClickedButtonImportsync();
    afx_msg void OnBnClickedEditaudio();
    afx_msg void OnBnClickedEditphonememap();
    afx_msg void OnBnClickedButtonOpenmapping();
    afx_msg void OnDeltaposSpinLipsync(NMHDR *pNMHDR, LRESULT *pResult);
};
