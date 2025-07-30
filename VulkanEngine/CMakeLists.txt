cmake_minimum_required(VERSION 3.12)
project(VulkanEngine)
add_definitions(-DREQUIRED_VR_VULKAN)

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_API_LAYERS
        ON
        CACHE INTERNAL "Use OpenXR layers"
)
set(BUILD_TESTS
        OFF
        CACHE INTERNAL "Build tests"
)
FetchContent_Declare(
    OpenXR
    EXCLUDE_FROM_ALL
    DOWNLOAD_EXTRACT_TIMESTAMP
    URL_HASH MD5=f52248ef83da9134bec2b2d8e0970677
    URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.1.49.tar.gz
    SOURCE_DIR
    openxr
)

FetchContent_MakeAvailable(OpenXR)

if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT
                                   "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}"
    )
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
    add_compile_options(-march=x86-64 -mtune=generic -O2)
endif()

# Tracy options
option(TRACY_ENABLE "Enable Tracy profiler" ON)
option(TRACY_ON_DEMAND "Enable Tracy on-demand mode" ON)

# Add dependencies
add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glm")
add_subdirectory("vendor/tracy")
add_subdirectory("vendor/yaml-cpp")
add_subdirectory("vendor/tinygltf")
add_subdirectory("vendor/spdlog")

# Source files
file(GLOB_RECURSE SRC_SOURCES "src/**/*.cpp" "src/*.cpp")
set(SRC_SOURCES ${SRC_SOURCES} "vendor/spirv_reflect/spirv_reflect.cpp")

# Define the static library
add_library(VulkanEngine STATIC ${SRC_SOURCES})

# Link directories
target_link_directories(VulkanEngine PUBLIC $ENV{VULKAN_SDK}/lib)

if(WIN32)
target_compile_options ( VulkanEngine     
PUBLIC
"/utf-8")
endif()

# Include directories
target_include_directories( VulkanEngine
PUBLIC
    src/
    src/Core
    src/Platform
    vendor/tinygltf
    vendor/spdlog/include
    vendor/glfw/include
    vendor/glm
    vendor/stb_image
    vendor/json_nlohmann/include
    vendor/spirv_reflect
    vendor/tracy/public
    vendor/yaml-cpp/include
    $ENV{VULKAN_SDK}/include
    "${openxr_SOURCE_DIR}/src/common"
    "${openxr_SOURCE_DIR}/external/include"

)
# Set macro for source path
target_compile_definitions(VulkanEngine 
PRIVATE 
    CHIKU_SRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/"  
    CHIKU_ENABLE_LOGGING
)

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

# Link libraries
target_link_libraries(VulkanEngine
    PUBLIC
        glfw
        Tracy::TracyClient
        yaml-cpp
        tinygltf
        ${VULKAN_LIB}
        openxr_loader
)

# Exception handling (for MSVC)
if (MSVC)
    target_compile_options(VulkanEngine PRIVATE /EHsc)
endif()