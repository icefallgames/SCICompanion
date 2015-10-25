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
#include "PhonemeDialog.h"
#include "PhonemeMap.h"
#include <sphelper.h>
#include "sapi_lipsync.h"
#include "LipSyncUtil.h"
#include "AppState.h"
#include "Audio.h"
#include "SoundUtil.h"

using namespace std;

#define UWM_LIPSYNCTASKDONE (WM_APP + 1)

std::vector<alignment_result> g_samplePhonemeSentence;
std::unique_ptr<ResourceEntity> g_samplePhonemeSentenceAudio;
std::string g_samplePhonemeSentenceText;

LRESULT PhonemeDialog::_OnLipSyncDone(WPARAM wParam, LPARAM lParam)
{
    g_samplePhonemeSentence = _taskSink->GetResponse();
    SetAudioComponents(g_samplePhonemeSentenceAudio->TryGetComponent<AudioComponent>(), _syncComponent.get());
    _UpdateLipSync();
    return 0;
}

PhonemeDialog::PhonemeDialog(int nView, int nLoop, PhonemeMap &map, CWnd* pParent)
    : AudioPlaybackUI<CExtResizableDialog>(PhonemeDialog::IDD, pParent),
    _viewResource(appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, nView)),
    _nView(nView),
    _nLoop(nLoop),
    m_wndMap(this, _viewResource.get(), nView, nLoop, map),
    _map(map)
{
}

BOOL PhonemeDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    OnInitDialogHelper();
    SetLoop(true);
    if (g_samplePhonemeSentence.empty())
    {
        _taskSink = std::make_unique<CWndTaskSink<std::vector<alignment_result>>>(this, UWM_LIPSYNCTASKDONE);

        try
        {
            std::string wavePath = appState->GetResourceMap().GetTopLevelSamplesFolder() + "\\PhonemeSentence.wav";
            g_samplePhonemeSentenceAudio = WaveResourceFromFilename(wavePath);

            std::string txtPath = appState->GetResourceMap().GetTopLevelSamplesFolder() + "\\PhonemeSentence.txt";
            std::ifstream file(txtPath);
            if (file.is_open())
            {
                std::string text;
                if (std::getline(file, text))
                {
                    g_samplePhonemeSentenceText = text;

                    _taskSink->StartTask(
                        [wavePath, text]()
                    {
                        std::vector<alignment_result> results;
                        CreateLipSyncDataFromWav(wavePath, text, results);
                        return results;
                    }
                    );
                }
            }
        }
        catch (...) {}
    }
    SetAudioComponents(g_samplePhonemeSentenceAudio->TryGetComponent<AudioComponent>(), _syncComponent.get());
    _UpdateLipSync();
    return fRet;
}

void PhonemeDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DoDataExchangeHelper(pDX);

    DDX_Control(pDX, IDC_PHONEME, m_wndMap);

    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_EDIT_SAMPLE, m_wndSampleText);
    DDX_Control(pDX, IDC_EDIT_DESCRIPTION2, m_wndDescription);
    m_wndDescription.SetWindowText(
        "Drag phonemes next to the mouth shape that best represents them. Press the play button to listen to a sample sentence using the phoneme map."
        );
    DDX_Control(pDX, IDC_ANIMATE, m_wndMouth);
    m_wndMouth.SetResource(_viewResource.get());
    m_wndMouth.SetLoop(_nLoop);
    SetMouthElement(&m_wndMouth);
}

void PhonemeDialog::OnMapUpdated()
{
    _UpdateLipSync();
}

void PhonemeDialog::_UpdateLipSync()
{
    // Combine the raw lip sync data with a new sync component
    if (!g_samplePhonemeSentence.empty())
    {
        _syncComponent = CreateLipSyncComponentFromPhonemes(_map, g_samplePhonemeSentence);
        // Update the audio player with a new sync component.
        SetSyncResource(_syncComponent.get());
    }
    m_wndSampleText.SetWindowText(g_samplePhonemeSentenceText.c_str());
}

BEGIN_MESSAGE_MAP(PhonemeDialog, AudioPlaybackUI<CExtResizableDialog>)
    ON_MESSAGE(UWM_LIPSYNCTASKDONE, _OnLipSyncDone)
END_MESSAGE_MAP()
