// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SoundDoc.h"
#include "ResourceEntity.h"
#include "SoundOperations.h"

using namespace std;

DeviceType CSoundDoc::s_defaultDeviceSCI0 = DeviceType::RolandMT32;
DeviceType CSoundDoc::s_defaultDeviceSCI1 = DeviceType::SCI1_RolandGM;

// CSoundDoc

IMPLEMENT_DYNCREATE(CSoundDoc, CResourceDocument)

CSoundDoc::CSoundDoc() : _selectedChannelId(-1)
{
    _cueIndex = -1;
    _wTempo = StandardTempo;
    _device = (appState->GetVersion().SoundFormat == SoundFormat::SCI1) ?  s_defaultDeviceSCI1 : s_defaultDeviceSCI0;
}


CSoundDoc::~CSoundDoc()
{
}

void CSoundDoc::v_OnUndoRedo()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed | _UpdateChannelId()));
}

void CSoundDoc::SetSoundResource(std::unique_ptr<ResourceEntity> pSound, int id)
{
    _checksum = id;
    _wTempo = pSound->GetComponent<SoundComponent>().GetTempo();
    AddFirstResource(move(pSound));
    _UpdateTitle();
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed));
}

SoundComponent *CSoundDoc::GetSoundComponent() const
{
    SoundComponent *pSound = nullptr;;
    const ResourceEntity *pResource = GetResource();
    if (pResource)
    {
        pSound = pResource->TryGetComponent<SoundComponent>();
    }
    return pSound;
}

void CSoundDoc::SetActiveCue(int index)
{
    if (_cueIndex != index)
    {
        _cueIndex = index;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::ActiveCueChanged));
    }
}

SoundChangeHint CSoundDoc::_UpdateChannelId()
{
    SoundChangeHint hint = SoundChangeHint::None;
    SoundComponent *sound = GetSoundComponent();
    if (sound && (_selectedChannelId >= (int)sound->GetChannelInfos().size()))
    {
        _selectedChannelId = -1;
        hint |= SoundChangeHint::SelectedChannelChanged;
    }
    return hint;
}

void CSoundDoc::SetDevice(DeviceType device, bool fNotify)
{
    if (_device != device)
    {
        _device = device;
        if (appState->GetVersion().SoundFormat == SoundFormat::SCI1)
        {
            s_defaultDeviceSCI1 = _device;
        }
        else
        {
            s_defaultDeviceSCI0 = _device;
        }
        if (fNotify)
        {
            UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::DeviceChanged));
        }
    }
}

void CSoundDoc::SetChannelId(int channelId)
{
    if (_selectedChannelId != channelId)
    {
        _selectedChannelId = channelId;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::SelectedChannelChanged));
    }
}

BEGIN_MESSAGE_MAP(CSoundDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
    ON_COMMAND(ID_IMPORT_MIDI, _OnImportMidi)
END_MESSAGE_MAP()

void CSoundDoc::_OnImportMidi()
{
    std::unique_ptr<ResourceEntity> pSound = GetResource()->Clone();
    if (ImportMidi(GetDevice(), pSound.get()))
    {
        AddNewResourceToUndo(move(pSound));
        SetModifiedFlag(TRUE);
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed | SoundChangeHint::DeviceChanged | _UpdateChannelId()));
    }
}

void CSoundDoc::SetTempo(WORD wTempo)
{
    if (_wTempo != wTempo)
    {
        _wTempo = wTempo;
        // Special case for tempo - don't make a new sound.  The tempo will change many times a second
        // when the user is sliding the tempo slider.
        SoundComponent *pSound = GetSoundComponent();
        pSound->SetTempo(wTempo);
    }
}

// CSoundDoc diagnostics

#ifdef _DEBUG
void CSoundDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSoundDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSoundDoc serialization

void CSoundDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

const char c_szMidiFilter[] = "Midi Files|*.mid;*.midi|All Files|*.*|";

bool ImportMidi(DeviceType device, ResourceEntity *resourceEntity)
{
    bool success = false;
    // Create a file dialog.
    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY, c_szMidiFilter);
    fileDialog.m_ofn.lpstrTitle = TEXT("Import midi file");
    if (IDOK == fileDialog.DoModal())
    {
        CString fileName = fileDialog.GetPathName();
        SoundComponent &sound = resourceEntity->GetComponent<SoundComponent>();
        if (InitializeFromMidi(device, sound, (PCSTR)fileName) != SoundChangeHint::None)
        {
            success = true;
        }
    }
    return success;
}

std::unique_ptr<ResourceEntity> ImportMidi(DeviceType device)
{
    std::unique_ptr<ResourceEntity> pResource(CreateSoundResource(appState->GetVersion()));

    if (!ImportMidi(device, pResource.get()))
    {
        pResource.reset(nullptr);
    }
    return pResource;
}
