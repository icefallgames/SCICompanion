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
// BitmapToPicDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "BitmapToPicDialog.h"
#include "PicOperations.h"
#include "Pic.h"
#include "BitmapToPicDialog.h"
#include "PicDrawManager.h"
#include "PicDoc.h"
#include <thread>
#include "PicCommands.h"

using namespace std;

#define FROMCLIPBOARD_TIMER 2345

int g_iAlgorithm = 1;   // RGB euclidian
int g_iPalette = 3;     // Smooth half-tone
int g_iScaleImage = 1;
int g_iIgnoreWhite = 0;
int g_nColors = 10;

struct THREADINFO
{
    HWND hwndDlg;
    int iAlgorithm;
    int iPalette;
    bool fIgnoreWhite;
    int nColors;
    COLORREF *pCRBitmap;
    SIZE size;
    HANDLE hEvent;
    EGACOLOR picPalette[40];
};

struct THREADRESPONSE
{
    unique_ptr<vector<PicCommand>> pcommands;
    HBITMAP hbm;
    bool fTooBig;

    THREADRESPONSE() { hbm = nullptr; }
    ~THREADRESPONSE() { DeleteObject(hbm); }
};

void FreeThreadInfo(THREADINFO *pInfo)
{
    if (pInfo->pCRBitmap)
    {
        delete [] pInfo->pCRBitmap;
        pInfo->pCRBitmap = nullptr;
    }
    if (pInfo->hEvent)
    {
        CloseHandle(pInfo->hEvent);
        pInfo->hEvent = nullptr;
    }
}

BOOL InitThreadInfo(THREADINFO *pInfo, HWND hwnd, bool fIgnoreWhite, int iAlgorithm, int iPalette, int nColors, CSize &size, const COLORREF *pCRBitmap, EGACOLOR *pPicPalette)
{
    BOOL fRet = FALSE;
    ZeroMemory(pInfo, sizeof(*pInfo));
    pInfo->hwndDlg = hwnd;
    pInfo->iAlgorithm = iAlgorithm;
    pInfo->iPalette = iPalette;
    pInfo->nColors = nColors;
    pInfo->pCRBitmap = new COLORREF[size.cx * size.cy];
    pInfo->size = size;
    pInfo->fIgnoreWhite = fIgnoreWhite;
    if (pInfo->pCRBitmap)
    {
        // Bitmap data
        CopyMemory(pInfo->pCRBitmap, pCRBitmap, size.cx * size.cy * sizeof(*pCRBitmap));
        // Optional palette.
        CopyMemory(pInfo->picPalette, pPicPalette, sizeof(pInfo->picPalette));
        pInfo->hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (pInfo->hEvent)
        {
            fRet = TRUE;
        }
        else
        {
            FreeThreadInfo(pInfo);
        }
    }
    return fRet;
}



// CBitmapToPicDialog dialog

#define UWM_PICREADY        (WM_APP + 0)
#define UWM_CONVERTPROGRESS (WM_APP + 1) // LPARAM is int from 0 to 100
#define UWM_CONVERTSTATUS   (WM_APP + 2) // LPARAM is text

CBitmapToPicDialog::CBitmapToPicDialog(CWnd* pParent /*=nullptr*/)
    : CExtNCW<CExtResizableDialog>(CBitmapToPicDialog::IDD, pParent), PrepareBitmapBase(IDC_BUTTONCONVERT, IDC_STATICORIG, size16(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT))
{
    _pCRBitmap = nullptr;
    _nColors = g_nColors;
    _iAlgorithm = g_iAlgorithm;
    _iPalette = g_iPalette;
    _iScaleImage = g_iScaleImage;
    _iIgnoreWhite = g_iIgnoreWhite;
    _size.SetSize(0, 0);

    _hEvent = nullptr;
    _fConverting = FALSE;
    _fTooBig = FALSE;

    _pbmpNormal = nullptr;
    _pbmpScaled = nullptr;
    _pbmpCurrent = nullptr;
    _bContrastCenterNormal = 0x80;
    _bContrastCenterScaled = 0x80;

    _nBrightness = 50;
    _nContrast = 50;
    _nSaturation = 50;
    _hue = 50;

    _fInitializedControls = false;
}

CBitmapToPicDialog::~CBitmapToPicDialog()
{
    delete [] _pCRBitmap;
    delete _pbmpNormal;
    delete _pbmpScaled;
    delete _pbmpCurrent;

    g_nColors = _nColors;
    g_iAlgorithm = _iAlgorithm;
    g_iPalette = _iPalette;
    g_iScaleImage = _iScaleImage;
    g_iIgnoreWhite = _iIgnoreWhite;
}

