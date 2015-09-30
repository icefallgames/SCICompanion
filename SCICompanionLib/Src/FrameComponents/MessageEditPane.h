#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "MessageEditorListCtrl.h"
#include "Message.h"
#include "AudioPlaybackUI.h"
#include "ViewUIElement.h"

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















#include <future>

template <typename _TResponse>
class CWndTaskWrapperNEW
{
    // Use a function object so that we can null out the hwnd?
    // That's an advanced feature, we won't bother for now.
};

template<typename _TResponse, typename _TInnerFunc>
_TResponse HWNDTaskWrapper(_TInnerFunc innerFunc, HWND hwnd, UINT message)
{
    _TResponse response = innerFunc();
    PostMessage(hwnd, message, 0, 0);
    return response;
}

// A more generic mechanism?
template<typename _TResponse>
class CWndTaskSink
{
public:
    // pwnd guaranteed to exist as long as CWndTaskSink does.
    CWndTaskSink(CWnd *pwnd, UINT message) : _pwnd(pwnd), _message(message) {}

    ~CWndTaskSink() { Abandon(); }

    template<typename _TFunc>
    void StartTask(_TFunc func)
    {
        // TODO: add futures to a queue, so we can instantiate new ones.
        _future = std::make_unique<std::future<_TResponse>>(std::async(std::launch::async, HWNDTaskWrapper<_TResponse, _TFunc>, func, _pwnd->GetSafeHwnd(), _message));
    }

    void Abandon()
    {
        // TODO
    }

    _TResponse GetResponse()
    {
        // Ok to block, since we posted the message just as we were about to be done. get blocks
        //if (future_status::ready == _future.wait_for(std::chrono::seconds(0)))
        return _future->get();
    }

private:
    std::unique_ptr<std::future<_TResponse>> _future;

    CWnd *_pwnd;
    UINT _message;
};










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

    TextEntry *_GetEntry(TextComponent &text);
    void _Update();

protected:
    void OnPlaybackTimer() override;
    void OnNewResourceCreated(std::unique_ptr<ResourceEntity> audioResource, const std::string &name) override;

    const TextComponent *_GetResource();
    const TextEntry *_GetEntry();
    int _GetSelectedIndex();
    void _AddEntryAtCurrentPosition(const TextEntry &entry);

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnPlayAudio();

    DECLARE_MESSAGE_MAP()

    BOOL OnEraseBkgnd(CDC *pDC);

private:
    void _UpdateSequence(int sequence);
    void _UpdateCombos(MessageChangeHint hint);
    void _UpdateAudio(const TextEntry &entry);

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

    CExtEdit m_wndEditSequence;

    // Visual
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtLabel m_wndLabel4;
    CExtLabel m_wndLabel5;
    CExtLabel m_wndLabel6;
    CExtButton m_wndButton1;
    CExtButton m_wndButton2;
    CExtButton m_wndButton3;
    CExtButton m_wndButtonFakeCommit;
    CExtSpinWnd m_wndSpinner;

    // Lipsync stuff
    ViewUIElement m_wndMouth;
    CExtButton m_wndQuickLipSync;
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
};
