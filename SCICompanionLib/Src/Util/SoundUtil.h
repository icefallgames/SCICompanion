#pragma once
#include "Sound.h"

struct AudioComponent;

void PopulateComboWithDevicesHelper(SCIVersion version, CComboBox &combo);
DeviceType GetDeviceFromComboHelper(SCIVersion version, CComboBox &combo);
void SelectDeviceInComboHelper(SCIVersion version, CComboBox &combo, DeviceType);
void AudioComponentFromWaveFile(sci::istream &stream, AudioComponent &audio);