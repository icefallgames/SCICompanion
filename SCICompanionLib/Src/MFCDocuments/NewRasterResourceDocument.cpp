// RasterResourceDocument.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NewRasterResourceDocument.h"
#include "AnimateDialog.h"
#include "FontOperations.h"
#include "PaletteOperations.h"
#include "format.h"
#include "ImageUtil.h"

volatile BOOL g_fDitherImages2 = FALSE;

// A sort of workaround
CHintWithObject<CelIndex> WrapRasterChange(RasterChange change) { return CHintWithObject<CelIndex>(static_cast<uint32_t>(change.hint), change.index); }

using namespace std;
// CNewRasterResourceDocument

const int EmbeddedPaletteId = -1;
const int EmbeddedPaletteOnlyId = -2;

IMPLEMENT_DYNCREATE(CNewRasterResourceDocument, CResourceDocument)

BEGIN_MESSAGE_MAP(CNewRasterResourceDocument, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTASBITMAP, CResourceDocument::OnUpdateAlwaysOn) // Put this here instead of in CResourceDocument
    ON_COMMAND(ID_ANIMATE, OnAnimate)
    ON_COMMAND(ID_IMPORT_IMAGESEQUENCE, OnImportImageSequence)
    ON_UPDATE_COMMAND_UI(ID_IMPORT_IMAGESEQUENCE, OnUpdateImportImage)
    ON_UPDATE_COMMAND_UI(ID_ANIMATE, OnUpdateAnimate)


END_MESSAGE_MAP()

CNewRasterResourceDocument::CNewRasterResourceDocument()
{
    _nPenWidth = 1;
    _fLocked = false;
    _fApplyToAllCels = FALSE;
    _nCel = 0;
    _nLoop = 0;
    _color = 0xf;           // white, in EGA
    _alternateColor = 0x0;  // black, in EGA
    memset(_currentPaletteVGA, 0, sizeof(_currentPaletteVGA));
}

void CNewRasterResourceDocument::v_OnUndoRedo()
{
    _ValidateCelIndex();
    _UpdateHelper(RasterChange(RasterChangeHint::NewView));
}

void CNewRasterResourceDocument::SetNewResource(std::unique_ptr<ResourceEntity> pResource)
{
    SetResource(move(pResource));
}

void CNewRasterResourceDocument::_SetInitialPalette()
{
    RasterComponent &raster = GetComponent<RasterComponent>();
    if (raster.Traits.PaletteType == PaletteType::VGA_256)
    {
        SetPaletteChoice(0, true);
    }
}

void CNewRasterResourceDocument::RefreshPaletteOptions()
{
    _paletteChoices = appState->GetResourceMap().GetPaletteList();
    const ResourceEntity *pResource = GetResource();
    if (pResource && pResource->TryGetComponent<PaletteComponent>())
    {
        _paletteChoices.insert(_paletteChoices.begin(), EmbeddedPaletteId);
        _paletteChoices.push_back(EmbeddedPaletteOnlyId);
    }
    _SetInitialPalette();
    _UpdateHelper(RasterChange(RasterChangeHint::NewView));
}

void CNewRasterResourceDocument::SetResource(std::unique_ptr<ResourceEntity> pResource, int id)
{
    _checksum = id;

    AddFirstResource(move(pResource));
    RefreshPaletteOptions();
    _ValidateCelIndex();
    _UpdateHelper(RasterChange(RasterChangeHint::NewView));
    _UpdateTitle();
}

std::string CNewRasterResourceDocument::GetPaletteChoiceName(int index)
{
    std::string name;
    if (index < (int)_paletteChoices.size())
    {
        if (_paletteChoices[index] == EmbeddedPaletteId)
        {
            name = "Embedded";
        }
        else if (_paletteChoices[index] == EmbeddedPaletteOnlyId)
        {
            name = "Embedded only";
        }
        else
        {
            name = fmt::format("{0}", _paletteChoices[index]);
        }
    }
    return name;
}

