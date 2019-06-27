/***************************************************************************
Copyright (c) 2019 Philip Fortier

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
#include "ViewScriptingDialog.h"
#include "Components.h"
#include "WindowsUtil.h"
#include "View.h"
#include <filesystem>
#include <regex>
#include <format.h>

using namespace std::tr2::sys;
using namespace fmt;
using namespace std;

const char *ChaiExtension = ".txt";

const int MaxBackground = 4;
int g_currentBackgroundVS = 0;

#define VS_ANIMATE_TIMER 1235

// ViewScriptingDialog dialog

ViewScriptingDialog::ViewScriptingDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ViewScriptingDialog::IDD, pParent), _initialized(false), _callback(nullptr), _resource(nullptr), _palette(nullptr), _fAnimating(true), _nLoop(0), _nCel(0)
{
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPALETTE), RT_ACCELERATOR);
    _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPALETTE));
}

BOOL ViewScriptingDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    /*
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }*/
    if (!fRet)
    {
        if (GetFocus() == static_cast<CWnd*>(&m_wndEditCode))
        {
            fRet = HandleEditBoxCommands(pMsg, m_wndEditCode);
        }
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

void ViewScriptingDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, ID_SAVE, m_wndSave);
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_EDIT_CODE, m_wndEditCode);
    int tabStop = 16;
    m_wndEditCode.SetTabStops(1, &tabStop);

    DDX_Control(pDX, IDC_ANIMATE, m_wndAnimate);
    DDX_Control(pDX, IDC_SLIDER, m_wndSlider);
    DDX_Control(pDX, IDC_BUTTONPLAY, m_wndButton);
    DDX_Control(pDX, IDC_BUTTONBG, m_wndButtonBG);

    DDX_Control(pDX, IDC_COMBOFILES, m_wndFiles);

    m_wndAnimate.SetResource(_resource, _palette);
    m_wndAnimate.SetLoop(_nLoop);

    _UpdateBackground();

    GetClientRect(&_rectDialogSize);

    SetTimer(VS_ANIMATE_TIMER, 100, NULL);

    if (!_initialized)
    {
        AddAnchor(ID_SAVE, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_COMBOFILES, CPoint(100, 0), CPoint(100, 0));
        AddAnchor(IDC_EDIT_CODE, CPoint(100, 0), CPoint(100, 100));
        AddAnchor(IDOK, CPoint(100, 100), CPoint(100, 100));
        AddAnchor(IDCANCEL, CPoint(100, 100), CPoint(100, 100));

        _PopulateFilesList();
        _initialized = true;
    }
}

BEGIN_MESSAGE_MAP(ViewScriptingDialog, CExtResizableDialog)
    ON_BN_CLICKED(ID_SAVE, &ViewScriptingDialog::OnBnClickedSave)

    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_HSCROLL()
    ON_COMMAND(IDC_BUTTONPLAY, OnPlay)
    ON_BN_CLICKED(IDC_BUTTONBG, &ViewScriptingDialog::OnBnClickedButtonbg)
    ON_CBN_SELCHANGE(IDC_COMBOFILES, &OnCbnSelchangeFiles)
END_MESSAGE_MAP()

void ViewScriptingDialog::OnBnClickedSave()
{
    string chaiFolder = appState->GetResourceMap().Helper().GetChaiFolder();
    EnsureFolderExists(chaiFolder, false);

    CString strText;
    m_wndFiles.GetWindowTextA(strText);
    if (strText.IsEmpty())
    {
        strText = "Untitled";
    }

    std::string filename = appState->GetResourceMap().Helper().GetViewScriptFilename((PCSTR)strText);

    CString code;
    m_wndEditCode.GetWindowTextA(code);
    code.Replace("\r\n", "\n");
    MakeTextFile(code, filename);
}


void ViewScriptingDialog::OnOK()
{
    if (_callback)
    {
        CString code;
        m_wndEditCode.GetWindowTextA(code);
        _callback->RunViewScript((PCSTR)code);
    }
    // Don't close the dialog.
    // Instead, call our callback here.
}




// Animate dialog

void ViewScriptingDialog::_UpdateButton()
{
    // Update command ui doesn't work here, so set the text directly.
    m_wndButton.SetWindowText(_fAnimating ? TEXT("Pause") : TEXT("Play"));
}

void ViewScriptingDialog::OnPlay()
{
    _fAnimating = !_fAnimating;
    _UpdateButton();
}


