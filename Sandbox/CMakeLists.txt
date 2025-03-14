cmake_minimum_required(VERSION 3.29)

project(Sandbox)


add_executable(${PROJECT_NAME} Source/EntryPoint.cpp)
set_target_properties(${PROJECT_NAME}  PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/JuicyEngine/Source
)

target_link_libraries(${PROJECT_NAME} PRIVATE JuicyEngine)