include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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