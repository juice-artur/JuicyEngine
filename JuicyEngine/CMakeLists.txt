set(PROJECT_NAME JuicyEngine)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})

file(GLOB Engine "src/JuicyEngine/*.h" "src/JuicyEngine/*.hpp" "src/JuicyEngine/*.cpp")
source_group("src/JuicyEngine" FILES ${Engine})

set(SOURCE_FILES ${src} ${Engine})

# Target
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC vendor/spdlog/include)

target_compile_definitions(${PROJECT_NAME} PUBLIC JE_PLATFORM_WINDOWS JE_BUILD_DLL)