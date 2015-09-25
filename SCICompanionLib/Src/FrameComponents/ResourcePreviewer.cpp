// ResourcePreviewer.cpp : implementation file
//
// Contains implementations of all the resource previewers.
//
#include "stdafx.h"
#include "AppState.h"
#include "ResourcePreviewer.h"
#include "CompiledScript.h"
#include "Vocab99x.h"
#include "Vocab000.h"
#include "PicDrawManager.h"
#include "Components.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
#include "SoundUtil.h"
#include "MidiPlayer.h"
#include "AudioPlayback.h"
#include "ResourceEntity.h"
#include "SummarizeScript.h"
#include "Audio.h"
#include "format.h"
#include <vfw.h>
#include "SoundOperations.h"
#include "ResourceUtil.h"
#include "Font.h"

BOOL ResourcePreviewer::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);
    return fRet;
}

//
// Pic Previewer
//
void PicPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_STATICVISUAL, m_wndVisual);
    DDX_Control(pDX, IDC_GROUPVISUAL, m_wndVisualGroup);
    m_wndVisualGroup.SetStyle(CExtGroupBox::e_style_t::STYLE_CAPTION);
    DDX_Control(pDX, IDC_STATICPRIORITY, m_wndPriority);
    DDX_Control(pDX, IDC_GROUPPRIORITY, m_wndPriorityGroup);
    m_wndPriorityGroup.SetStyle(CExtGroupBox::e_style_t::STYLE_CAPTION);
    DDX_Control(pDX, IDC_STATICCONTROL, m_wndControl);
    DDX_Control(pDX, IDC_GROUPCONTROL, m_wndControlGroup);
    m_wndControlGroup.SetStyle(CExtGroupBox::e_style_t::STYLE_CAPTION);

    // Visuals
    DDX_Control(pDX, IDC_BUTTON1, m_wndButton1);
    DDX_Control(pDX, IDC_BUTTON2, m_wndButton2);
    DDX_Control(pDX, IDC_BUTTON3, m_wndButton3);
    DDX_Control(pDX, IDC_BUTTON4, m_wndButton4);
    DDX_Control(pDX, IDC_STATICPALETTE, m_wndStaticPalette);
}

PicPreviewer::~PicPreviewer()
{
}

BEGIN_MESSAGE_MAP(PicPreviewer, ResourcePreviewer)
    ON_COMMAND_EX(IDC_BUTTON1, OnSetPalette)
    ON_COMMAND_EX(IDC_BUTTON2, OnSetPalette)
    ON_COMMAND_EX(IDC_BUTTON3, OnSetPalette)
    ON_COMMAND_EX(IDC_BUTTON4, OnSetPalette)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CNoFlickerStatic message handlers
BOOL PicPreviewer::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

BOOL PicPreviewer::OnSetPalette(UINT nID)
{
    ASSERT((IDC_BUTTON4 - IDC_BUTTON1) == 3);
    _paletteNumber = (BYTE)(nID - IDC_BUTTON1);
    PicDrawManager pdm(&_pic->GetComponent<PicComponent>(), _pic->TryGetComponent<PaletteComponent>());

    pdm.SetPalette(_paletteNumber);
    _ResetVisualBitmap(_pic->GetComponent<PicComponent>(), pdm);

    // Update buttons states - ON_UPDATE_COMMAND_UI doesn't work in dialogs.
    for (UINT buttonId = IDC_BUTTON1; buttonId <= IDC_BUTTON4; buttonId++)
    {
        CExtButton *pButton = static_cast<CExtButton*>(GetDlgItem(buttonId));
        pButton->SetCheck(buttonId == nID);
    }
    return TRUE;
}

void PicPreviewer::OnUpdatePaletteButton(CCmdUI *pCmdUI)
{
    BYTE paletteNumber = (BYTE)(pCmdUI->m_nID - IDC_BUTTON1);
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(paletteNumber == _paletteNumber);
}

void PicPreviewer::_ResetVisualBitmap(const PicComponent &pic, PicDrawManager &pdm)
{
    CRect rc;
    m_wndVisual.GetClientRect(&rc);
    CBitmap bitmap;
    bitmap.Attach(pdm.CreateBitmap(PicScreen::Visual, PicPosition::Final, pic.Size, rc.Width(), rc.Height()));
    m_wndVisual.FromBitmap((HBITMAP)bitmap, rc.Width(), rc.Height(), true);
}

