#include "tdpch.h"
#include "SoLoudSound.h"

#include "soloud_wav.h"
#include "soloud_wavstream.h"

#include <filesystem>

namespace Teddy
{
    namespace Utils
    {
        static SoundType GetSoundTypeFromFileExtension(const std::string& filename)
        {
            std::string extension = std::filesystem::path(filename).extension().string();
            if (extension == ".wav") return SoundType::WAV;
            if (extension == ".ogg") return SoundType::OGG_VORBIS;
            
            // TODO: Add more types like mp3 if needed

            return SoundType::Unknown;
        }
    }

    Ref<Sound> Sound::Create(const std::string& path)
    {
        return CreateRef<SoLoudSound>(path);
    }

    SoLoudSound::SoLoudSound(const std::string& path)
        : m_Path(path)
    {
        m_Type = Utils::GetSoundTypeFromFileExtension(path);

        SoLoud::result loadResult = SoLoud::FILE_NOT_FOUND; // Default to an error

        switch (m_Type)
        {
            case SoundType::WAV:
            {
                m_Wav = new SoLoud::Wav();
                loadResult = m_Wav->load(path.c_str());
                break;
            }
            case SoundType::OGG_VORBIS:
            {
                m_WavStream = new SoLoud::WavStream();
                loadResult = m_WavStream->load(path.c_str());
                break;
            }
            default:
                TD_CORE_WARN("Could not load sound '{0}': Unknown file type!", path);
                m_Wav = nullptr; // Ensure union is null
                return; // Exit early
        }

        if (loadResult != SoLoud::SO_NO_ERROR)
        {
            TD_CORE_ERROR("Failed to load audio file: {0}. SoLoud error code: {1}", path, loadResult);
            // Clean up the allocated object if loading failed
            if (m_Type == SoundType::WAV) delete m_Wav;
            else if (m_Type == SoundType::OGG_VORBIS) delete m_WavStream;
            m_Wav = nullptr; // Ensure union is null
        }
    }

    SoLoudSound::~SoLoudSound()
    {
        switch (m_Type)
        {
            case SoundType::WAV: delete m_Wav; break;
            case SoundType::OGG_VORBIS: delete m_WavStream; break;
            default: break; // Nothing to delete
        }
    }

    SoLoud::AudioSource* SoLoudSound::GetAudioSource() const
    {
        // The union allows us to just return the pointer, as they are at the same memory location.
        // This is valid as long as only one is ever non-null.
        return m_Wav;
    }
}
