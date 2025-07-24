#pragma once

#ifdef PLT_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32 
#define NOMINMAX  // Add this before glfw3native.h to suppress min/max macros
#elif PLT_UNIX
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>