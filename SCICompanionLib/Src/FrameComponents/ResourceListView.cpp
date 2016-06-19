/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/

// ResourceListView.cpp : implementation file
//

//#define MAKE_UNITTEST_FILES 1

#include "stdafx.h"
#include "AppState.h"
#include "ResourceListView.h"
#include "ResourceListDoc.h"
#include "GameExplorerFrame.h"
#include "ResourceEntity.h"
#include "ResourceContainer.h"
#include "CObjectWrap.h"
#include "Text.h"
#include "View.h"
#include "ResourceBlob.h"
#include "format.h"

using namespace std;

// CResourceListCtrl

static const int ColumnName = 0;
static const int ColumnNumber = 1;
static const int ColumnSize = 2;
static const int ColumnPackage = 3;
static const int ColumnEncoding = 4;
static const int ColumnStatus = 5;
static const int ColumnSource = 6;
static const int ColumnInfo = 7;
const struct
{
    PTSTR pszName; // Should be const, but the LVCOLUMN struct accepts only non-const
    int cx;
}
c_ColumnInfo [] =
{
    { "Name", 100 },
    { "Number", 70 },
    { "Size", 70 },
    { "Package", 80 },
    { "Encoding", 80 },
    { "Status", 80 },
    { "Source", 100 },
    { "Info", 100 },
};

IMPLEMENT_DYNCREATE(CResourceListCtrl, CListCtrl)

void GetStatusString(const ResourceBlob &data, TCHAR *pszBuffer, size_t cchBuffer, bool mostRecent);
void GetInfoString(const ResourceBlob &data, TCHAR *pszBuffer, size_t cchBuffer)
{
    *pszBuffer = 0;
}

ResourceBlobWrapper::ResourceBlobWrapper(std::unique_ptr<ResourceBlob> blob) : _blob(std::move(blob)) {}
ResourceBlob *ResourceBlobWrapper::GetBlob()
{
    return _blob.get();
}

CResourceListCtrl::CResourceListCtrl()
{
    _iView = -1; // Invalid viewmode
    _bFirstTime = TRUE;
    _bDidInitialUpdate = FALSE;
    _pfnOpen = NULL;
    _bInLabelEdit = FALSE;
    _bScrolling = FALSE;
    _iSortColumn = -1;
}

CResourceListCtrl::~CResourceListCtrl()
{
}

// Note: We can't put any ON_COMMAND or ON_UPDATE_COMMAND_UI maps here, because this is not a view.
// Put them in ResourceManagerView instead.
BEGIN_MESSAGE_MAP(CResourceListCtrl, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemClick)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDoubleClick)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClicked)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
    ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnNotifyDelete)
    ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnGetInfoTip)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_WM_CONTEXTMENU()
    // XP only...
    //ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, OnBeginScroll)
    //ON_NOTIFY_REFLECT(LVN_ENDSCROLL, OnEndScroll)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#define UNUSED_STATUS_TEXT TEXT("Unused")


void CResourceListCtrl::OnRename()
{
    // Enter label edit.
    int nItem = GetSelectedItem();
    if (nItem != -1)
    {
        EditLabel(nItem);
    }
}


void CResourceListCtrl::OnBeginScroll(NMHDR* pNMHDR, LRESULT* pResult)
{
    _bScrolling = TRUE;
}
void CResourceListCtrl::OnEndScroll(NMHDR* pNMHDR, LRESULT* pResult)
{
    _bScrolling = FALSE;
}


void CResourceListCtrl::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    // When an item is selected, tell the frame so it can update its preview.
    NMLISTVIEW *plv = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
    if ((plv->uChanged & LVIF_STATE) && (plv->uNewState & LVIS_SELECTED))
    {
        CGameExplorerFrame *pFrame = static_cast<CGameExplorerFrame*>(GetParentFrame());
        if (pFrame)
        {
            const ResourceBlob *pData = _GetResourceForItemRealized(plv->iItem);
            if (pData)
            {
                ResourceLoadStatusFlags originalFlags = pData->GetStatusFlags();
                pFrame->OnResourceSelected(*pData);
                _UpdateStatusIfFlagsChanged(*pData, originalFlags, plv->iItem);
            }
        }
    }
}


static int compareCI(const char* s1, const char* s2, size_t n) {
    while (n-- != 0) {
        if (toupper(*s1) < toupper(*s2)) return -1;
        if (toupper(*s1) > toupper(*s2)) return 1;
        ++s1; ++s2;
    }
    return 0;
}

