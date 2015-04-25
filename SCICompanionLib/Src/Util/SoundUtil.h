#pragma once
#include "Sound.h"

struct AudioComponent;

void PopulateComboWithDevicesHelper(CComboBox &combo);
DeviceType GetDeviceFromComboHelper(CComboBox &combo);
void SelectDeviceInComboHelper(CComboBox &combo, DeviceType);
void AudioComponentFromWaveFile(sci::istream &stream, AudioComponent &audio);