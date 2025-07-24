#pragma once
#include "Renderer/Renderer.h"
#include <filesystem>

namespace CHIKU
{
	namespace Utils
	{
		void CreateImage( 
			uint32_t width, 
			uint32_t height, 
			VkFormat format, 
			VkImageTiling tiling, 
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkImage& image, 
			VkDeviceMemory& imageMemory);

		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		void CreateTextureImage(const std::string& texturePath, VkImage& textureImage, VkDeviceMemory& textureImageMemory);
		VkImageView CreateTextureImageView(VkImage textureImage);
		VkSampler CreateTextureSampler();
	}
}