void CBitmapToPicDialog::DoDataExchange(CDataExchange* pDX)
{
    int nBrightness = _nBrightness;
    int nContrast = _nContrast;
    int nSaturation = _nSaturation;
    int hue = _hue;
    int iScale = _iScaleImage;

	CExtNCW<CExtResizableDialog>::DoDataExchange(pDX);

    if( m_wndSlider1.GetSafeHwnd() != nullptr )
    {
        m_wndSlider1.UpdateSliderWnd();
    }
    if (!_fInitializedControls)
    {
        // Necessary for prof-uis
        DDX_Control(pDX, IDC_STATICARROW, m_wndRightArrow);
        DDX_Control(pDX, IDC_SLIDERNUMCOLORS, m_wndSlider1);
        m_wndSlider1.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_SLIDERBRIGHTNESS, m_wndSlider2);
        m_wndSlider2.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_SLIDERCONTRAST, m_wndSlider3);
        m_wndSlider3.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_SLIDERSATURATION, m_wndSlider4);
        m_wndSlider4.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        DDX_Control(pDX, IDC_SLIDERHUE, m_wndSlider5);
        m_wndSlider5.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);

        DDX_Control(pDX, IDC_BUTTONCONVERT, m_wndButton1);
        DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndButton2);
        DDX_Control(pDX, IDOK, m_wndButton3);
        DDX_Control(pDX, IDCANCEL, m_wndButton4);
        DDX_Control(pDX, IDC_RADIO1, m_wndRadio1);
        DDX_Control(pDX, IDC_RADIO2, m_wndRadio2);
        DDX_Control(pDX, IDC_RADIO3, m_wndRadio3);
        DDX_Control(pDX, IDC_RADIO4, m_wndRadio4);
        DDX_Control(pDX, IDC_RADIO5, m_wndRadio5);
        DDX_Control(pDX, IDC_RADIOHALFTONE, m_wndRadio6);
        DDX_Control(pDX, IDC_RADIOSOLID, m_wndRadio7);
        DDX_Control(pDX, IDC_RADIOHALFTONESMOOTH, m_wndRadio8);
        DDX_Control(pDX, IDC_RADIOPICPALETTE, m_wndRadio9);
        DDX_Control(pDX, IDC_STATICGROUP1, m_wndGroup1);
        DDX_Control(pDX, IDC_STATICGROUP2, m_wndGroup2);
        DDX_Control(pDX, IDC_STATICGROUP3, m_wndGroup3);
        DDX_Control(pDX, IDC_STATICGROUP4, m_wndGroup4);
        DDX_Control(pDX, IDC_CHECKSCALE, m_wndCheck1);
        DDX_Control(pDX, IDC_CHECKIGNOREWHITE, m_wndCheck2);
        DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
        DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
        DDX_Control(pDX, IDC_STATIC3, m_wndLabel3);
        DDX_Control(pDX, IDC_STATIC4, m_wndLabel4);
        _fInitializedControls = true;
    }

    DDX_Control(pDX, IDC_EDITSTATUS, m_wndEditStatus);
    m_wndEditStatus.FmtLines(TRUE);
    DDX_Control(pDX, IDC_STATICPIC, m_wndPic);
    DDX_Control(pDX, IDC_STATICORIG, m_wndOrig);
    DDX_Control(pDX, IDC_BUTTONCLIPBOARD, m_wndFromClipboard);
    m_wndFromClipboard.EnableWindow(IsClipboardFormatAvailable(CF_BITMAP));
    DDX_Control(pDX, IDC_PROGRESSCONVERT, m_wndProgress);
    DDX_Check(pDX, IDC_CHECKSCALE, _iScaleImage);

    DDX_Check(pDX, IDC_CHECKIGNOREWHITE, _iIgnoreWhite);

    // MinMaxSlider busted in MFC 7.0
    // DDV_MinMaxSlider(pDX, _nColors, 1, 40); 
    GetDlgItem(IDC_SLIDERNUMCOLORS)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM) 1);
    GetDlgItem(IDC_SLIDERNUMCOLORS)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM) PALETTE_SIZE);
    GetDlgItem(IDC_SLIDERNUMCOLORS)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERNUMCOLORS, _nColors);

    GetDlgItem(IDC_SLIDERBRIGHTNESS)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM) 0);
    GetDlgItem(IDC_SLIDERBRIGHTNESS)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM) 100);
    GetDlgItem(IDC_SLIDERBRIGHTNESS)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERBRIGHTNESS, _nBrightness);

    GetDlgItem(IDC_SLIDERCONTRAST)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM) 0);
    GetDlgItem(IDC_SLIDERCONTRAST)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM) 100);
    GetDlgItem(IDC_SLIDERCONTRAST)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERCONTRAST, _nContrast);

    GetDlgItem(IDC_SLIDERSATURATION)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM) 0);
    GetDlgItem(IDC_SLIDERSATURATION)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM) 100);
    GetDlgItem(IDC_SLIDERSATURATION)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERSATURATION, _nSaturation);

    GetDlgItem(IDC_SLIDERHUE)->SendMessage(TBM_SETRANGEMIN, FALSE, (LPARAM)0);
    GetDlgItem(IDC_SLIDERHUE)->SendMessage(TBM_SETRANGEMAX, TRUE, (LPARAM)100);
    GetDlgItem(IDC_SLIDERHUE)->SendMessage(TBM_SETTICFREQ, 10, 0);
    DDX_Slider(pDX, IDC_SLIDERHUE, _hue);

    // Enable the convert button if we have a bitmap
    ((CButton*)GetDlgItem(IDC_BUTTONCONVERT))->EnableWindow(_pbmpCurrent != nullptr);

    int nID;
    switch(_iAlgorithm)
    {
    case 1:
        nID = IDC_RADIO1;
        break;
    case 2:
        nID = IDC_RADIO2;
        break;
    case 3:
        nID = IDC_RADIO3;
        break;
    case 4:
        nID = IDC_RADIO4;
        break;
    case 5:
        nID = IDC_RADIO5;
        break;
    default:
        ASSERT(FALSE);
    }
    ((CButton*)GetDlgItem(nID))->SetCheck(1);

    switch(_iPalette)
    {
    case 1:
        nID = IDC_RADIOHALFTONE;
        break;
    case 2:
        nID = IDC_RADIOSOLID;
        break;
    case 3:
        nID = IDC_RADIOHALFTONESMOOTH;
        break;
    case 4:
        nID = IDC_RADIOPICPALETTE;
        break;
    default:
        ASSERT(FALSE);
    }
    ((CButton*)GetDlgItem(nID))->SetCheck(1);

    m_wndProgress.SetRange(0, 100);

    if ((_nBrightness != nBrightness) ||
        (_nContrast != nContrast) ||
        (_nSaturation != nSaturation) ||
        (_hue != hue) ||
        (_iScaleImage != iScale))
    {
        _UpdateOrigBitmap(this);
    }

    SetTimer(FROMCLIPBOARD_TIMER, 1000, nullptr);
}

