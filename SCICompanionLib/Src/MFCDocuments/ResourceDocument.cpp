// ResourceDocument.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ResourceDocument.h"
#include "ResourceMap.h"
#include "SaveResourceDialog.h"
#include "ResourceEntity.h"
#include "RasterOperations.h"
#include "ResourceEntity.h"
#include <errno.h>

// CResourceDocument

IMPLEMENT_DYNAMIC(CResourceDocument, CDocument)

BEGIN_MESSAGE_MAP(CResourceDocument, CDocument)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_FILE_EXPORTASRESOURCE, OnExportAsResource)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateAlwaysOn)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTASRESOURCE, OnUpdateAlwaysOn)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_RESSIZE, OnUpdateResSize)
END_MESSAGE_MAP()

BOOL CResourceDocument::CanCloseFrame(CFrameWnd* pFrameArg)
{
	// Override this, since we may have more than one view in our childframes,
    // and if we do, the default implementation thinks there is still always
    // still one view around on the document (so it won't call SaveModified).
	return SaveModified();
}

void CResourceDocument::OnFileSave()
{
    if (IsModified())
    {
        DoPreResourceSave(FALSE);
    }
}

void CResourceDocument::OnFileSaveAs()
{
    DoPreResourceSave(TRUE);
}


// return TRUE if ok to continue
BOOL CResourceDocument::SaveModified()
{
	if (!IsModified())
		return TRUE;        // ok to continue

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, this->GetTitle());
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoPreResourceSave(FALSE))
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

void CResourceDocument::_UpdateTitle()
{
    TCHAR sz[20];
    const ResourceEntity *pResource = GetResource();
    if (pResource && (pResource->ResourceNumber != -1))
    {
        StringCchPrintf(sz, ARRAYSIZE(sz), TEXT("%s.%03d"), _GetTitleDefault(), pResource->ResourceNumber);
        // TODO: Use game.ini name for the title.
    }
    else
    {
        StringCchPrintf(sz, ARRAYSIZE(sz), TEXT("%s - new"), _GetTitleDefault());
    }

    // Set the title:
    SetTitle(sz);
    // Also update recency
    _fMostRecent = (appState->_resourceRecency.IsResourceMostRecent(this) != FALSE);
}

//
// Actually go ahead and serialize the resource, just to give an indication of its size!
//
void CResourceDocument::OnUpdateResSize(CCmdUI *pCmdUI)
{
    TCHAR szBuf[MAX_PATH];
    StringCchCopy(szBuf, ARRAYSIZE(szBuf), TEXT("Unknown size"));
    const ResourceEntity *pResource = GetResource();
    if (pResource && pResource->CanWrite())
    {
        sci::ostream serial;
        try
        {
            pResource->WriteToTest(serial, false, pResource->ResourceNumber);
            StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%s: %d bytes"), _GetTitleDefault(), serial.tellp());
        }
        catch (std::exception)
        {
            StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("unknown size"));
        }
    }
    pCmdUI->SetText(szBuf);
}

void _ShowCantSaveMessage()
{
    AfxMessageBox("Saving this kind of resource is not supported.", MB_ERRORFLAGS);
}

