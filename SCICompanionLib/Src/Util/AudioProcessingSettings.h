#pragma once

struct AudioProcessingSettings
{
    AudioProcessingSettings() : 
        Version(0),
        TrimLeftMS(0),
        TrimRightMS(0),
        AutoGain(FALSE),
        DetectStartEnd(FALSE),
        NoiseAttackTimeMS(0),
        NoiseReleaseTimeMS(0),
        NoiseHoldTimeMS(0),
        NoiseOpenThresholdDB(0),
        NoiseCloseThresholdDB(0)
    {
    }

    uint32_t Version;

    uint32_t TrimLeftMS;
    uint32_t TrimRightMS;
    BOOL AutoGain;
    BOOL DetectStartEnd;

    // Noise settings
    uint32_t NoiseAttackTimeMS;
    uint32_t NoiseReleaseTimeMS;
    uint32_t NoiseHoldTimeMS;
    int32_t NoiseOpenThresholdDB;
    int32_t NoiseCloseThresholdDB;
};