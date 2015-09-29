#include "stdafx.h"
#include <sphelper.h>
#include "sapi_lipsync.h"
#include "phone_estimate.h"
#include "LipSyncUtil.h"
#include "PhonemeMap.h"
#include "Sync.h"
#include <locale>
#include <codecvt>

// 60 ticks per second
uint16_t MillisecondsToSCITicks(long ms)
{
    return (uint16_t)(ms * 60 / 1000);
}

void AddSyncEntry(SyncComponent &sync, uint16_t ticks, uint16_t cel)
{
    if (sync.Entries.back().Tick == ticks)
    {
        // If it's the same ticks as the last one (possible, because SAPI is higher resoution than SCI)
        // then replace
        sync.Entries.back().Cel = cel;
    }
    else
    {
        // Otherwise add
        sync.Entries.emplace_back(ticks, cel);
    }
}

// TODO: GEnerate warning report hwne missing phonemes............
// TODO: Generate word report too
std::unique_ptr<SyncComponent> CreateLipSyncComponentFromPhonemes(PhonemeMap &phonemeMap, const std::vector<alignment_result> &alignments)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    std::unique_ptr<SyncComponent> syncComponent = std::make_unique<SyncComponent>();

    uint16_t silenceCel = (uint16_t)phonemeMap.PhonemeToCel("x");
    if (silenceCel == -1)
    {
        silenceCel = 0;
    }

    // Ensure we start with silence.
    syncComponent->Entries.emplace_back(0, silenceCel);

    for (const auto &alignment : alignments)
    {
        int error = alignment.m_msEnd - alignment.m_phonemeEndTimes.back();
        assert(abs(error) <= 1);
        for (size_t i = 0; i < alignment.m_phonemes.size(); i++)
        {
            uint16_t cel = phonemeMap.PhonemeToCel(converter.to_bytes(alignment.m_phonemes[i]));
            if (cel != 0xffff)
            {
                long startTime;
                if (i == 0)
                {
                    startTime = alignment.m_msStart;
                }
                else
                {
                    startTime = alignment.m_phonemeEndTimes[i - 1];
                }
                uint16_t ticks = MillisecondsToSCITicks(startTime);
                AddSyncEntry(*syncComponent, ticks, cel);
            }
        }

    }

    // Add on a closing marker.
    uint16_t finalTicks = alignments.empty() ? 0 : MillisecondsToSCITicks(alignments.back().m_msEnd);
    AddSyncEntry(*syncComponent, finalTicks, 0xffff);

    return syncComponent;
}