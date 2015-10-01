#pragma once

#include "resource.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"
#include "TalkerToViewMap.h"
#include "AudioWaveformUI.h"
#include "Task.h"

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
    virtual void OnCancel();

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

    // Visuals
    CExtButton m_wndCancel;
    CExtEdit m_wndViewNumber;
    CExtEdit m_wndLoopNumber;
    CExtLabel m_wndTalkerLabel;
    ViewUIElement m_wndMouth;
    AudioWaveformUI m_wndWaveform;
    CListCtrl m_wndSyncList;
    CExtEdit m_rawLipSyncWords;
    CExtButton m_wndLipSyncButton;

    std::unique_ptr<ResourceEntity> _audioResource;
    std::unique_ptr<ResourceEntity> _viewResource;
    uint8_t _talker;
    TalkerToViewMap _talkerToViewMap;

    std::unique_ptr<CWndTaskSink<LipSyncDialogTaskResult>> _taskSink;

    bool _initialized;

public:
    afx_msg LRESULT _OnLipSyncDone(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButtonResetmapping();
    afx_msg void OnEnKillfocusEditPhonememap();
    afx_msg void OnEnKillfocusEditView();
    afx_msg void OnEnKillfocusEditLoop();
    afx_msg void OnBnClickedGeneratelipsync();
};
