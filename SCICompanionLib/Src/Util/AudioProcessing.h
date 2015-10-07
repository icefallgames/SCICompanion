#pragma once

struct AudioNegativeComponent;
struct AudioComponent;

void SixteenBitToFloat(const int16_t *bufferIn, size_t sampleCount, std::vector<float> &result);
void FloatToSixteenBit(const std::vector<float> &bufferIn, std::vector<int16_t> &result);

void ProcessSound(const AudioNegativeComponent &negative, AudioComponent &audioFinal);