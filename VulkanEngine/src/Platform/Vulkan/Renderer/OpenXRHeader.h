#pragma once
#define XR_USE_GRAPHICS_API_VULKAN        // must come before OpenXR headers
#include <vulkan/vulkan.h>                // Vulkan types like VkInstance, VkDevice
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// Platform headers
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <unknwn.h>
#define XR_USE_PLATFORM_WIN32
#endif