set(SOURCES
    Source/main.cpp
)

add_executable(Sandbox)

target_sources(Sandbox PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(Sandbox JuicyEngine)

target_include_directories(Sandbox PRIVATE Source)

# Set working directory for VS debugger
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Sandbox"
)