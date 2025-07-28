project(Editor)

set(CMAKE_CXX_STANDARD 20)

# Set options before adding the OpenXR-SDK-Source
set(BUILD_API_LAYERS ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE EDITOR_SOURCES "src/*.cpp")

include_directories("src")

add_executable(Editor ${EDITOR_SOURCES})

target_link_libraries(Editor 
PRIVATE 
    VulkanEngine
)