BOOL CResourceDocument::DoPreResourceSave(BOOL fSaveAs)
{
    BOOL fRet = FALSE;
    if (appState->GetResourceMap().IsGameLoaded())
    {
        if (!appState->GetResourceMap().CanSaveResourcesToMap())
        {
            AfxMessageBox("Can't save to the version of the resource.map used by this game.", MB_ERRORFLAGS);
        }
        else
        {
            if (v_DoPreResourceSave())
            {
                // const_cast: special case, because we're saving... we're modifying the resource only
                // by giving it a resource package/number
                ResourceEntity *pResource = const_cast<ResourceEntity*>(GetResource());
                if (pResource)
                {
                    if (!pResource->CanWrite())
                    {
                        _ShowCantSaveMessage();
                    }
                    else
                    {
                        bool fCancelled = false;

                        // Make sure we have a package and resource number.
                        int iResourceNumber = pResource->ResourceNumber;
                        int iPackageNumber = pResource->PackageNumber;
                        if (fSaveAs || (iResourceNumber == -1) || (iPackageNumber == -1))
                        {
                            if (iResourceNumber == -1)
                            {
                                iResourceNumber = appState->GetResourceMap().SuggestResourceNumber(pResource->GetType());
                            }
                            // Invoke dialog.
                            SaveResourceDialog srd(true, pResource->GetType());
                            srd.Init(iPackageNumber, iResourceNumber);
                            if (IDOK == srd.DoModal())
                            {
                                iResourceNumber = srd.GetResourceNumber();
                                iPackageNumber = srd.GetPackageNumber();
                            }
                            else
                            {
                                fCancelled = true;
                            }
                        }

                        if (!fCancelled && (iResourceNumber != -1) && (iPackageNumber != -1))
                        {
                            // We're good to go.
                            fRet = _DoResourceSave(iPackageNumber, iResourceNumber);
                            if (fRet)
                            {
                                // We might have a new resource number, so update our title.
                                _UpdateTitle();
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // No game is loaded!  Assume the user wants to export.
        OnExportAsResource();
    }
    return fRet;
}

BOOL CResourceDocument::_DoResourceSave(int iPackageNumber, int iResourceNumber)
{
    // Ignore path name.
    const ResourceEntity *pResource = static_cast<const ResourceEntity *>(GetResource());
    bool saved = false;
    int checksum = 0;
    if (pResource)
    {
        saved = appState->GetResourceMap().AppendResource(*pResource, iPackageNumber, iResourceNumber, NoBase36, &checksum);
    }

    if (saved)
    {
        // If we successfully saved, make sure our resource has these
        // possibly new package/resource numbers. Need to do this prior to _OnSuccessfulSave!
        ResourceEntity *pEntityNC = const_cast<ResourceEntity*>(pResource);
        pEntityNC->PackageNumber = iPackageNumber;
        pEntityNC->ResourceNumber = iResourceNumber;

        _checksum = checksum;
        _OnSuccessfulSave(pResource);
        _UpdateTitle();
    }
    SetModifiedFlag(!saved);
    return saved;
}

void CResourceDocument::OnExportAsBitmap()
{
    const ResourceEntity *pResource = GetResource();
    if (pResource)
    {
        ExportResourceAsBitmap(*pResource);
    }
}

void ExportResourceAsBitmap(const ResourceEntity &resourceEntity)
{
    if (!resourceEntity.CanWrite())
    {
        return;
    }

    sci::ostream serial;
    resourceEntity.WriteToTest(serial, false, resourceEntity.ResourceNumber);
    ResourceBlob data;
    // Bring up the file dialog
    int iNumber = resourceEntity.ResourceNumber;
    if (iNumber == -1)
    {
        iNumber = 0;
    }

    sci::istream readStream = istream_from_ostream(serial);
    data.CreateFromBits(nullptr, resourceEntity.Traits.Type, &readStream, resourceEntity.PackageNumber, iNumber, resourceEntity.Base36Number, appState->GetVersion(), ResourceSourceFlags::PatchFile);
    CBitmap bitmap;
    SCIBitmapInfo bmi;
    BYTE *pBitsDest;
    bitmap.Attach(CreateBitmapFromResource(resourceEntity, nullptr, &bmi, &pBitsDest));
    if ((HBITMAP)bitmap)
    {
        // Now let's encode it...
        if (EncodeResourceInBitmap(data, bmi, pBitsDest))
        {
            // Get a file name
            std::string name = data.GetName();
            name += ".bmp";
            CFileDialog fileDialog(FALSE, nullptr, name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "Bitmaps (*.bitmap)|*.bitmap|All files (*.*)|*.*|");
            if (IDOK == fileDialog.DoModal())
            {
				std::string fileName = (PCSTR)fileDialog.GetPathName();
                if (!Save8BitBmp(fileName, bmi, pBitsDest, SCIResourceBitmapMarker))
                {
                    AfxMessageBox("Unable to save bitmap.", MB_ERRORFLAGS);
                }
            }
        }
        else
        {
            AfxMessageBox("Unable to put resource in bitmap.", MB_ERRORFLAGS);
        }
    }
}

void CResourceDocument::OnExportAsResource()
{
    // Ignore path name.
    const ResourceEntity *pResource = GetResource();
    if (pResource)
    {
        if (!pResource->CanWrite())
        {
            _ShowCantSaveMessage();
        }
        else
        {
            sci::ostream serial;
            bool fSaved = false;
            pResource->WriteToTest(serial, false, pResource->ResourceNumber);
            // Bring up the file dialog
            int iNumber = pResource->ResourceNumber;
            if (iNumber == -1)
            {
                iNumber = 0;
            }

            std::string filename = GetFileNameFor(GetType(), iNumber, pResource->Base36Number, appState->GetVersion());
            std::string filter = _GetFileDialogFilter();
            CFileDialog fileDialog(FALSE, nullptr, filename.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, filter.c_str());
            if (IDOK == fileDialog.DoModal())
            {
                CString strFileName = fileDialog.GetPathName();
                ResourceBlob data;

                sci::istream readStream = istream_from_ostream(serial);
                if (SUCCEEDED(data.CreateFromBits(nullptr, _GetType(), &readStream, pResource->PackageNumber, iNumber, NoBase36, appState->GetVersion(), ResourceSourceFlags::PatchFile)))
                {
                    HRESULT hr = data.SaveToFile((PCSTR)strFileName);
                    if (FAILED(hr))
                    {
                        DisplayFileError(hr, FALSE, strFileName);
                    }
                    fSaved = SUCCEEDED(hr);
                }
            }
            if (fSaved)
            {
                SetModifiedFlag(FALSE);
            }
            // else if we didn't save, we don't want to touch the modified flag.
            // The document may not have been dirty at all.
        }
    }
}

bool CResourceDocument::IsMostRecent() const
{
    return _fMostRecent;
}

int CResourceDocument::GetPackageHint() const
{
    const ResourceEntity *pRes = GetResource();
    if (pRes)
    {
        return pRes->PackageNumber;
    }
    else
    {
        return -1;
    }
}
int CResourceDocument::GetNumber() const
{
    const ResourceEntity *pRes = GetResource();
    if (pRes)
    {
        return pRes->ResourceNumber;
    }
    else
    {
        return -1;
    }
}
uint32_t CResourceDocument::GetBase36() const
{
    const ResourceEntity *pRes = GetResource();
    if (pRes)
    {
        return pRes->Base36Number;
    }
    else
    {
        return NoBase36;
    }

}
ResourceType CResourceDocument::GetType() const
{
    return _GetType();
}

std::string CResourceDocument::_GetFileDialogFilter()
{
    return GetFileDialogFilterFor(_GetType(), appState->GetVersion());
}


// CResourceDocument commands
