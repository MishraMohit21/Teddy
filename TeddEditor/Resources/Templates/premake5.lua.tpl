workspace "%{prj.name}"
	architecture "x64"
	startproject "GameAssembly"

	configurations
	{
		"Debug",
		"Release"
	}

-- C# Scripting Project
project "GameAssembly"
	kind "SharedLib"
	language "C#"
	clr "On"

	targetdir ("%{wks.location}/Binaries")
	objdir ("%{wks.location}/Binaries/Intermediates")

	files { "Scripts/**.cs" }
	excludes { "%{wks.location}/Binaries/**.cs" } -- Exclude temp files from previous builds

	-- Add reference to ScriptCore.dll
	libdirs { "%{wks.location}/Binaries/ScriptCore" }
	links { "ScriptCore" }
	copylocal "Off"

	filter "configurations:Debug"
		optimize "Off"
		symbols "On"

	filter "configurations:Release"
		optimize "On"
		symbols "Off"
