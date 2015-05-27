#pragma once

class MessageSource;

class MessageEditorListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(MessageEditorListCtrl)

public:
    MessageEditorListCtrl();
    virtual ~MessageEditorListCtrl();

    void SetSource(MessageSource *source);

protected:
    DECLARE_MESSAGE_MAP()

    void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

private:
    void _InitColumns();
    void _Populate();

    bool _initialized;
    MessageSource *_source;
};
