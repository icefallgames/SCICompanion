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
#pragma once

struct AudioNegativeComponent;
struct AudioComponent;
enum class AudioFlags : uint8_t;

void SixteenBitToFloat(const int16_t *bufferIn, size_t sampleCount, std::vector<float> &result);
void FloatToSixteenBit(const std::vector<float> &bufferIn, std::vector<int16_t> &result);

void ProcessSound(const AudioNegativeComponent &negative, AudioComponent &audioFinal, AudioFlags finalFlags);