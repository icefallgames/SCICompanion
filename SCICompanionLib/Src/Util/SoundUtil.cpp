#include "stdafx.h"
#include "SoundUtil.h"
#include "Audio.h"
#include "Sound.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "format.h"

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

    if ((riff != (*(uint32_t*)riffMarker)) ||
        (wave != (*(uint32_t*)waveMarker)))
    {
        throw std::exception("Wave file: invalid header.");
    }

    stream >> fmt;
    stream >> chunkSize;
    while (stream.good() && (fmt != (*(uint32_t*)fmtMarker)))
    {
        stream.skip(chunkSize);
        stream >> fmt;
        stream >> chunkSize;
    }

    if (!stream.good())
    {
        throw std::exception("Unable to find wave fmt marker.");
    }

    if (chunkSize < sizeof(WaveHeader))
    {
        throw std::exception("Wave file: invalid fmt header.");
    }

    WaveHeader header;
    stream >> header;
    static_assert(sizeof(WaveHeader) == 16, "bad wave header size");

    // There might be some extra bytes in the header
    stream.skip(chunkSize - sizeof(WaveHeader));
    stream >> data;
    stream >> dataSize;

    while (stream.good() && (data != (*(uint32_t*)dataMarker)))
    {
        stream.skip(dataSize);
        stream >> data;
        stream >> dataSize;
    }

    if (!stream.good())
    {
        throw std::exception("Unable to find wave data marker.");
    }

    // Now validate the format
    if (header.formatTag != 1)
    {
        throw std::exception("Only uncompressed wave files are supported");
    }
    if (header.channelCount != 1)
    {
        // Try to read stero things.
        // throw std::exception("Only mono wave files are supported");
    }
    // REVIEW: Check sample rate;
    uint16_t convertedBitsPerSample = header.bitsPerSample;
    if ((header.bitsPerSample != 8) && (header.bitsPerSample != 16))
    {
        convertedBitsPerSample = 16;
        throw std::exception(fmt::format("{0} bits per sample: Only 8 or 16 bit sound supported", header.bitsPerSample).c_str());
    }

    if (header.sampleRate > 22050)
    {
        AfxMessageBox("Warning: sounds with over 22050Hz may not play correctly in Sierra's interpreter.", MB_OK | MB_ICONWARNING);
    }
    if (header.formatTag != WAVE_FORMAT_PCM)
    {
        AfxMessageBox("Warning: Imported .wav is not PCM.", MB_OK | MB_ICONWARNING);
    }

    // Set up the AudioComponent and read the data.
    audio.Frequency = header.sampleRate;
    if (convertedBitsPerSample == 16)
    {
        audio.Flags |= AudioFlags::SixteenBit | AudioFlags::Signed;
    }

    // audio.DigitalSamplePCM.assign(dataSize, 0);
    // Attempt to get channel zero
    vector<uint8_t> temp;
    temp.assign(dataSize, 0);
    stream.read_data(&temp[0], temp.size());

    int sampleCount = dataSize / header.channelCount / (header.bitsPerSample / 8);
    int sampleSize = header.bitsPerSample / 8;
    int convertedSampleSize = convertedBitsPerSample / 8;

    vector<uint8_t> temp2;
    temp2.assign(dataSize / header.channelCount, 0);

    // temp -> temp2: Extract one channel 
    for (int i = 0; i < sampleCount; i++)
    {
        // The sampleSize/convertedSampleSize was an attempt at converting from 24/32 bit to 16 bit, but it sounds awful.
        int iSource = i * header.channelCount;
        uint32_t sampleIn = 0;
        for (int k = 0; k < sampleSize; k++)
        {
            sampleIn += ((uint32_t)temp[iSource * sampleSize + k] << (8 * k));
        }
        uint32_t sampleOut = sampleIn;
        for (int k = 0; k < convertedSampleSize; k++)
        {
            temp2[i * convertedSampleSize + k] = (uint8_t)(sampleOut >> (8 * k));
        }
    }

    // TODO: sample rate conversion.
    audio.DigitalSamplePCM.assign(dataSize / header.channelCount, 0);
    for (size_t i = 0; i < temp2.size(); i++)
    {
        audio.DigitalSamplePCM[i] = temp2[i];
    }

}

std::string _NameFromFilename(PCSTR pszFilename)
{
    PCSTR pszFile = PathFindFileName(pszFilename);
    PCSTR pszExt = PathFindExtension(pszFile);
    return string(pszFile, pszExt - pszFile);
}

bool IsWaveFile(PCSTR pszFileName)
{
    return (0 == _strcmpi(PathFindExtension(pszFileName), ".wav"));
}

void AddWaveFileToGame(const std::string &filename)
{
    std::unique_ptr<ResourceEntity> resource(CreateDefaultAudioResource(appState->GetVersion()));
    ScopedFile scopedFile(filename, GENERIC_READ, FILE_SHARE_WRITE, OPEN_EXISTING);
    sci::streamOwner owner(scopedFile.hFile);
    AudioComponentFromWaveFile(owner.getReader(), resource->GetComponent<AudioComponent>());
    // REVIEW: We should know ahead of time if the game uses Aud or Sfx.
    resource->SourceFlags = (appState->GetVersion().AudioVolumeName == AudioVolumeName::Sfx) ? ResourceSourceFlags::Sfx : ResourceSourceFlags::Aud;
    appState->GetResourceMap().AppendResourceAskForNumber(*resource, _NameFromFilename(filename.c_str()));
}