void ViewScriptingDialog::OnSize(UINT nType, int cx, int cy)
{
    CRect rectDialogSize;
    GetClientRect(&rectDialogSize);

    if (_initialized)
    {
        int dx = rectDialogSize.Width() - _rectDialogSize.Width();
        int dy = rectDialogSize.Height() - _rectDialogSize.Height();

        if (dx || dy)
        {
            CRect rectAnimateScreen;
            m_wndAnimate.GetWindowRect(&rectAnimateScreen);
            ScreenToClient(&rectAnimateScreen);
            rectAnimateScreen.right = rectAnimateScreen.left + rectAnimateScreen.Width() + dx;
            rectAnimateScreen.bottom = rectAnimateScreen.top + rectAnimateScreen.Height() + dy;
            m_wndAnimate.MoveWindow(&rectAnimateScreen, TRUE);

            int rgid[] = { IDOK, IDC_BUTTONPLAY };
            for (int i = 0; i < ARRAYSIZE(rgid); i++)
            {
                CWnd *pOk = GetDlgItem(rgid[i]);
                if (pOk)
                {
                    CRect rectOkScreen;
                    pOk->GetWindowRect(&rectOkScreen);
                    ScreenToClient(&rectOkScreen);
                    rectOkScreen.OffsetRect(dx, dy);
                    pOk->MoveWindow(&rectOkScreen, TRUE);
                }
            }

            CWnd *pSlider = GetDlgItem(IDC_SLIDER);
            if (pSlider)
            {
                CRect rectScreen;
                pSlider->GetWindowRect(&rectScreen);
                ScreenToClient(&rectScreen);
                rectScreen.OffsetRect(0, dy);
                pSlider->SetWindowPos(NULL, rectScreen.left, rectScreen.top, rectScreen.Width() + dx, rectScreen.Height(), SWP_NOZORDER);
            }

            m_wndAnimate.Invalidate(FALSE);
        }
    }
    _rectDialogSize = rectDialogSize;

    __super::OnSize(nType, cx, cy);
}

void ViewScriptingDialog::SetResource(const ResourceEntity *resource, const PaletteComponent *palette)
{
    _resource = resource;
    _palette = palette;
    if (_initialized)
    {
        m_wndAnimate.SetResource(_resource, _palette);
    }
}

void ViewScriptingDialog::SetLoop(int nLoop)
{
    _nLoop = nLoop;
    if (_initialized)
    {
        m_wndAnimate.SetLoop(_nLoop);
    }
}

void ViewScriptingDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == VS_ANIMATE_TIMER)
    {
        if (_resource)
        {
            const RasterComponent &raster = _resource->GetComponent<RasterComponent>();
            if ((raster.Loops.size() > 0) && _fAnimating)
            {
                int cCels = (int)raster.Loops[_nLoop].Cels.size();
                _nCel++;
                _nCel %= cCels;
                m_wndSlider.SetPos(_nCel);
                m_wndAnimate.SetCel(_nCel);
            }
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}


void ViewScriptingDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    switch (nSBCode)
    {
    case SB_THUMBTRACK:
        _nCel = nPos;
        _fAnimating = false;
        _UpdateButton();
        m_wndAnimate.SetCel(_nCel);
        break;

    case SB_LEFT:
    case SB_ENDSCROLL:
    case SB_LINELEFT:
    case SB_LINERIGHT:
    case SB_PAGELEFT:
    case SB_PAGERIGHT:
    case SB_RIGHT:
        _nCel = m_wndSlider.GetPos();
        _fAnimating = false;
        _UpdateButton();
        m_wndAnimate.SetCel(_nCel);
        break;

    case SB_THUMBPOSITION:
        // We update constantly, so no need to do anything here.
        break;
    }
}

void ViewScriptingDialog::OnBnClickedButtonbg()
{
    g_currentBackgroundVS++;
    g_currentBackgroundVS %= MaxBackground;
    _UpdateBackground();
}

void ViewScriptingDialog::_UpdateBackground()
{
    if (g_currentBackgroundVS == 1)
    {
        m_wndAnimate.SetPatterned(true);
    }
    else
    {
        m_wndAnimate.SetPatterned(false);
        COLORREF color = RGB(0, 0, 0);
        switch (g_currentBackgroundVS)
        {
        case 0:
            color = RGB(0, 0, 0);
            break;
        case 2:
            color = RGB(255, 0, 255);
            break;
        case 3:
            color = RGB(255, 255, 255);
            break;
        }
        m_wndAnimate.SetBackground(color);
    }
}

void ViewScriptingDialog::_PopulateFilesList()
{
    m_wndFiles.Clear();

    m_wndFiles.SetRedraw(FALSE);

    path enumPath = appState->GetResourceMap().Helper().GetChaiFolder();
    string regexString = "(\\w+)\\";
    regexString += ChaiExtension;
    auto matchRSTRegex = std::regex(regexString);
    for (auto it = directory_iterator(enumPath); it != directory_iterator(); ++it)
    {
        const auto &file = it->path();
        std::smatch sm;
        std::string temp = file.filename().string();
        if (!is_directory(file) && std::regex_search(temp, sm, matchRSTRegex) && (sm.size() > 1))
        {
            _fileNames.push_back(file.string());
            m_wndFiles.AddString(temp.c_str());
        }
    }

    m_wndFiles.SetRedraw(TRUE);
}

void ViewScriptingDialog::OnCbnSelchangeFiles()
{
    int curSel = m_wndFiles.GetCurSel();
    if (curSel != CB_ERR)
    {
        try
        {
            std::string filename = _fileNames[curSel];

            if (PathFileExists(filename.c_str()))
            {
                CStdioFile file;
                file.Open(filename.c_str(), CFile::modeRead | CFile::typeText);

                CString buffer;
                CString textfile;
                while (file.ReadString(textfile))
                {
                    buffer += textfile;
                    buffer += "\r\n";
                }

                file.Close();
                m_wndEditCode.SetWindowText(buffer);
            }
        }
        catch (std::exception)
        {
        }
    }
}