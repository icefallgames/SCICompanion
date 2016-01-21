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

// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SoundDoc.h"
#include "ResourceEntity.h"
#include "SoundOperations.h"
#include "SoundUtil.h"
#include "Audio.h"

using namespace std;

DeviceType CSoundDoc::s_defaultDeviceSCI0 = DeviceType::RolandMT32;
DeviceType CSoundDoc::s_defaultDeviceSCI1 = DeviceType::SCI1_RolandGM;

// CSoundDoc

IMPLEMENT_DYNCREATE(CSoundDoc, CResourceDocument)

CSoundDoc::CSoundDoc() : _selectedChannelId(-1)
{
    _cueIndex = -1;
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
    AddFirstResource(move(pSound));
    _UpdateTitle();
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed));
}

const SoundComponent *CSoundDoc::GetSoundComponent() const
{
    const SoundComponent *pSound = nullptr;;
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
    const SoundComponent *sound = GetSoundComponent();
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
    ON_COMMAND(ID_IMPORT_AUDIO, _OnImportWav)
    ON_UPDATE_COMMAND_UI(ID_IMPORT_AUDIO, _OnUpdateImportWav)
END_MESSAGE_MAP()

const int MaxSCI1SampleRate = 11025;

const DeviceType DefaultSCI1DigitalTracks[] =
{
    DeviceType::SCI1_Adlib,
    DeviceType::SCI1_GM,
    DeviceType::SCI1_GameBlaster,
    DeviceType::SCI1_RolandGM,
    DeviceType::SCI1_PCSpeaker,
    DeviceType::SCI1_Tandy,
    DeviceType::SCI1_AmigaMac,
    DeviceType::SCI1_Unknown08,
    DeviceType::SCI1_Unkonwn0b,
};

void CSoundDoc::_OnImportWav()
{
    const ResourceEntity *resource = GetResource();
    if (resource && (appState->GetVersion().SoundFormat == SoundFormat::SCI1))
    {
        CFileDialog fileDialog(TRUE, nullptr, nullptr, 0, "WAV files (*.wav)|*.wav|All Files|*.*|");
        fileDialog.m_ofn.lpstrTitle = "Add wav to game";
        if (IDOK == fileDialog.DoModal())
        {
            std::string filename = (PCSTR)fileDialog.GetPathName();
            try
            {
                ScopedFile scopedFile(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
                sci::streamOwner owner(scopedFile.hFile);
                std::unique_ptr<AudioComponent> audioComponent = std::make_unique<AudioComponent>();
                AudioComponentFromWaveFile(owner.getReader(), *audioComponent, nullptr, MaxSCI1SampleRate, true);
                ApplyChanges<SoundComponent>(
                    [](SoundComponent &sound)
                {
                    auto &trackInfos = sound.GetTrackInfos();
                    for (DeviceType defaultType : DefaultSCI1DigitalTracks)
                    {
                        // Turn digital on for all...
                        int digitalChannel = (int)sound.GetChannelInfos().size();
                        sound.SetChannelId(defaultType, digitalChannel, true);
                    }
                    return WrapHint(SoundChangeHint::Changed);
                },
                    [&audioComponent](ResourceEntity &resource)
                {
                    resource.AddComponent<AudioComponent>(std::move(audioComponent));
                }
                    );
            }
            catch (std::exception &e)
            {
                AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
            }
        }
    }
}

void CSoundDoc::_OnUpdateImportWav(CCmdUI *cmdUI)
{
    const ResourceEntity *sound = GetResource();
    cmdUI->Enable(sound && (appState->GetVersion().SoundFormat == SoundFormat::SCI1));
}


void CSoundDoc::_OnImportMidi()
{
    std::unique_ptr<ResourceEntity> pSound = GetResource()->Clone();

    // Import to devices (tracks) that are standard by default.
    std::vector<DeviceType> devices;
    if (appState->GetVersion().SoundFormat == SoundFormat::SCI1)
    {
        // REVIEW: Sounds appear not to play properly if these three devices don't appear in the sound resource.
        devices.insert(devices.end(), { DeviceType::SCI1_Adlib, DeviceType::SCI1_GM, DeviceType::SCI1_RolandGM });
    }
    else
    {
        devices.insert(devices.end(), { DeviceType::Adlib, DeviceType::NewGM, DeviceType::RolandMT32 });
    }

#if SUPPORT_COMBINING_MIDI_FILES
    // Remove any that are already present. We don't want to stomp.
    devices.erase(remove_if(devices.begin(), devices.end(),
        [&pSound](DeviceType leDevice) { return pSound->GetComponent<SoundComponent>().DoesDeviceHaveTracks(leDevice); }
        ), devices.end());
#endif

    // But also always insert the current device. We DO want to stomp.
    devices.push_back(GetDevice());

    if (ImportMidi(devices, pSound.get()))
    {
        AddNewResourceToUndo(move(pSound));
        SetModifiedFlag(TRUE);
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed | SoundChangeHint::DeviceChanged | _UpdateChannelId()));
    }
}

void CSoundDoc::SetTempo(WORD wTempo)
{
    SoundComponent *pSound = const_cast<SoundComponent*>(GetSoundComponent());
    if (pSound)
    {
        if (pSound->GetTempo() != wTempo)
        {
            // Special case for tempo - don't make a new sound.  The tempo will change many times a second
            // when the user is sliding the tempo slider.
            pSound->SetTempo(wTempo);
        }
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

bool ImportMidi(std::vector<DeviceType> devices, ResourceEntity *resourceEntity)
{
    bool success = false;
    // Create a file dialog.
    CFileDialog fileDialog(TRUE, NULL, NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR, c_szMidiFilter);
    fileDialog.m_ofn.lpstrTitle = TEXT("Import midi file");
    if (IDOK == fileDialog.DoModal())
    {
        CString fileName = fileDialog.GetPathName();
        SoundComponent &sound = resourceEntity->GetComponent<SoundComponent>();
        if (InitializeFromMidi(appState->GetVersion(), devices, sound, (PCSTR)fileName) != SoundChangeHint::None)
        {
            success = true;
        }
    }
    return success;
}

std::unique_ptr<ResourceEntity> ImportMidi(std::vector<DeviceType> devices)
{
    std::unique_ptr<ResourceEntity> pResource(CreateSoundResource(appState->GetVersion()));

    if (!ImportMidi(devices, pResource.get()))
    {
        pResource.reset(nullptr);
    }
    return pResource;
}
