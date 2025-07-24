#include "DescriptorPool.h"
#include "Vulkan/Renderer/VulkanRenderer.h"

namespace CHIKU
{
    VkDescriptorPool DescriptorPool::m_DescriptorPool = VK_NULL_HANDLE;

	void DescriptorPool::Init()
    {
        ZoneScoped;

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = MAX_UNIFORM_BUFFER_BINDINGS;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = MAX_SAMPLER_BINDINGS;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = MAX_DESCRIPTOR_SETS;

        if (vkCreateDescriptorPool(VulkanRenderer::GetVulkanDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void DescriptorPool::CleanUp()
    {
        ZoneScoped;
        if (m_DescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(VulkanRenderer::GetVulkanDevice(), m_DescriptorPool, nullptr);
            m_DescriptorPool = VK_NULL_HANDLE;
        }
	}
}