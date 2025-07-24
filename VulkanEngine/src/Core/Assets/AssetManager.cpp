#include "AssetManager.h"
#include "ModelAsset.h"
#include "MeshAsset.h"
#include "ShaderAsset.h"
#include "MaterialAsset.h"
#include "Utils/Utils.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace CHIKU
{
	std::unordered_map<AssetHandle, std::shared_ptr<Asset>> AssetManager::m_Assets;
	std::unordered_map<ReadableHandle,AssetHandle> AssetManager::m_Shaders;

	void AssetManager::Init()
	{
		ZoneScoped;
	}

	std::shared_ptr<Asset> AssetManager::LoadAsset(const AssetHandle& handle)
	{
		ZoneScoped;
		if (m_Assets.find(handle) != m_Assets.end())
		{
			return m_Assets[handle];
		}
		else
		{
			std::cerr << "Asset with handle " << handle << " not found!" << std::endl;
		}

		return nullptr;
	}

	AssetHandle AssetManager::AddModel(const AssetPath& path)
	{
		ZoneScoped;
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		m_Assets[newHandle] = std::make_shared<ModelAsset>( newHandle, path);

		return newHandle;
	}

	AssetHandle AssetManager::AddMesh(const VertexBufferMetaData& metaData ,const std::vector<uint8_t>& data, const std::vector<uint32_t>& indices)
	{
		ZoneScoped;

		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		std::shared_ptr<MeshAsset> meshAsset = MeshAsset::Create(newHandle);
		m_Assets[newHandle] = meshAsset;

		meshAsset->SetMetaData(metaData);
		meshAsset->SetData(data);

		if(!indices.empty())
			meshAsset->SetIndexData(indices);
		
		return newHandle;
	}

	AssetHandle AssetManager::AddMaterial(const AssetPath& path)
	{
		ZoneScoped;
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		m_Assets[newHandle] = MaterialAsset::Create(newHandle, path);

		return newHandle;
	}

	AssetHandle AssetManager::AddShader(const std::vector<AssetPath>& path)
	{
		ZoneScoped;
		AssetHandle newHandle = Utils::GetRandomNumber<AssetHandle>();
		std::shared_ptr<ShaderAsset> shaderAsset = ShaderAsset::Create(newHandle);
		shaderAsset->CreateShader(path);
		m_Assets[newHandle] = shaderAsset;

		if (m_Shaders.find(shaderAsset->GetShaderHandle()) != m_Shaders.end()) 
		{
			throw std::runtime_error("Shader already exists; new shader being created with a existing shader handle");
		}

		m_Shaders[shaderAsset->GetShaderHandle()] = newHandle;

		return newHandle;
	}

	void AssetManager::CleanUp()
	{
		ZoneScoped;
		for (auto& asset : m_Assets)
		{
			asset.second->CleanUp();
		}
		m_Assets.clear();
	}

	AssetHandle AssetManager::GetShaderAssetHandle(const ReadableHandle& shaderHandle) 
	{
		ZoneScoped;

		if (m_Shaders.find(shaderHandle) != m_Shaders.end())
		{
			return m_Shaders[shaderHandle];
		}
		
		return Asset::InvalidHandle;
	}

	std::shared_ptr<Asset> AssetManager::GetAsset(const AssetHandle& assetHandle)
	{
		ZoneScoped;

		auto it = m_Assets.find(assetHandle);
		if (assetHandle == Asset::InvalidHandle || it == m_Assets.end())
		{
			return nullptr;
		}
		return it->second;
	}
}