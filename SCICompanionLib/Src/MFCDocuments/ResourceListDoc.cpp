// ResourceListDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ResourceListDoc.h"
#include "CObjectWrap.h"

// ResourceListDoc

IMPLEMENT_DYNCREATE(CResourceListDoc, CDocument)

CResourceListDoc::CResourceListDoc() : _shownResourceType(ResourceType::None)
{
    // Add ourselves as a sync
    CResourceMap &map = appState->GetResourceMap();
    map.AddSync(this);
}

BOOL CResourceListDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    BOOL fRet =  __super::OnOpenDocument(lpszPathName);
    appState->GenerateBrowseInfo();
    return fRet;
}

BOOL CResourceListDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

    appState->GenerateBrowseInfo();

	return TRUE;
}

void CResourceListDoc::OnCloseDocument()
{
    std::string strEmpty;
    appState->GetResourceMap().SetGameFolder(strEmpty);

    // Remove ourselves as a sync
    CResourceMap &map = appState->GetResourceMap();
    map.RemoveSync((ISyncResourceMap*)this);

    appState->ResetClassBrowser();

    appState->ClearResourceManagerDoc();

    __super::OnCloseDocument();
}

CResourceListDoc::~CResourceListDoc()
{
}


BEGIN_MESSAGE_MAP(CResourceListDoc, CDocument)
    ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnDisableCommand)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnDisableCommand)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnDisableCommand)
END_MESSAGE_MAP()


// Always disable certain commands when this doc has focus.
// You can't save/saveas or close a resource doc
void CResourceListDoc::OnDisableCommand(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(FALSE);
}

void CResourceListDoc::OnResourceAdded(const ResourceBlob *pData)
{
    // Add this as the most recent resource of this type/number/package
    appState->_resourceRecency.AddResourceToRecency(pData);

    // Heinous re-interpret cast.
    UpdateAllViews(nullptr, 0, &WrapObject(ResourceMapChangeHint::Added, pData));
}

void CResourceListDoc::OnResourceDeleted(const ResourceBlob *pDataDeleted)
{
    // Delete this from the recency
    // It is crucial that this come before the update below.  We will check the recency list
    // for the ResourceBlob passed to UpdateAllViews, and at this point, we want to not find
    // it in the list.
    appState->_resourceRecency.DeleteResourceFromRecency(pDataDeleted);

    // Cast away constness...
    UpdateAllViews(nullptr, 0, &WrapObject(ResourceMapChangeHint::Deleted, pDataDeleted));
}

void CResourceListDoc::OnResourceMapReloaded(bool isInitialLoad)
{
    // Initial load is handled elsewhere (archive)
    if (!isInitialLoad)
    {
        UpdateAllViews(nullptr, 0, &WrapHint(ResourceMapChangeHint::Change));
    }
}

void CResourceListDoc::OnResourceTypeReloaded(ResourceType iType)
{
    UpdateAllViews(nullptr, 0, &WrapObject(ResourceMapChangeHint::Type, iType));
}

void CResourceListDoc::ShowResourceType(ResourceType iType)
{
    if (iType != _shownResourceType)
    {
        _shownResourceType = ValidateResourceType(iType);
        CResourceTypeWrap resourceType(iType);
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapObject(ResourceMapChangeHint::ShowType, iType));
    }
}

// ResourceListDoc diagnostics

#ifdef _DEBUG
void CResourceListDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CResourceListDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// ResourceListDoc serialization

void CResourceListDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // There is nothing to save.
        int x = 0;
    }
    else
    {
        CFile *pFile = ar.GetFile();

        // Set the current directory, so we know where to look for the resource files.
        // If not, clicking on an item in the recent documents list won't work
        CString path = pFile->GetFilePath();
        path.MakeLower();
        int iFileOffset = path.Find(TEXT("\\resource.map"));
        if (iFileOffset > 0)
        {
            path.SetAt(iFileOffset, 0); // Null terminate it

            // Set this folder as our new game folder
            CResourceMap &map = appState->GetResourceMap();
            map.SetGameFolder((PCSTR)path);

            appState->LogInfo(TEXT("Open game: %s"), (PCTSTR)path);

            UpdateAllViews(nullptr, 0, &WrapHint(ResourceMapChangeHint::Change));
        }
        else
        {
            AfxMessageBox(TEXT("SCI game resources must be called resource.map"), MB_OK | MB_ICONEXCLAMATION);
        }
    }
}


// ResourceListDoc commands
