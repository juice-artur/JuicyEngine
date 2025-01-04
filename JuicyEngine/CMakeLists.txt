set(PROJECT_NAME JuicyEngine)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})

file(GLOB_RECURSE Core "src/JuicyEngine/Core/*.h" "src/JuicyEngine/Core/*.hpp" "src/JuicyEngine/Core/*.cpp" )
source_group("src/JuicyEngine/Core" FILES ${Core})

file(GLOB_RECURSE Events "src/JuicyEngine/Events/*.h" "src/JuicyEngine/Events/*.hpp" "src/JuicyEngine/Events/*.cpp" )
source_group("src/JuicyEngine/Events" FILES ${Events})

if (WIN32)
    file(GLOB_RECURSE PlatformWindows "src/JuicyEngine/Platform/Windows/*.h" "src/JuicyEngine/Platform/Windows/*.hpp" "src/JuicyEngine/Platform/Windows/*.cpp" )
    source_group("src/JuicyEngine/Platform/Windows" FILES ${PlatformWindows})
endif ()

set(SOURCE_FILES ${src} ${Core} ${Events} ${PlatformWindows})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/jepch.h)


target_include_directories(${PROJECT_NAME} PRIVATE 
"src"
"vendor/spdlog/include")

target_compile_definitions(${PROJECT_NAME} PUBLIC JE_PLATFORM_WINDOWS)