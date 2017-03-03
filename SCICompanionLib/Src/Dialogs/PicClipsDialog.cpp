/***************************************************************************
Copyright (c) 2017 Philip Fortier

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***************************************************************************/

#include "stdafx.h"
#include "AppState.h"
#include "PicClipsDialog.h"
#include "Components.h"
#include "PicCommands.h"
#include "PicOperations.h"
#include <filesystem>
#include <regex>
#include <format.h>

using namespace std::tr2::sys;
using namespace fmt;
using namespace std;

const char *PicClipsExtension = ".picclip";

string RemoveExtension(const string &fullname)
{
    size_t lastindex = fullname.find_last_of(".");
    return fullname.substr(0, lastindex);
}

PicClip::PicClip(const std::string &name, const sRECT &bounds, std::unique_ptr<std::vector<PicCommand>> &&commands)
{
    this->Name = name;
    this->Commands = move(commands);
    this->Bounds.left = bounds.left;
    this->Bounds.right = bounds.right;
    this->Bounds.top = bounds.top;
    this->Bounds.bottom = bounds.bottom;
}

// PicClipsDialog dialog

PicClipsDialog::PicClipsDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(PicClipsDialog::IDD, pParent), _initialized(false), _number(0), _inLabelEdit(false)
{
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPALETTE), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPALETTE));
}

BOOL PicClipsDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

void PicClipsDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, ID_SAVE, m_wndSave);
    DDX_Control(pDX, IDC_LISTCLIPS, m_wndClips);
    DDX_Control(pDX, IDC_COMBOFILES, m_wndFiles);


    if (!_initialized)
    {
        _initialized = true;
        _InitClipsList();
        _PopulateFilesList();
    }
    //DDX_Control(pDX, IDOK, m_wndOK);
}


BEGIN_MESSAGE_MAP(PicClipsDialog, CExtResizableDialog)
    ON_BN_CLICKED(ID_SAVE, &PicClipsDialog::OnBnClickedSave)
    ON_COMMAND(ID_EDIT_PASTE, OnPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateCopyPaste)
    ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LISTCLIPS, &OnLvnBeginlabeleditListscripts)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LISTCLIPS, &OnLvnEndlabeleditListscripts)
    ON_CBN_SELCHANGE(IDC_COMBOFILES, &OnCbnSelchangeFiles)
END_MESSAGE_MAP()

void PicClipsDialog::OnLvnBeginlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    _inLabelEdit = true;
    *pResult = 0;
}


void PicClipsDialog::OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

    if (pDispInfo->item.pszText)
    {
        _clips[pDispInfo->item.iItem]->Name = pDispInfo->item.pszText;
        *pResult = 1;
    }
    else
    {
        *pResult = 0;
    }

    _inLabelEdit = false;
}


int _GetSelectedItem(CListCtrl &ctrl)
{
    POSITION pos = ctrl.GetFirstSelectedItemPosition();
    int selected = -1;
    if (pos != NULL)
    {
        while (pos)
        {
            int nItem = ctrl.GetNextSelectedItem(pos);
            selected = nItem;
        }
    }
    return selected;
}

std::vector<PicCommand> emptyCommands;

const std::vector<PicCommand> &PicClipsDialog::GetCurrentClip(CRect &boundsOut)
{
    int index = _GetSelectedItem(m_wndClips);
    if ((index < 0) || (index > (int)_clips.size()))
    {
        boundsOut = CRect();
        return emptyCommands;
    }
    boundsOut = _clips[index]->Bounds;
    return *_clips[index]->Commands;
}

void PicClipsDialog::MoveSelection(int delta)
{
    int index = _GetSelectedItem(m_wndClips);
    index += delta;
    int cItems = (int)_clips.size();
    index = index % cItems + cItems; // Handle -ve
    index %= cItems;
    m_wndClips.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}

void PicClipsDialog::_PopulateFilesList()
{
    m_wndFiles.Clear();

    m_wndFiles.SetRedraw(FALSE);

    path enumPath = appState->GetResourceMap().Helper().GetPicClipsFolder();
    string regexString = "(\\w+)\\";
    regexString += PicClipsExtension;
    auto matchRSTRegex = std::regex(regexString);
    for (auto it = directory_iterator(enumPath); it != directory_iterator(); ++it)
    {
        const auto &file = it->path();
        std::smatch sm;
        std::string temp = file.filename().string();
        if (!is_directory(file) && std::regex_search(temp, sm, matchRSTRegex) && (sm.size() > 1))
        {
            _fileNames.push_back(file.string());
            m_wndFiles.AddString(RemoveExtension(temp).c_str());
        }
    }

    m_wndFiles.SetRedraw(TRUE);
}


