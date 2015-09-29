#include "stdafx.h"
#include "SoundUtil.h"
#include "Audio.h"
#include "Sound.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "format.h"
#include "CDSPResampler.h"

using namespace std;
using namespace r8b;

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

const int MaxSierraSampleRate = 22050;

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
    if (header.formatTag != WAVE_FORMAT_PCM)
    {
        throw std::exception("Only uncompressed wave files are supported");
    }

    std::vector<CompileResult> conversionResults;

    if (header.channelCount != 1)
    {
        conversionResults.emplace_back("Extracting left channel from multi-channel wave.");
    }

    // REVIEW: Check sample rate;
    uint16_t convertedBitsPerSample = header.bitsPerSample;
    if ((header.bitsPerSample != 8) && (header.bitsPerSample != 16))
    {
        convertedBitsPerSample = 16;
        throw std::exception(fmt::format("{0} bits per sample: Only 8 or 16 bit sound supported", header.bitsPerSample).c_str());
    }

    if (header.sampleRate > MaxSierraSampleRate)
    {
        conversionResults.emplace_back(fmt::format("Downsampling from {0}Hz to {1}Hz.", header.sampleRate, MaxSierraSampleRate));
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
    // Also at this point, convert so floating point data in case we need to downsample.
    vector<double> fpData;
    for (int i = 0; i < sampleCount; i++)
    {
        // The sampleSize/convertedSampleSize was an attempt at converting from 24/32 bit to 16 bit, but it sounds awful.
        int iSource = i * header.channelCount;
        uint32_t theSample = 0;
        for (int k = 0; k < sampleSize; k++)
        {
            theSample += ((uint32_t)temp[iSource * sampleSize + k] << (8 * k));
        }

        if (sampleSize == 2)
        {
            // 16 bit signed
            int16_t value = (int16_t)(uint16_t)theSample;
            double f = ((double)value) / 32768.0;
            fpData.push_back(f);
        }
        else
        {
            // 8 bit unsigned
            uint8_t value = (uint8_t)theSample;
            double f = ((double)value) / 255.0;
            f = (f * 2.0) - 1.0;
            fpData.push_back(f);
        }

        for (int k = 0; k < convertedSampleSize; k++)
        {
            temp2[i * convertedSampleSize + k] = (uint8_t)(theSample >> (8 * k));
        }
    }

    // Sample rate conversion to 22Khz
    if (header.sampleRate > MaxSierraSampleRate)
    {
        int fpSampleCount = (int)fpData.size();
        // Convert to floating point.
        unique_ptr<CDSPResampler24> resampler = make_unique<CDSPResampler24>(header.sampleRate, MaxSierraSampleRate, fpSampleCount);
        double *results;
        int samplesWritten = resampler->process(&fpData[0], fpSampleCount, results);

        // Convert back to 16 bit (for now)
        audio.DigitalSamplePCM.assign(samplesWritten * convertedSampleSize, 0);
        for (int i = 0; i < samplesWritten; i++)
        {
            double f = fpData[i];
            uint16_t value;
            if (convertedSampleSize == 2)
            {
                // 16 bit signed
                f *= 32768.0;
                int32_t value32 = (int32_t)f;
                if (value32 > 32767) value32 = 32767;
                if (value32 < -32768) value32 = -32768;
                value = (uint16_t)(int16_t)value32;
            }
            else
            {
                // 8 bit unsigned
                f = (f + 1.0) * 0.5; // now 0->1
                f *= 255.0;         // now 0->255
                int32_t value32 = (int32_t)f;
                if (value32 > 255) value32 = 255;
                if (value32 < 0) value32 = 0;
                value = (uint16_t)(int16_t)value32;
            }
            for (int k = 0; k < convertedSampleSize; k++)
            {
                audio.DigitalSamplePCM[i * convertedSampleSize + k] = (uint8_t)(value >> (8 * k));
            }
        }
        audio.Frequency = MaxSierraSampleRate;
    }
    else
    {
        // Just copy over
        audio.DigitalSamplePCM.assign(dataSize / header.channelCount, 0);
        for (size_t i = 0; i < temp2.size(); i++)
        {
            audio.DigitalSamplePCM[i] = temp2[i];
        }
    }

    if (!conversionResults.empty())
    {
        appState->OutputResults(OutputPaneType::Compile, conversionResults);
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

void WriteWaveFile(const std::string &filename, const AudioComponent &audio)
{
    sci::ostream out;
    out << (*(uint32_t*)riffMarker);
    
    uint32_t fileSizePosition = out.tellp();
    uint32_t fileSize = 0;
    out << fileSize;    // We'll fill this in later
    out << (*(uint32_t*)waveMarker);

    out << (*(uint32_t*)fmtMarker);
    WaveHeader waveHeader = {};
    uint32_t chunkSize = sizeof(waveHeader);
    out << chunkSize;    // We'll fill this in later

    WORD blockAlign = IsFlagSet(audio.Flags, AudioFlags::SixteenBit) ? 2 : 1;
    waveHeader.formatTag = WAVE_FORMAT_PCM;
    waveHeader.channelCount = 1;
    waveHeader.sampleRate = audio.Frequency;
    waveHeader.blockAlign = blockAlign;
    waveHeader.bitsPerSample = 8 * blockAlign;
    waveHeader.bytesPerSecond = audio.Frequency * blockAlign;
    out << waveHeader;

    out << (*(uint32_t*)dataMarker);
    uint32_t dataSize = audio.DigitalSamplePCM.size();
    out << dataSize;
    out.WriteBytes(&audio.DigitalSamplePCM[0], dataSize);

    fileSize = out.GetDataSize();
    // Go back and write it...
    out.seekp(fileSizePosition);
    out << fileSize;

    ScopedFile file(filename, GENERIC_WRITE, 0, CREATE_ALWAYS);
    file.Write(out.GetInternalPointer(), out.GetDataSize());
}

AudioVolumeName GetVolumeToUse(SCIVersion version, uint32_t base36Number)
{
    AudioVolumeName volumeToUse = version.AudioVolumeName;
    if (version.AudioVolumeName == AudioVolumeName::Both)
    {
        // When both are present, base36-indexed audio is in Aud, and regular audio is in Sfx
        volumeToUse = (base36Number == NoBase36) ? AudioVolumeName::Sfx : AudioVolumeName::Aud;
    }
    return volumeToUse;
}

std::unique_ptr<ResourceEntity> WaveResourceFromFilename(const std::string &filename)
{
    std::unique_ptr<ResourceEntity> resource(CreateDefaultAudioResource(appState->GetVersion()));
    ScopedFile scopedFile(filename, GENERIC_READ, FILE_SHARE_WRITE, OPEN_EXISTING);
    sci::streamOwner owner(scopedFile.hFile);
    AudioComponentFromWaveFile(owner.getReader(), resource->GetComponent<AudioComponent>());
    return resource;
}

void AddWaveFileToGame(const std::string &filename)
{
    std::unique_ptr<ResourceEntity> resource = WaveResourceFromFilename(filename);
    resource->SourceFlags = (GetVolumeToUse(appState->GetVersion(), NoBase36) == AudioVolumeName::Sfx) ? ResourceSourceFlags::Sfx : ResourceSourceFlags::Aud;
    appState->GetResourceMap().AppendResourceAskForNumber(*resource, _NameFromFilename(filename.c_str()));
}

