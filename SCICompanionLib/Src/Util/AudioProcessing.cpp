#include "stdafx.h"
#include "AudioProcessing.h"

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
        result.push_back((int16_t)round(f * 32768.0f));
    }
}

void FloatToEightBit(const std::vector<float> &bufferIn, std::vector<uint8_t> &result)
{
    result.clear();
    result.reserve(bufferIn.size());
    for (float f : bufferIn)
    {
        result.push_back((uint8_t)round(f * 128.0f + 128.0f));
    }
}