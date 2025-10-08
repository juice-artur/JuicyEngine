set(SOURCES
    Source/Core/Application.h
    Source/Core/Application.cpp
)

add_library(JuicyEngine STATIC)

target_sources(JuicyEngine PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_include_directories(JuicyEngine PUBLIC "Source")