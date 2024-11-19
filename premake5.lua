workspace "JuicyEngine"
	architecture "x64"
	startproject "JuicyEditor"

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
IncludeDir["glm"] = "JuicyEngine/vendor/glm"
IncludeDir["stb_image"] = "JuicyEngine/vendor/stb_image"

group "Dependencies"
	include "JuicyEngine/vendor/GLFW"
	include "JuicyEngine/vendor/Glad"
	include "JuicyEngine/vendor/imgui"
group ""


project "JuicyEngine"
	location "JuicyEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "jepch.h"
	pchsource "JuicyEngine/src/jepch.cpp"

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
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
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
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
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
		"JuicyEngine/src",
		"JuicyEngine/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"JuicyEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

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

project "JuicyEditor"
	location "JuicyEditor"
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
		"JuicyEngine/vendor/spdlog/include",
		"JuicyEngine/src",
		"JuicyEngine/vendor",
		"%{IncludeDir.glm}"
	}
	links
	{
		"JuicyEngine"
	}
	filter "system:windows"
		systemversion "latest"
		
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