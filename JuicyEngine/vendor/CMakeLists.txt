add_subdirectory(SDL)
add_subdirectory(spdlog)
add_subdirectory(glm)

find_package(Vulkan REQUIRED)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE 
glm::glm
SDL3::SDL3 
spdlog::spdlog
Vulkan::Vulkan)