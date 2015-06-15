#pragma once
#include "Sound.h"

SoundChangeHint InitializeFromMidi(SCIVersion version, std::vector<DeviceType> devices, SoundComponent &sound, const std::string &filename);
std::string GetSoundLength(const SoundComponent &sound);