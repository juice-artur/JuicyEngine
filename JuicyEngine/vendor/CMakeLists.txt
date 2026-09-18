set(JE_MOLTENVK_XCFRAMEWORK "" CACHE PATH "Path to a MoltenVK.xcframework (Apple builds only)")
set(JE_MOLTENVK_VERSION "v1.4.2" CACHE STRING "MoltenVK release tag downloaded when no local SDK is found (Apple)")
set(JE_DOWNLOAD_MOLTENVK ON CACHE BOOL "Auto-download a MoltenVK release when no local installation exists (Apple)")

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
# MoltenVK discovery (Apple only)
# Resolution order:
#   1. Explicit -DJE_MOLTENVK_XCFRAMEWORK
#   2. LunarG Vulkan SDK (VULKAN_SDK) - the recommended install
#   3. Homebrew molten-vk (native macOS only)
#   4. Auto-download of the official MoltenVK release (contains the iOS
#      xcframework AND the Vulkan headers)
# ---------------------------------------------------------------------------
if(APPLE)
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
                    "/usr/local/opt/molten-vk/lib"
                NO_DEFAULT_PATH)
        endif()

        if(JE_MOLTENVK_BREW_DYLIB)
            message(STATUS "MoltenVK found (Homebrew): ${JE_MOLTENVK_BREW_DYLIB}")
            set(JE_MOLTENVK_LINK_ITEM "${JE_MOLTENVK_BREW_DYLIB}")
        endif()
    else()
        message(STATUS "MoltenVK found: ${JE_MOLTENVK_XCFRAMEWORK}")
        set(JE_MOLTENVK_LINK_ITEM "${JE_MOLTENVK_XCFRAMEWORK}")
    endif()

    if(NOT JE_MOLTENVK_LINK_ITEM)
        if(JE_DOWNLOAD_MOLTENVK)
            set(JE_MOLTENVK_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/_deps/moltenvk-${JE_MOLTENVK_VERSION}")
            set(JE_MOLTENVK_TAR "${CMAKE_BINARY_DIR}/_deps/MoltenVK-${JE_MOLTENVK_VERSION}-ios.tar")

            message(STATUS "Downloading MoltenVK ${JE_MOLTENVK_VERSION} - this provides the iOS/simulator slices")
            file(DOWNLOAD
                "https://github.com/KhronosGroup/MoltenVK/releases/download/${JE_MOLTENVK_VERSION}/MoltenVK-ios.tar"
                "${JE_MOLTENVK_TAR}"
                STATUS JE_MOLTENVK_DOWNLOAD_STATUS
                SHOW_PROGRESS)
            list(GET JE_MOLTENVK_DOWNLOAD_STATUS 0 JE_MOLTENVK_DOWNLOAD_CODE)
            if(NOT JE_MOLTENVK_DOWNLOAD_CODE EQUAL 0)
                list(GET JE_MOLTENVK_DOWNLOAD_STATUS 1 JE_MOLTENVK_DOWNLOAD_MSG)
                message(FATAL_ERROR "Failed to download MoltenVK: ${JE_MOLTENVK_DOWNLOAD_MSG}")
            endif()

            file(MAKE_DIRECTORY "${JE_MOLTENVK_DOWNLOAD_DIR}")
            file(ARCHIVE_EXTRACT INPUT "${JE_MOLTENVK_TAR}" DESTINATION "${JE_MOLTENVK_DOWNLOAD_DIR}")

            file(GLOB_RECURSE JE_MOLTENVK_FOUND_XCFRAMEWORK
                "${JE_MOLTENVK_DOWNLOAD_DIR}/**/MoltenVK.xcframework")
            list(LENGTH JE_MOLTENVK_FOUND_XCFRAMEWORK JE_MOLTENVK_FOUND_COUNT)

            if(JE_MOLTENVK_FOUND_COUNT GREATER 0)
                set(JE_MOLTENVK_INDEX -1)
                set(JE_MOLTENVK_I 0)
                foreach(JE_MOLTENVK_CANDIDATE IN LISTS JE_MOLTENVK_FOUND_XCFRAMEWORK)
                    if(JE_MOLTENVK_CANDIDATE MATCHES "/static/")
                        set(JE_MOLTENVK_INDEX ${JE_MOLTENVK_I})
                        break()
                    endif()
                    math(EXPR JE_MOLTENVK_I "${JE_MOLTENVK_I} + 1")
                endforeach()
                if(JE_MOLTENVK_INDEX EQUAL -1)
                    set(JE_MOLTENVK_INDEX 0)
                endif()
                list(GET JE_MOLTENVK_FOUND_XCFRAMEWORK ${JE_MOLTENVK_INDEX} JE_MOLTENVK_XCFRAMEWORK)
                message(STATUS "MoltenVK downloaded: ${JE_MOLTENVK_XCFRAMEWORK}")
                set(JE_MOLTENVK_LINK_ITEM "${JE_MOLTENVK_XCFRAMEWORK}")

                file(GLOB_RECURSE JE_MOLTENVK_FOUND_HEADER
                    "${JE_MOLTENVK_DOWNLOAD_DIR}/**/vulkan/vulkan.h")
                list(LENGTH JE_MOLTENVK_FOUND_HEADER JE_MOLTENVK_FOUND_HEADER_COUNT)
                if(JE_MOLTENVK_FOUND_HEADER_COUNT GREATER 0)
                    list(GET JE_MOLTENVK_FOUND_HEADER 0 JE_MOLTENVK_HEADER_FILE)
                    get_filename_component(JE_MOLTENVK_INCLUDE_DIR "${JE_MOLTENVK_HEADER_FILE}" DIRECTORY)
                    get_filename_component(JE_MOLTENVK_INCLUDE_DIR "${JE_MOLTENVK_INCLUDE_DIR}" DIRECTORY)
                else()
                    message(FATAL_ERROR
                        "Downloaded MoltenVK archive does not contain vulkan/vulkan.h. "
                        "Install the LunarG Vulkan SDK (https://vulkan.lunarg.com/sdk/home) and set VULKAN_SDK.")
                endif()
            else()
                message(FATAL_ERROR
                    "Downloaded MoltenVK archive does not contain a MoltenVK.xcframework. "
                    "Install the LunarG Vulkan SDK (https://vulkan.lunarg.com/sdk/home) and set VULKAN_SDK, "
                    "or pass -DJE_MOLTENVK_XCFRAMEWORK=/path/to/MoltenVK.xcframework.")
            endif()
        else()
            message(FATAL_ERROR
                "MoltenVK was not found and is required for Apple builds. "
                "Install the LunarG Vulkan SDK for macOS (https://vulkan.lunarg.com/sdk/home) and set VULKAN_SDK, e.g.:\n"
                "  export VULKAN_SDK=\"/usr/local/VulkanSDK/<version>/macOS\"\n"
                "or enable the automatic download with -DJE_DOWNLOAD_MOLTENVK=ON. "
                "For native macOS-only builds `brew install molten-vk` is sufficient.")
        endif()
    endif()
else()
    find_package(Vulkan REQUIRED)
endif()

# ---------------------------------------------------------------------------
# Vulkan headers
# ---------------------------------------------------------------------------
if(APPLE)
    set(JE_VULKAN_HEADER_HINTS
        "$ENV{VULKAN_SDK}/macOS/include"
        "$ENV{VULKAN_SDK}/MacOS/include"
        "$ENV{VULKAN_SDK}/include"
        "${JE_MOLTENVK_INCLUDE_DIR}"
        "/opt/homebrew/include"
        "/opt/homebrew/opt/vulkan-headers/include"
        "/usr/local/include"
        "/usr/local/opt/vulkan-headers/include")

    find_path(JE_VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h HINTS ${JE_VULKAN_HEADER_HINTS} NO_DEFAULT_PATH)

    # Fall back to CMake's FindVulkan for layout detection.
    find_package(Vulkan QUIET)
else()
    # CMake's FindVulkan is fine for non-Apple platforms.
    find_package(Vulkan QUIET)
endif()

set(JE_VULKAN_HEADERS_TARGET "")
if(JE_VULKAN_INCLUDE_DIR)
    add_library(JE_VulkanHeaders INTERFACE)
    target_include_directories(JE_VulkanHeaders INTERFACE "${JE_VULKAN_INCLUDE_DIR}")
    set(JE_VULKAN_HEADERS_TARGET JE_VulkanHeaders)
    message(STATUS "Vulkan headers: ${JE_VULKAN_INCLUDE_DIR}")
elseif(TARGET Vulkan::Headers)
    set(JE_VULKAN_HEADERS_TARGET Vulkan::Headers)
    message(STATUS "Vulkan headers: Vulkan::Headers (FindVulkan)")
elseif(TARGET Vulkan::Vulkan)
    set(JE_VULKAN_HEADERS_TARGET Vulkan::Vulkan)
    message(STATUS "Vulkan headers: Vulkan::Vulkan (FindVulkan)")
else()
    if(APPLE)
        message(FATAL_ERROR
            "Vulkan headers were not found. The MoltenVK release normally ships them; "
            "otherwise install them via Homebrew (`brew install vulkan-headers`) or set VULKAN_SDK "
            "(https://vulkan.lunarg.com/sdk/home).")
    else()
        message(FATAL_ERROR
            "Vulkan headers were not found. Install the LunarG Vulkan SDK "
            "(https://vulkan.lunarg.com/sdk/home).")
    endif()
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