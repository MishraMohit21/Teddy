workspace "Teddy"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Teddy/vendor/GLFW/include"
IncludeDir["Glad"] = "Teddy/vendor/Glad/include"
IncludeDir["ImGui"] = "Teddy/vendor/imgui"
IncludeDir["glm"] = "Teddy/vendor/glm"
IncludeDir["stb_image"] = "Teddy/vendor/stb_image"

group "Dependencies"
	include "Teddy/vendor/GLFW"
	include "Teddy/vendor/Glad"
	include "Teddy/vendor/imgui"

group ""

project "Teddy"
	location "Teddy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "tdpch.h"
	pchsource "Teddy/src/tdpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TD_PLATFORM_WINDOWS",
			"TD_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "TD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TD_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "TD_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Teddy/vendor/spdlog/include",
		"Teddy/src",
		"Teddy/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Teddy"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TD_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TD_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "TD_DIST"
		runtime "Release"
		optimize "on"


project "TeddEditor"
	location "TeddEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"Teddy/vendor/spdlog/include",
		"Teddy/src",
		"Teddy/vendor",
		"%{IncludeDir.glm}"
	}
	links
	{
		"Teddy"
	}
	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "TD_DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "TD_RELEASE"
		runtime "Release"
		optimize "on"
	filter "configurations:Dist"
		defines "TD_DIST"
		runtime "Release"
		optimize "on"