void PicPreviewer::SetResource(const ResourceBlob &blob)
{
    _pic = CreateResourceFromResourceData(blob);

    int showSCI0PaletteControls = (appState->GetVersion().PicFormat == PicFormat::EGA) ? SW_SHOW : SW_HIDE;

    m_wndButton1.ShowWindow(showSCI0PaletteControls);
    m_wndButton2.ShowWindow(showSCI0PaletteControls);
    m_wndButton3.ShowWindow(showSCI0PaletteControls);
    m_wndButton4.ShowWindow(showSCI0PaletteControls);
    m_wndStaticPalette.ShowWindow(showSCI0PaletteControls);

    const PicComponent &pic = _pic->GetComponent<PicComponent>();
    PicDrawManager pdm(&pic, _pic->TryGetComponent<PaletteComponent>());
    pdm.SetPalette(_paletteNumber);
    pdm.RefreshAllScreens(PicScreenFlags::All, PicPositionFlags::Final); // Be efficient - we're going to get all 3 screens.
    _ResetVisualBitmap(pic, pdm);

    // Do the priority and controls too.
    CRect rc;
    m_wndVisual.GetClientRect(&rc);
    CBitmap bitmapP;
    bitmapP.Attach(pdm.CreateBitmap(PicScreen::Priority, PicPosition::Final, pic.Size, rc.Width(), rc.Height()));
    m_wndPriority.FromBitmap((HBITMAP)bitmapP, rc.Width(), rc.Height(), true);
    CBitmap bitmapC;
    bitmapC.Attach(pdm.CreateBitmap(PicScreen::Control, PicPosition::Final, pic.Size, rc.Width(), rc.Height()));
    m_wndControl.FromBitmap((HBITMAP)bitmapC, rc.Width(), rc.Height(), true);
}

//
// View previewer
//
BEGIN_MESSAGE_MAP(ViewPreviewer, ResourcePreviewer)
END_MESSAGE_MAP()

void ViewPreviewer::SetResource(const ResourceBlob &blob)
{
    //
    // Generate a large bitmap containing all the views.
    //
    _view = CreateResourceFromResourceData(blob);
    CBitmap bitmap;
    SCIBitmapInfo bmi;
    BYTE *pBitsDest;

    m_wndStaticResNum.SetWindowText(GetFileNameFor(blob.GetType(), blob.GetNumber(), blob.GetVersion()).c_str());

    std::unique_ptr<PaletteComponent> optionalPalette;
    if (_view->GetComponent<RasterComponent>().Traits.PaletteType == PaletteType::VGA_256)
    {
        optionalPalette = appState->GetResourceMap().GetMergedPalette(*_view, 999);
    }
    bitmap.Attach(CreateBitmapFromResource(*_view, optionalPalette.get(), &bmi, &pBitsDest));
    m_wndView.FromBitmap((HBITMAP)bitmap, bmi.bmiHeader.biWidth, abs(bmi.bmiHeader.biHeight), true);

    // Don't use the fallback here:
    if (_view->TryGetComponent<PaletteComponent>())
    {
        CBitmap bitmapPalette;
        SCIBitmapInfo bmiPalette;
        COLORREF background = g_PaintManager->GetColor(COLOR_3DFACE);
        std::vector<const Cel*> cels;
        RasterComponent &raster = _view->GetComponent<RasterComponent>();
        for (Loop &loop : raster.Loops)
        {
            for (Cel &cel : loop.Cels)
            {
                cels.push_back(&cel);
            }
        }

        bitmapPalette.Attach(CreateBitmapFromPaletteResource(_view.get(), &bmiPalette, &pBitsDest, &background, &cels));
        m_wndPalette.ShowWindow(SW_SHOW);
        m_wndPalette.FromBitmap((HBITMAP)bitmapPalette, bmiPalette.bmiHeader.biWidth, abs(bmiPalette.bmiHeader.biHeight), false);
    }
    else
    {
        m_wndPalette.ShowWindow(SW_HIDE);
        //m_wndPalette.FromBitmap(nullptr, 0, 0);
    }
}

void ViewPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATICVIEW, m_wndView);
    AddAnchor(IDC_STATICVIEW, CPoint(0, 0), CPoint(100, 100));
    DDX_Control(pDX, IDC_STATIC2, m_wndPalette);
    AddAnchor(IDC_STATIC2, CPoint(50, 100), CPoint(50, 100));
    DDX_Control(pDX, IDC_STATICRESNUM, m_wndStaticResNum);
    AddAnchor(IDC_STATICRESNUM, CPoint(0, 0), CPoint(100, 0));
}

//
// Palette previewer
//
BEGIN_MESSAGE_MAP(PalettePreviewer, ResourcePreviewer)
END_MESSAGE_MAP()

void PalettePreviewer::SetResource(const ResourceBlob &blob)
{
    //
    // Generate a large bitmap containing all the views.
    //
    _palette = CreateResourceFromResourceData(blob);
    CBitmap bitmap;
    SCIBitmapInfo bmi;
    BYTE *pBitsDest;
    COLORREF background = g_PaintManager->GetColor(COLOR_3DFACE);
    bitmap.Attach(CreateBitmapFromPaletteResource(_palette.get(), &bmi, &pBitsDest, &background));
    m_wndView.FromBitmap((HBITMAP)bitmap, bmi.bmiHeader.biWidth, abs(bmi.bmiHeader.biHeight), false);

    m_wndStaticResNum.SetWindowText(GetFileNameFor(blob.GetType(), blob.GetNumber(), blob.GetVersion()).c_str());
}

void PalettePreviewer::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATICVIEW, m_wndView);
    AddAnchor(IDC_STATICVIEW, CPoint(0, 0), CPoint(100, 100));
    DDX_Control(pDX, IDC_STATIC2, m_wndPaletteNOT);
    AddAnchor(IDC_STATIC2, CPoint(100, 100), CPoint(100, 100));
    DDX_Control(pDX, IDC_STATICRESNUM, m_wndStaticResNum);
    AddAnchor(IDC_STATICRESNUM, CPoint(0, 0), CPoint(100, 0));
}

//
// Script previewer
//
BEGIN_MESSAGE_MAP(ScriptPreviewer, ResourcePreviewer)
END_MESSAGE_MAP()

ScriptPreviewer::~ScriptPreviewer() {}

void ScriptPreviewer::SetResource(const ResourceBlob &blob)
{
    // Try to find a source file.
    std::string scriptFileName = appState->GetResourceMap().Helper().GetScriptFileName(blob.GetName());

    std::ifstream scriptFile(scriptFileName.c_str());
    if (scriptFile.is_open())
    {
        ScriptId scriptId(scriptFileName);
        if (scriptId.Language() == LangSyntaxSCIStudio)
        {
            m_wndHeader.SetWindowText("Language: SCI Studio");
        }
        else
        {
            m_wndHeader.SetWindowText("Language: C syntax");
        }

        std::string scriptText;
        std::string line;
        while (std::getline(scriptFile, line))
        {
            scriptText += line;
            scriptText += "\r\n";

        }
        m_wndEdit.SetWindowText(scriptText.c_str());
    }
    else
    {
        m_wndHeader.SetWindowText("");

        // If that wasn't possible, spew info from the compiled script resource:
        CompiledScript compiledScript(0);
        if (compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), blob.GetNumber(), blob.GetReadStream()))
        {
            // Write some crap.
            std::stringstream out;
            DebugOut(compiledScript, out, true);
            // Now we have a stream.  Put it in the edit box
            m_wndEdit.SetWindowText(out.str().c_str());
        }
        else
        {
            m_wndEdit.SetWindowText("Unable to load script.");
        }
    }
}

void ScriptPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDITSCRIPT, m_wndEdit);
    DDX_Control(pDX, IDC_STATICHEADER, m_wndHeader);
    AddAnchor(IDC_EDITSCRIPT, CPoint(0, 0), CPoint(100, 100));
}



//
// Text previewer
//
BEGIN_MESSAGE_MAP(TextPreviewer, ResourcePreviewer)
END_MESSAGE_MAP()

TextPreviewer::~TextPreviewer() {}

