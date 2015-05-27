#include "stdafx.h"
#include "MessageEditorListCtrl.h"
#include "MessageSource.h"
#include "format.h"
#include "SCIStudioSyntaxParser.h"

using namespace std;

BEGIN_MESSAGE_MAP(CLVEdit, CEdit)
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void CLVEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
    lpwndpos->x = Rect.left;
    lpwndpos->y = Rect.top;
    lpwndpos->cx = Rect.Width();
    lpwndpos->cy = Rect.Height();
    CEdit::OnWindowPosChanging(lpwndpos);
}

IMPLEMENT_DYNAMIC(MessageEditorListCtrl, CListCtrl)
MessageEditorListCtrl::MessageEditorListCtrl() : _initialized(false), _source(nullptr), _inLabelEdit(false), _inValueEdit(false), _removeIfFailEdit(false)
{
}

MessageEditorListCtrl::~MessageEditorListCtrl()
{
}

#define COL_NAME 0
#define COL_VALUE 1

BEGIN_MESSAGE_MAP(MessageEditorListCtrl, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
END_MESSAGE_MAP()

bool _IsValidToken(const string &value)
{
    bool ret = false;
    if (!value.empty())
    {
        int index = 0;
        if (isalpha(value[index]) || (value[index] == '_'))
        {
            index++;
            ret = true;
            while (ret && (index < (int)value.size()))
            {
                ret = isalnum(value[index]) || (value[index] == '_');
                index++;
            }
        }
    }
    return ret;
}

void MessageEditorListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    *pResult = FALSE;
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    if (plvdi->item.pszText && _inLabelEdit)
    {
        CString strResult;
        GetEditControl()->GetWindowText(strResult);
        string result = (PCSTR)strResult;
        if (_inValueEdit)
        {
            _lvEdit.UnsubclassWindow();

            int value = stoi(result);
            int existingIndex = _source->IndexOf((uint16_t)value);
            if ((existingIndex != -1) && (existingIndex != plvdi->item.iItem))
            {
                AfxMessageBox("A define already exists with this value.", MB_ICONWARNING | MB_OK);
            }
            else if ((value < 1) || (value >= 256))
            {
                AfxMessageBox("The value must be between 1 and 255.", MB_ICONWARNING | MB_OK);
            }
            else
            {
                SetItemText(plvdi->item.iItem, COL_VALUE, strResult);
                _source->SetValue(plvdi->item.iItem, (uint16_t)value);
                _source->Commit();
                *pResult = TRUE;
            }

            _inValueEdit = false;
        }
        else
        {
            // Validate and commit the name.
            if (result.compare(0, _source->MandatoryPrefix.size(), _source->MandatoryPrefix) != 0)
            {
                result = _source->MandatoryPrefix + result;
            }
            int existingIndex = _source->IndexOf(result);
            if ((existingIndex != -1) && (existingIndex != plvdi->item.iItem))
            {
                AfxMessageBox("A define already exists with this name.", MB_ICONWARNING | MB_OK);
            }
            else if (!_IsValidToken(result))
            {
                AfxMessageBox("A define must begin with a letter, followed by letters or numbers.", MB_ICONWARNING | MB_OK);
            }
            else
            {
                SetItemText(plvdi->item.iItem, COL_NAME, result.c_str());
                _source->SetName(plvdi->item.iItem, result);
                _source->Commit();
                *pResult = TRUE;
            }
        }
    }

    if (_removeIfFailEdit)
    {
        if (!*pResult)
        {
            _source->DeleteDefine(plvdi->item.iItem);
            DeleteItem(plvdi->item.iItem);
        }
        _removeIfFailEdit = false;
    }

    _inLabelEdit = false;
}

void MessageEditorListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    DWORD pos = GetMessagePos();
    CPoint pt(LOWORD(pos), HIWORD(pos));
    ScreenToClient(&pt);
    int index = plvdi->item.iItem;

    CRect rc;
    GetSubItemRect(index, COL_VALUE, LVIR_BOUNDS, rc);
    if (rc.PtInRect(pt))
    {
        // Subclass the item so we can set its position over the subitem
        // SetWindowPos won't work here)
        _lvEdit.Rect = rc;
        _lvEdit.SubclassWindow(GetEditControl()->GetSafeHwnd());
        _inValueEdit = true;
        _lvEdit.SetWindowTextA(GetItemText(index, COL_VALUE));
        _lvEdit.ModifyStyle(0, ES_NUMBER);
    }

    _inLabelEdit = true;
}

/*
void MessageEditorListCtrl::OnClickOnNMClick(UINT nFlags, CPoint point)
{
    int index = HitTest(point);
    OutputDebugString("button up\n");
    if (index != -1)
    {
        OutputDebugString(fmt::format("hit test item {0}\n", index).c_str());
        if (GetItemState(index, LVIS_FOCUSED))
        {
            OutputDebugString("Clicked on focused item\n");
        }
    }
    //GetSubItemRect()
}*/

void MessageEditorListCtrl::_InitColumns()
{
    if (!_initialized)
    {
        ListView_SetExtendedListViewStyle(GetSafeHwnd(), LVS_EX_FULLROWSELECT);

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

void MessageEditorListCtrl::AddNewItem()
{
    // Add a new entry and begin label edit. Use the first free index.
    bool used[256] = { };
    for (auto &define : _source->GetDefines())
    {
        if (define.second < ARRAYSIZE(used))
        {
            used[define.second] = true;
        }
    }
    size_t newValue = ARRAYSIZE(used);
    for (size_t i = 1; i < ARRAYSIZE(used); i++)    // Skip zero
    {
        if (!used[i])
        {
            newValue = i;
            break;
        }
    }

    if (newValue < ARRAYSIZE(used))
    {
        size_t newIndex = _source->AddDefine(_source->MandatoryPrefix + "NEWITEM", (uint16_t)newValue);
        _Populate();
        SetFocus();
        EditLabel((int)newIndex);
        _removeIfFailEdit = true;
    }
}