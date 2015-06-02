#include "stdafx.h"
#include "NoFlickerStatic.h"
#include "PrepareBitmapBase.h"
#include "PaletteOperations.h"
#include "ImageUtil.h"

using namespace std;

PrepareBitmapBase::PrepareBitmapBase(int convertButtonId, int staticOriginalImageId) : _convertButtonId(convertButtonId), _staticOriginalImageId(staticOriginalImageId), _disableAllEffects(false)
{
}

PrepareBitmapBase::~PrepareBitmapBase()
{
}

//
// Updates the original bitmap.  This does the following:
// 1) Copies either _pbmpNormal or _pbmpScaled to _pbmpCurrent
//
void PrepareBitmapBase::_UpdateOrigBitmap(CWnd *pwnd)
{
    // Clear out the old Gdiplus Bitmap
    if (_pbmpCurrent)
    {
        delete _pbmpCurrent;
        _pbmpCurrent = nullptr;
    }

    // Get a clone of the appropriate bitmap
    if (_iScaleImage && !_disableAllEffects)
    {
        if (_pbmpScaled)
        {
            _pbmpCurrent = (Gdiplus::Bitmap*)((Gdiplus::Image*)_pbmpScaled)->Clone();
            _nContrastCenter = _bContrastCenterScaled;
        }
    }
    else
    {
        if (_pbmpNormal)
        {
            _pbmpCurrent = (Gdiplus::Bitmap*)((Gdiplus::Image*)_pbmpNormal)->Clone();
            _nContrastCenter = _bContrastCenterNormal;
        }
    }

    if (_pbmpCurrent)
    {
        if (!_disableAllEffects)
        {
            _ApplySettingsToCurrentBitmap();
        }

        HBITMAP hbmpOrig = nullptr;
        if (Ok == _pbmpCurrent->GetHBITMAP(Color::Black, &hbmpOrig))
        {
            ASSERT(hbmpOrig);
            CBitmap bitmapOrig;
            bitmapOrig.Attach(hbmpOrig);
            CExtNoFlickerStatic *pwndOrig = static_cast<CExtNoFlickerStatic*>(pwnd->GetDlgItem(_staticOriginalImageId));
            if (pwndOrig->GetSafeHwnd())
            {
                pwndOrig->FromBitmap(hbmpOrig, (int)_pbmpCurrent->GetWidth(), (int)_pbmpCurrent->GetHeight());
                // Redraw right away
                pwndOrig->RedrawWindow();
            }
            DeleteObject(hbmpOrig);
        }
    }
}


//
// Ensures we have a _pCRBitmap: the array of COLORREF values to convert
//
BOOL PrepareBitmapBase::_ReallocateCRBitmap(CSize size)
{
    if ((size != _size) || (_pCRBitmap == nullptr))
    {
        delete[] _pCRBitmap;
        _size = size;
        _pCRBitmap = new COLORREF[_size.cx * _size.cy];
    }
    return (_pCRBitmap != nullptr);
}

//
// Prepares _pbmpCurrent for conversion.
//
void PrepareBitmapBase::_PrepareBitmapForConversion()
{
    if (_pbmpCurrent)
    {
        CSize size((int)_pbmpCurrent->GetWidth(), (int)_pbmpCurrent->GetHeight());
        if (_ReallocateCRBitmap(size))
        {
            for (int y = 0; y < size.cy; y++)
            {
                COLORREF *pLineCR = _pCRBitmap + size.cx * y;
                for (int x = 0; x < size.cx; x++)
                {
                    Gdiplus::Color color;
                    if (Ok == _pbmpCurrent->GetPixel(x, y, &color))
                    {
                        *(pLineCR + x) = color.ToCOLORREF();
                    }
                }
            }
        }
    }
}



