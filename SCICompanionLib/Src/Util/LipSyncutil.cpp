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
#include <sphelper.h>
#include "sapi_lipsync.h"
#include "phone_estimate.h"
#include "LipSyncUtil.h"
#include "PhonemeMap.h"
#include "Sync.h"
#include <locale>
#include <codecvt>
#include "SoundUtil.h"
#include "AppState.h"
#include <regex>

// 60 ticks per second
uint16_t MillisecondsToSCITicks(long ms)
{
    return (uint16_t)(ms * SCITicksPerSecond / 1000);
}

int SCITicksToMilliseconds(int ticks)
{
    return ticks * 1000 / SCITicksPerSecond;
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

std::string RemoveVocalCues(const std::string text)
{
    std::regex pattern1("\\([^)]+\\)");
    std::string temp = std::regex_replace(text, pattern1, "");
    std::regex pattern2("\\|[^)]+\\|");
    return std::regex_replace(temp, pattern2, "");
}

std::unique_ptr<SyncComponent> CreateLipSyncComponentFromPhonemes(const PhonemeMap &phonemeMap, const std::vector<alignment_result> &alignments)
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

    // Add on a closing marker. Possibly this will overlap the last phoneme? How do we tell it to return to silence?
    uint16_t finalTicks = alignments.empty() ? 0 : MillisecondsToSCITicks(alignments.back().m_msEnd);
    AddSyncEntry(*syncComponent, finalTicks, silenceCel);
    // Sierra sync resources don't always end with 0xffff. So I'm not sure what I'm supposed to do.

    return syncComponent;
}

void CreateLipSyncDataFromWav(const std::string &wavePath, const std::string &optionalTextIn, std::vector<alignment_result> &rawResults)
{
    // Do the sapi thing
    HRESULT hrCoinit = CoInitialize(nullptr);
    if (SUCCEEDED(hrCoinit))
    {
        try
        {
            // 1. [optional] declare the SAPI 5.1 estimator. 
            // NOTE: for different phoneme sets: create a new estimator
            phoneme_estimator sapi51Estimator;

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wfilename = converter.from_bytes(wavePath);
            std::string optionalTextTemp = RemoveVocalCues(optionalTextIn);
            std::wstring optionalText = converter.from_bytes(optionalTextTemp);

            // 2. declare the sapi lipsync object and call the lipsync method to
            // start the lipsync process
            std::unique_ptr<sapi_textless_lipsync> lipSyncTextless;
            std::unique_ptr<sapi_textbased_lipsync> lipSyncTextbased;
            sapi_lipsync *lipSync = nullptr;
            if (optionalText.empty())
            {
                lipSyncTextless = std::make_unique<sapi_textless_lipsync>(&sapi51Estimator);
                if (lipSyncTextless->lipsync(wfilename))
                {
                    lipSync = lipSyncTextless.get();
                }
            }
            else
            {
                lipSyncTextbased = std::make_unique<sapi_textbased_lipsync>(&sapi51Estimator);
                if (lipSyncTextbased->lipsync(wfilename, optionalText))
                {
                    lipSync = lipSyncTextbased.get();
                }
            }

            if (lipSync)
            {
                // Apparently, even though this is not a UI thread, we need to pump messages.
                MSG msg;
                while (!lipSync->isDone())
                {
                    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                    Sleep(1);
                }

                lipSync->finalize_phoneme_alignment();

                rawResults = lipSync->get_phoneme_alignment();

            }
        }
        catch (...)
        {
            CoUninitialize();
        }
    }
}

std::unique_ptr<SyncComponent> CreateLipSyncComponentFromAudioAndPhonemes(const AudioComponent &audio, const std::string &optionalTextIn, const PhonemeMap &phonemeMap, std::vector<alignment_result> *optRawResults)
{
    std::unique_ptr<SyncComponent> result;

    char szTempFilename[MAX_PATH];
    if (GetTempFileName(appState->GetResourceMap().Helper().GameFolder.c_str(), "LIP", 0, szTempFilename))
    {
        std::string tempWaveFilename = szTempFilename;
        WriteWaveFile(tempWaveFilename, audio);

        std::vector<alignment_result> phonemeAlignment;
        CreateLipSyncDataFromWav(tempWaveFilename, optionalTextIn, phonemeAlignment);
        if (optRawResults)
        {
            *optRawResults = phonemeAlignment;
        }

        result = CreateLipSyncComponentFromPhonemes(phonemeMap, phonemeAlignment);

        deletefile(tempWaveFilename);
    }
    return result;
}