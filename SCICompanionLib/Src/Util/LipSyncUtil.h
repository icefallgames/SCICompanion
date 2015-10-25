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

struct SyncComponent;
class PhonemeMap;
class alignment_result;
struct AudioComponent;

int SCITicksToMilliseconds(int ticks);
uint16_t MillisecondsToSCITicks(long ms);
std::unique_ptr<SyncComponent> CreateLipSyncComponentFromPhonemes(const PhonemeMap &phonemeMap, const std::vector<alignment_result> &alignments);
std::unique_ptr<SyncComponent> CreateLipSyncComponentFromAudioAndPhonemes(const AudioComponent &audio, const std::string &optionalText, const PhonemeMap &phonemeMap, std::vector<alignment_result> *optRawResults = nullptr);
void CreateLipSyncDataFromWav(const std::string &wavePath, const std::string &optionalTextIn, std::vector<alignment_result> &rawResults);