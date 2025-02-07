-- JuicyEngine Dependencies
VULKAN_SDK = os.getenv("VULKAN_SDK")
IncludeDir = {}
IncludeDir["glm"] = "%{wks.location}/JuicyEngine/vendor/glm"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/JuicyEngine/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"