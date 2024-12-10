#pragma once

#include "Teddy/Scene/ScriptableEntity.h"

namespace Teddy
{
    class ScriptRegistry {
    public:
        using ScriptCreateFunc = ScriptableEntity * (*)();

        // Register a script type
        template<typename T>
        static void Register() {
            GetRegistry()[typeid(T).name()] = []() {
                return static_cast<ScriptableEntity*>(new T());
                };
        }

        // Lookup function to recreate a script by its type name
        static ScriptableEntity* CreateScript(const std::string& scriptClassName) {
            auto it = GetRegistry().find(scriptClassName);
            if (it != GetRegistry().end()) {
                return it->second();
            }
            return nullptr;
        }

    private:
        // Singleton-like access to the registry
        static std::unordered_map<std::string, ScriptCreateFunc>& GetRegistry() {
            static std::unordered_map<std::string, ScriptCreateFunc> registry;
            return registry;
        }
    };
}