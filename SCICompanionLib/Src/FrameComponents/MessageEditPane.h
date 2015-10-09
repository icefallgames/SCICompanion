#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "MessageEditorListCtrl.h"
#include "Message.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"
#include "Task.h"

class CMessageDoc;
struct SyncComponent;
struct TextComponent;
struct TextEntry;

enum class SidecarResourceStatus
{
    Added = 0x1,        // This has been added since we last saved
    Deleted = 0x2,      // This has been deleted since we last saved
    Moved = 0x4,        // This was moved (e.g. map needs to be updated, but not the thing)
};
DEFINE_ENUM_FLAGS(SidecarResourceStatus, uint32_t)

class MessageEditPane : public AudioPlaybackUI<CExtDialogFwdCmd>, public INonViewClient
{
public:
    MessageEditPane(CWnd* pParent = nullptr);   // standard constructor
    virtual ~MessageEditPane();

    // Dialog Data
    enum
    {
        IDD = IDD_MESSAGEEDIT,
    };

    void SetDocument(CDocument *pDoc);
    CMessageDoc *GetDocument() { return _pDoc; }

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void _Update();
    const TextEntry *_GetEntry();

protected:
    void OnNewResourceCreated(std::unique_ptr<ResourceEntity> audioResource, const std::string &name) override;

    const TextComponent *_GetResource();
    int _GetSelectedIndex();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnPlayAudio();

    DECLARE_MESSAGE_MAP()

    BOOL OnEraseBkgnd(CDC *pDC);

private:
    void _UpdateSequence(int sequence);
    void _UpdateCombos(MessageChangeHint hint);
    bool _UpdateAudio(const TextEntry &entry);

    CMessageDoc *_pDoc;

    CExtEdit m_wndEditMessage;

    CExtComboBox m_wndComboNoun;
    bool _nounEdited;
    CExtComboBox m_wndComboVerb;
    bool _verbEdited;
    CExtComboBox m_wndComboTalker;
    bool _talkerEdited;
    CExtComboBox m_wndComboCondition;
    bool _conditionEdited;

    CExtCheckBox m_wndUseText;
    CExtEdit m_wndEditSequence;

    // Visual
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtLabel m_wndLabel4;
    CExtLabel m_wndLabel5;
    CExtLabel m_wndLabel6;
    CExtLabel m_wndLabelBase36;
    CExtButton m_wndButton1;
    CExtButton m_wndButton2;
    CExtButton m_wndButton3;
    CExtButton m_wndButtonFakeCommit;
    CExtSpinWnd m_wndSpinner;
    CExtButton m_wndDeleteAudio;
    CExtButton m_wndEditAudio;

    // Lipsync stuff
    ViewUIElement m_wndMouth;
    CExtButton m_wndQuickLipSync;
    CExtButton m_wndLipSyncDialog;
    std::unique_ptr<ResourceEntity> _mouthView;
    int _mouthLoop;
    int _mouthCel;
    CBitmap _currentCelImage;

    CWndTaskSink<SyncComponent> _lipSyncTaskSink;

    int _spinnerValue;

    HACCEL _hAccel;
    bool _initialized;
public:
    afx_msg void OnBnClickedButtonaddnoun();
    afx_msg void OnBnClickedButtonaddcondition();
    afx_msg void OnEnChangeEditmessage();
    afx_msg void OnCbnSelchangeCombonoun();
    afx_msg void OnCbnSelchangeComboverb();
    afx_msg void OnEnChangeEditseq();
    afx_msg void OnEnKillfocusEditmessage();
    afx_msg void OnCbnSelchangeCombocondition();
    afx_msg void OnCbnSelchangeCombotalker();
    afx_msg void OnCbnEditchangeComboverb();
    afx_msg void OnCbnEditupdateComboverb();
    afx_msg void OnCbnKillfocusComboverb();
    afx_msg void OnCbnSetfocusComboverb();
    afx_msg void OnCbnSetfocusCombonoun();
    afx_msg void OnCbnKillfocusCombonoun();
    afx_msg void OnCbnEditchangeCombonoun();
    afx_msg void OnCbnSetfocusCombocondition();
    afx_msg void OnCbnKillfocusCombocondition();
    afx_msg void OnCbnEditchangeCombocondition();
    afx_msg void OnCbnSetfocusCombotalker();
    afx_msg void OnCbnKillfocusCombotalker();
    afx_msg void OnCbnEditchangeCombotalker();
    afx_msg void OnBnClickedButtonlipsync();
    LRESULT _OnLipSyncDone(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButtonlipsyncDialog();
    afx_msg void OnBnClickedButtondeleteaudio();
    afx_msg void OnBnClickedEditaudio();
};
