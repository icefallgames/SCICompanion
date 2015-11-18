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

// CGameExplorerView.cpp : implementation of the CGameExplorerView class
//

#include "stdafx.h"
#include "GameExplorerView.h"
#include "GameExplorerFrame.h"
#include "ResourceListDoc.h"
#include "ResourceListView.h"
#include "ResourcePicListView.h"
#include "RasterResourceListView.h"
#include "resource.h"
#include "AppState.h"
#include "Vocab000.h"
#include "VocabDoc.h"
#include "PicDoc.h"
#include "PicOperations.h"
#include "Pic.h"
#include "TextDoc.h"
#include "MessageDoc.h"
#include "Sound.h"
#include "SoundDoc.h"
#include "NewRasterResourceDocument.h"
#include "View.h"
#include "CObjectWrap.h"
#include "PaletteDoc.h"
#include "PaletteView.h"
#include "SoundUtil.h"
#include "Audio.h"
#include <regex>
#include "ResourceBlob.h"
#include "ImageUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef CDocument*(* PFNRESOURCEOPEN )(const ResourceBlob *pData);
CDocument* g_OpenScript(const ResourceBlob *pData)
{
    appState->OpenScript(pData->GetName(), pData);
    return nullptr;
}

CDocument* g_OpenView(const ResourceBlob *pData)
{
    CMultiDocTemplate *pTemplate = appState->GetViewTemplate();
    if (pTemplate)
    {
        CNewRasterResourceDocument *pDocument = static_cast<CNewRasterResourceDocument*>(pTemplate->OpenDocumentFile(nullptr, TRUE));
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenText(const ResourceBlob *pData)
{
    // Create the text
    CMultiDocTemplate *pTemplate = appState->GetTextTemplate();
    if (pTemplate)
    {
        CTextDoc *pDocument = (CTextDoc*)pTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetTextResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenSound(const ResourceBlob *pData)
{
    // Create the text
    CMultiDocTemplate *pTemplate = appState->GetSoundTemplate();
    if (pTemplate)
    {
        CSoundDoc *pDocument = (CSoundDoc*)pTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetSoundResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenVocab(const ResourceBlob *pData)
{
    if (pData->GetNumber() == pData->GetVersion().MainVocabResource)
    {
        // Create the vocab000
        CMultiDocTemplate *pTemplate = appState->GetVocabTemplate();
        if (pTemplate)
        {
            CVocabDoc *pDocument = (CVocabDoc*)pTemplate->OpenDocumentFile(nullptr, TRUE);
            if (pDocument)
            {
                unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
                pDocument->SetVocabResource(move(resource), pData->GetChecksum());
                return pDocument;
            }
        }
    }
    else
    {
        AfxMessageBox(TEXT("Only the main vocab resource (000 or 900, depending on the game) is editable."), MB_OK | MB_APPLMODAL);
    }
    return nullptr;
}


CDocument* g_OpenFont(const ResourceBlob *pData)
{
    CMultiDocTemplate *pTemplate = appState->GetFontTemplate();
    if (pTemplate)
    {
        CNewRasterResourceDocument *pDocument = (CNewRasterResourceDocument*)pTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenPic(const ResourceBlob *pData)
{
    // Get the document template, so we can create a new CPicDoc.
    CDocTemplate *pDocTemplate = appState->GetPicTemplate();
    if (pDocTemplate)
    {
        // and create the pic resource for it.
        CPicDoc *pDocument = (CPicDoc*)pDocTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetEditPic(appState->GetResourceMap().GetDependencyTracker(), move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenCursor(const ResourceBlob *pData)
{
    CMultiDocTemplate *pTemplate = appState->GetCursorTemplate();
    if (pTemplate)
    {
        CNewRasterResourceDocument *pDocument = (CNewRasterResourceDocument*)pTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenMessage(const ResourceBlob *pData)
{
    // Create the text
    CMultiDocTemplate *pTemplate = appState->GetMessageTemplate();
    if (pTemplate)
    {
        CMessageDoc *pDocument = (CMessageDoc*)pTemplate->OpenDocumentFile(nullptr, TRUE);
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetMessageResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenPalette(const ResourceBlob *pData)
{
    CMultiDocTemplate *pTemplate = appState->GetPaletteTemplate();
    if (pTemplate)
    {
        CPaletteDoc *pDocument = static_cast<CPaletteDoc*>(pTemplate->OpenDocumentFile(nullptr, TRUE));
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetResource(move(resource), pData->GetChecksum());
            return pDocument;
        }
    }
    return nullptr;
}

CDocument* g_OpenAudio(const ResourceBlob *pData)
{
    /*
    CMultiDocTemplate *pTemplate = appState->GetViewTemplate();
    if (pTemplate)
    {
        CNewRasterResourceDocument *pDocument = static_cast<CNewRasterResourceDocument*>(pTemplate->OpenDocumentFile(nullptr, TRUE));
        if (pDocument)
        {
            unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*pData);
            pDocument->SetResource(move(resource), pData->GetChecksum());
        }
    }*/
    return nullptr;
}


//
// These are used by the individual views, as well as by
// the OpenResource function.
// This list corresponds to ResourceType enumeration.
//
PFNRESOURCEOPEN g_openFunctions[] =
{
    g_OpenView,
    g_OpenPic,
    g_OpenScript,
    g_OpenText,
    g_OpenSound,
    nullptr,
    g_OpenVocab,
    g_OpenFont,
    g_OpenCursor,
    nullptr,    // patch
    nullptr,    // bitmap
    g_OpenPalette,
    nullptr,
    g_OpenAudio,
    nullptr,
    g_OpenMessage,
    nullptr,
};

BOOL OpenResource(const ResourceBlob *pData, bool setModified)
{
    BOOL fRet = FALSE;
    ResourceType iType = pData->GetType();
    if ((int)iType < ARRAYSIZE(g_openFunctions))
    {
        PFNRESOURCEOPEN pfnOpen = g_openFunctions[(int)iType];
        if (pfnOpen)
        {
            CDocument *newDoc = (*pfnOpen)(pData);
            if (newDoc && setModified)
            {
                newDoc->SetModifiedFlag(TRUE);
            }
            fRet = TRUE;
        }
        else
        {
            TCHAR szMsg[MAX_PATH];
            StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("%s resources are not supported for editing with this program."), GetResourceInfo(iType).pszTitleDefault);
            AfxMessageBox(szMsg, MB_ERRORFLAGS);
        }
    }
    else
    {
        AfxMessageBox(TEXT("Invalid resource."), MB_ERRORFLAGS);
    }
    return fRet;

}

CRuntimeClass *g_listClasses[] =
{
    RUNTIME_CLASS(CRasterResourceListCtrl),
    RUNTIME_CLASS(CResourcePicListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CRasterResourceListCtrl),
    RUNTIME_CLASS(CRasterResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CRasterResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),   
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
    RUNTIME_CLASS(CResourceListCtrl),
};

// CGameExplorerView

IMPLEMENT_DYNCREATE(CGameExplorerView, CView)

BEGIN_MESSAGE_MAP(CGameExplorerView, CView)
    ON_COMMAND(ID_VIEWDETAILS, OnViewDetails)
    ON_COMMAND(ID_VIEWTHUMBNAILS, OnViewThumbnails)
    ON_UPDATE_COMMAND_UI(ID_VIEWDETAILS, OnUpdateViewCommands)
    ON_UPDATE_COMMAND_UI(ID_VIEWTHUMBNAILS, OnUpdateViewCommands)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)
    ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateDelete)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTASRESOURCE, OnUpdateDelete)
    ON_UPDATE_COMMAND_UI(ID_VIEWITEM_VIEWRAWDATA, OnUpdateDelete)
    ON_COMMAND(ID_EDIT_RENAME, OnRename)
    ON_COMMAND(ID_FILE_EXPORTASRESOURCE, OnExtractResources)
    ON_COMMAND(ID_VIEWITEM_VIEWRAWDATA, OnViewRawData)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_RESCOUNT, OnUpdateItemCount)
END_MESSAGE_MAP()

// CGameExplorerView construction/destruction

CGameExplorerView::CGameExplorerView()
{
    _iSelectedTab = ResourceType::None;
    _bFirstSizing = TRUE;
}

void CGameExplorerView::OnDraw(CDC *pDC)
{
    // We're a plain old CView, but we don't draw anything, since our children take
    // up all our space.
}

void CGameExplorerView::OnViewDetails()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView)
    {
        pView->ChangeViewMode(LVS_REPORT);
    }
}

void CGameExplorerView::OnViewThumbnails()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView)
    {
        pView->ChangeViewMode(LVS_ICON);
    }
}

void CGameExplorerView::OnUpdateViewCommands(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView)
    {
        int iView = pView->GetViewMode();

        if (pCmdUI->m_nID == ID_VIEWTHUMBNAILS)
        {
            pCmdUI->SetCheck((iView == LVS_ICON) ? 1 : 0);
        }
        else if (pCmdUI->m_nID == ID_VIEWDETAILS)
        {
            pCmdUI->SetCheck((iView == LVS_REPORT) ? 1 : 0);
        }
    }
}


int CGameExplorerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void CGameExplorerView::OnSize(UINT nType, int cx, int cy)
{
    if (_bFirstSizing && (cx > 0) && (cy > 0))
    {
        // Select the first
        CResourceListDoc *pDoc = GetDocument();
        pDoc->ShowResourceType(ResourceType::View);
        _bFirstSizing = FALSE;
    }

    // Set the size of the currently selected tab:
    _SyncSize();
}

void CGameExplorerView::_SyncSize()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView && pView->GetSafeHwnd())
    {
        RECT rc;
        GetClientRect(&rc); // They occupy the full real-estate
        pView->MoveWindow(&rc);
    }
}

CResourceListCtrl *CGameExplorerView::_GetSelectedView()
{
    if (((int)_iSelectedTab >= 0) && ((int)_iSelectedTab < (int)ARRAYSIZE(_lists)))
    {
        return _lists[(int)_iSelectedTab].get();
    }
    return nullptr;
}

void CGameExplorerView::_OnSelChange(ResourceType iSelectedTab)
{
    if (_iSelectedTab != iSelectedTab)
    {
        _iSelectedTab = iSelectedTab;

        // Tell our frame
        CGameExplorerFrame *pFrame = static_cast<CGameExplorerFrame*>(GetParentFrame());
        if (pFrame)
        {
            pFrame->OnResourceTypeShown(_iSelectedTab);
        }
        CResourceListDoc *pDoc = GetDocument();
        if (pDoc)
        {
            pDoc->ShowResourceType(_iSelectedTab);
        }
        
        int listIndex = (int)iSelectedTab;
        assert(listIndex < (int)ARRAYSIZE(_lists));
        if (_lists[listIndex].get() == nullptr)
        {
            _lists[listIndex].reset((CResourceListCtrl*)g_listClasses[listIndex]->CreateObject());
            // First time this tab has been shown. Initialize and create the control.

            _lists[listIndex]->SetDocument(GetDocument());
            _lists[listIndex]->SetType(iSelectedTab);
            _lists[listIndex]->SetOpenFunction(g_openFunctions[listIndex]);
            
            RECT rc;
            GetClientRect(&rc);
            _lists[listIndex]->Create(WS_CHILD | WS_VISIBLE, rc, this, 0);
            _lists[listIndex]->UpdateView();
        }

        if (_lists[listIndex])
        {
            // Show this one:
            _SyncSize();  assert(_iSelectedTab == iSelectedTab);
            _lists[listIndex]->ShowWindow(SW_SHOW);

            // And hide the others:
            for (int i = 0; i < (int)ARRAYSIZE(_lists); i++)
            {
                if ((i != listIndex) && (_lists[i]->GetSafeHwnd()))
                {
                    _lists[i]->ShowWindow(SW_HIDE);
                }
            }
        }
    }
}

void CGameExplorerView::OnDelete()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView && !pView->InLabelEdit())
    {
        pView->OnDelete();
    }
}

