#include "tdpch.h"
#include "AudioSystem.h"

#include "Teddy/Scene/Component.h"
#include "Teddy/Audio/Sound.h"

#include "soloud.h"
#include "soloud_wav.h"

#include <unordered_map>

namespace Teddy
{
    // SoLoud engine instance
    static SoLoud::Soloud* s_AudioEngine = nullptr;

    // Cache for loaded sound assets
    static std::unordered_map<std::string, Ref<Sound>> s_SoundCache;

    void AudioSystem::Init()
    {
        TD_CORE_CRITICAL("AudioSystem::Init() called");
        s_AudioEngine = new SoLoud::Soloud();
        SoLoud::result initResult = s_AudioEngine->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::WINMM);
        if (initResult != SoLoud::SO_NO_ERROR)
        {
            TD_CORE_CRITICAL("SoLoud audio engine failed to initialize! Error code: {0}", initResult);
            delete s_AudioEngine;
            s_AudioEngine = nullptr;
        }
    }

    void AudioSystem::Shutdown()
    {
        s_SoundCache.clear();

        if (s_AudioEngine)
        {
            s_AudioEngine->deinit();
            delete s_AudioEngine;
            s_AudioEngine = nullptr;
        }
    }

    void AudioSystem::OnUpdate(Scene* scene, float deltaTime)
    {
        if (!s_AudioEngine) return;

        auto view = scene->GetAllEntitiesWith<AudioSourceComponent>();
        for (auto e : view)
        {
            Entity entity = { e, scene };
            auto& asc = entity.GetComponent<AudioSourceComponent>();

            if (asc.CurrentState == AudioSourceComponent::State::Playing)
            {
                if (!s_AudioEngine->isValidVoiceHandle(asc.RuntimeHandle))
                {
                    asc.CurrentState = AudioSourceComponent::State::Stopped;
                    asc.RuntimeHandle = 0;
                }
            }
        }
    }

    void AudioSystem::Play(Entity entity)
    {
		TD_CORE_CRITICAL("AudioSystem::Play called");
        if (!s_AudioEngine) return;
        TD_CORE_CRITICAL("AudioSystem::Placalledy ");

        TD_CORE_TRACE("AudioSystem::Play called for entity {0}", entity.GetComponent<UUIDComponent>().id);
        auto& asc = entity.GetComponent<AudioSourceComponent>();

        if (asc.FilePath.empty())
        {
            TD_CORE_WARN("AudioSourceComponent on entity {0} has an empty FilePath.", entity.GetComponent<UUIDComponent>().id);
            return;
        }

        TD_CORE_TRACE("Attempting to play audio file: {0}", asc.FilePath);

        Ref<Sound> sound = nullptr;
        if (s_SoundCache.find(asc.FilePath) != s_SoundCache.end())
        {
            sound = s_SoundCache.at(asc.FilePath);
            TD_CORE_TRACE("Sound found in cache.");
        }
        else
        {
            TD_CORE_TRACE("Sound not in cache, loading from file...");
            sound = Sound::Create(asc.FilePath);
            if (sound)
                s_SoundCache[asc.FilePath] = sound;
        }

        if (sound && sound->GetAudioSource())
        {
            asc.RuntimeHandle = s_AudioEngine->play(*sound->GetAudioSource(), asc.Volume, 0.0f, asc.bLoop);
            s_AudioEngine->setRelativePlaySpeed(asc.RuntimeHandle, asc.Pitch);
            asc.CurrentState = AudioSourceComponent::State::Playing;
            TD_CORE_TRACE("Successfully started playback. Handle: {0}", asc.RuntimeHandle);
        }
        else
        {
            TD_CORE_WARN("Could not play sound: Could not load file '{0}'", asc.FilePath);
        }
    }

    void AudioSystem::Stop(Entity entity)
    { 
        if (!s_AudioEngine) return;

        auto& asc = entity.GetComponent<AudioSourceComponent>();

        if (asc.CurrentState == AudioSourceComponent::State::Playing)
        {
            s_AudioEngine->stop(asc.RuntimeHandle);
            asc.CurrentState = AudioSourceComponent::State::Stopped;
            asc.RuntimeHandle = 0;
        }
    }

    void AudioSystem::Pause(Entity entity)
    {
        // TODO: Implement pause logic
    }

    void AudioSystem::SetMasterVolume(float volume)
    {
        if (!s_AudioEngine) return;

        s_AudioEngine->setGlobalVolume(volume);
    }
}