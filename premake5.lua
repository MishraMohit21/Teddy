VULKAN_SDK = "C:/VulkanSDK/1.3.296.0"

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
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"
IncludeDir["entt"] = "vendor/entt"
IncludeDir["yaml_cpp"] = "vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "vendor/Box2D/include"
IncludeDir["ImGuizmo"] = "vendor/ImGuizmo"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"
Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"


group "Dependencies"
	include "Teddy/vendor/GLFW"
	include "Teddy/vendor/Glad"
	include "Teddy/vendor/imgui"
	include "Teddy/vendor/yaml-cpp"
	include "Teddy/vendor/Box2D"
	

group ""

project "Teddy"
	location "Teddy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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

		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"$(ProjectDir)src",
		"$(ProjectDir)vendor/spdlog/include",
		"$(ProjectDir)%{IncludeDir.GLFW}",
		"$(ProjectDir)%{IncludeDir.Glad}",
		"$(ProjectDir)%{IncludeDir.ImGui}",
		"$(ProjectDir)%{IncludeDir.glm}",
		"$(ProjectDir)%{IncludeDir.stb_image}",
		"$(ProjectDir)%{IncludeDir.entt}",
		"$(ProjectDir)%{IncludeDir.ImGuizmo}",
		"$(ProjectDir)%{IncludeDir.Box2D}",
		"$(ProjectDir)%{IncludeDir.yaml_cpp}",
		"$(ProjectDir)%{IncludeDir.VulkanSDK}",

	}

	links 
	{ 
		"GLFW",
		"Glad",
		"Box2D",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

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

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "TD_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "TD_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.entt}"

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
	staticruntime "off"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Panels/**.h",
		"%{prj.name}/Panels/**.cpp",
	}
	includedirs
	{
		"Teddy/vendor/spdlog/include",
		"Teddy/src",
		"Teddy/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",	
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.entt}"
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