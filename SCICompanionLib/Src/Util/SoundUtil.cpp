#include "stdafx.h"
#include "SoundUtil.h"
#include "Audio.h"
#include "Sound.h"

using namespace std;

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
    { DeviceType::DevUnknown, "Unknown ($40)" },
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
    { DeviceType::SCI1_Unknown06, "Unknown ($06)" },
    { DeviceType::SCI1_Unknown08, "Unknown ($08)" },
    { DeviceType::SCI1_Unkonwn0b, "Unknown ($0b)" },
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

const char riffMarker[] = "RIFF";
const char waveMarker[] = "WAVE";
const char fmtMarker[] = "fmt ";
const char dataMarker[] = "data";

#include <pshpack1.h>
struct WaveHeader
{
    uint16_t formatTag;
    uint16_t channelCount;
    uint32_t sampleRate;
    uint32_t bytesPerSecond;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};
#include <poppack.h>

void AudioComponentFromWaveFile(sci::istream &stream, AudioComponent &audio)
{
    uint32_t riff, wave, fileSize, fmt, chunkSize, data, dataSize;
    stream >> riff;
    stream >> fileSize;
    stream >> wave;
    stream >> fmt;
    stream >> chunkSize;
    WaveHeader header;
    stream >> header;
    stream >> data;
    stream >> dataSize;

    if ((riff != (*(uint32_t*)riffMarker)) ||
        (wave != (*(uint32_t*)waveMarker)) ||
        (fmt != (*(uint32_t*)fmtMarker)) ||
        (data != (*(uint32_t*)dataMarker)) ||
        (chunkSize != 16))
    {
        throw std::exception("Not a recognized wave file");
    }

    // Now validate the format
    if (header.formatTag != 1)
    {
        throw std::exception("Only uncompressed wave files are supported");
    }
    if (header.channelCount != 1)
    {
        throw std::exception("Only mono wave files are supported");
    }
    // REVIEW: Check sample rate;
    if ((header.bitsPerSample != 8) && (header.bitsPerSample != 16))
    {
        throw std::exception("Only 8 or 16 bit sound supported");
    }

    // Set up the AudioComponent and read the data.
    audio.Frequency = header.sampleRate;
    if (header.bitsPerSample == 16)
    {
        audio.Flags |= AudioFlags::SixteenBit | AudioFlags::Signed;
    }
    audio.DigitalSamplePCM.assign(dataSize, 0);
    stream.read_data(&audio.DigitalSamplePCM[0], audio.DigitalSamplePCM.size());
}