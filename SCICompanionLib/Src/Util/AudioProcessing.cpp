/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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

float CalculateMaxAmplitude(const float *buffer, size_t totalFloats)
{
    float maxAmp = 0.0f;
    for (size_t i = 0; i < totalFloats; i++)
    {
        maxAmp = max(abs(buffer[i]), maxAmp);
    }
    return maxAmp;
}

// Auto gain will bring the waveform to within 80% of max. This is fairly in line with
// other Sierra games. Higher than that and we risk clipping when we apply compression.
void ApplyAutoGain(float *buffer, size_t totalFloats, float maxAmp)
{
    float scale = 10.0f;    // At most...
    if (maxAmp > 0.002f)    // epsilon
    {
        maxAmp += 0.001f;   // Ensure we don't go QUITE to max.
        scale = min(scale, 0.8f / maxAmp);
    }

    if (scale > 1.0f)
    {
        for (size_t i = 0; i < totalFloats; i++)
        {
            buffer[i] = buffer[i] * scale;
        }
    }
}

float dbToRms(float db)
{
    return pow(10.0f, db / 20.0f);
}

const float DC_OFFSET = 0.00001f;
const float DB_2_LOG = 0.115129254649f;
const float LOG_2_DB = 8.685889638065f;

float DecibelsToLinear(float dB)
{
    return std::exp(dB * DB_2_LOG);
}
float LinearToDecibels(float lin)
{
    return std::log(lin) * LOG_2_DB;
}

const float MaxAllowedAmplitudeDb = -0.4455f;   // 0.95 amplitude

void ApplyCompression(float *buffer, int totalFloats, float sampleRate, const AudioProcessingSettings &settings, float maxAmplitude)
{
    // Algorithm from NAudio

    // For now, we've hard-coded these values (suitable for voice)
    const float Threshold = -8.0f;      // db above with compression is applied
    const float Ratio = 0.2f;           // 5:1 compression ratio
    const uint32_t AttackMS = 1;
    const uint32_t ReleaseMS = 400;     // Make it more?

    // We apply MakeUpGain so that the compression doesn't make the signal *less* loud.
    // However, we don't want the gain to bring us up above MaxAllowedAmplitudeDb
    float maxAmplitudeDb = LinearToDecibels(maxAmplitude);
    // After compression, this amplitude will be less though...
    if (maxAmplitudeDb > Threshold)
    {
        // Ignoring attack, this is our max amplitude after compression
        // maxAmplitudeDb = Threshold + (maxAmplitudeDb - Threshold) * Ratio;
        // Except we can't really ignore attack... it's spikes that we're worried about... Even with a 1ms attack,
        // the waveform could reach its highest level.
    }
    float MakeUpGain = max(0.0f, MaxAllowedAmplitudeDb - maxAmplitudeDb);
    MakeUpGain = min(3.0f, MakeUpGain); // But no more than 3db of makeup gain.


    float attackCoeff = std::exp(-1.0f / (0.001f * (float)AttackMS * sampleRate));
    float releaseCoeff = std::exp(-1.0f / (0.001f * (float)ReleaseMS * sampleRate));

    float envdB = DC_OFFSET;

    for (int i = 0; i < totalFloats; i++)
    {
        // Get current input level
        float link = abs(buffer[i]);

        link += DC_OFFSET;					// add DC offset to avoid log( 0 )
        float keydB = LinearToDecibels(link);		// convert linear -> dB

        // threshold
        float overdB = keydB - Threshold;	// delta over threshold
        if (overdB < 0.0f)
            overdB = 0.0f;

        // attack/release

        overdB += DC_OFFSET;					// add DC offset to avoid denormal

        if (overdB > envdB)
        {
            // Attack
            envdB = overdB + attackCoeff * (envdB - overdB);
        }
        else
        {
            // Release
            envdB = overdB + releaseCoeff * (envdB - overdB);
        }

        overdB = envdB - DC_OFFSET;			// subtract DC offset

        // Regarding the DC offset: In this case, since the offset is added before 
        // the attack/release processes, the envelope will never fall below the offset,
        // thereby avoiding denormals. However, to prevent the offset from causing
        // constant gain reduction, we must subtract it from the envelope, yielding
        // a minimum value of 0dB.

        // transfer function
        float gr = overdB * (Ratio - 1.0f);	// gain reduction (dB)
        gr = DecibelsToLinear(gr) * DecibelsToLinear(MakeUpGain); // convert dB -> linear

        // output gain
        buffer[i] *= gr;	// apply gain reduction to input

        assert(buffer[i] >= -0.96f);
    }
}

