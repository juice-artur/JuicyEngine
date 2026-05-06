add_subdirectory(SDL)
add_subdirectory(spdlog)
add_subdirectory(glm)
add_subdirectory(stb)

find_package(Vulkan REQUIRED)

set(IMGUI_SOURCES
    imgui-docking/imgui.cpp
    imgui-docking/imgui_draw.cpp
    imgui-docking/imgui_widgets.cpp
    imgui-docking/imgui_tables.cpp
    imgui-docking/imgui_demo.cpp
    imgui-docking/backends/imgui_impl_sdl3.cpp
    imgui-docking/backends/imgui_impl_vulkan.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL3::SDL3)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE 
    glm::glm
    SDL3::SDL3 
    spdlog::spdlog
    Vulkan::Vulkan
    stb
    imgui
)