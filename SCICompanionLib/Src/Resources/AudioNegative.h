#pragma once

#include "Components.h"
#include "Audio.h"
#include "AudioProcessingSettings.h"

// Contains an original 16-bit recording (the audio "negative")
// along with some settings.
// This does not correspond to any Sierra resource. It's simply used in our audio cache files.
// The persisted form of this is a wav file.
struct AudioNegativeComponent : public ResourceComponent
{
public:
    AudioNegativeComponent() {}

    ResourceComponent *Clone() const override
    {
        return new AudioNegativeComponent(*this);
    }

    AudioComponent Audio;

    AudioProcessingSettings Settings;
};
