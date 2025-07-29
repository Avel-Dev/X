#pragma once


#ifdef PLT_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32 
#define NOMINMAX  // Add this before glfw3native.h to suppress min/max macros
#elif PLT_UNIX
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif

#define RENDERER_VULKAN
#define ENABLE_VALIDATION_LAYERS
#define CHIKU_ENABLE_LOGGING

#ifdef RENDERER_VULKAN
#include "volk.h"
//#define GLFW_INCLUDE_VULKAN
#endif // RENDERER_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define SOURCE_DIR std::string(CHIKU_SRC_PATH)
#define ASSET_REGISTRY SOURCE_DIR + std::string(STR(AssetRegistry.json)) 

//#define ENABLE_VALIDATION_LAYERS
#define DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT 1

#define MAX_UNIFORM_BUFFER_BINDINGS 20
#define MAX_SAMPLER_BINDINGS 20
#define MAX_TEXTURE_PER_MATERIAL 5
#define MAX_FRAMES_IN_FLIGHT 3
#define MAX_DESCRIPTOR_SETS 200
#define MAX_DESCRIPTOR_SET_LAYOUTS 1000

#define STR2(x) #x
#define STR(x) STR2(x)

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cstring>
#include <array>
#include <unordered_map>
#include <optional>
#include <memory>
#include <algorithm>
#include <bitset>

#define SHARED std::shared_ptr
#define WEAK std::weak_ptr
#define UNIQUE std::unique_ptr

#ifdef CHIKU_ENABLE_LOGGING
	#include "Logging/Logger.h"
#endif // CHIKU_ENABLE_LOGGING
