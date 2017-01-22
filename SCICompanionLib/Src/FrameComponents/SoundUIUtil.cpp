#include "stdafx.h"
#include "Sound.h"
#include "SoundUIUtil.h"

struct DeviceAndName
{
    DeviceType type;
    const char *pszName;
};

const DeviceAndName c_rgDeviceNamesSCI0[] =
{
    { DeviceType::RolandMT32, "Roland MT32 ($01)" },
    { DeviceType::YamahaFB01, "Yamaha FB-01 ($02)" },
    { DeviceType::Adlib, "Adlib ($04)" },
    { DeviceType::Casio, "Casio  ($08)" },
    { DeviceType::Tandy1000, "Tandy 1000 ($10)" },
    { DeviceType::PCSpeaker, "PC Speaker ($20)" },
    { DeviceType::AmigaMac, "Amiga/Mac ($40)" },
    { DeviceType::NewGM, "GM ($80)" },
};

const DeviceAndName c_rgDeviceNamesSCI1[] =
{
    { DeviceType::SCI1_Adlib, "Adlib, Sound Blaster ($00)" },
    { DeviceType::SCI1_GM, "General Midi ($07)" },
    { DeviceType::SCI1_GameBlaster, "Game Blaster ($09)" },
    { DeviceType::SCI1_RolandGM, "Roland MT-32, GM ($0c)" },
    { DeviceType::SCI1_PCSpeaker, "PC Speaker ($12)" },
    { DeviceType::SCI1_Tandy, "Tandy 3v / IBM PS1 ($13)" },
    { DeviceType::SCI1_AmigaMac, "Amiga/Mac ($06)" },
    { DeviceType::SCI1_Unknown08, "Unknown ($08)" },
    { DeviceType::SCI1_Unkonwn0b, "Unknown ($0b)" },
    { DeviceType::Digital, "Digital" },
};


void GetDeviceNames(SCIVersion version, const DeviceAndName* &deviceNames, size_t &size)
{
    deviceNames = (version.SoundFormat == SoundFormat::SCI1) ? c_rgDeviceNamesSCI1 : c_rgDeviceNamesSCI0;
    size = (version.SoundFormat == SoundFormat::SCI1) ? ARRAYSIZE(c_rgDeviceNamesSCI1) : ARRAYSIZE(c_rgDeviceNamesSCI0);
}

void PopulateComboWithDevicesHelper(SCIVersion version, CComboBox &combo)
{
    size_t size;
    const DeviceAndName *deviceNames;
    GetDeviceNames(version, deviceNames, size);
    for (size_t i = 0; i < size; i++)
    {
        combo.InsertString(-1, deviceNames[i].pszName);
    }
}

void SelectFirstDeviceWithChannels(SCIVersion version, CComboBox &combo, const SoundComponent &sound)
{
    if (combo.GetCurSel() != -1)
    {
        DeviceType device = GetDeviceFromComboHelper(version, combo);
        if (sound.DoesDeviceHaveTracks(device))
        {
            return; // We're good, current selection has channels
        }
    }
    size_t size;
    const DeviceAndName *deviceNames;
    GetDeviceNames(version, deviceNames, size);
    for (size_t i = 0; i < size; i++)
    {
        if (sound.DoesDeviceHaveTracks(deviceNames[i].type))
        {
            combo.SetCurSel((int)i);
            return;
        }
    }
}

DeviceType GetDeviceFromComboHelper(SCIVersion version, CComboBox &combo)
{
    size_t size;
    const DeviceAndName *deviceNames;
    GetDeviceNames(version, deviceNames, size);
    return deviceNames[combo.GetCurSel()].type;
}

void SelectDeviceInComboHelper(SCIVersion version, CComboBox &combo, DeviceType type)
{
    size_t size;
    const DeviceAndName *deviceNames;
    GetDeviceNames(version, deviceNames, size);
    for (size_t i = 0; i < size; i++)
    {
        if (deviceNames[i].type == type)
        {
            combo.SetCurSel(i);
            return;
        }
    }
}