template<int Multiplier>
int CALLBACK ColumnSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    char sz1[100];
    char sz2[100];

    int iColumn = static_cast<int>(lParamSort);
    ResourceBlob *p1 = reinterpret_cast<ResourceBlobWrapper*>(lParam1)->GetBlob();
    ResourceBlob *p2 = reinterpret_cast<ResourceBlobWrapper*>(lParam2)->GetBlob();
    int iRet = 0;
    switch (iColumn)
    {
    case ColumnName:
        iRet = compareCI(p1->GetName().c_str(), p2->GetName().c_str(), min(p1->GetName().length(), p2->GetName().length()));
        break;
    case ColumnNumber:
        iRet = p1->GetNumber() - p2->GetNumber();
        break;
    case ColumnSize:
        iRet = p1->GetLength() - p2->GetLength();
        break;
    case ColumnPackage:
        iRet = p1->GetPackageHint() - p2->GetPackageHint();
        break;
    case ColumnEncoding:
        iRet = p1->GetEncoding() - p2->GetEncoding();
        break;
    case ColumnSource:
        iRet = (int)p1->GetSourceFlags() - (int)p2->GetSourceFlags();
        break;
    case ColumnStatus:
        GetStatusString(*p1, sz1, ARRAYSIZE(sz1), false);
        GetStatusString(*p2, sz2, ARRAYSIZE(sz2), false);
        iRet = lstrcmp(sz1, sz2);
        break;
    case ColumnInfo:
        GetInfoString(*p1, sz1, ARRAYSIZE(sz1));
        GetInfoString(*p2, sz2, ARRAYSIZE(sz2));
        iRet = lstrcmp(sz1, sz2);
        break;
    }
    return iRet * Multiplier;
}

void CResourceListCtrl::OnColumnClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLISTVIEW *plv = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
    ASSERT(plv->iItem == -1);
    _iSortColumn = plv->iSubItem;
    _SortItemsHelper(_iSortColumn, true);
}

void CResourceListCtrl::_SortItemsHelper(int sortColumn, bool toggle)
{
    auto it = _sortOrder.find(sortColumn);
    if (it == _sortOrder.end())
    {
        _sortOrder[sortColumn] = true;
    }
    else if (toggle)
    {
        _sortOrder[sortColumn] = !it->second;
    }
    SortItems(_sortOrder[sortColumn] ? ColumnSort<1> : ColumnSort<-1>, sortColumn);
}

void CResourceListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    *pResult = FALSE;
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    if (plvdi->item.pszText)
    {
        // Validate the name.
        if (IsValidResourceName(plvdi->item.pszText))
        {
            // We have a new name.
            // Get the item.
            ResourceBlob *pData = _GetResourceForItemMetadataOnly(plvdi->item.iItem);
            assert(pData);
            // Put the name in game.ini
            std::string oldName = pData->GetName();
            pData->SetName(plvdi->item.pszText);
            appState->GetResourceMap().AssignName(*pData);

            // Change the name in the view:
            *pResult = TRUE; // This doesn't seem to work.
            SetItemText(plvdi->item.iItem, 0, plvdi->item.pszText);

            if (pData->GetType() == ResourceType::Script)
            {
                // Rename the actual things.
                const GameFolderHelper &helper = appState->GetResourceMap().Helper();
                
                std::string oldScriptFilename = helper.GetScriptFileName(oldName);
                std::string oldObjectFilename = helper.GetScriptObjectFileName(oldName);
                try
                {
                    movefile(oldScriptFilename, helper.GetScriptFileName(plvdi->item.pszText));
                    movefile(oldObjectFilename, helper.GetScriptObjectFileName(plvdi->item.pszText));
                    std::vector<CompileResult> messages;
                    messages.emplace_back(fmt::format("Renamed {0} to {1}", oldScriptFilename, helper.GetScriptFileName(plvdi->item.pszText)));
                    messages.emplace_back(fmt::format("Renamed {0} to {1}", oldObjectFilename, helper.GetScriptObjectFileName(plvdi->item.pszText)));
                    appState->OutputResults(OutputPaneType::Compile, messages);
                }
                catch (std::exception e)
                {
                    AfxMessageBox(e.what(), MB_ICONWARNING | MB_OK);
                }
            }
        }
        else
        {
            DisplayInvalidResourceNameMessage(plvdi->item.pszText);
        }
    }
    _bInLabelEdit = FALSE;
}

void CResourceListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    _bInLabelEdit = TRUE;
}

BOOL CResourceListCtrl::PreTranslateMessage(MSG *pMsg)
{
	if (_bInLabelEdit && (pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST))
	{
        // Fwd the delete key to the edit control
        if ((pMsg->message != WM_CHAR) && (pMsg->wParam == VK_DELETE))
        {
            HWND hwndEdit = reinterpret_cast<HWND>(SendMessage(LVM_GETEDITCONTROL, 0, 0));
            if (hwndEdit)
            {
                ::SendMessage(hwndEdit, pMsg->message, pMsg->wParam, pMsg->lParam);
                return 1; // Don't dispatch message, we handled it.
            }
        }
    }
    return __super::PreTranslateMessage(pMsg);
}

