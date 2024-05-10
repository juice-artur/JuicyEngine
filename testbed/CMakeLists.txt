
cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME testbed)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(src)
include_directories(GLOB_RECURSE ../engine/src)

add_compile_definitions(D_DEBUG JE_IMPORT)

set(SRC_FILES src/main.cpp)

set(INCLUDE_FILES)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
	${SRC_FILES} ${INCLUDE_FILES}
)


add_executable(${PROJECT_NAME} ${SRC_FILES} ${INCLUDE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/../engine/bin/$<CONFIG>/JuicyEngine.lib")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/../engine/bin/$<CONFIG>/JuicyEngine.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>
)



