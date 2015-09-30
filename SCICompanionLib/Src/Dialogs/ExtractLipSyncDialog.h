#pragma once

#include "resource.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"
#include "TalkerToViewMap.h"

struct AudioComponent;
class ResourceEntity;

class ExtractLipSyncDialog : public AudioPlaybackUI<CExtResizableDialog>
{
public:
    ExtractLipSyncDialog(const AudioComponent &audio, uint8_t talker, CWnd* pParent = NULL);   // standard constructor
    virtual ~ExtractLipSyncDialog();
    virtual void OnCancel();

    // Dialog Data
    enum { IDD = IDD_LIPSYNCDIALOG };

protected:
    void _UpdateViewLoop();
    void _SyncViewLoop();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

    afx_msg void OnTimer(UINT_PTR nIDEvent);

    CExtProgressWnd m_wndProgress;

    // Visuals
    CExtButton m_wndCancel;
    CExtEdit m_wndViewNumber;
    CExtEdit m_wndLoopNumber;
    CExtLabel m_wndTalkerLabel;
    ViewUIElement m_wndMouth;

    std::unique_ptr<ResourceEntity> _viewResource;
    std::unique_ptr<AudioComponent> _audioCopy;
    uint8_t _talker;
    TalkerToViewMap _talkerToViewMap;

public:
    afx_msg void OnBnClickedButtonResetmapping();
    afx_msg void OnEnKillfocusEditPhonememap();
    afx_msg void OnEnKillfocusEditView();
    afx_msg void OnEnKillfocusEditLoop();
    afx_msg void OnBnClickedGeneratelipsync();
};
