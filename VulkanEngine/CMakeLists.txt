set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
    add_compile_options(-march=x86-64 -mtune=generic -O2)
endif()

# set options before add_subdirectory
# available options: TRACY_ENABLE, TRACY_LTO, TRACY_ON_DEMAND, TRACY_NO_BROADCAST,
option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)

add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glm")
add_subdirectory("vendor/tracy")
add_subdirectory("vendor/yaml-cpp")
add_subdirectory("vendor/tinygltf")
#add_subdirectory("vendor/tinygltf/tinygltf/examples/build-gltf")

include_directories("src/")
include_directories("src/Core")
include_directories("src/Platform")
include_directories("vendor/tinygltf")
include_directories("vendor/glfw/include")
include_directories("vendor/glm")
include_directories("vendor/stb_image")
include_directories("vendor/json_nlohmann/include")
include_directories("vendor/spirv_reflect")
include_directories("vendor/tracy/public")
include_directories("vendor/yaml-cpp/include")
 
include_directories($ENV{VULKAN_SDK}/include)
link_directories($ENV{VULKAN_SDK}/lib)

add_compile_options(/EHsc)

file(GLOB_RECURSE SRC_SOURCES "src/**/*.cpp" "src/*.cpp")

add_executable(VulkanEngine
    ${SRC_SOURCES}
    "vendor/spirv_reflect/spirv_reflect.cpp"
    "main.cpp")

message("Source :" ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(VulkanEngine PRIVATE CHIKU_SRC_PATH=${CMAKE_CURRENT_SOURCE_DIR}/)

if(WIN32)
    set(VULKAN_LIB vulkan-1)
    target_compile_definitions(VulkanEngine PRIVATE PLT_WINDOWS)
    message(STATUS "Running on Windows")
elseif(UNIX AND NOT APPLE)
    set(VULKAN_LIB vulkan)
    target_compile_definitions(VulkanEngine PRIVATE PLT_UNIX)
    message(STATUS "Running on Linux")
elseif(APPLE)
    target_compile_definitions(VulkanEngine PRIVATE PLT_MAC)
    message(STATUS "Running on macOS")
else()
    message(STATUS "Unknown OS")
endif()

target_link_libraries(VulkanEngine
PUBLIC
    glfw
    Tracy::TracyClient
    ${VULKAN_LIB}
    yaml-cpp
    tinygltf
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanEngine PROPERTY CXX_STANDARD 20)
endif()