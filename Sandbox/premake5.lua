project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
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
	}
	links
	{
		"JuicyEngine"
	}

	filter "system:windows"
		systemversion "latest"
		defines { "JE_PLATFORM_WINDOWS" }

	filter "configurations:Debug"
		defines "JE_DEBUG"
        runtime "Debug"
        symbols "On"

	filter "configurations:Release"
		defines "JE_RELEASE"
        optimize "On"
        symbols "On"

	filter "configurations:Dist"
		defines "JE_DIST"
		runtime "Release"
		optimize "On"
		symbols "Off"