void CGameExplorerView::OnExtractResources()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView && !pView->InLabelEdit())
    {
        pView->OnExtractResources();
    }
}

void CGameExplorerView::OnViewRawData()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView && !pView->InLabelEdit())
    {
        pView->OnViewRawData();
    }
}

void CGameExplorerView::OnRename()
{
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView && !pView->InLabelEdit())
    {
        pView->OnRename();
    }
}


void CGameExplorerView::OnUpdateDelete(CCmdUI *pCmdUI)
{
    BOOL bEnable = FALSE;
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView && !pView->InLabelEdit())
    {
        bEnable = (pView->GetSelectedItem() != -1);
    }
    pCmdUI->Enable(bEnable);
}

void CGameExplorerView::OnUpdateItemCount(CCmdUI *pCmdUI)
{
    // We need to show something useful, so how about how many items are in the view.
    CResourceListCtrl *pView = _GetSelectedView();
    if (pView)
    {
        TCHAR szBuf[MAX_PATH];
        int iSelectedTab = (int)_iSelectedTab;
        ASSERT(iSelectedTab < ARRAYSIZE(g_resourceInfo));
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d %s resources"), pView->GetItemCount(), g_resourceInfo[iSelectedTab].pszTitleDefault);
        pCmdUI->SetText(szBuf);
    }
}

