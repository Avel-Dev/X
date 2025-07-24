#include "SpriteAsset.h"
#include "Vulkan/Utils/VulkanImageUtils.h"

namespace CHIKU
{
	void SpriteAsset::CreateTexture()
	{
		ZoneScoped;

		Utils::CreateTextureImage(m_SourcePath, m_TextureImage, m_TextureImageMemory);
		m_TextureImageView = Utils::CreateTextureImageView(m_TextureImage);
		m_TextureSampler = Utils::CreateTextureSampler();
	}
}