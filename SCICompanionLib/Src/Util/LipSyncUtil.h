#pragma once

struct SyncComponent;
class PhonemeMap;
class alignment_result;

std::unique_ptr<SyncComponent> CreateLipSyncComponentFromPhonemes(PhonemeMap &phonemeMap, const std::vector<alignment_result> &alignments);