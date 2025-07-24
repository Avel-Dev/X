#pragma once
#include "Assets/MeshAsset.h"

namespace CHIKU
{
	class VulkanMeshAsset : public MeshAsset
	{
	public:
		VulkanMeshAsset() : MeshAsset() {}
		VulkanMeshAsset(AssetHandle handle) : MeshAsset(handle) {}
		VulkanMeshAsset(AssetHandle handle, AssetPath path) : MeshAsset(handle, path) {}

		virtual void Draw() const override;
	};
}