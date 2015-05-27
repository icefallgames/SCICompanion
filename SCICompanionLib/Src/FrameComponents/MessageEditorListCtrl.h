#pragma once

class MessageSource;

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

    void SetSource(MessageSource *source);
    void AddNewItem();

protected:
    DECLARE_MESSAGE_MAP()
    void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

private:
    void _InitColumns();
    void _Populate();

    bool _initialized;
    MessageSource *_source;
    bool _inLabelEdit;
    bool _inValueEdit;
    bool _removeIfFailEdit;
    CLVEdit _lvEdit;
};
