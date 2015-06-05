// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SoundDoc.h"
#include "ResourceEntity.h"
#include "SoundOperations.h"

using namespace std;

DeviceType CSoundDoc::s_defaultDevice = DeviceType::RolandMT32;

// CSoundDoc

IMPLEMENT_DYNCREATE(CSoundDoc, CResourceDocument)

CSoundDoc::CSoundDoc()
{
    _cueIndex = -1;
    _wTempo = StandardTempo;
    _device = s_defaultDevice;
}


CSoundDoc::~CSoundDoc()
{
}

void CSoundDoc::v_OnUndoRedo()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed));
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

void CSoundDoc::SetDevice(DeviceType device, bool fNotify)
{
    if (_device != device)
    {
        _device = device;
        s_defaultDevice = _device;
        if (fNotify)
        {
            UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::DeviceChanged));
        }
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
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(SoundChangeHint::Changed | SoundChangeHint::DeviceChanged));
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
