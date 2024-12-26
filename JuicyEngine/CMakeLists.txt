set(PROJECT_NAME JuicyEngine)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})

file(GLOB Engine "src/JuicyEngine/*.h" "src/JuicyEngine/*.hpp" "src/JuicyEngine/*.cpp")
source_group("src/JuicyEngine" FILES ${Engine})

file(GLOB Core "src/JuicyEngine/Core/*.h" "src/JuicyEngine/Core/*.hpp" "src/JuicyEngine/Core/*.cpp")
source_group("src/JuicyEngine/Core" FILES ${Core})

IF (WIN32)
file(GLOB Platform-Windows "src/JuicyEngine/Platform/Windows/*.h" "src/JuicyEngine/Platform/Windows/*.hpp" "src/JuicyEngine/Platform/Windows/*.cpp")
source_group("src/JuicyEngine/Platform/Windows" FILES ${Platform-Windows})
ENDIF()

set(SOURCE_FILES ${src} ${Engine} ${Core} ${Platform-Windows})

# Target
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC 
"${CMAKE_CURRENT_SOURCE_DIR}/src"
"${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")

target_compile_definitions(${PROJECT_NAME} PUBLIC JE_PLATFORM_WINDOWS JE_BUILD_DLL)

# Precompiled header setup
target_precompile_headers(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jepch.h"
)

# Ensure the precompiled header source file is compiled
target_sources(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jepch.cpp"
)