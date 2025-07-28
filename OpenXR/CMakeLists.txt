project(OpenXR)

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

# Set options before adding the OpenXR-SDK-Source
set(BUILD_API_LAYERS ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")

include_directories("src")

add_executable(OpenXR ${GAME_SOURCES})

if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT
                                   "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}"
    )
endif()

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        "${openxr_SOURCE_DIR}/src/common"
        "${openxr_SOURCE_DIR}/external/include"
)

target_link_libraries(OpenXR 
PRIVATE 
    VulkanEngine
    openxr_loader
)