set(SOURCES
    Source/jepch.h
    Source/JuicyEngine.h
    Source/Core/Core.h
    Source/Core/EntryPoint.h
    Source/Core/Application.h
    Source/Core/Application.cpp
    Source/Core/Log.h
    Source/Core/Log.cpp
    Source/Core/Window.h
    Source/Core/Window.cpp
    Source/Core/Layer.h
    Source/Core/Layer.cpp
    Source/Core/LayerStack.h
    Source/Core/LayerStack.cpp
    Source/Core/Input.h
    Source/Events/ApplicationEvent.h
    Source/Events/Event.h
    Source/Events/KeyEvent.h
    Source/Events/MouseEvent.h
    Source/Platform/Windows/WindowsWindow.cpp
    Source/Platform/Windows/WindowsWindow.h
    Source/Platform/Windows/WindowsInput.h
    Source/Platform/Windows/WindowsInput.cpp
    Source/Renderer/GraphicsContext.h
    Source/Platform/Vulkan/VulkanContext.h
    Source/Platform/Vulkan/VulkanContext.cpp
    Source/Platform/Vulkan/VulkanDevice.h
    Source/Platform/Vulkan/VulkanDevice.cpp
)


add_subdirectory(vendor)

add_library(JuicyEngine STATIC)

target_precompile_headers(JuicyEngine PRIVATE Source/jepch.h)

target_sources(JuicyEngine PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})


target_link_libraries(JuicyEngine vendor)

target_include_directories(JuicyEngine PUBLIC "Source")