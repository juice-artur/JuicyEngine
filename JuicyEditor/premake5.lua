project "JuicyEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	includedirs
	{
		"%{wks.location}/JuicyEngine/vendor/spdlog/include",
		"%{wks.location}/JuicyEngine/src",
		"%{wks.location}/JuicyEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
	}
	links
	{	
		"JuicyEngine"
	}

	postbuildcommands
	{
		"{COPYDIR} \"%{LibraryDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\""
	}

	filter "system:windows"
		systemversion "latest"
	filter "configurations:Debug"
		defines "JE_DEBUG"
		buildoptions "/MDd"
		symbols "on"
	filter "configurations:Release"
		defines "JE_RELEASE"
		buildoptions "/MD"
		optimize "on"
	filter "configurations:Dist"
		defines "JE_DIST"
		buildoptions "/MD"
		optimize "on"