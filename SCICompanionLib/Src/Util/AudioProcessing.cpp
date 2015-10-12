#include "stdafx.h"
#include "AudioProcessing.h"
#include "Audio.h"
#include "AudioNegative.h"
#include <random>

void SixteenBitToFloat(const int16_t *bufferIn, size_t sampleCount, std::vector<float> &result)
{
    result.clear();
    result.reserve(sampleCount);
    for (size_t i = 0; i < sampleCount; i++)
    {
        result.push_back(bufferIn[i] / 32768.0f);
    }
}

void FloatToSixteenBit(const std::vector<float> &bufferIn, std::vector<int16_t> &result)
{
    result.clear();
    result.reserve(bufferIn.size());
    for (float f : bufferIn)
    {
        int32_t temp = (int32_t)round(f * 32768.0f);
        result.push_back((int16_t)max(-32768, min(32767, temp)));
    }
}

void ApplyAutoGain(float *buffer, size_t totalFloats)
{
    float maxAmp = 0.0f;
    for (size_t i = 0; i < totalFloats; i++)
    {
        maxAmp = max(abs(buffer[i]), maxAmp);
    }
    float scale = 10.0f;    // At most...
    if (maxAmp > 0.002f)    // epsilon
    {
        maxAmp += 0.001f;   // Ensure we don't go QUITE to max.
        scale = min(scale, 1.0f / maxAmp);
    }

    for (size_t i = 0; i < totalFloats; i++)
    {
        buffer[i] = buffer[i] * scale;
    }
}

float dbToRms(float db)
{
    return pow(10.0f, db / 20.0f);
}

void ApplyNoiseGate(float *buffer, int totalFloats, float sampleRate, const AudioProcessingSettings &settings)
{
    const float AttackTime = (float)settings.Noise.AttackTimeMS / 1000.0f;
    const float ReleaseTime = (float)settings.Noise.ReleaseTimeMS / 1000.0f;
    const float HoldTime = (float)settings.Noise.HoldTimeMS / 1000.0f;
    const float OpenThresholdDb = (float)settings.Noise.OpenThresholdDB;
    const float CloseThresholdDb = (float)settings.Noise.CloseThresholdDB;

    float attenuation = 0.0f;
    float level = 0.0f;
    float heldTime = 0.0f;
    bool isOpen = false;

    float OpenThreshold = dbToRms(OpenThresholdDb);
    float CloseThreshold = dbToRms(CloseThresholdDb);

    if ((OpenThreshold < 0.0001f) && (CloseThreshold < 0.0001f))
    {
        // No noise reduction
        return;
    }

    const float SAMPLE_RATE_F = sampleRate;
    const float dtPerSample = 1.0f / SAMPLE_RATE_F;

    // Convert configuration times into per-sample amounts
    const float attackRate = 1.0f / (AttackTime * SAMPLE_RATE_F);
    const float releaseRate = 1.0f / (ReleaseTime * SAMPLE_RATE_F);

    // Determine level decay rate. We don't want human voice (75-300Hz) to cross the close
    // threshold if the previous peak crosses the open threshold.
    const float thresholdDiff = OpenThreshold - CloseThreshold;
    const float minDecayPeriod = (1.0f / 75.0f) * SAMPLE_RATE_F;
    const float decayRate = thresholdDiff / minDecayPeriod;

    // We can't use SSE as the processing of each sample depends on the processed
    // result of the previous sample.
    for (int i = 0; i < totalFloats; i++)
    {
        // Get current input level
        float curLvl = abs(buffer[i]);

        // Test thresholds
        if (curLvl > OpenThreshold && !isOpen)
            isOpen = true;
        if (level < CloseThreshold && isOpen)
        {
            heldTime = 0.0f;
            isOpen = false;
        }

        // Decay level slowly so human voice (75-300Hz) doesn't cross the close threshold
        // (Essentially a peak detector with very fast decay)
        level = max(level, curLvl) - decayRate;

        // Apply gate state to attenuation
        if (isOpen)
            attenuation = min(1.0f, attenuation + attackRate);
        else
        {
            heldTime += dtPerSample;
            if (heldTime > HoldTime)
                attenuation = max(0.0f, attenuation - releaseRate);
        }

        // Attenuate!
        buffer[i] *= attenuation;
    }
}