void CNewRasterResourceDocument::SetPaletteChoice(int index, bool force)
{
    if ((force || (index != _currentPaletteIndex)) && (index >= 0) && (index < (int)_paletteChoices.size()))
    {
        _currentPaletteIndex = index;
        int choice = _paletteChoices[index];
        if (choice == EmbeddedPaletteId)
        {
            _currentPaletteComponent = appState->GetResourceMap().GetMergedPalette(*GetResource(), 999);
        }
        else if (choice == EmbeddedPaletteOnlyId)
        {
            PaletteComponent *temp = GetResource()->TryGetComponent<PaletteComponent>();
            if (temp)
            {
                _currentPaletteComponent = std::make_unique<PaletteComponent>(*temp);
            }
            else
            {
                _currentPaletteComponent.reset(nullptr);
            }
        }
        else
        {
            _currentPaletteComponent = appState->GetResourceMap().GetPalette(choice);
        }

        if (_currentPaletteComponent)
        {
            memcpy(_currentPaletteVGA, _currentPaletteComponent->Colors, sizeof(_currentPaletteVGA));
        }

        _UpdateHelper(RasterChange(RasterChangeHint::PaletteChoice));
    }
}

void CNewRasterResourceDocument::SetPenWidth(int nPenWidth)
{
    int nNewPenWidth = max(1, min(8, nPenWidth));
    if (nNewPenWidth != _nPenWidth)
    {
        _nPenWidth = nNewPenWidth;
        _UpdateHelper(RasterChange(RasterChangeHint::PenWidth));
    }
}

void CNewRasterResourceDocument::SetViewColor(uint8_t color)
{
    _color = color;
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::Color));
}

void CNewRasterResourceDocument::SetAlternateViewColor(uint8_t color)
{
    _alternateColor = color;
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::Color));
}

COLORREF CNewRasterResourceDocument::SCIColorToCOLORREF(uint8_t color)
{
    // The correct thing to do is to check our traits
    COLORREF colorRef = RGB(255, 0, 255);
    const ResourceEntity *pResource = GetResource();
    if (pResource)
    {
        RGBQUAD rgb;
        RasterComponent &raster = GetComponent<RasterComponent>();
        if (raster.Traits.PaletteType == PaletteType::VGA_256)
        {
            rgb = _currentPaletteVGA[color];

        }
        else
        {
            rgb = g_egaColors[color % 16];
        }
        colorRef = RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
    }
    return colorRef;
}

const RGBQUAD *CNewRasterResourceDocument::GetPaletteVGA() const
{
    return _currentPaletteVGA;
}

const PaletteComponent *CNewRasterResourceDocument::GetCurrentPaletteComponent() const
{
    return _currentPaletteComponent.get();
}

int CNewRasterResourceDocument::GetSelectedGroup(CelIndex *rgGroups, size_t ceGroup)
{
    int iRet = 0;
    if (_fApplyToAllCels)
    {
        RasterComponent &raster = GetComponent<RasterComponent>();
        size_t iCelCount = raster.Loops[_nLoop].Cels.size();
        if (iCelCount <= ceGroup)
        {
            iRet = iCelCount;
            for (size_t i = 0; i < iCelCount; i++)
            {
                rgGroups[i] = CelIndex(_nLoop, (int)i);
            }
        }
    }
    else
    {
        // Gets the  group indices of the selected cels in this resource. (e.g. all the cels of one view loop, if
        // drawing is applied to all cels).  By default, this is just the single selected cel.
        assert(ceGroup > 0);
        if (ceGroup > 0)
        {
            *rgGroups = GetSelectedIndex();
            iRet = 1;
        }
        else
        {
            iRet = 0;
        }
    }
    return iRet;
}

//
// Ensures the select loop/cel exist in the current resource.
//
void CNewRasterResourceDocument::_ValidateCelIndex()
{
    RasterComponent &raster = GetComponent<RasterComponent>();
    int cLoops = (int)raster.Loops.size();
    _nLoop = min(_nLoop, cLoops - 1);
    _nLoop = max(_nLoop, 0);

    int cCels = (int)raster.Loops[_nLoop].Cels.size();
    _nCel = min(_nCel, cCels - 1);
    _nCel = max(_nCel, 0);
}

void CNewRasterResourceDocument::SetSelectedLoop(int nLoop)
{
    if (nLoop != _nLoop)
    {
        _nLoop = nLoop;
        _ValidateCelIndex();
        _UpdateHelper(RasterChange(RasterChangeHint::LoopSelection));
    }
}

void CNewRasterResourceDocument::_UpdateHelper(RasterChange change)
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapObject(change.hint, change.index));
}

void CNewRasterResourceDocument::SetSelectedCel(int nCel)
{
    // It is essential to check this, otherwise we'll end up in an infinite loop!
    if (nCel != _nCel)
    {
        _nCel = nCel;
        _ValidateCelIndex();
        _UpdateHelper(RasterChange(RasterChangeHint::CelSelection));
    }
}

