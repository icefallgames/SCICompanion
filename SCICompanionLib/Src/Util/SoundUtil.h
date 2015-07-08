#pragma once
#include "Sound.h"

struct AudioComponent;
struct SoundComponent;

void PopulateComboWithDevicesHelper(SCIVersion version, CComboBox &combo);
DeviceType GetDeviceFromComboHelper(SCIVersion version, CComboBox &combo);
void SelectDeviceInComboHelper(SCIVersion version, CComboBox &combo, DeviceType);
void SelectFirstDeviceWithChannels(SCIVersion version, CComboBox &combo, const SoundComponent &sound);
void AudioComponentFromWaveFile(sci::istream &stream, AudioComponent &audio);

void AddWaveFileToGame(const std::string &filename);
bool IsWaveFile(PCSTR pszFileName);