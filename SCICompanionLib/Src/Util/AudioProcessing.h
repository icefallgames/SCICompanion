#pragma once

void SixteenBitToFloat(const int16_t *bufferIn, size_t sampleCount, std::vector<float> &result);
void FloatToSixteenBit(const std::vector<float> &bufferIn, std::vector<int16_t> &result);
void FloatToEightBit(const std::vector<float> &bufferIn, std::vector<uint8_t> &result);

