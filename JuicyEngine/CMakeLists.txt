set(PROJECT_NAME JuicyEngine)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})

file(GLOB_RECURSE Core "src/JuicyEngine/Core/*.h" "src/JuicyEngine/Core/*.hpp" "src/JuicyEngine/Core/*.cpp" )
source_group("src/JuicyEngine/Core" FILES ${Core})

file(GLOB_RECURSE Events "src/JuicyEngine/Events/*.h" "src/JuicyEngine/Events/*.hpp" "src/JuicyEngine/Events/*.cpp" )
source_group("src/JuicyEngine/Events" FILES ${Events})

set(SOURCE_FILES ${src} ${Core} ${Events})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})



target_include_directories(${PROJECT_NAME} PRIVATE 
"src"
"vendor/spdlog/include")