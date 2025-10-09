add_subdirectory(SDL)
add_subdirectory(spdlog)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 
spdlog::spdlog)