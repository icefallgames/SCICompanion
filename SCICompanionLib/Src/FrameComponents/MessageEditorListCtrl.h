#pragma once

class MessageSource;
class CMessageDoc;
enum class MessageSourceType;

class CLVEdit : public CEdit
{
public:
    CRect Rect;
    virtual ~CLVEdit() {};

protected:
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

    DECLARE_MESSAGE_MAP()
};

class MessageEditorListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(MessageEditorListCtrl)

public:
    MessageEditorListCtrl();
    virtual ~MessageEditorListCtrl();

    // Instead of setting the MessageSource directly, we'll set the type.
    // This is because we retrieve MessageSource as a weak ptr that may be reloaded
    // at any time. So we always need to fetch it from the source.
    void SetSource(CMessageDoc *pDoc, MessageSourceType sourceType);
    void AddNewItem();
    void DeleteSelectedItem();

    BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
    DECLARE_MESSAGE_MAP()
    void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

private:
    MessageSource *_GetSource(bool reload = false);

    template<typename _Func>
    void ApplyMessageChanges(_Func f);


    void _InitColumns();
    void _Populate();
    void _Commit();

    MessageSourceType _sourceType;
    CMessageDoc *_pDoc;

    bool _initialized;
    bool _inLabelEdit;
    bool _inValueEdit;
    bool _removeIfFailEdit;
    CLVEdit _lvEdit;
};