void CResourceListCtrl::OnNotifyDelete(NMHDR* pNMHDR, LRESULT* pResult)
{
    // Free the ResourceBlob that was attached to the item.
    NMLISTVIEW *pnmlv = (NMLISTVIEW*)pNMHDR;
    delete ((ResourceBlobWrapper*)pnmlv->lParam);
}

void CResourceListCtrl::OnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVGETINFOTIP *pGetInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
    const ResourceBlob *pData = reinterpret_cast<ResourceBlobWrapper*>(GetItemData(pGetInfoTip->iItem))->GetBlob();
    if (pData)
    {
        StringCchPrintf(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, "%03d\n%d bytes.", pData->GetNumber(), pData->GetLength());
    }
}

void CResourceListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_RETURN)
    {
        int nItem = GetSelectedItem();
        if (nItem != -1)
        {
            const ResourceBlob *pData = _GetResourceForItemRealized(nItem);
            assert(pData);
            _OnItemDoubleClick(pData);
        }
    }
    else
    {
        __super::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

void CResourceListCtrl::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW *pnmlv = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    NMCUSTOMDRAW *pNMCD = reinterpret_cast<NMCUSTOMDRAW*>(pNMHDR);
    *pResult = 0;
    switch (pNMCD->dwDrawStage)
    {
    case CDDS_PREPAINT:
        {
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;
        }

    case CDDS_ITEMPREPAINT:
        {
            // If this is not the most recent resource of this type/number/package, then
            // color the item grey.
            const ResourceBlob *pData = reinterpret_cast<ResourceBlobWrapper*>(pnmlv->nmcd.lItemlParam)->GetBlob();
            if (!appState->_resourceRecency.IsResourceMostRecent(pData))
            {
                pnmlv->clrText = GetSysColor(COLOR_GRAYTEXT);
            }
            else if (IsFlagSet(pData->GetSourceFlags(), ResourceSourceFlags::PatchFile))
            {
                pnmlv->clrText = RGB(0, 0, 255);
            }
            else
            {
                if (appState->_resourceRecency.WasResourceJustAdded(pData))
                {
                    pnmlv->clrText = RGB(255, 0, 0);
                }
                else
                {
                    pnmlv->clrText = GetSysColor(COLOR_HOTLIGHT);
                }
            }
            break;
        }
    }
}

void CResourceListCtrl::OnContextMenu(CWnd *pWnd, CPoint point)
{
    int iItem = GetSelectedItem();
    CMenu contextMenu; 
    if (iItem != -1)
    {
        if ((point.x == -1) && (point.y == -1))
        {
            // Activated by keyboard.  Make the menu appear near the top left of the main selected item.
            CRect rc;
            if (GetItemRect(iItem, &rc, LVIR_BOUNDS))
            {
                point = rc.TopLeft();
                point.Offset(5, 5);
                ClientToScreen(&point);
            }
        }
        contextMenu.LoadMenu(IDR_MENUVIEWITEM);
    }
    else
    {
        if ((point.x == -1) && (point.y == -1))
        {
            // Activated by keyboard.  Make the menu appear at the top left of the view
            point.x = 0;
            point.y = 0;
            ClientToScreen(&point);
        }
        contextMenu.LoadMenu(IDR_MENUVIEWBG);
    }

    CMenu* pTracker; 
    pTracker = contextMenu.GetSubMenu(0); 
    if (pTracker)
    {
        //ClientToScreen(&point);
        pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x , point.y , AfxGetMainWnd()); 
    }
    //has always to be called (function of base class)
    __super::OnContextMenu(pWnd, point);
}

void CResourceListCtrl::_OnItemDoubleClick(const ResourceBlob *pData)
{
    if (_pfnOpen)
    {
        (*_pfnOpen)(pData);
    }
}

BOOL CResourceListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
    // We wish to enable label edits.
    cs.style |= LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_AUTOARRANGE;
    cs.style |= WS_TABSTOP;
	return __super::PreCreateWindow(cs);
}


void CResourceListCtrl::OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW *pNMLV = (NMLISTVIEW*)pNMHDR;
    int iViewIndex = pNMLV->iItem;
    if (iViewIndex != -1) // Could be -1 if user clicked on the background.
    {
        const ResourceBlob *pData = _GetResourceForItemRealized(iViewIndex);
        assert(pData);
        _OnItemDoubleClick(pData);
    }
    *pResult = 0;
}


//
// Returns -1 if there is no item selected.
//
int CResourceListCtrl::GetSelectedItem()
{
    int nItem = -1;
    POSITION pos = GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        nItem = GetNextSelectedItem(pos);
    }
    return nItem;
}

