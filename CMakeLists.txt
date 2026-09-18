cmake_minimum_required(VERSION 3.24)

project(GameEngine)

list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake"
)

set(CMAKE_CXX_STANDARD 23)

option(JE_ENABLE_VALIDATION_LAYERS "Enable Vulkan validation layers when available" ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Projects
add_subdirectory(JuicyEngine)
add_subdirectory(Sandbox)