BOOL CGameExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
    return __super::PreCreateWindow(cs);
}


void CGameExplorerView::OnInitialUpdate()
{
    appState->ShowResourceType(ResourceType::View);
    _dropTarget.Register(this);
    __super::OnInitialUpdate();
}


void CGameExplorerView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    ResourceMapChangeHint hint = GetHint<ResourceMapChangeHint>(pHint);

    if (IsFlagSet(hint, ResourceMapChangeHint::Added | ResourceMapChangeHint::Replaced | ResourceMapChangeHint::Deleted))
    {
        // Go tell each of our listviews that a resource was added (or deleted)
        ResourceBlob *pData = UnwrapObject<ResourceBlob*>(pHint);
        // If we got a resource data, be selective, and only tell the view that represents that resource type
        int iType = (int)pData->GetType();
        assert(iType < (int)ARRAYSIZE(_lists));
        if (_lists[iType])
        {
            _lists[iType]->OnUpdate(lHint, pHint);
        }
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::Change | ResourceMapChangeHint::Image))
    {
        // Tell everyone.
        for (int i = 0; i < (int)ARRAYSIZE(_lists); i++)
        {
            if (_lists[i])
            {
                _lists[i]->OnUpdate(lHint, pHint);
            }
        }
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::Type))
    {
        int iType = (int)UnwrapObject<ResourceType>(pHint);
        if ((iType >= 0) && (iType < (int)ARRAYSIZE(_lists)) && _lists[iType])
        {
            // Tell the appropriate list (assuming it exists) to update
            _lists[iType]->OnUpdate(lHint, pHint);
        }
    }
    else if (IsFlagSet(hint, ResourceMapChangeHint::ShowType))
    {
        int iType = (int)UnwrapObject<ResourceType>(pHint);
        if ((iType >= 0) && (iType < (int)ARRAYSIZE(_lists)))
        {
            _OnSelChange((ResourceType)iType);
        }
    }
}