//
// Allows the callers to move "selection"
//
void CNewRasterResourceDocument::MoveSelectedCel(CPoint point)
{
    // if the y changed, that indicates a loop change.
    if (point.y != 0)
    {
        int nLoop = _nLoop + point.y;
        SetSelectedLoop(nLoop);
    }
    if (point.x != 0)
    {
        int nCel = _nCel + point.x;
        SetSelectedCel(nCel);
    }
}

int CNewRasterResourceDocument::GetDefaultZoom() const
{
    return GetComponent<RasterComponent>().Settings.DefaultZoom;
}
void CNewRasterResourceDocument::SetDefaultZoom(int iZoom) const
{
    GetComponent<RasterComponent>().Settings.DefaultZoom = iZoom;
}

void GetSCIBitsFromFileName(PCTSTR pszFileName, BOOL bTransparent, uint16_t &width, uint16_t &height, std::vector<std::unique_ptr<BYTE[]>> &celsBits)
{
#ifdef UNICODE
    Bitmap *pgdiplusBitmap = Bitmap::FromFile(pszFileName);
#else
    // GDI+ only deals with unicode.
    int a = lstrlenA(pszFileName);
    BSTR unicodestr = SysAllocStringLen(NULL, a);
    MultiByteToWideChar(CP_ACP, 0, pszFileName, a, unicodestr, a);
    Bitmap *pgdiplusBitmap = Bitmap::FromFile(unicodestr);
    //... when done, free the BSTR
    SysFreeString(unicodestr);
#endif    
    if (pgdiplusBitmap)
    {
        width = (uint16_t)pgdiplusBitmap->GetWidth();
        height = (uint16_t)pgdiplusBitmap->GetHeight();

        // Clamp size to allowed values.
        width = min(320, width);
        width = max(1, width);
        height = min(190, height);
        height = max(1, height);

        std::unique_ptr<PaletteComponent> palette = GetPaletteFromImage(pgdiplusBitmap);
        // There are a number of possibilities here:
        // 1) EGA, no problem, we ignore palettes
        // 1) VGA with palette that corresponds to current pic palette, no problem
        // 2) VGA with palette that conflicts with current pic palette, no problem
        // 3) VGA with no palette.... ugh, so many possibilities.

        UINT count = pgdiplusBitmap->GetFrameDimensionsCount();
        std::unique_ptr<GUID[]> dimensionIds = std::make_unique<GUID[]>(count);
        pgdiplusBitmap->GetFrameDimensionsList(dimensionIds.get(), count);
        UINT frameCount = pgdiplusBitmap->GetFrameCount(&dimensionIds.get()[0]);
        for (UINT frame = 0; frame < frameCount; frame++)
        {
            GUID guid = FrameDimensionTime;
            pgdiplusBitmap->SelectActiveFrame(&guid, frame);

            PixelFormat temp = pgdiplusBitmap->GetPixelFormat();

            // Allocate our SCI bitmap data.
            std::unique_ptr<uint8_t[]> bits = make_unique<uint8_t[]>(CX_ACTUAL(width)* height);
            for (int y = 0; y < height; y++)
            {
                BYTE *pBitsRow = bits.get() + ((height - y - 1) * CX_ACTUAL(width));
                for (int x = 0; x < width; x++)
                {
                    Color color;
                    if (Ok == pgdiplusBitmap->GetPixel(x, y, &color))
                    {
                        // find closest match.
                        EGACOLOR curColor = GetClosestEGAColor(1, g_fDitherImages2 ? 3 : 2, color.ToCOLORREF());
                        *(pBitsRow + x) = ((x^y) & 1) ? curColor.color1 : curColor.color2;
                    }
                }
            }
            celsBits.push_back(move(bits));
        }
        delete pgdiplusBitmap;
    }
}