//
// BrowseForFolder callback to set default selection
// 
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        const LPITEMIDLIST pidl = appState->GetExportFolder();
        if (pidl)
        {
            SendMessage(hwnd, BFFM_SETSELECTION, FALSE, (LPARAM)pidl);
        }
    }
    return 0;
}

void CResourceListCtrl::OnViewRawData()
{
    DWORD dwCount = 0;
    HRESULT hr = S_OK;
    POSITION pos = GetFirstSelectedItemPosition();
    while (SUCCEEDED(hr) && (pos != NULL))
    {
        int nItem = GetNextSelectedItem(pos);
        const ResourceBlob *pData = _GetResourceForItemRealized(nItem);
        if (pData)
        {
            std::string filename = GetFileNameFor(*pData);
            filename += ".txt";
            if (SUCCEEDED(hr))
            {
                std::string text = GetBinaryDataVisualization(pData->GetData(), pData->GetLength());
                ShowTextFile(text.c_str(), filename);
            }
        }
    }
}

#ifdef MAKE_UNITTEST_FILES
// Quickly generate patch files and bitmaps for unit-testing...

#include "PicDrawManager.h"
#include "Pic.h"
#include "ImageUtil.h"
#include "PaletteOperations.h"

void TempPhil2(const std::string &filename, PicDrawManager &pdm, PicScreen screen, const PaletteComponent *paletteIn)
{
    std::unique_ptr<Cel> cel = pdm.MakeCelFromPic(screen, PicPosition::Final);
    std::unique_ptr<PaletteComponent> palette;
    if (paletteIn && (screen == PicScreen::Visual))
    {
        palette = std::make_unique<PaletteComponent>(*paletteIn);
    }
    else
    {
        // EGA
        palette = std::make_unique<PaletteComponent>();
        memcpy(palette->Colors, g_egaColors, sizeof(g_egaColors));
    }
    Save8BitBmpGdiP(filename.c_str(), *cel, *palette, false);
}

// Temporary stuff to create things for unit tests
void TempPhil(const ResourceBlob &blob, const std::string &patchFileName)
{
    if (blob.GetType() == ResourceType::Pic)
    {
        std::unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(blob);
        PicDrawManager pdm(resource->TryGetComponent<PicComponent>(), resource->TryGetComponent<PaletteComponent>());
        const PicComponent &pic = resource->GetComponent<PicComponent>();
        const PaletteComponent *palette = resource->TryGetComponent<PaletteComponent>();
        std::string filename = patchFileName + "-vis.bmp";
        TempPhil2(filename, pdm, PicScreen::Visual, palette);
        filename = patchFileName + "-pri.bmp";
        TempPhil2(filename, pdm, PicScreen::Priority, palette);
        filename = patchFileName + "-ctl.bmp";
        TempPhil2(filename, pdm, PicScreen::Control, palette);
    }
    else if(blob.GetType() == ResourceType::View)
    {
        std::unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(blob);
    }
}
#endif

void CResourceListCtrl::OnExtractResources()
{
    UINT cCount = GetSelectedCount();
    if (cCount > 1)
    {
        if (SUCCEEDED(CoInitialize(nullptr)))
        {
            BROWSEINFO browseInfo = { 0 };
            browseInfo.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
            browseInfo.lpszTitle = TEXT("Please choose a folder where you would like to export the resources.");
            browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
            browseInfo.lpfn = BrowseCallbackProc;
            LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo);
            if (pidl)
            {
                TCHAR szFolder[MAX_PATH];
                if (SHGetPathFromIDList(pidl, szFolder))
                {
                    DWORD dwCount = 0;
                    HRESULT hr = S_OK;
                    POSITION pos = GetFirstSelectedItemPosition();
                    const ResourceBlob *pData = nullptr; // keep this in scope for below
                    while (SUCCEEDED(hr) && (pos != nullptr))
                    {
                        int nItem = GetNextSelectedItem(pos);
                        pData = _GetResourceForItemRealized(nItem);
                        if (pData)
                        {
                            std::string filename = szFolder;
                            filename = filename + "\\" + GetFileNameFor(*pData);
                            if (SUCCEEDED(hr))
                            {
                                hr = pData->SaveToFile(filename.c_str());
                                if (SUCCEEDED(hr))
                                {
                                    dwCount++;
                                }
                            }
                        }
                    }

                    TCHAR szMsg[MAX_PATH * 2];
                    if (SUCCEEDED(hr))
                    {
                        StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("Success!  Wrote %d resources to %s."), dwCount, szFolder);
                        AfxMessageBox(szMsg, MB_OK | MB_APPLMODAL);
                    }
                    else
                    {
                        // Prepare error.
                        TCHAR szError[MAX_PATH];
                        szError[0] = 0;
                        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, HRESULT_CODE(hr), 0, szError, ARRAYSIZE(szError), NULL);

                        StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("There was an error writing the resource %03d to %s\n%s"), pData->GetNumber(), szFolder, szError);
                        AfxMessageBox(szMsg, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL, 0);
                    }
                }

                appState->SetExportFolder(pidl); // Takes ownership of pidl.
                //CoTaskMemFree(pidl);
            }
            CoUninitialize();
        }
    }
    else if (cCount == 1)
    {
        const ResourceBlob *pData = _GetResourceForItemRealized(GetSelectedItem());
        if (pData)
        {
            std::string filename = GetFileNameFor(*pData);
            std::string filter = GetFileDialogFilterFor(pData->GetType(), pData->GetVersion());
            CFileDialog fileDialog(FALSE, NULL, filename.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, filter.c_str());
            if (IDOK == fileDialog.DoModal())
            {
                CString strFileName = fileDialog.GetPathName();
                HRESULT hr = pData->SaveToFile((PCSTR)strFileName);
                if (FAILED(hr))
                {
                    DisplayFileError(hr, FALSE, strFileName);
                }
#ifdef MAKE_UNITTEST_FILES
                else
                {
                    TempPhil(*pData, (PCSTR)strFileName);
                }
#endif
            }
        }
    }
}

