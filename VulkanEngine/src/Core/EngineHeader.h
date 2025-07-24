#pragma once

#define SOURCE_DIR std::string(STR(CHIKU_SRC_PATH))
#define ASSET_REGISTRY SOURCE_DIR + std::string(STR(AssetRegistry.json)) 

#define ENABLE_VALIDATION_LAYERS
#define DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT 1

#define MAX_UNIFORM_BUFFER_BINDINGS 20
#define MAX_SAMPLER_BINDINGS 20
#define MAX_TEXTURE_PER_MATERIAL 5
#define MAX_FRAMES_IN_FLIGHT 3
#define MAX_DESCRIPTOR_SETS 200
#define MAX_DESCRIPTOR_SET_LAYOUTS 1000

#define STR2(x) #x
#define STR(x) STR2(x)

#define RENDERER_VULKAN

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <tracy/tracy.hpp>
#include <tracy/tracyC.h>

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
