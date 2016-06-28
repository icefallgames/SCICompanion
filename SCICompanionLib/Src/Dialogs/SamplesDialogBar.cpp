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

// QuickScripts.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SamplesDialogBar.h"
#include "ScriptDocument.h"
#include "PicOperations.h"
#include "Pic.h"
#include "ResourceListDoc.h"
#include "ResourceEntity.h"
#include "CObjectWrap.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
#include "ResourceBlob.h"

using namespace sci;
using namespace std;

#define MAX_SAMPLES (ID_GOTOVIEW28 - ID_GOTOVIEW1 + 1)

// CSamplesDialogBar dialog
CSamplesDialogBar::CSamplesDialogBar(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CSamplesDialogBar::IDD, pParent)
{
    _bFirstTime = true;
    _pDoc = NULL;
    _samples.resize(MAX_SAMPLES);
}

CSamplesDialogBar::~CSamplesDialogBar()
{
}


void CSamplesDialogBar::_PrepareViewMenu(int cItems)
{
    if (m_wndButtonViews.m_menu == NULL)
    {
        CMenu menu;
        if (menu.LoadMenu(IDR_MENUVIEWS))
        {
            // We only need to do this once...
            m_wndButtonViews.m_menu.Attach(menu.Detach());
        }
    }
    if (m_wndButtonViews.m_menu)
    {
        UINT nID;
        if (_GetMenuItem("placeholder", &m_wndButtonViews.m_menu, &nID))
        {
            CMenu *pMenuSub = m_wndButtonViews.m_menu.GetSubMenu(nID);

            // Clear the menu out first.
            UINT cItemsInMenu = pMenuSub->GetMenuItemCount();
            while (cItemsInMenu > 0)
            {
                pMenuSub->RemoveMenu(0, MF_BYPOSITION);
                cItemsInMenu--;
            }

            // And rebuild
            for (int iIndex = 0; iIndex < cItems; iIndex++)
            {
                MENUITEMINFO mii = { 0 };
                mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_ID | MIIM_STRING;
                mii.wID = ID_GOTOVIEW1 + iIndex;
                mii.dwTypeData = "foo";
                pMenuSub->InsertMenuItem(ID_GOTOVIEW1 + iIndex, &mii, FALSE);
            }
        }
    }
}

void CSamplesDialogBar::_PrepareSamples()
{
    _samples.clear();

    if (_pDoc)
    {
        SCIVersion sampleVersion = (appState->GetVersion().MapFormat <= ResourceMapFormat::SCI0_LayoutSCI1) ? sciVersion0 : sciVersion1_1;

        ResourceType type = _pDoc->GetShownResourceType();
        if (type != ResourceType::None)
        {
            // Find all the samples in a particular folder.
            std::string samplesFolder = appState->GetResourceMap().GetSamplesFolder() + "\\" + GetResourceInfo(type).pszSampleFolderName + "s";
            std::string findFirstString = samplesFolder + "\\*.bin";
            WIN32_FIND_DATA findData;
            HANDLE hFolder = FindFirstFile(findFirstString.c_str(), &findData);
            if (hFolder != INVALID_HANDLE_VALUE)
            {
                BOOL fOk = TRUE;
                while (fOk)
                {
                    std::string fileName = samplesFolder + "\\" + findData.cFileName;
                    std::unique_ptr<ResourceBlob> blob = std::make_unique<ResourceBlob>();
                    // Use the filename minus the .bin:
                    TCHAR *pszExt = PathFindExtension(findData.cFileName);
                    *pszExt = 0; // Get rid of it.
                    if (SUCCEEDED(blob->CreateFromFile(findData.cFileName, fileName.c_str(), sampleVersion, appState->GetResourceMap().GetDefaultResourceSaveLocation(), - 1, -1)))
                    {
                        if (blob->GetType() == type)
                        {
                            if (IsVersionCompatible(type, blob->GetVersion(), appState->GetVersion()))
                            {
                                _samples.push_back(std::move(blob));
                            }
                        }
                    }
                    fOk = FindNextFile(hFolder, &findData);
                }
                FindClose(hFolder);
            }

            // Now go through all the samples we got.
            for (size_t iIndex = 0; iIndex < _samples.size() && iIndex < MAX_SAMPLES; ++iIndex)
            {
                UINT nID = ID_GOTOVIEW1 + (UINT)iIndex;
                // Ensure we have a command entry for this.
                CExtCmdItem *pCmdItem;
	            pCmdItem = g_CmdManager->CmdGetPtr(appState->_pszCommandProfile, nID);
                if (pCmdItem == NULL)
                {
                    pCmdItem = g_CmdManager->CmdAllocPtr(appState->_pszCommandProfile, nID);
                }
                // Update the command entry with an icon and text
                if (pCmdItem)
                {
                    // Try to get a bitmap.
                    CBitmap bitmap;
                    unique_ptr<ResourceEntity> resource(CreateResourceFromResourceData(*_samples[iIndex]));
                    assert(resource); // Since we succeeded CreateResource
                    CelIndex celIndex(0, 0); // Normally we'll use cel/loop 0
                    if (resource->GetType() == ResourceType::Font)
                    {
                        // But not for fonts...
                        celIndex.cel = 65; // 'a'
                    }
                    PicComponent *pPic = resource->TryGetComponent<PicComponent>();
                    RasterComponent *pRaster = resource->TryGetComponent<RasterComponent>();
                    PaletteComponent *pPalette = resource->TryGetComponent<PaletteComponent>();
                    if (pPic)
                    {
                        // Special handler for pics.
                        bitmap.Attach(GetPicBitmap(PicScreen::Visual, *pPic, pPalette, 40, 24));
                    }
                    else if (pRaster)
                    {
                        // Incorporate the global palette if necessary
                        std::unique_ptr<PaletteComponent> temp;
                        if (pRaster->Traits.PaletteType == PaletteType::VGA_256)
                        {
                            temp = appState->GetResourceMap().GetMergedPalette(*resource, 999);
                            pPalette = temp.get();
                        }

                        // Note: if the index is out of bounds, it will return a NULL HBITMAP
                        bitmap.Attach(GetBitmap(*pRaster, pPalette, celIndex, 24, 24, BitmapScaleOptions::AllowMag | BitmapScaleOptions::AllowMin));
                    }

                    if ((HBITMAP)bitmap)
                    {
                        CExtBitmap extBitmap;
                        extBitmap.FromBitmap((HBITMAP)bitmap);
                        g_CmdManager->CmdSetIcon(appState->_pszCommandProfile, nID, extBitmap, RGB(255, 255, 255), CRect(0, 0, 24, 24));
                    }

                    // The text:
                    pCmdItem->m_sMenuText = _samples[iIndex]->GetName().c_str();
                    pCmdItem->m_sTipTool = pCmdItem->m_sMenuText;
                }
            }
        }
    }
}

void CSamplesDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTONSAMPLES, m_wndButtonViews);
    m_wndButtonViews.SetSamplesDialogBar(this);
}

BOOL CSamplesDialogBar::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);
    AddAnchor(IDC_BUTTONSAMPLES, CPoint(100, 0), CPoint(100, 0));
    return fRet;
}

void CSamplesDialogBar::TrackPopup()
{
    _ResetUI();
}

BEGIN_MESSAGE_MAP(CSamplesDialogBar, CExtResizableDialog)
    ON_UPDATE_COMMAND_UI(ID_VIEWMENU, OnUpdateAlwaysOn)
    ON_COMMAND_RANGE(ID_GOTOVIEW1, ID_GOTOVIEW28, OnGotoView)
END_MESSAGE_MAP()


// CSamplesDialogBar message handlers



void CSamplesDialogBar::Initialize()
{
    if (_bFirstTime)
    {

    }
}


void CSamplesDialogBar::_ResetUI()
{
    _PrepareSamples();
    _PrepareViewMenu((int)_samples.size());
}


void CSamplesDialogBar::OnGotoView(UINT nID)
{
    ResourceBlob &blob = *_samples[nID - ID_GOTOVIEW1];
    appState->GetResourceMap().AppendResourceAskForNumber(blob, true);
}


void CSamplesDialogBar::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CResourceListDoc*>(pDoc);
    if (_pDoc)
    {
        ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CResourceListDoc)));
        _pDoc->AddNonViewClient(this);
    }
}

// INonViewClient
void CSamplesDialogBar::UpdateNonView(CObject *pObject)
{
    ResourceMapChangeHint hint = GetHint<ResourceMapChangeHint>(pObject);
    if (_pDoc && IsFlagSet(hint, ResourceMapChangeHint::ShowType))
    {
        // m_wndButtonViews
        std::string message = "Insert ";
        
        ResourceType type = _pDoc->GetShownResourceType();
        message += GetResourceInfo(type).pszSampleFolderName;
        message += " sample";
        m_wndButtonViews.SetWindowText(message.c_str());

        m_wndButtonViews.EnableWindow(type != ResourceType::None && type != ResourceType::Script && type != ResourceType::Text && type != ResourceType::Vocab);
    }
}

bool CExtDelayedButton::_OnTrackPopup(bool bSelectAny)
{
    _pDlgBar->TrackPopup();
    return __super::_OnTrackPopup(bSelectAny);
}
