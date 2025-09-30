#pragma once

#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/Entity.h"

namespace Teddy
{
    class AudioSystem
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnUpdate(Scene* scene, float deltaTime);

        // API for scripts and other systems
        static void Play(Entity entity);
        static void Stop(Entity entity);
        static void Pause(Entity entity);
        
        // Global controls
        static void SetMasterVolume(float volume);
    };
}