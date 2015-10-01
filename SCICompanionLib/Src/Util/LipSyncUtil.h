#pragma once

struct SyncComponent;
class PhonemeMap;
class alignment_result;
struct AudioComponent;

int SCITicksToMilliseconds(int ticks);
uint16_t MillisecondsToSCITicks(long ms);
std::unique_ptr<SyncComponent> CreateLipSyncComponentFromPhonemes(const PhonemeMap &phonemeMap, const std::vector<alignment_result> &alignments);
std::unique_ptr<SyncComponent> CreateLipSyncComponentFromAudioAndPhonemes(const AudioComponent &audio, const PhonemeMap &phonemeMap, std::vector<alignment_result> *optRawResults = nullptr);