bool ApplyNoiseGate(float *buffer, int totalFloats, float sampleRate, const AudioProcessingSettings &settings)
{
    const float AttackTime = (float)settings.Noise.AttackTimeMS / 1000.0f;
    const float ReleaseTime = (float)settings.Noise.ReleaseTimeMS / 1000.0f;
    const float HoldTime = (float)settings.Noise.HoldTimeMS / 1000.0f;
    const float OpenThresholdDb = (float)settings.Noise.OpenThresholdDB;
    const float CloseThresholdDb = (float)settings.Noise.CloseThresholdDB;

    const size_t LookAheadSampleCount = (size_t)(AttackTime * sampleRate);

    float attenuation = 0.0f;
    float level = 0.0f;
    float heldTime = 0.0f;
    bool isOpen = false;

    float OpenThreshold = dbToRms(OpenThresholdDb);
    float CloseThreshold = dbToRms(CloseThresholdDb);

    if ((OpenThreshold < 0.0001f) && (CloseThreshold < 0.0001f))
    {
        // No noise reduction
        return false;
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
    size_t lastIndex = totalFloats - 1;
    for (int i = 0; i < totalFloats; i++)
    {
        // Get current input level
        //float curLvl = abs(buffer[i]);
        float curLvl = abs(buffer[min(i + LookAheadSampleCount, lastIndex)]);

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
    return true;
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

    bool hadNoiseGate = false;
    if (!buffer.empty())
    {
        float maxAmp = 1.0f;
        if (negative.Settings.AutoGain || negative.Settings.Compression)
        {
            maxAmp = CalculateMaxAmplitude(&buffer[0], buffer.size());
        }

        // I figure autogain should be applied after the noise gate, otherwise we'll need different noise settings
        // for every single "volume" of source audio.
        // NOPE - changed my mind
        if (negative.Settings.AutoGain)
        {
            ApplyAutoGain(&buffer[0], buffer.size(), maxAmp);
        }

        hadNoiseGate = ApplyNoiseGate(&buffer[0], buffer.size(), negative.Audio.Frequency, negative.Settings);

        if (negative.Settings.Compression)
        {
            ApplyCompression(&buffer[0], buffer.size(), negative.Audio.Frequency, negative.Settings, maxAmp);
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
        if (!processedSound.empty())
        {
            uint8_t *asBytes = reinterpret_cast<uint8_t*>(&processedSound[0]);
            std::copy(asBytes, asBytes + processedSound.size() * 2, std::back_inserter(audioFinal.DigitalSamplePCM));
        }
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

            // If the value is zero, it's probably from a noise gate. Keep it quiet rather than
            // introducing dither noise at zero level.
            if (hadNoiseGate && (value != 0))
            {
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
            }

            // I think this is more accurate:
            int32_t signed16Raw = value + 32768;
            int32_t signed16 = min(signed16Raw + 128, 65535);     // +128 acts as rounding so we can truncate
            signed16 = max(signed16, 0);
            uint8_t unsigned8 = (uint8_t)(signed16 / 256);
            prevError = (int32_t)signed16Raw - (int32_t)(unsigned8 * 256);
            audioFinal.DigitalSamplePCM.push_back(unsigned8);

            // But this flattens out noise around 0 better.
            //int8_t signed8 = sixteenBitBuffer[i] / 256;
            //audio->DigitalSamplePCM.push_back(signed8 + 128);
        }

        int x = 0;
    }
    audioFinal.ScanForClipped();
}
