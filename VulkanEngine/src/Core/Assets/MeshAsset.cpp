#include "MeshAsset.h"
#include "Renderer/Renderer.h"
#ifdef RENDERER_VULKAN
#include "Vulkan/Assets/VulkanMeshAsset.h"
#endif // RENDERER_VULKAN


namespace CHIKU
{
	SHARED<MeshAsset> MeshAsset::Create()
	{
		return std::make_shared<VulkanMeshAsset>();
	}

	SHARED<MeshAsset> MeshAsset::Create(AssetHandle handle)
	{
		return std::make_shared<VulkanMeshAsset>(handle);
	}

	SHARED<MeshAsset> MeshAsset::Create(AssetHandle handle, AssetPath path)
	{
		return std::make_shared<VulkanMeshAsset>(handle,path);
	}
}