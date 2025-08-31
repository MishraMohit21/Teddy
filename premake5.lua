VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "Teddy"
	architecture "x64"
	startproject "TeddEditor"

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
IncludeDir["mono"] = "vendor/mono/include"
IncludeDir["ImGuizmo"] = "vendor/ImGuizmo"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"
LibraryDir["mono"] = "$(SolutionDir)Teddy/vendor/mono/lib/%{cfg.buildcfg}"


Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"
Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"


Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"


group "Dependencies"
	include "Teddy/vendor/GLFW"
	include "Teddy/vendor/Glad"
	include "Teddy/vendor/imgui"
	include "Teddy/vendor/yaml-cpp"
	include "Teddy/vendor/Box2D"
	

group ""


group "Main"
 
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
		"$(ProjectDir)%{IncludeDir.mono}",
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
		"opengl32.lib",
		"ScriptCore",
		"%{Library.mono}",

	}

	filter "files:Teddy/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TD_PLATFORM_WINDOWS",
			"TD_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		links
		{
    		"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
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


project "ScriptCore"
		location "ScriptCore"
		kind "SharedLib"
		language "C#"
		dotnetframework "4.7.2"
		
		targetdir ("$(SolutionDir)TeddEditor/Resources/Scripts")
		objdir ("$(SolutionDir)TeddEditor/Resources/Scripts/Intermediates")
		
		files
		{
		    "ScriptCore/Source/ **. cs",
		    "ScriptCore/Properties/ **. cs"
    	}
		
		filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
		
		filter "configurations:Release"
		optimize "On"
		symbols "Default"
		
		filter "configurations:Dist"
		optimize "Full"
		symbols "Off"

group ""

group "Misc"	

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
		"Teddy/%{IncludeDir.glm}",
		"Teddy/%{IncludeDir.yaml_cpp}",
		"Teddy/%{IncludeDir.ImGuizmo}",
		"Teddy/%{IncludeDir.entt}"

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

group ""

group "Editor"

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
		"Teddy/%{IncludeDir.glm}",
		"Teddy/%{IncludeDir.yaml_cpp}",
		"Teddy/%{IncludeDir.ImGuizmo}",	
		"Teddy/%{IncludeDir.Box2D}",
		"Teddy/%{IncludeDir.entt}"
	}
	-- In your TeddEditor project
	postbuildcommands
	{
		'{COPY} "%{wks.location}/TeddEditor/mono/lib" "%{cfg.targetdir}/../lib/mono"',
		-- '{COPY} "%{wks.location}/TeddEditor/mono/etc" "%{cfg.targetdir}/etc"' -- Comment out or delete this line
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


group ""