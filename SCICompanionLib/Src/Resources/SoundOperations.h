#pragma once
#include "Sound.h"

SoundChangeHint InitializeFromMidi(SCIVersion version, DeviceType device, SoundComponent &sound, const std::string &filename);