void TextPreviewer::SetResource(const ResourceBlob &blob)
{
    std::unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(blob);
    TextComponent *pText = resource->TryGetComponent<TextComponent>();

    if (blob.GetType() == ResourceType::Message)
    {
        m_wndTitle.SetWindowText("Message resources are used for displaying text in-game.");
    }
    else
    {
        if (appState->GetVersion().SupportsMessages)
        {
            m_wndTitle.SetWindowText("Text resources can be used for displaying text in-game. Message resources have generally replaced Text resources for SCI1 and above.");
        }
        else
        {
            m_wndTitle.SetWindowText("Text resources can be used for displaying text in-game.");
        }
    }

    if (pText)
    {
        std::stringstream ss;
        for (auto &aString : pText->Texts)
        {
            ss << aString.Text << "\r\n";
        }
        m_wndEdit.SetWindowText(ss.str().c_str());
    }
    else
    {
        m_wndEdit.SetWindowText("Unable to load text resource.");
    }
}

void TextPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDITSCRIPT, m_wndEdit);
    DDX_Control(pDX, IDC_STATICTITLE, m_wndTitle);
    AddAnchor(IDC_EDITSCRIPT, CPoint(0, 0), CPoint(100, 100));
}



//
// Vocab previewer
//
BEGIN_MESSAGE_MAP(VocabPreviewer, ResourcePreviewer)
END_MESSAGE_MAP()

VocabPreviewer::~VocabPreviewer() {}


void VocabPreviewer::_Populate(const std::vector<std::string> &names, const bool prependNumber)
{
    std::vector<std::string>::const_iterator it = names.begin();
    
    std::stringstream text;
    int index = 0;
    while (it != names.end())
    {
        if (prependNumber)
        {
            text << index << ": ";
        }

        const std::string &line = (*it);
        // Replace \n with \r\n for display in the edit control
        size_t position = 0;
        size_t carriageReturnPos = line.find('\n', position);
        while (carriageReturnPos != std::string::npos)
        {
            text << line.substr(position, carriageReturnPos - position);
            position = carriageReturnPos + 1;

            text << "\r\n";
            carriageReturnPos = line.find('\n', position);
        }

        text << line.substr(position);

        text << "\r\n";
        ++it;
        index++;
    }
    m_wndEdit.SetWindowText(text.str().c_str());
}

void VocabPreviewer::SetResource(const ResourceBlob &blob)
{
    // It appears that the different vocabs have different formats.  We would need
    // to write previers for each kind.
    // Let's use some heuristics to determine how to load them.
    int iNumber = blob.GetNumber();
    bool fSuccess = false;

    if (iNumber == appState->GetVersion().MainVocabResource)
    {
        CPrecisionTimer timer;
        timer.Start();
        double initTime = 0.0;
        double popTime = 0.0;
        std::unique_ptr<ResourceEntity> pResource = CreateResourceFromResourceData(blob);
        if (pResource)
        {
            initTime = timer.Stop();

            timer.Start();
            _Populate(pResource->GetComponent<Vocab000>().GetWords());
            popTime = timer.Stop();
            fSuccess = true;
        }
    }
    else
    {
        switch (iNumber)
        {
        case 995: // debug info
        {
            CVocabWithNames vocab;
            if (vocab.Create(&blob.GetReadStream(), true))
            {
                _Populate(vocab.GetNames());
                fSuccess = true;
            }
        }
        break;
        case 996: // species table
        {
            SpeciesTable species;
            if (species.Load(appState->GetResourceMap().Helper()))
            {
                _Populate(species.GetNames());
                fSuccess = true;
            }
        }
        break;
        case 997: // selector table
        {
            SelectorTable selectors;
            if (selectors.Load(appState->GetResourceMap().Helper()))
            {
                _Populate(selectors.GetNamesForDisplay(), false);
                fSuccess = true;
            }
        }
        break;
        case 999: // kernel functions
        {
            KernelTable kernels;
            if (kernels.Load(appState->GetResourceMap().Helper()))
            {
                _Populate(kernels.GetNames(), true);
                fSuccess = true;
            }
        }
        break;
        default:
            m_wndEdit.SetWindowText(GetBinaryDataVisualization(blob.GetData(), blob.GetLength()).c_str());
            fSuccess = true;
            break;
        }
    }

    if (!fSuccess)
    {
        m_wndEdit.SetWindowText("Unable to load vocab resource.");
    }
}

void VocabPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDITSCRIPT, m_wndEdit);
    AddAnchor(IDC_EDITSCRIPT, CPoint(0, 0), CPoint(100, 100));
}



//
// Font previewer
//
BEGIN_MESSAGE_MAP(FontPreviewer, ResourcePreviewer)
END_MESSAGE_MAP()