void CResourceListCtrl::OnDelete()
{
    // Bail out for heap...
    if (this->GetType() == ResourceType::Heap)
    {
        if (IDNO == AfxMessageBox("Heap resources are closely tied to script resources - one cannot exist without the other.\nYou can delete both via the script pane.\nContinue deleting just the heap resource anyway?", MB_ICONWARNING | MB_YESNO))
        {
            return;
        }
    }

    if (this->GetType() == ResourceType::AudioMap)
    {
        if (IDNO == AfxMessageBox("Audio map resources are closely tied to the digital audio resources, and are created and managed by SCI Companion automatically. Deleting them may result in corrupt audio or crashes.\nContinue with deleting the map resource anyway?", MB_ICONWARNING | MB_YESNO))
        {
            return;
        }
    }

    bool deleteCompanionAudio = false;
    if ((this->GetType() == ResourceType::Message) && appState->GetVersion().HasSyncResources)
    {
        deleteCompanionAudio = (IDYES == AfxMessageBox("Do you also want to delete any audio resources associated with this message resource(s)?", MB_YESNO | MB_ICONWARNING));
    }

    std::set<int> deletedResourceNumbers;
    UINT cCount = GetSelectedCount();
    if (cCount > 1)
    {
        // Multiple items
        TCHAR szBuffer[MAX_PATH];
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Delete these %d items?"), cCount);
        if (IDYES == AfxMessageBox(szBuffer, MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION))
        {
            // Grab all the selected ResourceBlob's, before we start deleting any items.
            vector<const ResourceBlob*> resourcesTempCopy;
            POSITION pos = GetFirstSelectedItemPosition();
            while (pos != nullptr)
            {
                int nItem = GetNextSelectedItem(pos);
                resourcesTempCopy.push_back(_GetResourceForItemMetadataOnly(nItem));
            }

            for (const ResourceBlob *pData : resourcesTempCopy)
            {
                deletedResourceNumbers.insert(pData->GetNumber());
                assert(pData);
                // We'll get updated a lot here.
                appState->GetResourceMap().DeleteResource(pData);
                // We don't need to free the pData right here.  It will happen when
                // we tell the resource map to delete the resource - upon which we'll get notified that
                // a resource has been deleted.
            }
        }
    }
    else
    {
        int nItem = GetSelectedItem();
        if (nItem != -1)
        {
            const ResourceBlob *pData = _GetResourceForItemMetadataOnly(nItem);
            assert(pData);
            TCHAR szBuffer[MAX_PATH];
            StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Delete %s?"), pData->GetName().c_str());
            if (IDYES == AfxMessageBox(szBuffer, MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION))
            {
                deletedResourceNumbers.insert(pData->GetNumber());
                appState->GetResourceMap().DeleteResource(pData);
            }
        }
    }

    if (deleteCompanionAudio)
    {
        std::vector<std::unique_ptr<ResourceBlob>> audioResourcesToDelete;
        {
            auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::AudioMap, ResourceEnumFlags::IncludeCacheFiles);
            for (auto &blob : *resourceContainer)
            {
                if (deletedResourceNumbers.find(blob->GetNumber()) != deletedResourceNumbers.end())
                {
                    audioResourcesToDelete.push_back(std::move(blob));
                }
            }
        }

        for (auto &blob : audioResourcesToDelete)
        {
            appState->GetResourceMap().DeleteResource(blob.get());
        }
    }
}

