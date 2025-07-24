#pragma once
#include "Renderer/Buffer/UniformBuffer.h"
#include "Vulkan/VulkanHeader.h"

namespace CHIKU
{
	struct TextureData
    {
        VkImage TextureImage;
        VkImageView TextureImageView;
        VkDeviceMemory TextureImageMemory;
        VkSampler TextureSampler;
    };

    struct UniformBufferStorage
    {
        std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
        std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
        std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
    };

    struct UniformSetStorage
    {
        // key is the binding Index
        std::unordered_map<uint32_t, UniformBufferStorage> BindingStorage;
        VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
        std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
    };

    struct UniformBufferDescriptorInfo
    {
        VkDescriptorSetLayout DescriptorSetLayouts;
        std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
        std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> UniformBuffersMemory;
        std::array<void*, MAX_FRAMES_IN_FLIGHT> UniformBuffersMapped;
        std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
    };
}