#pragma once

#include "NonViewClient.h"
#include "ExtDialogFwdCmd.h"
#include "NoFlickerStatic.h"
#include "MessageEditorListCtrl.h"
#include "Message.h"

class CMessageDoc;
struct TextComponent;
struct TextEntry;

class MessageEditPane : public CExtDialogFwdCmd, public INonViewClient
{
public:
    MessageEditPane(CWnd* pParent = NULL);   // standard constructor
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
    const TextComponent *_GetResource();
    const TextEntry *_GetEntry();
    int _GetSelectedIndex();
    void _AddEntryAtCurrentPosition(const TextEntry &entry);

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoTalkers();
    afx_msg void OnGotoVerbs();

    DECLARE_MESSAGE_MAP()

    BOOL OnEraseBkgnd(CDC *pDC);

private:
    void _UpdateSequence(int sequence);
    void _UpdateCombos(MessageChangeHint hint);

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
    int _spinnerValue;

    HACCEL _hAccel;
    bool _initialized;
public:
    afx_msg void OnBnClickedButtonaddnoun();
    afx_msg void OnBnClickedButtonaddcondition();
    afx_msg void OnBnClickedButtonaddseq();
    afx_msg void OnBnClickedButtonclone();
    afx_msg void OnEnChangeEditmessage();
    afx_msg void OnCbnSelchangeCombonoun();
    afx_msg void OnCbnSelchangeComboverb();
    afx_msg void OnEnChangeEditseq();
    afx_msg void OnEnKillfocusEditmessage();
    afx_msg void OnCbnSelchangeCombocondition();
    afx_msg void OnCbnSelchangeCombotalker();
    afx_msg void OnBnClickedButtonaddnew();
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
};