void PicClipsDialog::OnBnClickedSave()
{
    string picClipsFolder = appState->GetResourceMap().Helper().GetPicClipsFolder();

    EnsureFolderExists(picClipsFolder, false);

    path savePath = picClipsFolder;
    CString strText;
    m_wndFiles.GetWindowTextA(strText);
    if (strText.IsEmpty())
    {
        strText = "Untitled";
    }
    strText += PicClipsExtension;
    savePath.append((PCSTR)strText);

    sci::ostream data;

    data << (int)_clips.size();
    for (auto &clip : _clips)
    {
        data << clip->Name;
        data << clip->Bounds;

        sci::ostream inner;
        size_t cCommands = clip->Commands->size();
        // Save them in a clipboard format. Theoretically not portable between machines, but oh well.
        for (size_t i = 0; i < cCommands; i++)
        {
            (*clip->Commands)[i].SerializeForClipboard(&inner);
        }
        data << inner.GetDataSize();
        sci::istream reader = istream_from_ostream(inner);
        transfer(reader, data, inner.GetDataSize());
    }

    try
    {
        ScopedFile holderMap(savePath.string(), GENERIC_WRITE, 0, CREATE_ALWAYS);
        holderMap.Write(data.GetInternalPointer(), data.GetDataSize());

        // Add it to the list if it wasn't overwriting something...
        if (_fileNames.end() == std::find(_fileNames.begin(), _fileNames.end(), savePath.string()))
        {
            _fileNames.push_back(savePath.string());
            m_wndFiles.AddString(RemoveExtension(savePath.filename().string()).c_str());
        }
    }
    catch (std::exception)
    {

    }
}

void PicClipsDialog::OnUpdateCopyPaste(CCmdUI *pCmd)
{
    pCmd->Enable(TRUE);
}


void PicClipsDialog::OnPaste()
{
    auto commands = std::make_unique<vector<PicCommand>>();
    if (CopyRangeFromClipboard(*this, nullptr, *commands))
    {
        std::string name;
        if (_clips.empty())
        {
            // Come up with name based on resource number.
            name = format("pic{0:03}_0", _number);
        }
        else
        {
            std::string prevName = _clips.back()->Name;
            size_t last_index = prevName.find_last_not_of("0123456789");
            string base = prevName.substr(0, last_index + 1);
            string numberString = prevName.substr(last_index + 1);
            int number = StrToInt(numberString.c_str());
            name = format("{0}{1}", base, number + 1);
        }

        sRECT rcPasted;
        PastedCommands_GetBounds(size16(320, 190), &(*commands)[0], commands->size(), &rcPasted);


        _clips.push_back(std::make_unique<PicClip>(name, rcPasted, move(commands)));
        _UpdateList();
    }
}

const int TheNameColumn = 0;

struct
{
    char *text;
    int width;
    int id;
}
c_ClipsColumns[] =
{
    { "Name", 150, TheNameColumn },
};

void PicClipsDialog::_InitClipsList()
{
    for (int i = 0; i < ARRAYSIZE(c_ClipsColumns); i++)
    {
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        col.iOrder = i;
        col.iSubItem = c_ClipsColumns[i].id;
        col.pszText = c_ClipsColumns[i].text;
        col.cx = c_ClipsColumns[i].width;
        col.fmt = LVCFMT_LEFT;
        m_wndClips.InsertColumn(i, &col);
    }
}

void PicClipsDialog::_UpdateList()
{
    SetRedraw(FALSE);
    m_wndClips.DeleteAllItems();
    int itemNumber = 0;
    for (auto &clip : _clips)
    {
        string name = clip->Name;
        LVITEM item = {};
        item.mask = LVIF_TEXT;
        item.pszText = const_cast<LPSTR>(name.c_str());
        item.iItem = itemNumber;
        item.iSubItem = TheNameColumn;
        m_wndClips.InsertItem(&item);
        itemNumber++;
    }
    SetRedraw(TRUE);
    m_wndClips.Invalidate();
}

void PicClipsDialog::OnCbnSelchangeFiles()
{
    int curSel = m_wndFiles.GetCurSel();
    if (curSel != CB_ERR)
    {
        try
        {
            // Load clips from file.
            std::string filename = _fileNames[curSel];
            ScopedFile scoped(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
            sci::streamOwner streamOwner(scoped.hFile);
            sci::istream reader = streamOwner.getReader();

            std::vector<std::unique_ptr<PicClip>> newClips;
            int count;
            reader >> count;
            for (int i = 0; i < count; i++)
            {
                std::unique_ptr<PicClip> clip = make_unique<PicClip>();
                reader >> clip->Name;
                reader >> clip->Bounds;
                uint32_t innerDataSize;
                reader >> innerDataSize;
                clip->Commands = make_unique<vector<PicCommand>>();
                GetCommandsFromRaw(reader.GetInternalPointer() + reader.tellg(), innerDataSize, nullptr, *clip->Commands);
                newClips.push_back(move(clip));
                reader.skip(innerDataSize);
            }

            _clips.swap(newClips);
            _UpdateList();
        }
        catch (std::exception)
        {
        }
    }
}