void PrepareBitmapBase::_ApplySettingsToCurrentBitmap()
{
    int nBrightness = (_nBrightness - 50) * 255 / 50; // 0 - 100  --> -255 to 255

    int nContrast = (100 - _nContrast);
    nContrast = max(1, nContrast); // avoid divide by zero
    nContrast = 255 * 50 / nContrast;

    int nSat = (100 - _nSaturation);
    nSat = max(1, nSat); // avoid divide by zero
    nSat = 255 * 50 / nSat;

    UINT cx = _pbmpCurrent->GetWidth();
    UINT cy = _pbmpCurrent->GetHeight();

    Gdiplus::Rect rect(0, 0, cx, cy);
    Gdiplus::BitmapData bitmapData;
    if (Ok == _pbmpCurrent->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData))
    {
        int nCCGreen = 0;
        int nCCRed = 0;
        int nCCBlue = 0;
        for (UINT y = 0; y < cy; y++)
        {
            ARGB *pLine = ((COLORREF*)bitmapData.Scan0) + y * bitmapData.Width;
            for (UINT x = 0; x < cx; x++, pLine++)
            {
                Gdiplus::Color color;
                color.SetValue(*pLine);

                int red = (int)color.GetR();
                int blue = (int)color.GetB();
                int green = (int)color.GetG();
                int alpha = (int)color.GetA();
                if (nBrightness != 0)
                {
                    red += nBrightness;
                    blue += nBrightness;
                    green += nBrightness;
                }

                red = min(255, max(red, 0));
                blue = min(255, max(blue, 0));
                green = min(255, max(green, 0));
                nCCRed += red;
                nCCGreen += green;
                nCCBlue += blue;

                *pLine = Gdiplus::Color::MakeARGB(alpha, red, green, blue);
            }
        }
        nCCGreen /= (cx * cy);
        nCCRed /= (cx * cy);
        nCCBlue /= (cx * cy);

        for (UINT y = 0; y < cy; y++)
        {
            ARGB *pLine = ((COLORREF*)bitmapData.Scan0) + y * bitmapData.Width;
            for (UINT x = 0; x < cx; x++, pLine++)
            {
                Gdiplus::Color color;
                color.SetValue(*pLine);

                int red = (int)color.GetR();
                int blue = (int)color.GetB();
                int green = (int)color.GetG();
                int alpha = (int)color.GetA();
                if (nContrast != 255)
                {
                    red = ((red - nCCRed) * nContrast / 255) + nCCRed;
                    green = ((green - nCCGreen) * nContrast / 255) + nCCGreen;
                    blue = ((blue - nCCBlue) * nContrast / 255) + nCCBlue;
                }

                if (nSat != 255)
                {
                    // Take the avg of the colours
                    int avg = (red + green + blue) / 3;

                    // Everything will tend towards or away from this colour.
                    red = ((red - avg) * nSat / 255) + avg;
                    blue = ((blue - avg) * nSat / 255) + avg;
                    green = ((green - avg) * nSat / 255) + avg;
                }

                red = min(255, max(red, 0));
                blue = min(255, max(blue, 0));
                green = min(255, max(green, 0));
                *pLine = Gdiplus::Color::MakeARGB(alpha, red, green, blue);
            }
        }


        _pbmpCurrent->UnlockBits(&bitmapData);
    }
}

void PrepareBitmapBase::_CalculateContrastCenter(Gdiplus::Bitmap *pBitmap, BYTE *pbContrastCenter)
{
    UINT cx = pBitmap->GetWidth();
    UINT cy = pBitmap->GetHeight();

    Gdiplus::Rect rect(0, 0, cx, cy);
    Gdiplus::BitmapData bitmapData;
    int iTotal = 0;
    if (Ok == pBitmap->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData))
    {
        for (UINT y = 0; y < cy; y++)
        {
            ARGB *pLine = ((COLORREF*)bitmapData.Scan0) + y * bitmapData.Width;
            for (UINT x = 0; x < cx; x++, pLine++)
            {
                Gdiplus::Color color;
                color.SetValue(*pLine);
                iTotal += (int)color.GetR() + (int)color.GetB() + (int)color.GetG();
            }
        }
        iTotal /= (cx * cy * 3);
        *pbContrastCenter = (BYTE)iTotal;
        pBitmap->UnlockBits(&bitmapData);
    }
}

