#pragma once
#include "Asset.h"
#include "EngineHeader.h"

namespace CHIKU
{
	class SpriteAsset : public Asset
	{
	public:
		SpriteAsset() : Asset(AssetType::Texture2D) {}
		SpriteAsset(AssetHandle handle) : Asset(handle, AssetType::Texture2D) {}
		SpriteAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Texture2D, path) {}

	private:
		void CreateTexture();

	private:
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
	};
}