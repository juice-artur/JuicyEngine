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

if(WIN32)
    if(TARGET SDL3::SDL3)
        add_custom_command(TARGET Sandbox POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:SDL3::SDL3>"
                "$<TARGET_FILE_DIR:Sandbox>"
            COMMENT "Copying SDL3 DLL to output directory"
        )
    else()
        message(WARNING "SDL3::SDL3 target not found - DLL won't be copied automatically")
    endif()
endif() 