void CBitmapToPicDialog::OnCancel()
{
    if (_fConverting)
    {
        if (_hEvent)
        {
            SetEvent(_hEvent);
            _hEvent = nullptr;
        }
    }
    else
    {
        __super::OnCancel();
    }
}

void CBitmapToPicDialog::OnOK()
{
    ASSERT(!_fConverting);
    BOOL fClose = TRUE;
    if (_fTooBig)
    {
        if (IDNO == AfxMessageBox("The pic is too big to be saved as an SCI resource.  Accept it anyway?", MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO))
        {
            fClose = FALSE;
        }
    }
    if (fClose)
    {
        __super::OnOK();
    }
}

BEGIN_MESSAGE_MAP(CBitmapToPicDialog, CExtNCW<CExtResizableDialog>)
    ON_BN_CLICKED(IDC_BUTTONCONVERT, OnConvert)
    ON_MESSAGE(UWM_PICREADY, _OnConvertDone)
    ON_MESSAGE(UWM_CONVERTSTATUS, _OnConvertStatus)
    ON_MESSAGE(UWM_CONVERTPROGRESS, _OnConvertProgress)
    ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
    ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
    ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
    ON_BN_CLICKED(IDC_RADIO5, OnBnClickedRadio5)
    ON_BN_CLICKED(IDC_RADIOHALFTONE, OnBnClickedRadiohalftone)
    ON_BN_CLICKED(IDC_RADIOSOLID, OnBnClickedRadiosolid)
    ON_BN_CLICKED(IDC_RADIOHALFTONESMOOTH, OnBnClickedRadiohalftonesmooth)
    ON_BN_CLICKED(IDC_CHECKSCALE, OnScaleClicked)
    ON_BN_CLICKED(IDC_BUTTONBROWSE, OnBrowse)
    ON_BN_CLICKED(IDC_BUTTONCLIPBOARD, OnPasteFromClipboard)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_RADIOPICPALETTE, OnBnClickedRadiopicpalette)
    ON_BN_CLICKED(IDC_CHECKIGNOREWHITE, OnBnClickedCheckignorewhite)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CBitmapToPicDialog message handlers

BOOL IsOneOf(EGACOLOR b, EGACOLOR *rgBytes, int cBytes)
{
    for (int i = 0; i < cBytes; i++)
    {
        if ((rgBytes[i].color1 == b.color1) && (rgBytes[i].color2 == b.color2))
        {
            return TRUE;
        }
    }
    return FALSE;
}

void _SendStatus(HWND hwnd, PCTSTR pszText)
{
    SendMessage(hwnd, UWM_CONVERTSTATUS, 0, (LPARAM)pszText);
}