BOOL IsResourceFileName(PCTSTR pszFileName, int *piNumber, std::string &name)
{
    SCIVersion version = appState->GetVersion();
    std::smatch cm;
    std::string filename = pszFileName;
    return MatchesResourceFilenameFormat(filename, version, piNumber, name);
}

bool _IsBitmapFile(PCSTR pszFileName)
{
    return (0 == _strcmpi(PathFindExtension(pszFileName), ".bmp"));
}

//
// Drops the files in pDropFiles into the game.
//
void DropResourceFiles(CArray<CString, CString&> *pDropFiles)
{
    // Be sparing in our DeferAppends, since if they are just dropping one file,
    // we'll be nice and highlight it in red.  If we Begin/End DeferAppend, then we'll lose
    // this information (since we completely reload the resource map for that type)
    DeferResourceAppend defer(appState->GetResourceMap(), pDropFiles->GetCount() > 1);

    for (int i = 0; i < pDropFiles->GetCount(); i++)
    {
        int iNumber;
        std::string resNameFromFilename;
 
        if (_IsBitmapFile(pDropFiles->GetAt(i)))
        {
            // It's a bmp file... it might have a resource encoded in it.
            std::unique_ptr<ResourceBlob> data = Load8BitBmp(appState->GetVersion(), (PCSTR)pDropFiles->GetAt(i));
            if (data)
            {
                char szName[MAX_PATH];
                StringCchCopy(szName, ARRAYSIZE(szName), PathFindFileName(pDropFiles->GetAt(i)));
                *PathFindExtension(szName) = 0; // cut off extension.
                data->SetName(szName);
                appState->GetResourceMap().AppendResourceAskForNumber(*data, false);
            }
            else
            {
                AfxMessageBox("There doesn't appear to be an SCI resource encoded in this .bmp file.", MB_ERRORFLAGS);
            }
        }
        else if (IsWaveFile(pDropFiles->GetAt(i)) && (appState->GetVersion().SoundFormat == SoundFormat::SCI1))
        {
            // We can add wave files to SCI1+ games
            try
            {
                AddWaveFileToGame((PCSTR)pDropFiles->GetAt(i));
            }
            catch (std::exception &e)
            {
                AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
            }
        }
        else if (IsResourceFileName(PathFindFileName(pDropFiles->GetAt(i)), &iNumber, resNameFromFilename))
        {
            ResourceBlob data;
            if (SUCCEEDED(data.CreateFromFile(nullptr, (PCSTR)pDropFiles->GetAt(i), appState->GetVersion(), appState->GetVersion().DefaultVolumeFile, iNumber)))
            {
                // Before adding it, check to see if this resource number already exists.
                bool askForNumber = (iNumber == -1);
                if (!resNameFromFilename.empty())
                {
                    data.SetName(resNameFromFilename.c_str());
                }
                std::string existingName;
                if (iNumber != -1)
                {
                    if (appState->GetResourceMap().DoesResourceExist(data.GetType(), iNumber, &existingName))
                    {
                        if (resNameFromFilename.empty())
                        {
                            data.SetName(existingName.c_str());
                        }
                        askForNumber = (IDYES == AfxMessageBox(fmt::format("Resource {0} is already been used. Use a different resource number?", iNumber).c_str(), MB_OK | MB_YESNO));
                    }
                }

                if (askForNumber)
                {
                    appState->GetResourceMap().AppendResourceAskForNumber(data, true);
                }
                else
                {
                    appState->GetResourceMap().AppendResource(data);
                }
            }
        }
    }
    defer.Commit();
}