void CResourceListCtrl::ChangeViewMode(int iView)
{
    if (iView != _iView)
    {
        _iView = iView;
        LONG_PTR dwStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE); 
        dwStyle &= ~LVS_TYPEMASK;
        dwStyle |= _iView;
        SetWindowLongPtr(m_hWnd, GWL_STYLE, (LONG)dwStyle);
    }
}

void CResourceListCtrl::_InitColumns()
{
    for (int i = 0; i < ARRAYSIZE(c_ColumnInfo); i++)
    {
        // Name
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
        col.iOrder = i;
        col.iSubItem = i;
        col.pszText = c_ColumnInfo[i].pszName;
        col.cx = c_ColumnInfo[i].cx;
        col.fmt = LVCFMT_RIGHT;
        InsertColumn(i, &col);
    }
}

void CResourceListCtrl::_ReevaluateRecency(const ResourceBlob *pData)
{
    int cItems = GetItemCount();
    for (int i = 0; i < cItems; i++)
    {
        const ResourceBlob *pDataCur = _GetResourceForItemMetadataOnly(i);
        if ((pDataCur->GetPackageHint() == pData->GetPackageHint()) &&
            (pDataCur->GetNumber() == pData->GetNumber()))
        {
            // This is just like the item that was added or removed.  So let's re-evaluate its recency.
            bool fMostRecent = appState->_resourceRecency.IsResourceMostRecent(pDataCur);

            // ...by specifying an overlay image (if appropriate)
            LVITEM item = { 0 };
            item.iItem = i;
            item.mask = LVIF_STATE;
            item.state = INDEXTOOVERLAYMASK(fMostRecent ? 0 : OVERLAYINDEX_UNUSED);
            item.stateMask = LVIS_OVERLAYMASK;
            SetItem(&item);

            // ...and by adjusting its status.
            TCHAR szBuf[100];
            szBuf[0] = 0;
            LVITEM itemSub = { 0 };
            itemSub.iItem = i;
            itemSub.iSubItem = ColumnStatus;
            itemSub.mask = LVIF_TEXT;
            itemSub.pszText = szBuf;
            GetStatusString(*pDataCur, szBuf, ARRAYSIZE(szBuf), fMostRecent);
            SetItem(&itemSub);
        }
    }
}


void CResourceListCtrl::_PrepareLVITEM(LVITEM *pItem)
{
    // Subclasses override to provide more LVITEM properties.
}

void CResourceListCtrl::_OnInitListView(int cItems)
{
    // subclasses can override and  must be able to handle multiple calls to this.
}

void CResourceListCtrl::_DeleteMatchingItems(int resourceNumber, int packageNumber, ResourceSourceFlags sourceFlags)
{
    int cItems = GetItemCount();
    // Go backwards so that 
    for (int i = cItems - 1; (i >= 0); i--)
    {
        const ResourceBlob *pData = _GetResourceForItemMetadataOnly(i);
        if ((pData->GetNumber() == resourceNumber) && (pData->GetPackageHint() == packageNumber) && (pData->GetSourceFlags() == sourceFlags))
        {
            DeleteItem(i);
        }
    }
}

void CResourceListCtrl::_DeleteItem(const ResourceBlob *pData)
{
    int iIndex = _GetItemForResource(pData);
    DeleteItem(iIndex);
}

