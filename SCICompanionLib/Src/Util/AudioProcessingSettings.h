#pragma once

struct NoiseSettings
{
    uint32_t AttackTimeMS;
    uint32_t ReleaseTimeMS;
    uint32_t HoldTimeMS;
    int32_t OpenThresholdDB;
    int32_t CloseThresholdDB;
};

struct AudioProcessingSettings
{
    AudioProcessingSettings() : 
        Version(0),
        TrimLeftMS(0),
        TrimRightMS(0),
        AutoGain(FALSE),
        DetectStartEnd(FALSE),
        AudioDither(FALSE),
        Noise()
    {
    }

    uint32_t Version;

    uint32_t TrimLeftMS;
    uint32_t TrimRightMS;
    BOOL AutoGain;
    BOOL DetectStartEnd;
    BOOL AudioDither;

    // Noise settings
    NoiseSettings Noise;
};