//
// Fills pFileList with filenames from the dataobject that meet "being a resource" requirements.
// Returns TRUE if there was at least one that met this requirement.
//
BOOL GetDropFiles(COleDataObject *pDataObject, CArray<CString, CString&> *pFileList)
{
    BOOL fRet = FALSE;
    // Get the HDROP data from the data object.
    GlobalAllocGuard globalAlloc(pDataObject->GetGlobalData(CF_HDROP));
    if (globalAlloc.Global)
    {
        GlobalLockGuard<DROPFILES*> globalLock(globalAlloc);
        DROPFILES *pDropFiles = globalLock.Object;
        if (pDropFiles)
        {
            // We have to do different things depending on whether it's unicode or ansi.
            if (pDropFiles->fWide)
            {
                PWSTR pwsz = (PWSTR)((BYTE*)pDropFiles + pDropFiles->pFiles);
                while (*pwsz)
                {
                    CString strFile;
                    PSTR psz = strFile.GetBuffer(lstrlenW(pwsz) + 1);
                    WideCharToMultiByte(CP_ACP, 0, pwsz, -1, psz, lstrlenW(pwsz) + 1, nullptr, nullptr);
                    PTSTR pszFileName = PathFindFileName(strFile);
                    int iNumber;
                    std::string resNameFromFilename;
                    if (IsResourceFileName(pszFileName, &iNumber, resNameFromFilename) || _IsBitmapFile(pszFileName) || IsWaveFile(pszFileName))
                    {
                        pFileList->Add(strFile);
                        fRet = TRUE;
                    }
                    strFile.ReleaseBuffer();
                    pwsz += lstrlenW(pwsz);
                }
            }
            else
            {
                PSTR psz = (PSTR)((BYTE*)pDropFiles + pDropFiles->pFiles);
                while (*psz)
                {
                    PTSTR pszFileName = PathFindFileName(psz);
                    int iNumber;
                    std::string resNameFromFilename;
                    if (IsResourceFileName(pszFileName, &iNumber, resNameFromFilename) || _IsBitmapFile(pszFileName))
                    {
                        CString strFile = psz;
                        pFileList->Add(strFile);
                        fRet = TRUE;
                    }
                    psz += lstrlen(psz);
                }
            }
        }
    }    
    return fRet;
}

