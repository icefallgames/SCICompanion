#include "stdafx.h"
#include "MessageEditorListCtrl.h"
#include "MessageSource.h"
#include "format.h"

using namespace std;

IMPLEMENT_DYNAMIC(MessageEditorListCtrl, CListCtrl)
MessageEditorListCtrl::MessageEditorListCtrl() : _initialized(false), _source(nullptr)
{
}

MessageEditorListCtrl::~MessageEditorListCtrl()
{
}


BEGIN_MESSAGE_MAP(MessageEditorListCtrl, CListCtrl)
    ON_WM_CREATE()
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
END_MESSAGE_MAP()

void MessageEditorListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{

}

void MessageEditorListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{

}

void MessageEditorListCtrl::_InitColumns()
{
    if (!_initialized)
    {
        // Name
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        col.iOrder = 0;
        col.iSubItem = 0;
        col.pszText = "Name";
        col.cx = 100;
        col.fmt = LVCFMT_LEFT;
        InsertColumn(0, &col);

        col.iOrder = 1;
        col.iSubItem = 1;
        col.pszText = "Value";
        col.cx = 30;
        col.fmt = LVCFMT_LEFT;
        InsertColumn(1, &col);

        _initialized = true;
    }
}

void MessageEditorListCtrl::_Populate()
{
    _InitColumns();

    this->SetRedraw(FALSE);
    DeleteAllItems();

    if (_source)
    {
        int index = 0;
        for (auto &define : _source->GetDefines())
        {
            InsertItem(index, define.first.c_str());
            string value = fmt::format("{0}", define.second);
            SetItem(index, 1, LVIF_TEXT, value.c_str(), 0, 0, 0, 0);
            index++;
        }
    }
    this->SetRedraw(TRUE);
}

void MessageEditorListCtrl::SetSource(MessageSource *source)
{
    if (_source != source)
    {
        _source = source;
        _Populate();
    }
}