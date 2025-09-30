project "SoLoud"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("../../../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../../../bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/core/**.cpp",
        "src/core/**.h",
        "src/audiosource/wav/**.cpp",
        "src/audiosource/wav/**.h",
        "src/audiosource/wav/stb_vorbis.c",
        "src/backend/miniaudio/**.c",
        "src/backend/miniaudio/**.h",
        "src/backend/null/**.cpp",
        "src/backend/null/**.h",
        "src/backend/wasapi/**.cpp",
        "src/backend/wasapi/**.h",
        "src/backend/winmm/**.cpp",
        "src/backend/winmm/**.h"
    }

    includedirs
    {
        "include"
    }

    defines
    {
        "SOLOUD_STATIC"
    }

    filter "system:windows"
        defines { "WITH_WINMM", "WITH_WASAPI" }

    filter {}