set(PROJECT_NAME Sandbox)

file(GLOB src "src/*.h" "src/*.hpp" "src/*.cpp")
source_group("src" FILES ${src})


set(SOURCE_FILES ${src})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../JuicyEngine/src"
)

# Link with oxylus.
target_link_libraries(${PROJECT_NAME} PRIVATE
    JuicyEngine
)