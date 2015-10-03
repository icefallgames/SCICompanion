#pragma once
#include "Sound.h"

struct AudioComponent;
struct SoundComponent;
class ResourceEntity;
enum class AudioVolumeName : uint8_t;

void PopulateComboWithDevicesHelper(SCIVersion version, CComboBox &combo);
DeviceType GetDeviceFromComboHelper(SCIVersion version, CComboBox &combo);
void SelectDeviceInComboHelper(SCIVersion version, CComboBox &combo, DeviceType);
void SelectFirstDeviceWithChannels(SCIVersion version, CComboBox &combo, const SoundComponent &sound);
void AudioComponentFromWaveFile(sci::istream &stream, AudioComponent &audio);
std::unique_ptr<ResourceEntity> WaveResourceFromFilename(const std::string &filename);
std::string _NameFromFilename(PCSTR pszFilename);
void AddWaveFileToGame(const std::string &filename);
AudioVolumeName GetVolumeToUse(SCIVersion version, uint32_t base36Number);
std::string GetAudioVolumePath(const std::string &gameFolder, bool bak, AudioVolumeName volumeToUse, ResourceSourceFlags *sourceFlags = nullptr);
bool IsWaveFile(PCSTR pszFileName);
void WriteWaveFile(const std::string &filename, const AudioComponent &audio);