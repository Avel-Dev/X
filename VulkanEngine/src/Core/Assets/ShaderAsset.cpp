#include "ShaderAsset.h"

#ifdef RENDERER_VULKAN
#include "Vulkan/Assets/VulkanShaderAsset.h"
#endif // RENDERER_VULKAN

namespace CHIKU
{
	std::shared_ptr<ShaderAsset> ShaderAsset::Create()
	{
		ZoneScoped;
		return std::make_shared<VulkanShaderAsset>();
	}

	std::shared_ptr<ShaderAsset> ShaderAsset::Create(AssetHandle handle)
	{
		ZoneScoped;
		return std::make_shared<VulkanShaderAsset>(handle);
	}
}