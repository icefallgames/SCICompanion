#include "stdafx.h"
#include "SoundUtil.h"

const struct
{
    DeviceType type;
    const char *pszName;
}
c_rgDeviceNames[] =
{
    { DeviceType::RolandMT32, "Roland MT32 ($01)" },
    { DeviceType::YamahaFB01, "Yamaha FB-01 ($02)" },
    { DeviceType::Adlib, "Adlib ($04)" },
    { DeviceType::Casio, "Casio  ($08)" },
    { DeviceType::Tandy1000, "Tandy 1000 ($10)" },
    { DeviceType::PCSpeaker, "PC Speaker ($20)" },
    { DeviceType::DevUnknown, "Unknown ($40)" },
    { DeviceType::NewGM, "GM ($80)" },
};

void PopulateComboWithDevicesHelper(CComboBox &combo)
{
    for (int i = 0; i < ARRAYSIZE(c_rgDeviceNames); i++)
    {
        combo.InsertString(-1, c_rgDeviceNames[i].pszName);
    }
}

DeviceType GetDeviceFromComboHelper(CComboBox &combo)
{
    return c_rgDeviceNames[combo.GetCurSel()].type;
}

void SelectDeviceInComboHelper(CComboBox &combo, DeviceType type)
{
    for (int i = 0; i < ARRAYSIZE(c_rgDeviceNames); i++)
    {
        if (c_rgDeviceNames[i].type == type)
        {
            combo.SetCurSel(i);
            return;
        }
    }
}