//
// Send progress, and bail out if the handle is set.
//
BOOL _SendProgressAndCheckAbort(HWND hwnd, HANDLE hEvent, int nCurrent, int nMax)
{
    if (WAIT_OBJECT_0 != WaitForSingleObject(hEvent, 0))
    {
        SendMessage(hwnd, UWM_CONVERTPROGRESS, 0, (LPARAM)(100 * nCurrent / nMax));
        return FALSE;
    }
    else
    {
        return TRUE; // Abort!
    }
}

UINT CBitmapToPicDialog::s_ThreadWorker(THREADINFO *pInfo)
{
    CSize size = pInfo->size;
    int cPixels = pInfo->size.cx * pInfo->size.cy;
    int iAlgorithm = pInfo->iAlgorithm;
    int iPalette = pInfo->iPalette;
    bool fIgnoreWhite = pInfo->fIgnoreWhite;
    EGACOLOR *pPicPalette = pInfo->picPalette;
    int cPicPalette = sizeof(pInfo->picPalette);
    COLORREF *pCRBitmap = pInfo->pCRBitmap;
    std::unique_ptr<vector<PicCommand>> pcommands = make_unique<vector<PicCommand>>();
    HWND hwnd = pInfo->hwndDlg;
    int nColors = pInfo->nColors;
    BOOL fAbort = FALSE;
    TCHAR szMsg[MAX_PATH];

    BOOL fSuccess = FALSE;
    EGACOLOR *pegaTemp = new EGACOLOR[cPixels];
    if (pegaTemp)
    {
        int rgColorCounts[256]; // Indexed by EGACOLOR_TO_BYTE
        ZeroMemory(rgColorCounts, sizeof(rgColorCounts));

        _SendStatus(hwnd, TEXT("Mapping image colors to SCI colors"));

        // Figure out which EGACOLOR each pixel maps to.
        int iInterval = cPixels / 100;
        for (int i = 0; !fAbort && (i < cPixels); i++)
        {
            EGACOLOR curColor;
            if (iPalette == 4)
            {
                // iPalette = 4 means that we use the current pic's palette.
                curColor = GetClosestEGAColorFromSet(iAlgorithm, *(pCRBitmap + i), pPicPalette, cPicPalette);
            }
            else
            {
                curColor = GetClosestEGAColor(iAlgorithm, iPalette, *(pCRBitmap + i));
            }

            *(pegaTemp + i) = curColor;
            // Keep a count of the number of times each colour appears.
            rgColorCounts[EGACOLOR_TO_BYTE(curColor)]++;

            if ((i % iInterval) == 0)
            {
                fAbort = _SendProgressAndCheckAbort(hwnd, pInfo->hEvent, i, cPixels);
            }
        }

        int cMostCommonColors = 0;
        EGACOLOR rgMostCommonColors[256];
        if (!fAbort)
        {
            if (iPalette == 4)
            {
                // This is easy, just copy the pic's palette.
                CopyMemory(rgMostCommonColors, pPicPalette, cPicPalette);
                // We have 40 "most common colours"
                cMostCommonColors = cPicPalette;
            }
            else
            {
                // Count unique colors for status purposes
                int nUniqueColors = 0;
                int i = 0;
                for (; i < ARRAYSIZE(rgColorCounts); i++)
                {
                    if (rgColorCounts[i])
                    {
                        nUniqueColors++;
                    }
                }
                StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("Finding most common of %d colors"), nUniqueColors);
                _SendStatus(hwnd, szMsg);

                // Then figure out the most common MAXCONVERTEDCOLORS
                for (int i = 0; !fAbort && (i < nColors); i++)
                {
                    int cCount = 0;
                    int bBest = -1;
                    for (int j = 0; j < ARRAYSIZE(rgColorCounts); j++)
                    {
                        if (rgColorCounts[j] > cCount)
                        {
                            cCount = rgColorCounts[j];
                            bBest = j;
                        }
                    }
                    if (bBest != -1)
                    {
                        rgMostCommonColors[cMostCommonColors] = EGAColorFromByte(bBest);
                        rgColorCounts[bBest] = 0; // Set it to zero so it won't be included in future counts
                        cMostCommonColors++;
                    }
                    else
                    {
                        // We're done.
                        break;
                    }
                    fAbort = _SendProgressAndCheckAbort(hwnd, pInfo->hEvent, i, nColors);
                }
            }
        }

        if (!fAbort)
        {
            StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("Remapping to %d most common"), cMostCommonColors);
            _SendStatus(hwnd, szMsg);


            // Now figure out which of the most common MAXCONVERTEDCOLORS each pixel belongs to.
            EGACOLOR *pegaTempI = pegaTemp;
            COLORREF *pcrTempI = pCRBitmap;
            for (int i = 0; !fAbort && (i < cPixels); pegaTempI++, pcrTempI++, i++)
            {
                if (!IsOneOf(*pegaTempI, rgMostCommonColors, cMostCommonColors))
                {
                    // It wasn't one of our most common colors, so re-evaluate it.
                    *pegaTempI = GetClosestEGAColorFromSet(iAlgorithm, *pcrTempI, rgMostCommonColors, cMostCommonColors);
                }

                if ((i % iInterval) == 0)
                {
                    fAbort = _SendProgressAndCheckAbort(hwnd, pInfo->hEvent, i, cPixels);
                }
            }
        }

        if (!fAbort)
        {
            // 4 == use pic's palette
            fAbort = s_ConvertToPic(hwnd, pInfo->hEvent, *pcommands, pegaTemp, size, fIgnoreWhite, rgMostCommonColors, cMostCommonColors, (iPalette == 4));
        }

        if (!fAbort)
        {
            // Put the pic in the static, and give stats on it.
            std::unique_ptr<ResourceEntity> pResource(CreatePicResource(appState->GetVersion()));
            PicComponent &pic = pResource->GetComponent<PicComponent>();
            InsertCommands(pic, -1, pcommands->size(), &(*pcommands)[0]);

            // Make sure the pic is small enough.
            bool fTooBig = false;
            sci::ostream serial;
            pResource->WriteToTest(serial, false, pResource->ResourceNumber);

            DWORD cMax = 0xff00; // Provide safe boundary, since we have other pic commands in pic
            if (serial.tellp() > cMax)
            {
                fTooBig = true;
                StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("Warning! Pic size: %d bytes (%d too many)"), serial.tellp(), (serial.tellp() - cMax));
            }
            else
            {
                StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("Success! Pic size: %d bytes"), serial.tellp());
            }
            _SendStatus(hwnd, szMsg);

            // We're good.
            PicDrawManager pdm(&pic);
            HBITMAP hbm = pdm.CreateBitmap(PicScreen::Visual, PicPosition::Final, pic.Size, pic.Size.cx, pic.Size.cy);
            if (hbm)
            {
                THREADRESPONSE *pResponse = new THREADRESPONSE;
                if (pResponse)
                {
                    pResponse->hbm = hbm;
                    pResponse->pcommands = move(pcommands);
                    pResponse->fTooBig = fTooBig;
                    fSuccess = TRUE;
                    if (::SendMessage(hwnd, UWM_PICREADY, 0, (LPARAM)pResponse))
                    {
                        // The information has been transfered.
                        hbm = nullptr;
                    }
                }
                if (hbm)
                {
                    DeleteObject(hbm);
                }
            }
        }

        delete [] pegaTemp;
    }

    if (!fSuccess)
    {
        // Send null response.
        ::SendMessage(hwnd, UWM_PICREADY, 0, 0);
    }

    // Clean up the struct that was passed to us.
    FreeThreadInfo(pInfo);
    delete pInfo;

    return 0;
}