void CResourceListCtrl::_InsertItem(std::unique_ptr<ResourceBlob> pData)
{
    ResourceBlob *blob = pData.get();
    auto wrapper = std::make_unique<ResourceBlobWrapper>(std::move(pData));

    TCHAR szName[100];
    StringCchCopy(szName, ARRAYSIZE(szName), blob->GetName().c_str()); // Copy, since pszText takes a non-const
    LVITEM item = { 0 };
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.iItem = 0;
    item.iSubItem = 0;
    item.pszText = szName;
    item.lParam = (LPARAM)wrapper.release();
    _PrepareLVITEM(&item);
    int iIndex = InsertItem(&item);

    if (iIndex == -1)
    {
        appState->LogInfo(TEXT("Failed to insert item in listview: %s"), szName);    
    }

    // Now the columns
    for (int iSub = 1; iSub < ARRAYSIZE(c_ColumnInfo); iSub++)
    {
        TCHAR szBuf[100];
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = iIndex;
        item.iSubItem = iSub;
        item.pszText = szBuf;
        switch (iSub)
        {
        case ColumnNumber:
            StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%03d"), blob->GetNumber());
            break;
        case ColumnPackage:
            StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%03d"), blob->GetPackageHint());
            break;
        case ColumnSize:
            {
                int iLength = blob->GetLength();
                int iThousandths = (iLength % 1024) * 1000 / 1024;
                int kiloBytes = iLength / 1024;
                int iRoundedHundredths = (iLength % 1024) * 100 / 1024;
                if (iRoundedHundredths * 10 + 5 < iThousandths)
                {
                    iRoundedHundredths++;
                    if (iRoundedHundredths == 100)
                    {
                        kiloBytes++;
                        iRoundedHundredths = 0;
                    }
                }
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d.%02dKB"), kiloBytes, iRoundedHundredths);
            }
            break;

        case ColumnEncoding:
            {
                StringCchCopy(szBuf, ARRAYSIZE(szBuf), blob->GetEncodingString().c_str());
            }
            break;
        case ColumnStatus:
            szBuf[0] = 0;
            GetStatusString(*blob, szBuf, ARRAYSIZE(szBuf), appState->_resourceRecency.IsResourceMostRecent(blob));
            break;

        case ColumnSource:
        {
            ResourceSourceFlags sourceFlags = blob->GetSourceFlags();
            if (IsFlagSet(sourceFlags, ResourceSourceFlags::PatchFile))
            {
                item.pszText = "Patch";
            }
            else if (IsFlagSet(sourceFlags, ResourceSourceFlags::MessageMap))
            {
                item.pszText = "message.map";
            }
            else if (IsFlagSet(sourceFlags, ResourceSourceFlags::Aud))
            {
                item.pszText = "resource.aud";
            }
            else if (IsFlagSet(sourceFlags, ResourceSourceFlags::Sfx))
            {
                item.pszText = "resource.sfx";
            }
            else if (IsFlagSet(sourceFlags, ResourceSourceFlags::AudioCache | ResourceSourceFlags::AudioMapCache))
            {
                item.pszText = "audio cache";
            }
            else
            {
                szBuf[0] = 0;
            }
            break;
        }

        case ColumnInfo:
        {
            szBuf[0] = 0;
            GetInfoString(*blob, szBuf, ARRAYSIZE(szBuf));
            break;
        }

        default:
            StringCchCopy(szBuf, ARRAYSIZE(szBuf), "Test");
            break;
        }
        SetItem(&item);
    }
}

void CResourceListCtrl::_UpdateStatusIfFlagsChanged(const ResourceBlob &data, ResourceLoadStatusFlags originalFlags, int nItem)
{
    if (originalFlags != data.GetStatusFlags())
    {
        TCHAR szBuf[100];
        szBuf[0] = 0;
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = nItem;
        item.iSubItem = ColumnStatus;
        item.pszText = szBuf;
        GetStatusString(data, szBuf, ARRAYSIZE(szBuf), appState->_resourceRecency.IsResourceMostRecent(&data));
        SetItem(&item);
    }
}

void GetStatusString(const ResourceBlob &data, TCHAR *pszBuffer, size_t cchBuffer, bool mostRecent)
{
    bool needsSeparator = false;
    if (!mostRecent)
    {
        StringCchCopy(pszBuffer, cchBuffer, UNUSED_STATUS_TEXT);
        needsSeparator = true;
    }

    if (IsFlagSet(data.GetStatusFlags(), ResourceLoadStatusFlags::DecompressionFailed))
    {
        StringCchCat(pszBuffer, cchBuffer, (needsSeparator ? ", Decompress failed" : "Decompress failed"));
        needsSeparator = true;
    }

    if (IsFlagSet(data.GetStatusFlags(), ResourceLoadStatusFlags::ResourceCreationFailed))
    {
        StringCchCat(pszBuffer, cchBuffer, (needsSeparator ? ", Resource load failed" : "Resource load failed"));
        needsSeparator = true;
    }

    if (IsFlagSet(data.GetStatusFlags(), ResourceLoadStatusFlags::Corrupted))
    {
        StringCchCat(pszBuffer, cchBuffer, (needsSeparator ? ", Corrupt" : "Corrupt"));
        needsSeparator = true;
    }
}

void CResourceListCtrl::UpdateView()
{
    _UpdateEntries();
}

void CResourceListCtrl::OnUpdate(LPARAM lHint, CObject *pHint)
{
    ResourceMapChangeHint hint = GetHint<ResourceMapChangeHint>(pHint);
    if (IsFlagSet(hint, ResourceMapChangeHint::Change))
    {
        _UpdateEntries();
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::Type))
    {
        _UpdateEntries();
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::Added | ResourceMapChangeHint::Replaced))
    {
        ResourceBlob *pData = UnwrapObject<ResourceBlob*>(pHint);

        if (IsFlagSet(hint, ResourceMapChangeHint::Replaced))
        {
            _DeleteMatchingItems(pData->GetNumber(), pData->GetPackageHint(), pData->GetSourceFlags());
        }

        // Make our own copy, since we're storing this away.
        auto pDataClone = std::make_unique<ResourceBlob>(*pData);
        ResourceBlob *pTemp = pDataClone.get();
        _InsertItem(std::move(pDataClone));
        _ReevaluateRecency(pTemp);
        // Sort
        _SortItemsHelper(_iSortColumn, false);
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::Deleted))
    {
        ResourceBlob *pData = UnwrapObject<ResourceBlob*>(pHint);
        // Re-evaluate recency while the pData is still valid.
        _ReevaluateRecency(pData);
        // This will free the memory for pData.
        _DeleteItem(pData);
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::Image))
    {
        _RegenerateImages();
    }
}

