cmake_minimum_required(VERSION 3.31.0 FATAL_ERROR)

project(EngineDev CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VS_PLATFORM_NAME "x64")

option(ENABLE_ASAN "Enable address sanitizer" OFF)

# Global configuration types
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    "Distribution"
    CACHE STRING "" FORCE
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ASAN
if (ENABLE_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

# Sub-projects
add_subdirectory(JuicyEngine)
add_subdirectory(Sandbox)
