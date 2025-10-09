set(SOURCES
    Source/JuicyEngine.h
    Source/Core/EntryPoint.h
    Source/Core/Application.h
    Source/Core/Application.cpp
    Source/Core/Log.h
    Source/Core/Log.cpp
    Source/Core/Window.h
    Source/Core/Window.cpp
    Source/Events/ApplicationEvent.h
    Source/Events/Event.h
    Source/Events/KeyEvent.h
    Source/Events/MouseEvent.h
    Source/Platform/Windows/WindowsWindow.cpp
    Source/Platform/Windows/WindowsWindow.h
)

add_subdirectory(vendor)

add_library(JuicyEngine STATIC)

target_sources(JuicyEngine PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})


target_link_libraries(JuicyEngine vendor)

target_include_directories(JuicyEngine PUBLIC "Source")