LRESULT CBitmapToPicDialog::_OnConvertDone(WPARAM wParam, LPARAM lParam)
{
    _fConverting = FALSE;
    _hEvent = nullptr;
    THREADRESPONSE *pResponse = (THREADRESPONSE*)lParam;
    if (pResponse)
    {
        CRect rectBitmap;
        m_wndPic.GetClientRect(&rectBitmap);
        m_wndPic.FromBitmap(pResponse->hbm, rectBitmap.Width(), rectBitmap.Height(), true);

        ASSERT(!_pcommands);
        _pcommands = move(pResponse->pcommands);

        // We got a response, re-enable Ok.
        GetDlgItem(IDOK)->EnableWindow();

        _fTooBig = pResponse->fTooBig;

        delete pResponse;
    }
    else
    {
        // It failed for some other reason
    }

    // Re-enable the convert button, now that we're done.
    GetDlgItem(IDC_BUTTONCONVERT)->EnableWindow();

    // Reset progress
    m_wndProgress.SetPos(0);

    return TRUE; // Important, so the caller knows we handled it.
}

LRESULT CBitmapToPicDialog::_OnConvertStatus(WPARAM wParam, LPARAM lParam)
{
    _AddToEdit((PCTSTR)lParam);
    return TRUE;
}

LRESULT CBitmapToPicDialog::_OnConvertProgress(WPARAM wParam, LPARAM lParam)
{
    m_wndProgress.SetPos((int)lParam);
    return TRUE;
}

