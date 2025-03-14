cmake_minimum_required(VERSION 3.29)

project(JuicyEngine)

set(WindowsSource 
    Source/Platform/Windows/WindowsWindow.cpp)


add_library(${PROJECT_NAME} 
    Source/EntryPoint.h 
    Source/Application.h 
    Source/Application.cpp
    Source/Core/Window.h
    ${WindowsSource})
    
include_directories(${PROJECT_SOURCE_DIR}/Source/Core)

set_target_properties(${PROJECT_NAME}  PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")


find_package(Vulkan REQUIRED)

if (Vulkan_FOUND)
    message(STATUS "Found Vulkan: ${Vulkan_LIBRARY}")
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
else()
    message(FATAL_ERROR "Vulkan not found!")
endif()


find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)