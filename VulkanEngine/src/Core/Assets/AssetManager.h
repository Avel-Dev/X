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
		static SHARED<Asset> LoadAsset(const AssetHandle& assetHandle);
		static AssetHandle AddModel(const AssetPath& path);
		static AssetHandle AddMesh(const VertexBufferMetaData& metaData, const std::vector<uint8_t>& data, const std::vector<uint32_t>& indices);
		static AssetHandle AddMaterial(const AssetPath& path);
		static AssetHandle AddShader(const std::vector<AssetPath>& path);
		
		static void Init();
		static void CleanUp();

		static AssetHandle GetShaderAssetHandle(const ReadableHandle& assetHandle);	
		static SHARED<Asset> GetAsset(const AssetHandle& assetHandle);

	private:
		static std::unordered_map<AssetHandle, SHARED<Asset>> m_Assets;
		static std::unordered_map<ReadableHandle,AssetHandle> m_Shaders;
	};
}