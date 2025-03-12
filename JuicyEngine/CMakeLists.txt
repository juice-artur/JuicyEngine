cmake_minimum_required(VERSION 3.29)

project(JuicyEngine)

add_library(${PROJECT_NAME} Source/EntryPoint.h Source/Application.h Source/Application.cpp)

set_target_properties(${PROJECT_NAME}  PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)