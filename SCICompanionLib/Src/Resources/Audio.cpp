#include "stdafx.h"
#include "Audio.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "Sync.h"
#include "format.h"

using namespace std;

std::string GetAudioLength(const AudioComponent &audio)
{
    int freq = max(1, audio.Frequency);
    if (IsFlagSet(audio.Flags, AudioFlags::SixteenBit))
    {
        freq *= 2;
    }
    int seconds = (int)round((double)audio.GetLength() / (double)freq);
    return fmt::format("{0:02}:{1:02}", seconds / 60, seconds % 60);
}

uint32_t AudioComponent::GetBytesPerSecond() const
{
    return Frequency * (IsFlagSet(Flags, AudioFlags::SixteenBit) ? 2 : 1);
}

uint32_t AudioComponent::GetLengthInTicks() const
{
    int bytePerSecond = max(1, Frequency);
    if (IsFlagSet(Flags, AudioFlags::SixteenBit))
    {
        bytePerSecond *= 2;
    }
    return SCITicksPerSecond * DigitalSamplePCM.size() / bytePerSecond;
}

// Decompression routines From SCUMMVM:

static const uint16_t tableDPCM16[128] = {
    0x0000, 0x0008, 0x0010, 0x0020, 0x0030, 0x0040, 0x0050, 0x0060, 0x0070, 0x0080,
    0x0090, 0x00A0, 0x00B0, 0x00C0, 0x00D0, 0x00E0, 0x00F0, 0x0100, 0x0110, 0x0120,
    0x0130, 0x0140, 0x0150, 0x0160, 0x0170, 0x0180, 0x0190, 0x01A0, 0x01B0, 0x01C0,
    0x01D0, 0x01E0, 0x01F0, 0x0200, 0x0208, 0x0210, 0x0218, 0x0220, 0x0228, 0x0230,
    0x0238, 0x0240, 0x0248, 0x0250, 0x0258, 0x0260, 0x0268, 0x0270, 0x0278, 0x0280,
    0x0288, 0x0290, 0x0298, 0x02A0, 0x02A8, 0x02B0, 0x02B8, 0x02C0, 0x02C8, 0x02D0,
    0x02D8, 0x02E0, 0x02E8, 0x02F0, 0x02F8, 0x0300, 0x0308, 0x0310, 0x0318, 0x0320,
    0x0328, 0x0330, 0x0338, 0x0340, 0x0348, 0x0350, 0x0358, 0x0360, 0x0368, 0x0370,
    0x0378, 0x0380, 0x0388, 0x0390, 0x0398, 0x03A0, 0x03A8, 0x03B0, 0x03B8, 0x03C0,
    0x03C8, 0x03D0, 0x03D8, 0x03E0, 0x03E8, 0x03F0, 0x03F8, 0x0400, 0x0440, 0x0480,
    0x04C0, 0x0500, 0x0540, 0x0580, 0x05C0, 0x0600, 0x0640, 0x0680, 0x06C0, 0x0700,
    0x0740, 0x0780, 0x07C0, 0x0800, 0x0900, 0x0A00, 0x0B00, 0x0C00, 0x0D00, 0x0E00,
    0x0F00, 0x1000, 0x1400, 0x1800, 0x1C00, 0x2000, 0x3000, 0x4000
};


static void deDPCM16(byte *soundBuf, sci::istream &audioStream, uint32_t n) {
    int16_t *out = (int16_t *)soundBuf;

    int32_t s = 0;
    for (uint32_t i = 0; i < n; i++) {
        uint8_t b;
        audioStream >> b;
        if (b & 0x80)
            s -= tableDPCM16[b & 0x7f];
        else
            s += tableDPCM16[b];

        s = min(32767, max(-32768, s));
        *out++ = (int16_t)s;
    }
}

static const uint8_t tableDPCM8[8] = { 0, 1, 2, 3, 6, 10, 15, 21 };

static void deDPCM8Nibble(byte *soundBuf, int32_t &s, uint8_t b) {
    if (b & 8) {
            s -= tableDPCM8[7 - (b & 7)];
    }
    else
        s += tableDPCM8[b & 7];
    s = min(255, max(0, s));
    *soundBuf = s;
}

static void deDPCM8(uint8_t *soundBuf, sci::istream &audioStream, uint32_t n) {
    int32_t s = 0x80;

    for (uint32_t i = 0; i < n; i++) {
        uint8_t b;
        audioStream >> b;

        deDPCM8Nibble(soundBuf++, s, b >> 4);
        deDPCM8Nibble(soundBuf++, s, b & 0xf);
    }
}

const char solMarker[] = "SOL";

void AudioWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    uint32_t baseOffset = byteStream.tellp();
    // Write the sync data first, if we have it.
    if (resource.TryGetComponent<SyncComponent>())
    {
        SyncWriteTo(resource, byteStream);
        propertyBag[BlobKey::LipSyncDataSize] = (byteStream.tellp() - baseOffset);
    }

    const AudioComponent &audio = resource.GetComponent<AudioComponent>();

    AudioHeader header = {};
    header.resourceType = (uint8_t)(0x80 | (int)ResourceType::Audio);
    header.headerSize = sizeof(header) - 2;
    header.flags = audio.Flags;
    header.audioType = *((uint32_t*)solMarker);
    header.sampleRate = audio.Frequency;
    header.sizeExcludingHeader = audio.GetLength();
    // PROBLEM: headers are different sizes in different games.
    // This particular one only works with SQ5 and KQ6
    byteStream << header;
    byteStream.WriteBytes(&audio.DigitalSamplePCM[0], (int)audio.DigitalSamplePCM.size());
}

void AudioReadFrom(ResourceEntity &resource, sci::istream &stream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    uint32_t offset = 0;
    auto itLipSyncDataSize = propertyBag.find(BlobKey::LipSyncDataSize);
    if (itLipSyncDataSize != propertyBag.end())
    {
        // This resource contains lip sync data
        sci::istream streamLipSync = stream;
        uint16_t lipSyncMarker;
        streamLipSync >> lipSyncMarker;
        if (lipSyncMarker == 0x8e)
        {
            resource.AddComponent<SyncComponent>(make_unique<SyncComponent>());
            SyncReadFrom(resource, streamLipSync);
        }
        // else if it's not 0x8e, then it means this resource is kind of corrupt and *only* has raw lipsync data.

        // We can still read in the raw data in either case though. It should be the rest of the data between the end
        // of the normal lipsync data and the beginning of the audio.
        if (streamLipSync.tellg() < (*itLipSyncDataSize).second)
        {
            uint32_t bufferSize = (*itLipSyncDataSize).second - streamLipSync.tellg();
            std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(bufferSize);
            streamLipSync.read_data(buffer.get(), bufferSize);
            std::string rawLipSyncData;
            std::transform(&buffer[0], &buffer[0] + bufferSize, std::back_inserter(rawLipSyncData),
                [](uint8_t data) { return (char)data; }
                );

            SyncComponent *sync = resource.TryGetComponent<SyncComponent>();
            if (sync)
            {
                std::copy(&buffer[0], &buffer[0] + bufferSize, std::back_inserter(sync->RawDataBinary));
                sync->RawData = rawLipSyncData;
            }
        }

        offset = (*itLipSyncDataSize).second;
    }
    stream.skip(offset);

    AudioComponent &audio = resource.GetComponent<AudioComponent>();

    AudioHeader header;
    stream >> header;

    stream.seekg(offset + header.headerSize + 2);

    audio.Frequency = header.sampleRate;
    audio.Flags = header.flags;
    
    if (header.sizeExcludingHeader > 0)
    {
        if (IsFlagSet(audio.Flags, AudioFlags::SixteenBit))
        {
            assert(IsFlagSet(audio.Flags, AudioFlags::Signed));
            if (IsFlagSet(audio.Flags, AudioFlags::DPCM))
            {
                uint32_t newSize = header.sizeExcludingHeader * 2;
                audio.DigitalSamplePCM.assign(newSize, 0);
                deDPCM16(&audio.DigitalSamplePCM[0], stream, header.sizeExcludingHeader);
            }
            else
            {
                // LSL6?
                audio.DigitalSamplePCM.assign(header.sizeExcludingHeader, 0);
                stream.read_data(&audio.DigitalSamplePCM[0], audio.DigitalSamplePCM.size());
            }
        }
        else
        {
            assert(!IsFlagSet(audio.Flags, AudioFlags::Signed));
            if (IsFlagSet(audio.Flags, AudioFlags::DPCM))
            {
                // Decompress it - KQ6, ....
                uint32_t newSize = header.sizeExcludingHeader * 2;
                audio.DigitalSamplePCM.assign(newSize, 0);
                deDPCM8(&audio.DigitalSamplePCM[0], stream, header.sizeExcludingHeader);
            }
            else
            {
                // Copy it directly over, uncompressed. SQ5, ....
                audio.DigitalSamplePCM.assign(header.sizeExcludingHeader, 0);
                stream.read_data(&audio.DigitalSamplePCM[0], audio.DigitalSamplePCM.size());
            }
        }
    }
}

ResourceTraits audioTraits =
{
    ResourceType::Audio,
    &AudioReadFrom,
    &AudioWriteTo,
    &NoValidationFunc,
    nullptr
};

ResourceEntity *CreateAudioResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(audioTraits);
    pResource->AddComponent(move(make_unique<AudioComponent>()));
    return pResource.release();
}

ResourceEntity *CreateDefaultAudioResource(SCIVersion version)
{
    // Nothing different.
    return CreateAudioResource(version);
}