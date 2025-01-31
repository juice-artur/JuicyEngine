project "JuicyEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	pchheader "jepch.h"
	pchsource "src/jepch.cpp"
	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl", 
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.glm}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"%{Library.Vulkan}",
	}


	filter "system:windows"
		systemversion "latest"
		defines { "JE_PLATFORM_WINDOWS" }

	filter "configurations:Debug"
		defines "JE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "JE_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "On"

	filter "configurations:Dist"
		defines "JE_DIST"
		runtime "Release"
		optimize "On"
		symbols "Off"