BOOL PrepareBitmapBase::_Init(std::unique_ptr<Gdiplus::Bitmap> pImageIn, CWnd *pwnd)
{
    _pbmpOrig = std::move(pImageIn);
    // Let's see if we can extract a palette.
    // Note that if we're using the original palette, we can't scale or adjust the thingies.
    _originalPalette.reset(nullptr);
    _numberOfPaletteEntries = 0;
    _originalPalette = GetPaletteFromImage(_pbmpOrig.get(), &_numberOfPaletteEntries);

    CSize size(min(sPIC_WIDTH, _pbmpOrig->GetWidth()), min(sPIC_HEIGHT, _pbmpOrig->GetHeight()));
    BOOL fRet = FALSE;

    // Clear out any old ones.
    delete _pbmpNormal;
    delete _pbmpScaled;
    _pbmpNormal = nullptr;
    _pbmpScaled = nullptr;

    // Make a scaled and normal copy of the original.
    CDC dcTemp;
    if (dcTemp.CreateCompatibleDC(nullptr))
    {
        _pbmpNormal = new Gdiplus::Bitmap(size.cx, size.cy, PixelFormat32bppARGB);
        if (_pbmpNormal)
        {
            // WOW! This is crucial. Our "non-scaled" image will be scaled if we don't set these to whatever
            // the default screen resolution is.
            REAL verticalDPI = _pbmpNormal->GetVerticalResolution();
            REAL horizontalDPI = _pbmpNormal->GetHorizontalResolution();
            _pbmpOrig->SetResolution(horizontalDPI, verticalDPI);

            Graphics graphics(_pbmpNormal);
            fRet = (Ok == graphics.DrawImage(_pbmpOrig.get(), 0, 0, 0, 0, _pbmpOrig->GetWidth(), _pbmpOrig->GetHeight(), UnitPixel));
            if (fRet)
            {
                _CalculateContrastCenter(_pbmpNormal, &_bContrastCenterNormal);
            }
        }

        if (fRet)
        {
            _pbmpScaled = new Gdiplus::Bitmap(sPIC_WIDTH, sPIC_HEIGHT, PixelFormat32bppARGB);
            if (_pbmpScaled)
            {
                Graphics graphics(_pbmpScaled);
                fRet = (Ok == graphics.DrawImage(_pbmpOrig.get(), 0, 0, sPIC_WIDTH, sPIC_HEIGHT));
                if (fRet)
                {
                    _CalculateContrastCenter(_pbmpScaled, &_bContrastCenterScaled);
                }
            }
        }
    }

    // Enable the convert button upon success
    if (_convertButtonId != -1)
    {
        ((CButton*)pwnd->GetDlgItem(_convertButtonId))->EnableWindow(fRet);
    }

    return fRet;
}

void PrepareBitmapBase::_OnPasteFromClipboard(CWnd *pwnd)
{
    if (IsClipboardFormatAvailable(CF_BITMAP))
    {
        OpenClipboardGuard guard(pwnd);
        if (guard.IsOpen())
        {
            HBITMAP hBmp = (HBITMAP)GetClipboardData(CF_BITMAP);

            unique_ptr<Gdiplus::Bitmap> pBitmap(Gdiplus::Bitmap::FromHBITMAP(hBmp, nullptr));
            if (pBitmap)
            {
                if (_Init(move(pBitmap), pwnd))
                {
                    _UpdateOrigBitmap(pwnd);
                }
            }
        }
    }
}


void PrepareBitmapBase::_OnBrowse(CWnd *pwnd)
{
    CFileDialog dialog(TRUE, nullptr, nullptr, 0, g_szGdiplusFilter);
    if (IDOK == dialog.DoModal())
    {
        CString strFileName = dialog.GetPathName();
#ifdef UNICODE
        unique_ptr<Gdiplus::Bitmap> pImage(Bitmap::FromFile(strFileName));
#else
        // GDI+ only deals with unicode.
        int a = lstrlenA(strFileName);
        BSTR unicodestr = SysAllocStringLen(nullptr, a);
        MultiByteToWideChar(CP_ACP, 0, strFileName, a, unicodestr, a);
        unique_ptr<Gdiplus::Bitmap> pImage(Bitmap::FromFile(unicodestr));

        // Just test various things, for investigation:
        PixelFormat format = pImage->GetPixelFormat();
        UINT dimensionsCount = pImage->GetFrameDimensionsCount();
        if (dimensionsCount > 0)
        {
            std::unique_ptr<GUID[]> dimensionIds = make_unique<GUID[]>(dimensionsCount);
            pImage->GetFrameDimensionsList(dimensionIds.get(), dimensionsCount);
            for (UINT i = 0; i < dimensionsCount; i++)
            {
                UINT frameCount = pImage->GetFrameCount(&dimensionIds[i]);
                int x = frameCount;
            }
        }

        //... when done, free the BSTR
        SysFreeString(unicodestr);
#endif    
        if (pImage)
        {
            if (_Init(move(pImage), pwnd))
            {
                _UpdateOrigBitmap(pwnd);
            }
        }
    }
}