FontPreviewer::~FontPreviewer() {}

void FontPreviewer::SetResource(const ResourceBlob &blob)
{
    _pFont = CreateResourceFromResourceData(blob);
    // Show a preview of entire font.
    std::stringstream ss;
    RasterComponent &rasterComponent = _pFont->GetComponent<RasterComponent>();
    for (size_t i = 0; i < rasterComponent.Loops[0].Cels.size(); i++)
    {
        ss << (char)i;
    }
    _pWndFontView->OverridePreviewLetters(ss.str());
    _pWndFontView->SetFontResource(_pFont.get());
}

BOOL FontPreviewer::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    CRect rc;
    GetClientRect(&rc);
    CCreateContext context;
    context.m_pNewViewClass = RUNTIME_CLASS(CFontPreviewView);
    _pWndFontView = static_cast<CFontPreviewView*>(context.m_pNewViewClass->CreateObject());
    if (!_pWndFontView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		rc, this, 12345, &context))
    {
        return FALSE;
    }
    else
    {
        AddAnchor(_pWndFontView->GetSafeHwnd(), CSize(0, 0), CSize(100, 100));
    }
    return fRet;
}

void FontPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}




//
// Sound previewer
//
BEGIN_MESSAGE_MAP(SoundPreviewer, ResourcePreviewer)
    ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, OnSynthChoiceChange)
    ON_CBN_SELCHANGE(IDC_COMBO_MIDIDEVICE, OnMIDIDeviceChange)
    ON_BN_CLICKED(IDC_BUTTON_PLAY, OnPlay)
    ON_BN_CLICKED(IDC_BUTTON_STOP, OnStop)
    ON_WM_TIMER()
END_MESSAGE_MAP()

#define SOUND_TIMER 5003

SoundPreviewer::SoundPreviewer() : _lastVersion({ })
{
}

void SoundPreviewer::SetResource(const ResourceBlob &blob)
{
    // Add the items to the combobox.
    if (blob.GetVersion() != _lastVersion)
    {
        _lastVersion = blob.GetVersion();

        m_wndSynths.ResetContent();
        PopulateComboWithDevicesHelper(blob.GetVersion(), m_wndSynths);
        m_wndSynths.SetCurSel(0);
        OnSynthChoiceChange();
    }

    _sound = CreateResourceFromResourceData(blob);

    std::string durationString = "Duration: ";
    SoundComponent *soundComp = _sound->TryGetComponent<SoundComponent>();
    if (soundComp)
    {
        SelectFirstDeviceWithChannels(blob.GetVersion(), m_wndSynths, *soundComp);
        OnSynthChoiceChange();

        m_wndSynths.EnableWindow(TRUE);
        g_midiPlayer.SetSound(*soundComp, StandardTempo); // We don't have a tempo control

        m_wndChannels.ShowWindow(soundComp->Traits.CanEditChannelMask ? SW_SHOW : SW_HIDE);
        m_wndDuration.SetWindowText((durationString + GetSoundLength(*soundComp)).c_str());
    }
    else
    {
        m_wndSynths.EnableWindow(FALSE);
        m_wndChannels.ShowWindow(SW_HIDE);
    }
    OnSynthChoiceChange();
    if (m_wndAutoPreview.GetCheck() == BST_CHECKED)
    {
        OnPlay();
    }
    else
    {
        OnStop();
    }
}

void SoundPreviewer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_COMBO_DEVICE, m_wndSynths);
    DDX_Control(pDX, IDC_EDIT_CHANNELS, m_wndChannels);
    DDX_Control(pDX, IDC_STATIC_SYNTH, m_wndStaticSynth);
    DDX_Control(pDX, IDC_BUTTON_PLAY, m_wndPlay);
    DDX_Control(pDX, IDC_BUTTON_STOP, m_wndStop);
    DDX_Control(pDX, IDC_SLIDER, m_wndSlider);
    DDX_Control(pDX, IDC_CHECK_AUTOPREV, m_wndAutoPreview);
    DDX_Control(pDX, IDC_STATIC_DURATION, m_wndDuration);
    DDX_Control(pDX, IDC_STATICTITLE, m_wndTitle);

    AddAnchor(IDC_COMBO_DEVICE, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_SLIDER, CPoint(0, 0), CPoint(100, 0));
}

