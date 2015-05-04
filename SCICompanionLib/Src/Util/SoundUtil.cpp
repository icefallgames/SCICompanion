#include "stdafx.h"
#include "SoundUtil.h"
#include "Audio.h"

using namespace std;

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