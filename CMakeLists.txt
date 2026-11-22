cmake_minimum_required(VERSION 3.20)

project(GameEngine)

list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake"
)

# Shared target settings
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
set_target_properties(project_options PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)

add_library(project_warnings INTERFACE)
if(MSVC)
    target_compile_options(project_warnings INTERFACE /W4 /WX)
else()
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Projects
add_subdirectory(JuicyEngine)
add_subdirectory(Sandbox)
