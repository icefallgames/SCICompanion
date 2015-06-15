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
#include "CustomMessageBox.h"

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
    ON_COMMAND(ID_MAKEFONT, MakeFont)
    ON_UPDATE_COMMAND_UI(ID_IMPORT_IMAGESEQUENCE, OnUpdateImportImage)
    ON_UPDATE_COMMAND_UI(ID_ANIMATE, OnUpdateAnimate)
    ON_UPDATE_COMMAND_UI(ID_MAKEFONT, OnUpdateFont)
END_MESSAGE_MAP()

CNewRasterResourceDocument::CNewRasterResourceDocument()
{
    // Add ourselves as a sync
    CResourceMap &map = appState->GetResourceMap();
    map.AddSync(this);

    _currentPaletteIndex = 0;
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
    PostApplyChanges(&WrapRasterChange(RasterChangeHint::NewView));
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
        SetPaletteChoice(_currentPaletteIndex, true);
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

void CNewRasterResourceDocument::SwitchToEmbeddedPalette()
{
    if (_currentPaletteIndex < (int) _paletteChoices.size())
    {
        int id = _paletteChoices[_currentPaletteIndex];
        if ((id == EmbeddedPaletteId) || (id == EmbeddedPaletteOnlyId))
        {
            // We're good
            return;
        }
    }

    for (int i = 0; i < (int)_paletteChoices.size(); i++)
    {
        if (_paletteChoices[i] == EmbeddedPaletteOnlyId)
        {
            SetPaletteChoice(i, false);
            break;
        }
    }
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

void CNewRasterResourceDocument::PostApplyChanges(CObject *pObj)
{ 
    // If palette choice changed or view changed, then we need to update palette choice (if it's currently an embedded palette)
    // This has to be done before we calls views to update them, since they will ask us for the current palette to render with.
    RasterChangeHint hint = GetHint<RasterChangeHint>(pObj);
    if (IsFlagSet(hint, RasterChangeHint::NewView))
    {
        RefreshPaletteOptions();
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

// We need collect all the info first, then make a decision.
// We have: palette/cel combinations and gidplus images.

struct ImageSequenceItem
{
    ImageSequenceItem() {}
    ImageSequenceItem(ImageSequenceItem&& other)
    {
        Palette = other.Palette;
        Cels = other.Cels;
        Bitmap = move(other.Bitmap);
    }

    ImageSequenceItem& operator=(ImageSequenceItem&& other)
    {
        if (this != &other)
        {
            Palette = other.Palette;
            Cels = other.Cels;
            Bitmap = move(other.Bitmap);
        }
    }

    // Either we have:
    PaletteComponent Palette;
    std::vector<Cel> Cels;

    // Or we have a 24 bit bitmap in this form:
    std::unique_ptr<Bitmap> Bitmap;
};

void _FinalizeSequence(int *trackUsage, vector<Cel> &finalCels, bool optionalNewPalette, std::vector<ImageSequenceItem> &items, uint8_t transparentColor, bool isEGA16, bool dither, int colorCount, const uint8_t *paletteMapping, const RGBQUAD *colors)
{
    for (ImageSequenceItem &item : items)
    {
        if (item.Cels.empty())
        {
            // Convert the bitmap using the palette we have
            ConvertBitmapToCel(trackUsage, *item.Bitmap, transparentColor, isEGA16, dither, colorCount, paletteMapping, colors, item.Cels);
        }
        else
        {
            if (!optionalNewPalette)
            {
                // Remap cels
                for (Cel &cel : item.Cels)
                {
                    ConvertCelToNewPalette(trackUsage, cel, item.Palette, transparentColor, isEGA16, dither, colorCount, paletteMapping, colors);
                }
            }
            // else we're good
        }
        if (!trackUsage)
        {
            finalCels.insert(finalCels.end(), item.Cels.begin(), item.Cels.end());
        }
    }
}

void CNewRasterResourceDocument::_ApplyImageSequence(uint8_t transparentColor, const PaletteComponent *optionalNewPalette, std::vector<ImageSequenceItem> &items)
{
    // If optionalNewPalette isn't provided, we need to obtain the palette we're converting to.
    // That might be:
    //  - EGA palette
    //  - embedded palette
    //  - global palette
    RasterComponent &raster = GetComponent<RasterComponent>();
    // TODO: Handle transparent colors?
    bool isEGA16 = false;
    const uint8_t *paletteMapping = optionalNewPalette ? optionalNewPalette->Mapping : raster.Traits.PaletteMapping;
    int colorCount = optionalNewPalette ? 256 : 0;
    const RGBQUAD *colors = optionalNewPalette ? optionalNewPalette->Colors : raster.Traits.Palette;
    switch (raster.Traits.PaletteType)
    {
        case PaletteType::VGA_256:
        {
            colorCount = 256;
            const PaletteComponent *embedded = GetResource()->TryGetComponent<PaletteComponent>();
            if (embedded)
            {
                colors = embedded->Colors;
            }
            else
            {
                const PaletteComponent *global = this->GetCurrentPaletteComponent();
                if (global)
                {
                    colors = global->Colors;
                }
            }
            break;
        }
        case PaletteType::EGA_Four:
            colorCount = 4;
            break;
        case PaletteType::EGA_Sixteen:
            isEGA16 = true;
            colorCount = 16;
            break;
        case PaletteType::EGA_Two:
            colorCount = 2;
            break;
    }

    if (colors)
    {
        // Let's turn this into a sequence of cels before applying changes. That way the we'll just need
        // to copy the cels to the loop
        vector<Cel> finalCels;
        std::unique_ptr<int[]> trackUsage = std::make_unique<int[]>(colorCount);
        // First, do everything, but just track how often each index would be used
        _FinalizeSequence(trackUsage.get(), finalCels, !!optionalNewPalette, items, transparentColor, isEGA16, !!g_fDitherImages2, colorCount, paletteMapping, colors);
        // So that we can truly find a good transparent color
        transparentColor = (uint8_t)(std::min_element(&trackUsage[0], &trackUsage[colorCount]) - &trackUsage[0]);
        // Now do it for real
        _FinalizeSequence(nullptr, finalCels, !!optionalNewPalette, items, transparentColor, isEGA16, !!g_fDitherImages2, colorCount, paletteMapping, colors);

        int nLoop = GetSelectedLoop();

        ApplyChanges<RasterComponent>(
            [nLoop, &finalCels](RasterComponent &raster)
        {
            RasterChange chnage = ApplyCelsToLoop(raster, nLoop, finalCels);
            chnage.hint |= RasterChangeHint::PaletteChoice;
            return WrapRasterChange(chnage);
        },

            // Apply the new palette if needed.
            [optionalNewPalette](ResourceEntity &resource)
        {
            if (optionalNewPalette)
            {
                if (resource.TryGetComponent<PaletteComponent>())
                {
                    PaletteComponent &existingPalette = resource.GetComponent<PaletteComponent>();
                    existingPalette = *optionalNewPalette;
                }
                else
                {
                    resource.AddComponent<PaletteComponent>(std::make_unique<PaletteComponent>(*optionalNewPalette));
                }
            }
        }
        );
    }
}

void CNewRasterResourceDocument::_InsertFiles(const vector<string> &files)
{
    RasterComponent &raster = GetComponent<RasterComponent>();
    bool isVGA = (raster.Traits.PaletteType == PaletteType::VGA_256);

    uint8_t transparentColor = 0;   // ????

    // First, get a collection of usable images
    std::vector<ImageSequenceItem> imageSequenceItems;
    for (const string &file : files)
    {
        imageSequenceItems.emplace_back();
        ImageSequenceItem &item = imageSequenceItems.back();
        bool success = false;
        if (0 == lstrcmpi(".gif", PathFindExtension(file.c_str())))
        {
            // Try our gif loader, which understands palettes better than gdip
            success = GetCelsAndPaletteFromGIFFile(file.c_str(), item.Cels, item.Palette);
        }
        if (!success)
        {
            // Use gdiplus to load the image.
#ifdef UNICODE
            item.Bitmap.reset(Bitmap::FromFile(pszFileName));
#else
            // GDI+ only deals with unicode.
            BSTR unicodestr = SysAllocStringLen(NULL, file.length());
            MultiByteToWideChar(CP_ACP, 0, file.c_str(), file.length(), unicodestr, file.length());
            item.Bitmap.reset(Bitmap::FromFile(unicodestr));
            //... when done, free the BSTR
            SysFreeString(unicodestr);
#endif    
            success = item.Bitmap->GetLastStatus() == Gdiplus::Ok;
            if (success)
            {
                // See if we can convert this into a cel now.
                if (GetCelsAndPaletteFromGdiplus(*item.Bitmap, transparentColor, item.Cels, item.Palette))
                {
                    // Then get rid of the bitmap
                    item.Bitmap.reset(nullptr);
                }
            }
        }
        if (!success)
        {
            imageSequenceItems.pop_back();
        }
    }

    // If we got some, then figure out what to do
    if (!imageSequenceItems.empty())
    {
        // If we got a palette, then (if VGA) we need to figure out what to do. 
        // We'll just look at the first palette.
        unique_ptr<PaletteComponent> onePalette;
        for (auto &item : imageSequenceItems)
        {
            if (!item.Cels.empty())
            {
                onePalette = make_unique<PaletteComponent>(item.Palette);
                break;
            }
        }
       
        if (isVGA && onePalette)
        {
            // We have a few choices here. Lots of choices, but we'll only handle a few.
            // If it's identical to our current palette, then no problem. This could be the case when the user is importing multiple image sequences for different loops.
            // If we don't have a palette, we might want to offer the option of remapping to the global palette.
            // If we have a palette, and it doesn't match, then we can offer the option of remapping to the embedded palette.
            // Basically, we'll offer these choices:
            //  - use palette from image?
            //  - remap to embedded (or global)
            //
            // For 24bit images, or when we're EGA, we'll just remap to the embedded.
            PaletteComponent *existingPalette = GetResource()->TryGetComponent<PaletteComponent>();
            if (!existingPalette || !DoPalettesMatch(*existingPalette, *onePalette))
            {
                string remapOrApply = "Re-map";
                string message = "A palette was found in the loaded image(s) which doesn't match the current view palette.\nReplace view's palette with it, or re-map it to the view's palette?";
                if (!existingPalette)
                {
                    message = "A palette was found in the loaded image(s).\nApply this to the view or re-map to the currently selected palette?";
                    remapOrApply = "Apply";
                }

                MessageBoxCustomization mb(
                { { MessageBoxCustomization::Yes, "Replace" }, { MessageBoxCustomization::No, remapOrApply } }
                );
                if (IDNO == AfxMessageBox(message.c_str(), MB_YESNO | MB_ICONINFORMATION))
                {
                    // Pretend there wasn't a palette in the loaded image, which will then force us to re-map
                    onePalette.reset(nullptr);
                }
            }
        }
        else
        {
            onePalette.reset(nullptr); // Don't need this
        }

        // At this point, if onePalette is not null, we should apply it to the view and directly copy any cel data (gdiplus bitmaps will still be remapped)
        // Otherwise, we should remap everything based on RGB.
        _ApplyImageSequence(transparentColor, onePalette.get(), imageSequenceItems);
    }

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

void CNewRasterResourceDocument::OnUpdateFont(CCmdUI *pCmdUI)
{
    const ResourceEntity *resource = GetResource();
    if (resource && resource->TryGetComponent<FontComponent>())
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
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

void CNewRasterResourceDocument::OnCloseDocument()
{
    // Remove ourselves as a sync
    CResourceMap &map = appState->GetResourceMap();
    map.RemoveSync((ISyncResourceMap*)this);
    __super::OnCloseDocument();
}

void CNewRasterResourceDocument::OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior)
{
    if (pData->GetType() == ResourceType::Palette)
    {
        RefreshPaletteOptions();
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::NewView));
    }
}