#pragma once

#include "Teddy/Audio/Sound.h"

// Forward declare SoLoud types to keep this header clean
namespace SoLoud
{
    class Wav;
    class WavStream;
}

namespace Teddy
{
    class SoLoudSound : public Sound
    {
    public:
        SoLoudSound(const std::string& path);
        virtual ~SoLoudSound();

        virtual const std::string& GetPath() const override { return m_Path; }
        virtual SoLoud::AudioSource* GetAudioSource() const override;
        virtual SoundType GetType() const override { return m_Type; }

    private:
        std::string m_Path;
        SoundType m_Type = SoundType::Unknown;

        // The sound data. We use a union because a sound is either
        // fully loaded (Wav) or streamed (WavStream), but never both.
        union 
        {
            SoLoud::Wav* m_Wav;
            SoLoud::WavStream* m_WavStream;
        };
    };
}