DROPEFFECT CGameExplorerView::OnDragEnter(COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
    DROPEFFECT effect = DROPEFFECT_NONE;
    _dropFiles.RemoveAll();
    // Get the HDROP data from the data object.
    GlobalAllocGuard allocGuard(pDataObject->GetGlobalData(CF_HDROP));
    if (allocGuard.Global)
    {
        GlobalLockGuard<DROPFILES*> globalLock(allocGuard);
        if (globalLock.Object)
        {
            if (GetDropFiles(pDataObject, &_dropFiles))
            {
                effect = DROPEFFECT_COPY;
            }
        }
    }
    return effect;
}
DROPEFFECT CGameExplorerView::OnDragOver(COleDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
    DROPEFFECT effect = DROPEFFECT_NONE;
    if (!_dropFiles.IsEmpty())
    {
        effect = DROPEFFECT_COPY;
    }
    return effect;

}
void CGameExplorerView::OnDragLeave()
{
    _dropFiles.RemoveAll();
}

BOOL CGameExplorerView::OnDrop(COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point)
{
    BOOL fRet = FALSE;
    if (dropEffect == DROPEFFECT_COPY)
    {
        _dropFiles.RemoveAll();
        // Get the HDROP data from the data object.
        HGLOBAL hg = pDataObject->GetGlobalData(CF_HDROP);
        if (hg)
        {
            DROPFILES *pDropFiles = (DROPFILES*)(HDROP)GlobalLock(hg);
            if (pDropFiles )
            {
                GetDropFiles(pDataObject, &_dropFiles);
                GlobalUnlock(hg);
            }
        }

        if (!_dropFiles.IsEmpty())
        {
            // Do the drop.
            DropResourceFiles(&_dropFiles);
        }
    }
    return fRet;
}



// CGameExplorerView diagnostics

#ifdef _DEBUG
void CGameExplorerView::AssertValid() const
{
    __super::AssertValid();
}

void CGameExplorerView::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}

CResourceListDoc* CGameExplorerView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CResourceListDoc)));
    return (CResourceListDoc*)m_pDocument;
}
#endif //_DEBUG


// CGameExplorerView message handlers
