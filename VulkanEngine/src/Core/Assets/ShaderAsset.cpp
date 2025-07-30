#include "ShaderAsset.h"

#if defined(RENDERER_VULKAN) || defined(REQUIRED_VR_VULKAN) 
#include "Vulkan/Assets/VulkanShaderAsset.h"
#endif // RENDERER_VULKAN

namespace CHIKU
{
	SHARED<ShaderAsset> ShaderAsset::Create()
	{
		ZoneScoped;
		return std::make_shared<VulkanShaderAsset>();
	}

	SHARED<ShaderAsset> ShaderAsset::Create(AssetHandle handle)
	{
		ZoneScoped;
		return std::make_shared<VulkanShaderAsset>(handle);
	}
}