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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
group ""

include "JuicyEngine"
include "Sandbox"
