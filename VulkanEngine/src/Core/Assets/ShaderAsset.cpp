#include "ShaderAsset.h"

#ifdef RENDERER_VULKAN
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