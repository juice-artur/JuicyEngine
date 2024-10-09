workspace "JuicyEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "JuicyEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "JuicyEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "JuicyEngine/vendor/imgui"

group "Dependencies"
	include "JuicyEngine/vendor/GLFW"
	include "JuicyEngine/vendor/Glad"
	include "JuicyEngine/vendor/imgui"
group ""


project "JuicyEngine"
	location "JuicyEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "jepch.h"
	pchsource "JuicyEngine/src/jepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}
	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"JE_PLATFORM_WINDOWS",
			"JE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "JE_DEBUG"
		buildoptions "/MD"
		symbols "On"

	filter "configurations:Release"
		defines "JE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "JE_DIST"
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"JuicyEngine/vendor/spdlog/include",
		"JuicyEngine/src"
	}

	links
	{
		"JuicyEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"JE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "JE_DEBUG"
		buildoptions "/MD"
		symbols "On"

	filter "configurations:Release"
		defines "JE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "JE_DIST"
		buildoptions "/MD"
		optimize "On"