void CBitmapToPicDialog::OnConvert()
{
    // Delete any old commands
    _pcommands.reset(nullptr);

    // Get the most recent values.
    UpdateData(TRUE);

    // Do some validation
    if ((_nColors < 1) || (_nColors > PALETTE_SIZE))
    {
        AfxMessageBox(TEXT("Number of colours must be between 1 and 40"), MB_ERRORFLAGS);
        return;
    }

    // Disable the Convert and Ok buttons
    GetDlgItem(IDC_BUTTONCONVERT)->EnableWindow(FALSE);
    GetDlgItem(IDOK)->EnableWindow(FALSE);

    // Clear out the status box
    m_wndEditStatus.SetWindowText(TEXT(""));

    _PrepareBitmapForConversion();

    CSize size = _size;
    COLORREF *pCRBitmap = _pCRBitmap;

    THREADINFO *pInfo = new THREADINFO; // Will be deleted by thread.
    if (InitThreadInfo(pInfo, GetSafeHwnd(), (_iIgnoreWhite != 0), _iAlgorithm, _iPalette, _nColors, size, _pCRBitmap, _picPalette))
    {
        _fConverting = TRUE;
        _hEvent = pInfo->hEvent;

        try
        {
            std::thread thread = std::thread(s_ThreadWorker, pInfo);
            thread.detach();
            pInfo = nullptr;
        }
        catch (std::system_error)
        {
            FreeThreadInfo(pInfo);
            _fConverting = FALSE;
        }
        if (pInfo)
        {
            delete pInfo;
        }
    }
}


void CBitmapToPicDialog::OnBrowse()
{
    _OnBrowse(this);
}

void CBitmapToPicDialog::InsertIntoPic(CPicDoc *pDoc)
{
    ASSERT(_pcommands);
    pDoc->InsertCommands(_pcommands->size(), &(*_pcommands)[0]);
}

void CBitmapToPicDialog::_AddToEdit(PCTSTR pszText)
{
    m_wndEditStatus.SetWindowText(pszText);
}

BOOL CanConnect(EGACOLOR colorScreen, const EGACOLOR *rgBestColors,  int iColorIndex, BOOL *pfExact)
{
    *pfExact = FALSE;
    // The colorScreen
    BYTE bScreen = EGACOLOR_TO_BYTE(colorScreen);
    /*if (bScreen == 0xff)
    {
        return FALSE; // pure white
    }*/
    // REVIEW: possible optimization: do a fill color at the beginning for the most popular color.
    // And then remove that from the list of sorted colours we do.  And we'll need *include* it above.

    if (EGACOLOR_EQUAL(rgBestColors[iColorIndex], colorScreen))
    {
        *pfExact = TRUE;
        return TRUE; // Yes, this is our color.
    }

    // We can still connect, as long as it isn't one of the colors we're already used.
    for (int i = 0; i < iColorIndex; i++)
    {
        if (EGACOLOR_EQUAL(colorScreen, rgBestColors[i]))
        {
            return FALSE;
        }
    }
    return TRUE;
}


BYTE GetPaletteIndexOf(EGACOLOR color, EGACOLOR *rgColor, int cColors)
{
    for (int i = 0; i < cColors; i++)
    {
        if (EGACOLOR_EQUAL(color, rgColor[i]))
        {
            return (BYTE)i;
        }
    }
    ASSERT(FALSE);
    return 0;
}

#define IS_WHITE(color) (EGACOLOR_TO_BYTE(color) == 0xff)