BOOL SoundPreviewer::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    CRect rc;
    GetClientRect(&rc);

    CDC *pDC = GetDC();
    {
        LOGFONT logFont = { 0 };
        StringCchCopy(logFont.lfFaceName, ARRAYSIZE(logFont.lfFaceName), "Marlett");
        logFont.lfHeight = -MulDiv(10, GetDeviceCaps((HDC)*pDC, LOGPIXELSY), 72);
	    logFont.lfWeight = FW_NORMAL;
	    logFont.lfItalic = FALSE;
	    logFont.lfCharSet = DEFAULT_CHARSET;
	    logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	    logFont.lfQuality = DEFAULT_QUALITY;
	    logFont.lfPitchAndFamily = FIXED_PITCH;
        _marlettFont.CreateFontIndirect(&logFont);
        m_wndPlay.SetFont(&_marlettFont);
        m_wndStop.SetFont(&_marlettFont);
    }

    m_wndSlider.SetRange(0, 100);

    _UpdatePlayState();

    ReleaseDC(pDC);
    return fRet;
}

std::string SoundPreviewer::_FillChannelString(BYTE bChannel, bool fHeader)
{
    std::string channelString;
    ASSERT(((0x1 << bChannel) & _wChannelMask) || (bChannel == 0));
    WORD wMask = _wChannelMask;
    for (int i = 0; i < 15; i++)
    {
        if (wMask & 0x1)
        {
            // This channel is used.
            if (fHeader)
            {
                char sz[3];
                StringCchPrintf(sz, ARRAYSIZE(sz), "%1x", i);
                channelString += sz;
            }
            else
            {
                if (bChannel == i)
                {
                    // It's used by this event.
                    channelString += "X";
                }
                else
                {
                    channelString += " ";
                }
            }
        }
        wMask >>= 1;
    }
    return channelString;
}

void SoundPreviewer::OnSynthChoiceChange()
{
    // Recalculate the mask.
    _device = GetDeviceFromComboHelper(appState->GetVersion(), m_wndSynths);
    if (_sound)
    {
        SoundComponent *soundComp = _sound->TryGetComponent<SoundComponent>();
        if (soundComp)
        {
            std::string channelText;
            _wChannelMask = soundComp->CalculateChannelMask(_device);
            for (int i = 0; i < 16; i++)
            {
                channelText += ((_wChannelMask >> i) & 0x1) ? "1" : "0";
            }
            m_wndChannels.SetWindowText(channelText.c_str());
        }
        g_midiPlayer.SetDevice(_device);
    }
}

void SoundPreviewer::OnMIDIDeviceChange()
{
    // TODO
}

bool SoundPreviewer::_IsPlaying()
{
    return g_midiPlayer.IsPlaying();
}

void SoundPreviewer::_UpdatePlayState()
{
    if (_IsPlaying())
    {
        SetTimer(SOUND_TIMER, 100, NULL);
    }
    else
    {
        KillTimer(SOUND_TIMER);
    }
    m_wndPlay.EnableWindow(_sound.get() && !_IsPlaying());
    m_wndStop.EnableWindow(_sound.get() && _IsPlaying());
}
void SoundPreviewer::OnPlay()
{
    g_midiPlayer.Play();
    _UpdatePlayState();
}
void SoundPreviewer::OnStop()
{
    g_midiPlayer.Stop();
    _UpdatePlayState();
}
void SoundPreviewer::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == SOUND_TIMER)
    {
        if (g_midiPlayer.IsPlaying())
        {
            m_wndSlider.SetPos(g_midiPlayer.QueryPosition(100));
        }
        if (!_IsPlaying())
        {
            _UpdatePlayState();
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}


void AudioPreviewer::SetResource(const ResourceBlob &blob)
{
    _resource = CreateResourceFromResourceData(blob);
    SetAudioResource(_resource.get());
}

void AudioPreviewer::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DoDataExchangeHelper(pDX);

    AddAnchor(IDC_EDIT_DESCRIPTION, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_SLIDER, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_EDIT_SAMPLEBIT, CPoint(0, 0), CPoint(100, 0));
}

BOOL AudioPreviewer::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    OnInitDialogHelper();
    return fRet;
}




//
// Blank previewer
//
BEGIN_MESSAGE_MAP(BlankPreviewer, ResourcePreviewer)
END_MESSAGE_MAP()


