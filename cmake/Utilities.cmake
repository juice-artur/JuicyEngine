function(copy_runtime_dll TARGET DLL_TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${DLL_TARGET}>
            $<TARGET_FILE_DIR:${TARGET}>
    )
endfunction()