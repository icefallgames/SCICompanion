#pragma once
#include "Sound.h"

SoundChangeHint InitializeFromMidi(SCIVersion version, std::vector<DeviceType> devices, SoundComponent &sound, const std::string &filename);