//
// Does the meat of the conversion
// Returns TRUE if it was aborted.
//
BOOL CBitmapToPicDialog::s_ConvertToPic(HWND hwnd, HANDLE hEvent, std::vector<PicCommand> &commands, EGACOLOR *pegaTemp, CSize &size, bool fIgnoreWhite, EGACOLOR *rgColors, int cColors, bool fDontSetPalette)
{
    BOOL fAbort = FALSE;
    // Clean out the commands array.
    commands.clear();

    if (!fDontSetPalette)
    {
        // Prepare our palette
        EGACOLOR palette[40];
        ZeroMemory(palette, sizeof(palette));
        ASSERT(cColors <= PALETTE_SIZE);
        for (int paletteColor = 0; paletteColor < cColors; paletteColor++)
        {
            palette[paletteColor] = rgColors[paletteColor];
        }
        commands.push_back(PicCommand::CreateSetPalette(0, palette));
    }

    // An array of EGACOLOR arrays, each of which is cColors long
    EGACOLOR *rgOrderedColorsPerLine[190];
    int rgNumberOfColorsPerLine[190]; // From 1 to cColors
    ZeroMemory(rgOrderedColorsPerLine, sizeof(rgOrderedColorsPerLine));
    // Allocate one big blob for all the sub-arrays.  We'll point the members of
    // rgOrderedColorsPerLine in the next for loop.
    ASSERT(size.cy <= ARRAYSIZE(rgOrderedColorsPerLine));
    ASSERT(size.cy <= ARRAYSIZE(rgNumberOfColorsPerLine));

    _SendStatus(hwnd, TEXT("Analyzing lines"));

    EGACOLOR *rgAllColors = new EGACOLOR[cColors * ARRAYSIZE(rgOrderedColorsPerLine)];
    if (rgAllColors)
    {
        for (int line = 0; !fAbort && (line < size.cy); line++)
        {
            
            // Assign the array:
            rgOrderedColorsPerLine[line] = rgAllColors + (cColors * line);

            // An array to count how many strips there are of each on this line.
            int fragmentsPerColor[256]; // Uses real EGA numbers
            ZeroMemory(fragmentsPerColor, sizeof(fragmentsPerColor));

            {
                const EGACOLOR *pLine = pegaTemp + size.cx * line;
                EGACOLOR curColor = *pLine;

                int cFragments = 0;
                int startFragment = 0;
                int endFragment = 0;

                for (int x = 1; x < size.cx; x++)
                {
                    EGACOLOR color = *(pLine + x);
                    if (!EGACOLOR_EQUAL(color, curColor))
                    {
                        if (startFragment == endFragment)
                        {
                            // It was just a single dot.  That doesn't counts as much as a regular
                            // fragment - since we care less about optimizing it into a big line, since
                            // a bunch of little dots are cheaper than a bunch of little lines.
                            // I have found that the best thing is just to not count it at all.
                            // However, since we must count this color at least once (or else it will be
                            // excluded from the line completely), then count the first one.
                            if (fragmentsPerColor[EGACOLOR_TO_BYTE(curColor)] == 0)
                            {
                                fragmentsPerColor[EGACOLOR_TO_BYTE(curColor)]++;
                            }
                        }
                        else
                        {
                            fragmentsPerColor[EGACOLOR_TO_BYTE(curColor)]++;
                        }

                        // Start a new fragment.
                        cFragments++;
                        startFragment = x;
                        endFragment = x;

                        curColor = color;
                    }
                    else
                    {
                        endFragment = x;
                    }
                }
                fragmentsPerColor[EGACOLOR_TO_BYTE(curColor)]++;
            }

            if (fIgnoreWhite)
            {
                // If we make white the "most common" on every line, then it will be drawn first,
                // which is essential if we're "ignoring white"
                fragmentsPerColor[0xff] = DEFAULT_PIC_WIDTH + 1;
            }

            // Now we have a count of the most popular colours on this line, and how many fragments there are of each.
            EGACOLOR *rgOrderedColorsForThisLine = rgOrderedColorsPerLine[line];
            if (rgOrderedColorsForThisLine)
            {
                int i = 0;
                for (; i < cColors; i++)
                {
                    int cFragCount = 0;
                    BYTE bestColorIndex = 0;
                    for (WORD j = 0; j < cColors; j++)
                    {
                        BYTE indexIntoFPC = EGACOLOR_TO_BYTE(rgColors[j]);
                        if (fragmentsPerColor[indexIntoFPC] > cFragCount)
                        {
                            cFragCount = fragmentsPerColor[indexIntoFPC];
                            bestColorIndex = (BYTE)j;
                        }
                    }
                    if (cFragCount == 0)
                    {
                        break;
                    }
                    else
                    {
                        rgOrderedColorsForThisLine[i] = rgColors[bestColorIndex];
                        // zero out, so we don't count this again.
                        fragmentsPerColor[EGACOLOR_TO_BYTE(rgColors[bestColorIndex])] = 0;
                    }
                }
                rgNumberOfColorsPerLine[line] = i;
                assert(i > 0);
            }

            fAbort = _SendProgressAndCheckAbort(hwnd, hEvent, line, size.cy);
        }
    }

    if (rgAllColors && !fAbort)
    {
        _SendStatus(hwnd, (TEXT("Generating commands...")));

        // Algorithm:
        // 1) Have a number for each line, that represents where we are in the ordered array of colors for that line:
        int rgCurrentColorPosPerLine[190];
        ZeroMemory(rgCurrentColorPosPerLine, sizeof(rgCurrentColorPosPerLine));
        // 2) Have a number that indicates, in general, what are we drawing right now.
        int currentColorPosWereDrawing = 0;
        // 3) for each color pos, 0 to cColors, we do:
        for ( ; !fAbort && (currentColorPosWereDrawing < cColors); currentColorPosWereDrawing++)
        {
            fAbort = _SendProgressAndCheckAbort(hwnd, hEvent, currentColorPosWereDrawing, cColors);

            // 4) for each line, we do:
            for (int line = 0; line < size.cy; line++)
            {
                if ((currentColorPosWereDrawing < rgNumberOfColorsPerLine[line]) &&  // Verify we don't start looking at garbage colours in this line
                    (rgCurrentColorPosPerLine[line] == currentColorPosWereDrawing))  // Is this line current at the same "color pos" that we're drawing?
                {
                    // This is the colour we'll be using:
                    EGACOLOR currentColor = rgOrderedColorsPerLine[line][currentColorPosWereDrawing];

                    // Make this the current color - set it as color 0, and set the visual to taht.
                    // TODO: PERF: improve size, by setting a palette @ the beginning.
                    commands.push_back(PicCommand::CreateSetVisual(0, GetPaletteIndexOf(currentColor, rgColors, cColors)));

                    for (int realLine = line; realLine < size.cy; realLine++)
                    {
                        // Decide to draw this line if it is the same color as THE line
                        EGACOLOR *rgOrderedColorsForThisLine = rgOrderedColorsPerLine[realLine];
                        if (EGACOLOR_EQUAL(currentColor, rgOrderedColorsForThisLine[currentColorPosWereDrawing]))
                        {
                            const EGACOLOR *pLine = pegaTemp + size.cx * realLine;
                            for (int x = 0; x < size.cx; x++)
                            {
                                if (EGACOLOR_EQUAL(currentColor, *(pLine + x)))
                                {
                                    // We've hit one of our colours.  Now continue until we can't anymore.
                                    int xStart = x;
                                    int xLastExact = xStart;
                                    BOOL fExact;
                                    while ((x < size.cx) && CanConnect(*(pLine + x), rgOrderedColorsForThisLine, currentColorPosWereDrawing, &fExact))
                                    {
                                        if (fExact)
                                        {
                                            xLastExact = x; // This is the last colour that was an exact match.
                                        }
                                        x++;
                                    }
                                    ASSERT(xStart < x); // We must always hit the above loop once.
                                    //int xEnd = x - 1;
                                    int xEnd = xLastExact;

                                    if (fIgnoreWhite && IS_WHITE(currentColor) && ((xStart == 0) || (xEnd == (size.cx - 1))))
                                    {
                                        // Don't do anything.
                                    }
                                    else
                                    {
                                        // Draw line to xStart to xEnd;
                                        if (xStart == xEnd)
                                        {
                                            // Just a dot
                                            commands.push_back(PicCommand::CreatePattern((WORD)xStart, (WORD)realLine, 0, 0, FALSE, FALSE));
                                        }
                                        else
                                        {
                                            // A line
                                            commands.push_back(PicCommand::CreateLine((WORD)xStart, (WORD)realLine, (WORD)xEnd, (WORD)realLine));
                                        }
                                    }
                                }
                            }                    

                            // Increment the colorpos for this line.
                            rgCurrentColorPosPerLine[realLine]++;
                        }
                    }
                }
            }
        }
    }

    if (rgAllColors)
    {
        delete [] rgAllColors;
    }

    return fAbort;
}

