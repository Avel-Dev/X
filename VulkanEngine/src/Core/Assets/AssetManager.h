#pragma once
#include "EngineHeader.h"
#include "Asset.h"
#include <memory.h>

namespace CHIKU
{
	struct VertexBufferMetaData;

	class AssetManager
	{
	public:
		static std::shared_ptr<Asset> LoadAsset(const AssetHandle& assetHandle);
		static AssetHandle AddModel(const AssetPath& path);
		static AssetHandle AddMesh(const VertexBufferMetaData& metaData, const std::vector<uint8_t>& data, const std::vector<uint32_t>& indices);
		static AssetHandle AddMaterial(const AssetPath& path);
		static AssetHandle AddShader(const std::vector<AssetPath>& path);
		
		static void Init();
		static void CleanUp();

		static AssetHandle GetShaderAssetHandle(const ReadableHandle& assetHandle);	
		static std::shared_ptr<Asset> GetAsset(const AssetHandle& assetHandle);

	private:
		static std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_Assets;
		static std::unordered_map<ReadableHandle,AssetHandle> m_Shaders;
	};
}