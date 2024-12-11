include "./vendor/premake/premake_customization/solution_items.lua"

workspace "JuicyEngine"
	architecture "x86_64"
	startproject "JuicyEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".clang-format"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/JuicyEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/JuicyEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/JuicyEngine/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/JuicyEngine/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/JuicyEngine/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/JuicyEngine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/JuicyEngine/vendor/yaml-cpp/include"

group "Dependencies"
	include "vendor/premake"
	include "JuicyEngine/vendor/GLFW"
	include "JuicyEngine/vendor/Glad"
	include "JuicyEngine/vendor/imgui"
	include "JuicyEngine/vendor/yaml-cpp"
group ""

include "JuicyEngine"
include "Sandbox"
include "JuicyEditor"