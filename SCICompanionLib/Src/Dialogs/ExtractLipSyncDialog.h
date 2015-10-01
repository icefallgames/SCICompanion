#pragma once

#include "resource.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"
#include "TalkerToViewMap.h"
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
    ExtractLipSyncDialog(const ResourceEntity &resource, uint8_t talker, CWnd* pParent = NULL);   // standard constructor
    virtual ~ExtractLipSyncDialog();
    std::unique_ptr<SyncComponent> GetSyncComponent() const;

    // Dialog Data
    enum { IDD = IDD_LIPSYNCDIALOG };

protected:
    void _UpdateViewLoop();
    void _SyncViewLoop();
    void _UpdateSyncList();
    void _InitSyncListColumns();
    void _UpdateWords(const std::vector<alignment_result> &rawResults);

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

    afx_msg void OnTimer(UINT_PTR nIDEvent);

    CProgressCtrl m_wndProgress;

    CExtEdit m_wndViewNumber;
    CExtEdit m_wndLoopNumber;
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
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic4;

    std::unique_ptr<ResourceEntity> _audioResource;
    std::unique_ptr<ResourceEntity> _viewResource;
    bool _haveActualViewResource;
    uint8_t _talker;
    TalkerToViewMap _talkerToViewMap;

    std::unique_ptr<CWndTaskSink<LipSyncDialogTaskResult>> _taskSink;

    std::unique_ptr<PhonemeMap> _phonemeMap;

    bool _initialized;

public:
    afx_msg LRESULT _OnLipSyncDone(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButtonResetmapping();
    afx_msg void OnEnKillfocusEditPhonememap();
    afx_msg void OnEnKillfocusEditView();
    afx_msg void OnEnKillfocusEditLoop();
    afx_msg void OnBnClickedGeneratelipsync();
    afx_msg void OnBnClickedButtonCommitmapping();
};
