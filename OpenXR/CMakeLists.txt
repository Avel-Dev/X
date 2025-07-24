project(OpenXR)

add_subdirectory("vendor/openxr/OpenXR-SDK-Source")
file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")

include_directories("vendor/OpenXR-SDK-Source/include")

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