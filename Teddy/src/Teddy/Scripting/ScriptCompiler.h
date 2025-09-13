#pragma once

#include "Teddy/Project/Project.h"

#include <filesystem>

namespace Teddy
{
    class ScriptCompiler
    {
    public:
        static bool Compile(const Ref<Project>& project);
        static bool IsSolutionReady(const Ref<Project>& project);
    private:
        static bool GenerateSolution(const Ref<Project>& project);
    };
}
