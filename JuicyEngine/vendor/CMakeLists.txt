set(JE_MOLTENVK_XCFRAMEWORK "" CACHE PATH "Path to a MoltenVK.xcframework (Apple builds only)")

if(APPLE)
    # SDL must be a static library for iOS app bundles.
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(SDL)
add_subdirectory(spdlog)
add_subdirectory(glm)
add_subdirectory(stb)

# ---------------------------------------------------------------------------
# Vulkan headers
# ---------------------------------------------------------------------------
# CMake's FindVulkan does not look inside Homebrew or the default LunarG SDK
# layout, so probe the well-known locations manually before falling back to it.
set(JE_VULKAN_HEADER_HINTS
    "$ENV{VULKAN_SDK}/macOS/include"
    "$ENV{VULKAN_SDK}/MacOS/include"
    "$ENV{VULKAN_SDK}/include"
    "/opt/homebrew/include"
    "/opt/homebrew/opt/vulkan-headers/include"
    "/usr/include"
    "/usr/local/include"
    "/usr/local/opt/vulkan-headers/include")

find_path(JE_VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h HINTS ${JE_VULKAN_HEADER_HINTS} NO_DEFAULT_PATH)

find_package(Vulkan QUIET)

set(JE_VULKAN_HEADERS_TARGET "")
if(TARGET Vulkan::Headers)
    set(JE_VULKAN_HEADERS_TARGET Vulkan::Headers)
elseif(JE_VULKAN_INCLUDE_DIR)
    add_library(JE_VulkanHeaders INTERFACE)
    target_include_directories(JE_VulkanHeaders INTERFACE "${JE_VULKAN_INCLUDE_DIR}")
    set(JE_VULKAN_HEADERS_TARGET JE_VulkanHeaders)
endif()

if(NOT JE_VULKAN_HEADERS_TARGET)
    message(FATAL_ERROR
        "Vulkan headers were not found. Install them via Homebrew "
        "(`brew install vulkan-headers`), or install the LunarG Vulkan SDK and set "
        "the VULKAN_SDK environment variable (see https://vulkan.lunarg.com/sdk/home).")
endif()

if(APPLE)
    # -----------------------------------------------------------------------
    # MoltenVK (the Vulkan-to-Metal driver) for Apple targets.
    # -----------------------------------------------------------------------
    if(JE_MOLTENVK_XCFRAMEWORK STREQUAL "")
        foreach(SUBDIR
                "Latest/MoltenVK/static"
                "Package/Latest/MoltenVK/static"
                "MacOS/lib"
                "macOS/lib"
                "lib")
            set(CANDIDATE "$ENV{VULKAN_SDK}/${SUBDIR}/MoltenVK.xcframework")
            if(EXISTS "${CANDIDATE}")
                set(JE_MOLTENVK_XCFRAMEWORK "${CANDIDATE}")
                break()
            endif()
        endforeach()
    endif()

    if(JE_MOLTENVK_XCFRAMEWORK STREQUAL "")
        # Homebrew's molten-vk ships macOS libraries only, so it is a valid
        # fallback for native macOS builds but not for iOS/tvOS targets.
        if(NOT IOS)
            find_file(JE_MOLTENVK_BREW_DYLIB
                NAMES libMoltenVK.dylib
                HINTS
                    "/opt/homebrew/opt/molten-vk/lib"
                    "/usr/local/opt/molten-vk/lib"
                    "/usr/local/lib"
                NO_DEFAULT_PATH)
        endif()

        if(JE_MOLTENVK_BREW_DYLIB)
            message(STATUS "Using MoltenVK (Homebrew): ${JE_MOLTENVK_BREW_DYLIB}")
            set(JE_MOLTENVK_LINK_ITEM "${JE_MOLTENVK_BREW_DYLIB}")
        else()
            message(FATAL_ERROR
                "MoltenVK was not found and is required for Apple builds."
                "iOS/tvOS targets need an SDK that ships the iOS/simulator slices of "
                "MoltenVK, so install the LunarG Vulkan SDK for macOS "
                "(https://vulkan.lunarg.com/sdk/home) and configure with VULKAN_SDK set, e.g.:\n"
                "  export VULKAN_SDK=\"/usr/local/VulkanSDK/<version>/macOS\"\n"
                "Alternatively set -DJE_MOLTENVK_XCFRAMEWORK=/path/to/MoltenVK.xcframework. "
                "For native macOS-only builds `brew install molten-vk` is sufficient.")
        endif()
    else()
        message(STATUS "Using MoltenVK: ${JE_MOLTENVK_XCFRAMEWORK}")
        set(JE_MOLTENVK_LINK_ITEM "${JE_MOLTENVK_XCFRAMEWORK}")
    endif()
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
        "${JE_VULKAN_HEADERS_TARGET}"
        SDL3::SDL3
        "${JE_MOLTENVK_LINK_ITEM}"
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
        "${JE_VULKAN_HEADERS_TARGET}"
        stb
        imgui
        "${JE_MOLTENVK_LINK_ITEM}"
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