-- JuicyEngine Dependencies
VULKAN_SDK = os.getenv("VULKAN_SDK")
IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/JuicyEngine/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/JuicyEngine/vendor/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/JuicyEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/JuicyEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/JuicyEngine/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/JuicyEngine/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/JuicyEngine/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/JuicyEngine/vendor/entt/include"
IncludeDir["shaderc"] = "%{wks.location}/JuicyEngine/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/JuicyEngine/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"
Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{VULKAN_SDK}/Bin"
Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"