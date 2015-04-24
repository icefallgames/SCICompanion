#include "stdafx.h"
#include "Audio.h"
#include "AppState.h"
#include "ResourceEntity.h"

using namespace std;

// Decompression routines From SCUMMVM:
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

void AudioReadFrom(ResourceEntity &resource, sci::istream &stream)
{
    AudioComponent &audio = resource.GetComponent<AudioComponent>();

    AudioHeader header;
    stream >> header;

    audio.Length = header.sizeExcludingHeader;
    audio.Frequency = header.sampleRate;
    audio.Flags = header.flags;
    assert(!IsFlagSet(audio.Flags, AudioFlags::SixteenBit));
    assert(!IsFlagSet(audio.Flags, AudioFlags::Stereo));
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

// REVIEW: We don't even know how to read SCI1 sounds yet.
ResourceTraits audioTraits =
{
    ResourceType::Sound,
    &AudioReadFrom,
    nullptr,
    &NoValidationFunc
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