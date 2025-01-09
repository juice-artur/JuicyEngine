set(PROJECT_NAME JuicyEngine)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})

file(GLOB_RECURSE Core "src/JuicyEngine/Core/*.h" "src/JuicyEngine/Core/*.hpp" "src/JuicyEngine/Core/*.cpp" )
source_group("src/JuicyEngine/Core" FILES ${Core})

file(GLOB_RECURSE Events "src/JuicyEngine/Events/*.h" "src/JuicyEngine/Events/*.hpp" "src/JuicyEngine/Events/*.cpp" )
source_group("src/JuicyEngine/Events" FILES ${Events})

file(GLOB VulkanRHI "src/JuicyEngine/RHI/VulkanRHI/*.h" "src/JuicyEngine/RHI/VulkanRHI/*.hpp" "src/JuicyEngine/RHI/VulkanRHI/*.cpp" )
source_group("src/JuicyEngine/RHI/VulkanRHI" FILES ${VulkanRHI})

file(GLOB_RECURSE VulkanRHIPlatform "src/JuicyEngine/RHI/VulkanRHI/Platform/VulkanPlatform.h")

list(APPEND VulkanRHIPlatform "src/JuicyEngine/RHI/VulkanRHI/Platform/VulkanPlatformWindows.cpp")


source_group("src/JuicyEngine/RHI/VulkanRHI/Platform" FILES ${VulkanRHIPlatform})

file(GLOB_RECURSE Renderer "src/JuicyEngine/Renderer/*.h" "src/JuicyEngine/Renderer/*.hpp" "src/JuicyEngine/Renderer/*.cpp" )
source_group("src/JuicyEngine/Renderer" FILES ${Renderer})

if (WIN32)
    file(GLOB_RECURSE PlatformWindows "src/JuicyEngine/Platform/Windows/*.h" "src/JuicyEngine/Platform/Windows/*.hpp" "src/JuicyEngine/Platform/Windows/*.cpp" )
    source_group("src/JuicyEngine/Platform/Windows" FILES ${PlatformWindows})
endif ()

set(SOURCE_FILES ${src} ${Core} ${Events} ${PlatformWindows} ${VulkanRHI} ${Renderer} ${VulkanRHIPlatform})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/jepch.h)


target_include_directories(${PROJECT_NAME} PRIVATE 
"src"
"vendor/spdlog/include"
"vendor/glm")


target_compile_definitions(${PROJECT_NAME} PUBLIC JE_PLATFORM_WINDOWS JE_DEBUG)