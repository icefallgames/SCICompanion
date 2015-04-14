#pragma once
#include "Sound.h"

void PopulateComboWithDevicesHelper(CComboBox &combo);
DeviceType GetDeviceFromComboHelper(CComboBox &combo);
void SelectDeviceInComboHelper(CComboBox &combo, DeviceType);