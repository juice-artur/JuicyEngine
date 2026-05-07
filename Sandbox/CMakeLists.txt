include(Utilities)

set(SOURCES
    Source/main.cpp
)

add_executable(Sandbox)

target_sources(Sandbox PRIVATE ${SOURCES})

set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin"

    VS_DEBUGGER_WORKING_DIRECTORY
        $<TARGET_FILE_DIR:Sandbox>
)

target_link_libraries(Sandbox JuicyEngine)

target_include_directories(Sandbox PRIVATE Source)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${SOURCES}
)

if(WIN32)
    if(TARGET SDL3::SDL3)
        copy_runtime_dll(Sandbox SDL3::SDL3)
    endif()
    
    add_custom_command(TARGET Sandbox POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Sandbox/Assets
            $<TARGET_FILE_DIR:Sandbox>/Assets
    )
endif() 