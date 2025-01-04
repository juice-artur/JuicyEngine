set(PROJECT_NAME JuicyEngine)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})

file(GLOB_RECURSE Core "src/JuicyEngine/Core/*.h" "src/JuicyEngine/Core/*.hpp" "src/JuicyEngine/Core/*.cpp" )
source_group("src/JuicyEngine/Core" FILES ${Core})

set(SOURCE_FILES ${src} ${Core})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})