void CBitmapToPicDialog::OnBnClickedRadio1()
{
    _iAlgorithm = 1;
}

void CBitmapToPicDialog::OnBnClickedRadio2()
{
    _iAlgorithm = 2;
}

void CBitmapToPicDialog::OnBnClickedRadio3()
{
    _iAlgorithm = 3;
}

void CBitmapToPicDialog::OnBnClickedRadio4()
{
    _iAlgorithm = 4;
}

void CBitmapToPicDialog::OnBnClickedRadio5()
{
    _iAlgorithm = 5;
}

void CBitmapToPicDialog::OnBnClickedRadiohalftone()
{
    _iPalette = 1;
}

void CBitmapToPicDialog::OnBnClickedRadiosolid()
{
    _iPalette = 2;
}

void CBitmapToPicDialog::OnBnClickedRadiohalftonesmooth()
{
    _iPalette = 3;
}

void CBitmapToPicDialog::OnBnClickedRadiopicpalette()
{
    _iPalette = 4;
}

void CBitmapToPicDialog::OnScaleClicked()
{
    // Get the most recent values
    UpdateData(TRUE);
}

void CBitmapToPicDialog::OnBnClickedCheckignorewhite()
{
    UpdateData(TRUE);
}


void CBitmapToPicDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    UpdateData(TRUE);
}

void CBitmapToPicDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == FROMCLIPBOARD_TIMER)
    {
        m_wndFromClipboard.EnableWindow(IsClipboardFormatAvailable(CF_BITMAP));
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void CBitmapToPicDialog::OnPasteFromClipboard()
{
    _OnPasteFromClipboard(this);
}



BOOL CBitmapToPicDialog::OnInitDialog()
{
    __super::OnInitDialog();

    // Disable the Ok button until we have a converted pic.
    GetDlgItem(IDOK)->EnableWindow(FALSE);

    // Put a little help text
    _AddToEdit(TEXT("Choose options, then click Convert"));

    OnPasteFromClipboard();

    return TRUE;
}