ResourceBlob *CResourceListCtrl::_GetResourceForItemMetadataOnly(LPARAM lParam)
{
    return reinterpret_cast<ResourceBlobWrapper*>(lParam)->GetBlob();
}

ResourceBlob *CResourceListCtrl::_GetResourceForItemRealized(LPARAM lParam)
{
    auto blob = reinterpret_cast<ResourceBlobWrapper*>(lParam)->GetBlob();
    blob->EnsureRealized();
    return blob;
}

ResourceBlob *CResourceListCtrl::_GetResourceForItemMetadataOnly(int nItem)
{
    return reinterpret_cast<ResourceBlobWrapper*>(GetItemData(nItem))->GetBlob();
}

ResourceBlob *CResourceListCtrl::_GetResourceForItemRealized(int nItem)
{
    auto blob = reinterpret_cast<ResourceBlobWrapper*>(GetItemData(nItem))->GetBlob();
    blob->EnsureRealized();
    return blob;
}

HRESULT CResourceListCtrl::_UpdateEntries()
{
    HRESULT hr = E_FAIL;
    // Clear out old items.
    DeleteAllItems();

    try
    {
        if (!appState->GetResourceMap().GetGameFolder().empty()) // else no game loaded. We get here during initialization, as frames are created prior to the document.
        {

            // Temporary cache of ResourceBlob's from the resource map enumerator
            vector<unique_ptr<ResourceBlob>> resources;
            ResourceEnumFlags enumFlags = ResourceEnumFlags::NameLookups | ResourceEnumFlags::CalculateRecency;
            if (GetType() == ResourceType::Audio)
            {
                // This is special. We want to prioritize cache files, and if they are present, NOT fallback to the game's resources.
                // SCI 1.1 doesn't allow dupes in general anyway, so we can say "most recent only"
                enumFlags |= ResourceEnumFlags::IncludeCacheFiles | ResourceEnumFlags::MostRecentOnly;
                // And we don't need to bother calculating recency.
                // enumFlags &= ~ResourceEnumFlags::CalculateRecency;
            }
            auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeToFlag(GetType()), enumFlags);
            // Copy the ResourceBlobs into resources, but delay decompression (for performance)
            // REVIEW: We might want to have a wrapper.
            for (auto it = resourceContainer->begin(); it != resourceContainer->end(); ++it)
            {
                resources.push_back(std::move(it.CreateButDelayDecompression()));
            }

            if (!resources.empty())
            {
                appState->LogInfo("Found %d resources for type %x", resources.size(), GetType());
                if (_bFirstTime)
                {
                    _InitColumns();
                    ChangeViewMode(GetDefaultViewMode());
                    _bFirstTime = FALSE;
                }
                _OnInitListView((int)resources.size());
                _bDidInitialUpdate = TRUE;

                SetRedraw(FALSE);

                for (auto &blob : resources)
                {
                    // Ownership of prd is transfered to listview:
                    _InsertItem(std::move(blob));
                }

                // Sort
                _iSortColumn = ColumnNumber;
                _SortItemsHelper(_iSortColumn, false);
                SetRedraw(TRUE);
                hr = S_OK;
            }
            else
            {
                appState->LogInfo("Found zero resources for type %x", GetType());
            }
        }
    }
    catch (std::exception &e)
    {
        AfxMessageBox(fmt::format("Error enumerating items: {}", e.what()).c_str(), MB_OK | MB_ICONERROR);
    }
    return hr;
}

int CResourceListCtrl::_GetItemForResource(const ResourceBlob *pData)
{
    int cItems = GetItemCount();
    bool fFound = false;
    int i = 0;
    for (; !fFound && (i < cItems); i++)
    {
        fFound = (_GetResourceForItemMetadataOnly(i) == pData);
    }
    assert(fFound); // Otherwise someone called us with a bad resource data.
    return i - 1;
}


// CResourceListCtrl diagnostics

#ifdef _DEBUG
void CResourceListCtrl::AssertValid() const
{
	CListCtrl::AssertValid();
}

void CResourceListCtrl::Dump(CDumpContext& dc) const
{
	CListCtrl::Dump(dc);
}


CResourceListDoc* CResourceListCtrl::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CResourceListDoc)));
    return (CResourceListDoc*)m_pDocument;
}

#endif //_DEBUG


// CResourceListCtrl message handlers
