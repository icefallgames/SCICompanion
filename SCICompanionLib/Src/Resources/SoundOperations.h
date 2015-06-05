#pragma once
#include "Sound.h"

SoundChangeHint InitializeFromMidi(DeviceType device, SoundComponent &sound, const std::string &filename);