void ProcessSound(const AudioNegativeComponent &negative, AudioComponent &audioFinal, AudioFlags finalFlags)
{
    audioFinal.DigitalSamplePCM.clear();
    audioFinal.Frequency = negative.Audio.Frequency;
    audioFinal.Flags = finalFlags;

    std::vector<float> buffer;
    int blockAlign = 2;
    SixteenBitToFloat(reinterpret_cast<const int16_t*>(&negative.Audio.DigitalSamplePCM[0]), negative.Audio.DigitalSamplePCM.size() / blockAlign, buffer);

    // Trim the ends.
    uint32_t samplesToRemoveOffBack = min(negative.Audio.Frequency * negative.Settings.TrimRightMS / 1000, buffer.size());
    buffer.resize(buffer.size() - samplesToRemoveOffBack);
    uint32_t samplesToRemoveOffFront = min(negative.Audio.Frequency * negative.Settings.TrimLeftMS / 1000, buffer.size());
    buffer.erase(buffer.begin(), buffer.begin() + samplesToRemoveOffFront);

    if (!buffer.empty())
    {
        ApplyNoiseGate(&buffer[0], buffer.size(), negative.Audio.Frequency, negative.Settings);

        // I figure autogain should be applied after the noise gate, otherwise we'll need different noise settings
        // for every single "volume" of source audio.
        if (negative.Settings.AutoGain)
        {
            ApplyAutoGain(&buffer[0], buffer.size());
        }

        // This makes most sense after a noise gate has been applied:
        if (negative.Settings.DetectStartEnd)
        {
            // Remove quiet at end:
            while (!buffer.empty() && (buffer.back() == 0.0f))
            {
                buffer.pop_back();
            }
            // Remove quiet at beginning:
            int zeroCount = 0;
            for (float value : buffer)
            {
                if (value == 0.0f)
                {
                    zeroCount++;
                }
                else
                {
                    break;
                }
            }
            buffer.erase(buffer.begin(), buffer.begin() + zeroCount);
        }
    }

    std::vector<int16_t> processedSound;
    FloatToSixteenBit(buffer, processedSound);

    if (IsFlagSet(finalFlags, AudioFlags::SixteenBit))
    {
        // Just a straight copy
        uint8_t *asBytes = reinterpret_cast<uint8_t*>(&processedSound[0]);
        std::copy(asBytes, asBytes + processedSound.size() * 2, std::back_inserter(audioFinal.DigitalSamplePCM));
    }
    else
    {
        std::random_device rd;
        std::mt19937 myRandom(rd());
        std::uniform_int_distribution<int32_t> distribution(0, 128);
        std::uniform_int_distribution<int32_t> distribution2(0, 64);

        // Track quantization error
        int32_t prevError = 0;

        // We always record in 16bit. Now we'll reduce to 8 bit.
        // We want [-128,128] to map to [127.5,128.5]
        DWORD samples = processedSound.size();
        audioFinal.DigitalSamplePCM.reserve(samples);
        for (DWORD i = 0; i < samples; i++)
        {
            int32_t value = processedSound[i];

            // Add error from previous sample
            value += prevError;

            if (negative.Settings.AudioDither)
            {
                // Triangular pdf
                // Alternative -ve/+ve to try to simulate a frequency of 11Khz, which
                // the human ear isn't that senstive to (assuming sampling rate of 22050Hz).
                // This sounds significantly better than a purely random value.
                if (i % 2 == 0)
                {
                    value += distribution(myRandom) + distribution(myRandom);
                }
                else
                {
                    value -= distribution(myRandom) + distribution(myRandom);
                }
            }
            // TODO: Clamp properly... I guess it's ok with 65535 below, but the -ve will be bad.

            // I think this is more accurate:
            uint32_t unsigned16Raw = value + 32768;
            uint32_t unsigned16 = min(unsigned16Raw + 128, 65535);     // +128 acts as rounding so we can truncate
            uint8_t unsigned8 = (uint8_t)(unsigned16 / 256);
            prevError = (int32_t)unsigned16Raw - (int32_t)(unsigned8 * 256);
            audioFinal.DigitalSamplePCM.push_back(unsigned8);

            // But this flattens out noise around 0 better.
            //int8_t signed8 = sixteenBitBuffer[i] / 256;
            //audio->DigitalSamplePCM.push_back(signed8 + 128);
        }

        int x = 0;
    }

}