void CNewRasterResourceDocument::_InsertFiles(const vector<string> &files)
{
    ApplyChanges<RasterComponent>(
        [&](RasterComponent &raster)
        {
            // Use the current loop.
            int nLoop = GetSelectedLoop();

            Loop &loop = raster.Loops[nLoop];

            // Delete all but cel 0.
            int cCels = (int)loop.Cels.size();
            for (int i = cCels - 1; i > 0; i--)
            {
                ::RemoveCel(raster, CelIndex(nLoop, i));
            }

            int nCel = 0;
            uint8_t transparentColor = loop.Cels[0].TransparentColor;

            for (const string &file : files)
            {
                uint16_t width, height;
                std::vector<std::unique_ptr<BYTE[]>> celsBits;
                GetSCIBitsFromFileName(file.c_str(), transparentColor, width, height, celsBits);
                for (auto &celBits : celsBits)
                {
                    CelIndex celIndex(nLoop, nCel);
                    if (nCel > 0)
                    {
                        // Loop already has one cel.  Use it - but for subsequent cels,
                        // we need to add them.
                        ::InsertCel(raster, CelIndex(nLoop, nCel - 1), false, false);
                    }
                    ::SetSize(raster, celIndex, size16(width, height), RasterResizeFlags::Normal);
                    ::SetBitmapData(raster, celIndex, celBits.get());
                    nCel++;
                }
            }
            return WrapHint(RasterChangeHint::NewView);
        }
        );

    // Reset the selection.
    _nCel = 0;
}


#define MULTISELECT_BUFFERLENGTH 4096

class CDitherFileDialog : public CFileDialog
{
public:
    CDitherFileDialog(int fDither) : CFileDialog(TRUE, nullptr, nullptr,
        fDither ? OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_READONLY : OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT,
        g_szGdiplusFilter,
        nullptr,
        0,
        false)
    {
        _pszOldBuffer = GetOFN().lpstrFile;
        GetOFN().nMaxFile = MULTISELECT_BUFFERLENGTH;
        GetOFN().lpstrFile = new TCHAR[MULTISELECT_BUFFERLENGTH];
        *GetOFN().lpstrFile = 0;
        _fDither = fDither;
    }

    ~CDitherFileDialog()
    {
        delete[] GetOFN().lpstrFile;
        GetOFN().lpstrFile = _pszOldBuffer;
    }

    BOOL GetDither()
    {
        return _fDither;
    }

protected:
    void OnInitDone()
    {
        __super::OnInitDone();
        this->GetParent()->SetDlgItemText(chx1, TEXT("Dither images"));
    }

    BOOL OnFileNameOK()
    {
        // Get the state of the checkbox (OFN_READONLY doesn't work when multi-select)
        HWND hwndCheck = GetParent()->GetDlgItem(chx1)->GetSafeHwnd();
        _fDither = (::SendMessage(hwndCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
        return __super::OnFileNameOK();
    }

private:
    TCHAR *_pszOldBuffer;
    BOOL _fDither;
};

void CNewRasterResourceDocument::OnImportImageSequence()
{
    // Create a file dialog.
    CDitherFileDialog fileDialog(g_fDitherImages2);
    fileDialog.m_ofn.lpstrTitle = TEXT("Import image sequence");
    if (IDOK == fileDialog.DoModal())
    {
        g_fDitherImages2 = fileDialog.GetDither();
        vector<string> fileList;
        POSITION pos = fileDialog.GetStartPosition();
        while (pos != nullptr)
        {
            fileList.push_back((PCTSTR)fileDialog.GetNextPathName(pos));
        }
        sort(fileList.begin(), fileList.end());
        _InsertFiles(fileList);
    }
}

void CNewRasterResourceDocument::OnUpdateImportImage(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(
        !appState->_fNoGdiPlus &&
        ((GetComponent<RasterComponent>().Traits.Caps & RasterCaps::Animate) != RasterCaps::None)
        );
}

void CNewRasterResourceDocument::OnUpdateAnimate(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CNewRasterResourceDocument::OnAnimate()
{
    if (HasUndos() && !_fLocked)
    {
        CAnimateDialog dialog(nullptr, GetComponent<RasterComponent>(), _currentPaletteComponent.get());
        dialog.SetTitle(GetTitle());
        dialog.SetLoop(_nLoop);
        dialog.DoModal();
    }
}

void CNewRasterResourceDocument::MakeFont()
{
    CFontDialog fontDialog;
    if (IDOK == fontDialog.DoModal())
    {
        LOGFONT *pLogFont = fontDialog.m_cf.lpLogFont;
        if (pLogFont)
        {
            FontComponent &font = GetComponent<FontComponent>();
            ApplyChanges<RasterComponent>(
                [&](RasterComponent &raster)
            {
                InitFontFromLOGFONT(raster, font, pLogFont);
                return WrapHint(RasterChangeHint::Cel | RasterChangeHint::NewView);
            }
            );
        }
    }
}
