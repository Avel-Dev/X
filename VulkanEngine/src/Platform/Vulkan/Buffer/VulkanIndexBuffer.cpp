#include "VulkanIndexBuffer.h"
#include "Vulkan/Renderer/VulkanRenderer.h"
#include "Vulkan/Utils/VulkanBufferUtils.h"

namespace CHIKU
{
    void VulkanIndexBuffer::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        ZoneScoped;

        count = (uint32_t)indices.size();
        VkDeviceSize bufferSize = sizeof(indices[0]) * count;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanRenderer::GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanRenderer::GetVulkanDevice(), stagingBufferMemory);

        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

        Utils::CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(VulkanRenderer::GetVulkanDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanRenderer::GetVulkanDevice(), stagingBufferMemory, nullptr);
    }

    void VulkanIndexBuffer::Bind() const
    {
        ZoneScoped;

        vkCmdBindIndexBuffer(VulkanRenderer::GetVulkanCommandBuffer(), m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanIndexBuffer::CleanUp()
    {
        ZoneScoped;

        vkDestroyBuffer(VulkanRenderer::GetVulkanDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(VulkanRenderer::GetVulkanDevice(), m_IndexBufferMemory, nullptr);
    }
}