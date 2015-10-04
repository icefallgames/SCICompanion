#pragma once

#include "resource.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"
#include "AudioWaveformUI.h"
#include "Task.h"

class PhonemeMap;
struct AudioComponent;
struct SyncComponent;
class ResourceEntity;
class alignment_result;
struct LipSyncDialogTaskResult;

class ExtractLipSyncDialog : public AudioPlaybackUI<CExtResizableDialog>
{
public:
    ExtractLipSyncDialog(const ResourceEntity &resource, uint8_t talker, const std::string &talkerName, const std::string &messageText, bool textless, CWnd* pParent = NULL);   // standard constructor
    virtual ~ExtractLipSyncDialog();
    std::unique_ptr<SyncComponent> GetSyncComponent() const;

    // Dialog Data
    enum { IDD = IDD_LIPSYNCDIALOG };

protected:
    void _SyncViewLoop();
    void _UpdateSyncList();
    void _InitSyncListColumns();
    void _UpdateWords(const std::vector<alignment_result> &rawResults);

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
    CExtButton m_wndCommitMapping;
    CExtButton m_wndResetMapping;
    CExtButton m_wndSetView;
    CExtButton m_wndDeleteSync;
    CExtButton m_wndButtonRaw;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;

    CExtGroupBox m_wndGroupViewLoop;
    CExtGroupBox m_wndGroupMessage;
    CExtGroupBox m_wndGroupTalker;

    CExtLabel m_wndStatic4;
    CExtCheckBox m_wndUseSample;
    CExtCheckBox m_wndTextless;

    std::unique_ptr<ResourceEntity> _audioResource;
    std::unique_ptr<ResourceEntity> _viewResource;
    bool _wantToUseSample;
    bool _actuallyUsingSample;
    int _nView, _nLoop;
    uint8_t _talker;
    bool _textless;
    std::string _talkerName;
    
    std::string _messageText;

    std::unique_ptr<CWndTaskSink<LipSyncDialogTaskResult>> _taskSink;

    std::unique_ptr<PhonemeMap> _phonemeMap;

    bool _initialized;

public:
    afx_msg LRESULT _OnLipSyncDone(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButtonResetmapping();
    afx_msg void OnBnClickedGeneratelipsync();
    afx_msg void OnBnClickedButtonCommitmapping();
    afx_msg void OnBnClickedButtonSetview();
    afx_msg void OnBnClickedCheckUsesample();
    afx_msg void OnBnClickedButtonDeleteSync();
    afx_msg void OnBnClickedButtonRaw();
    afx_msg void OnEnChangeEditPhonememap();
};
