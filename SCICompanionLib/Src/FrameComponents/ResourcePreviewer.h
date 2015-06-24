#pragma once

#include "Pic.h"
#include "Text.h"
#include "Sound.h"
#include "FontPreviewView.h"
#include "NoFlickerStatic.h"
// ResourcePreviewer dialog

// fwd decl
class PicDrawManager;

//
// Common interface for previewers
//
class ResourcePreviewer : public CExtResizableDialog
{
public:
    virtual ~ResourcePreviewer() {}
    BOOL Create(CWnd *pParent) { return __super::Create(GetIDD(), pParent); }
    virtual void SetResource(const ResourceBlob &blob) = 0;
    virtual UINT GetIDD() = 0;

protected:
    BOOL OnInitDialog();
};

class PicPreviewer : public ResourcePreviewer
{
public:
    PicPreviewer() { _paletteNumber = 0; }
	virtual ~PicPreviewer();

    UINT GetIDD() { return IDD_PREVIEWPIC; }
    void SetResource(const ResourceBlob &blob);

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void _ResetVisualBitmap(PicDrawManager &pdm);
    BOOL OnSetPalette(UINT nID);
    afx_msg void OnUpdatePaletteButton(CCmdUI *pCmdUI);
    BOOL OnEraseBkgnd(CDC *pDC);
	DECLARE_MESSAGE_MAP()

    BYTE _paletteNumber;

    std::unique_ptr<ResourceEntity> _pic;
    CExtNoFlickerStatic m_wndVisual;
    CExtNoFlickerStatic m_wndPriority;
    CExtNoFlickerStatic m_wndControl;

    // Just visuals
    CExtButton m_wndButton1;
    CExtButton m_wndButton2;
    CExtButton m_wndButton3;
    CExtButton m_wndButton4;
    CExtLabel m_wndStaticPalette;
    CExtGroupBox m_wndVisualGroup;
    CExtGroupBox m_wndPriorityGroup;
    CExtGroupBox m_wndControlGroup;
};



class ViewPreviewer : public ResourcePreviewer
{
public:
    ViewPreviewer() { }

    UINT GetIDD() { return IDD_PREVIEWVIEW; }
    void SetResource(const ResourceBlob &blob);

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

    CExtNoFlickerStatic m_wndView;
    CExtNoFlickerStatic m_wndPalette;
    CExtLabel m_wndStaticResNum;
    std::unique_ptr<ResourceEntity> _view;
};



class ScriptPreviewer : public ResourcePreviewer
{
public:
    ScriptPreviewer() { }
	virtual ~ScriptPreviewer();

    UINT GetIDD() { return IDD_PREVIEWSCRIPT; }
    void SetResource(const ResourceBlob &blob);

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

    CExtEdit m_wndEdit;
    CExtLabel m_wndHeader;
};




class TextPreviewer : public ResourcePreviewer
{
public:
    TextPreviewer() { }
	virtual ~TextPreviewer();

    UINT GetIDD() { return IDD_PREVIEWTEXT; }
    void SetResource(const ResourceBlob &blob);

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

    CExtEdit m_wndEdit;
};




class VocabPreviewer : public ResourcePreviewer
{
public:
    VocabPreviewer() { }
	virtual ~VocabPreviewer();

    UINT GetIDD() { return IDD_PREVIEWTEXT; }
    void SetResource(const ResourceBlob &blob);

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
    void _Populate(const std::vector<std::string> &names, bool prependNumber = false);

    CExtEdit m_wndEdit;
};




class FontPreviewer : public ResourcePreviewer
{
public:
    FontPreviewer() { }
	virtual ~FontPreviewer();

    UINT GetIDD() { return IDD_PREVIEWFONT; }
    void SetResource(const ResourceBlob &blob);

private:
    BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

    CFontPreviewView *_pWndFontView; // Gets deleted automatically
    std::unique_ptr<ResourceEntity> _pFont;
};


class BlankPreviewer : public ResourcePreviewer
{
public:
    UINT GetIDD() { return IDD_PREVIEWBLANK; }
    void SetResource(const ResourceBlob &blob) {}

private:
	DECLARE_MESSAGE_MAP()
};

class PalettePreviewer : public ResourcePreviewer
{
public:
    PalettePreviewer() { }

    UINT GetIDD() { return IDD_PREVIEWVIEW; }
    void SetResource(const ResourceBlob &blob);

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    CExtNoFlickerStatic m_wndView;
    CExtNoFlickerStatic m_wndPaletteNOT;
    std::unique_ptr<ResourceEntity> _palette;
};

class SoundPreviewer : public ResourcePreviewer
{
public:
    SoundPreviewer();
    UINT GetIDD() { return IDD_PREVIEWSOUND; }
    void SetResource(const ResourceBlob &blob);

private:
    afx_msg void OnSynthChoiceChange();
    afx_msg void OnMIDIDeviceChange();
    afx_msg void OnPlay();
    afx_msg void OnStop();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    std::string _FillChannelString(BYTE bChannel, bool fHeader);
    std::string _GetEventString(const SoundEvent &command, DWORD dwTime);
    void _UpdatePlayState();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
    bool _IsPlaying();

    CExtComboBox m_wndSynths;
    CExtEdit m_wndChannels;
    CExtLabel m_wndStaticSynth;
    CExtButton m_wndPlay;
    CExtButton m_wndStop;
    CExtSliderWnd m_wndSlider;
    CExtCheckBox m_wndAutoPreview;
    CExtLabel m_wndDuration;

    DeviceType _device;
    WORD _wChannelMask;
    std::unique_ptr<ResourceEntity> _sound;

    SCIVersion _lastVersion;
    CFont _marlettFont;
};


