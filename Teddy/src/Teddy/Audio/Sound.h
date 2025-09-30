#pragma once

#include "Teddy/Core/Core.h"
#include <string>

namespace SoLoud
{
    class AudioSource;
}

namespace Teddy
{
    // Forward declaration
    class Sound;

    enum class SoundType
    {
        Unknown = 0,
        WAV,      // For short sound effects, loaded into memory
        OGG_VORBIS // For music, streamed from disk
    };


    class Sound
    {
    public:
        virtual ~Sound() = default;

        virtual const std::string& GetPath() const = 0;
        virtual SoLoud::AudioSource* GetAudioSource() const = 0;
        virtual SoundType GetType() const = 0;

        static Ref<Sound> Create(const std::string& path);
    };
}
