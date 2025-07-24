#include "MeshAsset.h"
#include "Renderer/Renderer.h"
#ifdef RENDERER_VULKAN
#include "Vulkan/Assets/VulkanMeshAsset.h"
#endif // RENDERER_VULKAN


namespace CHIKU
{
	std::shared_ptr<MeshAsset> MeshAsset::Create()
	{
		return std::make_shared<VulkanMeshAsset>();
	}

	std::shared_ptr<MeshAsset> MeshAsset::Create(AssetHandle handle)
	{
		return std::make_shared<VulkanMeshAsset>(handle);
	}

	std::shared_ptr<MeshAsset> MeshAsset::Create(AssetHandle handle, AssetPath path)
	{
		return std::make_shared<VulkanMeshAsset>(handle,path);
	}
}