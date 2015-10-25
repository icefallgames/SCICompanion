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
#pragma once

#include "ResourceEntityDocument.h"
#include "Sound.h"

class ResourceEntity;

class CSoundDoc : public ResourceEntityDocument
{
	DECLARE_DYNCREATE(CSoundDoc)

public:
	CSoundDoc();
	virtual ~CSoundDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // Takes ownership:
    void SetSoundResource(std::unique_ptr<ResourceEntity> pSound, int id = -1);
    const SoundComponent *GetSoundComponent() const;
    DeviceType GetDevice() const { return _device; }
    void SetDevice(DeviceType device, bool fNotify = true);

    void SetChannelId(int channelId);
    int GetChannelId() const { return _selectedChannelId; }

    void SetTempo(WORD wTempo);

    void SetActiveCue(int index);
    int GetActiveCue() const { return _cueIndex; }
    void v_OnUndoRedo();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    void afx_msg _OnImportMidi();
    virtual ResourceType _GetType() const { return ResourceType::Sound; }

	DECLARE_MESSAGE_MAP()

private:
    SoundChangeHint _UpdateChannelId();
    int _cueIndex;
    DeviceType _device;
    int _selectedChannelId;

    static DeviceType s_defaultDeviceSCI0;
    static DeviceType s_defaultDeviceSCI1;
};

std::unique_ptr<ResourceEntity> ImportMidi(std::vector<DeviceType> devices);
bool ImportMidi(std::vector<DeviceType> devices, ResourceEntity *resourceEntity);
