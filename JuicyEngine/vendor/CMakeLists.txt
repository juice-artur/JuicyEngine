set(JE_MOLTENVK_XCFRAMEWORK "" CACHE PATH "Path to MoltenVK.xcframework (Apple builds only)")

if(APPLE)
    # SDL must be a static library for iOS app bundles.
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(SDL)
add_subdirectory(spdlog)
add_subdirectory(glm)
add_subdirectory(stb)

if(APPLE)
    find_package(Vulkan REQUIRED)

    if(JE_MOLTENVK_XCFRAMEWORK STREQUAL "")
        foreach(SUBDIR MacOS macOS lib)
            set(CANDIDATE "$ENV{VULKAN_SDK}/${SUBDIR}/lib/MoltenVK.xcframework")
            if(EXISTS "${CANDIDATE}")
                set(JE_MOLTENVK_XCFRAMEWORK "${CANDIDATE}")
                break()
            endif()
        endforeach()
    endif()

    if(JE_MOLTENVK_XCFRAMEWORK STREQUAL "")
        message(FATAL_ERROR
            "MoltenVK not found. Install the LunarG Vulkan SDK (macOS) with the iOS/MoltenVK "
            "components and set the VULKAN_SDK environment variable, or set "
            "JE_MOLTENVK_XCFRAMEWORK to the MoltenVK.xcframework path.")
    endif()

    message(STATUS "Using MoltenVK: ${JE_MOLTENVK_XCFRAMEWORK}")
else()
    find_package(Vulkan REQUIRED)
endif()

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

if(APPLE)
    set(JE_APPLE_FRAMEWORKS
        "-framework Metal"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework Foundation"
        "-framework UIKit"
        "-framework IOSurface"
    )
    target_link_libraries(imgui PUBLIC
        Vulkan::Headers
        SDL3::SDL3
        "${JE_MOLTENVK_XCFRAMEWORK}"
        ${JE_APPLE_FRAMEWORKS}
    )
else()
    target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL3::SDL3)
endif()

add_library(vendor INTERFACE)
if(APPLE)
    target_link_libraries(vendor INTERFACE 
        glm::glm
        SDL3::SDL3 
        spdlog::spdlog
        Vulkan::Headers
        stb
        imgui
        "${JE_MOLTENVK_XCFRAMEWORK}"
        ${JE_APPLE_FRAMEWORKS}
    )
else()
    target_link_libraries(vendor INTERFACE 
        glm::glm
        SDL3::SDL3 
        spdlog::spdlog
        Vulkan::Vulkan
        stb
        imgui
    )
endif()