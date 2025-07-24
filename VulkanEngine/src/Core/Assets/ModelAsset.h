#pragma once
#include "MeshAsset.h"
#include "MaterialAsset.h"
#include <unordered_map>

namespace CHIKU
{
	class ModelAsset : public Asset
	{
	public:
		ModelAsset() : Asset(AssetType::Model) {}
		ModelAsset(AssetHandle handle) : Asset(handle,AssetType::Model) {}
		ModelAsset(AssetHandle handle, AssetPath path) : Asset(handle,AssetType::Model,path) 
		{
			LoadModel(path);
		}

		bool LoadModel(const AssetPath& path);
		void Draw() const;

	private:
		//The meshes inside the model and the materils for each mesh
		std::unordered_map<AssetHandle, AssetHandle> m_MeshesMaterials;

		std::unordered_map<std::shared_ptr<MeshAsset>, std::shared_ptr<MaterialAsset>> m_MeshesMaterialsAssets;
	};
}