cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME "JuicyEngine")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIRS 
    "$ENV{VULKAN_SDK}/Include"
)

set(LINKER_FLAGS 
    "-luser32"
    "-lvulkan-1"
    "-L$ENV{VULKAN_SDK}/Lib"
)

add_compile_definitions(D_DEBUG JE_EXPORT)


set(SRC_FILES src/Engine.cpp
src/Core/Log.cpp
src/Platform/Windows/PlatformWindows.cpp)

set(INCLUDE_FILES src/CoreDefines.h
                src/Core/Log.h
                src/Platform/Platform.h
                src/Engine.h
                src/Platform/Windows/PlatformWindows.h
)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
	${SRC_FILES} ${INCLUDE_FILES}
)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${INCLUDE_FILES})

target_include_directories(${PROJECT_NAME}  PUBLIC ${INCLUDE_DIRS}   )

  
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/Platform
  ${CMAKE_SOURCE_DIR}/src/Core)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)



set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/$<CONFIG>
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/$<CONFIG>
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/$<CONFIG>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND_EXPAND_LISTS
  )

add_subdirectory(external)