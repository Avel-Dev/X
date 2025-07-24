#include "VulkanVertexBuffer.h"
#include "Vulkan/Renderer/VulkanRenderer.h"
#include "Vulkan/Utils/VulkanBufferUtils.h"

namespace CHIKU
{
    void VulkanVertexBuffer::CreateVertexBuffer(const std::vector<uint8_t>& vertices)
    {
        ZoneScoped;

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        Utils::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanRenderer::GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanRenderer::GetVulkanDevice(), stagingBufferMemory);

        Utils::CreateBuffer(bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_VertexBuffer, m_VertexBufferMemory);

        Utils::CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(VulkanRenderer::GetVulkanDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanRenderer::GetVulkanDevice(), stagingBufferMemory, nullptr);
    }

    void VulkanVertexBuffer::Bind() const
    {
        ZoneScoped;

        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(VulkanRenderer::GetVulkanCommandBuffer(), 0, 1, vertexBuffers, offsets);
    }

    void VulkanVertexBuffer::CleanUp()
    {
        ZoneScoped;

        vkDestroyBuffer(VulkanRenderer::GetVulkanDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(VulkanRenderer::GetVulkanDevice(), m_VertexBufferMemory, nullptr);
    }

    void VulkanVertexBuffer::PrepareBindingDescription()
    {
        ZoneScoped;

        m_BindingDescription = {};
        m_BindingDescription.binding = m_Binding;
        m_BindingDescription.stride = m_MetaData.Layout.Stride;
        m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    void VulkanVertexBuffer::PrepareAttributeDescriptions()
    {
        ZoneScoped;

        const auto& layout = m_MetaData.Layout;

        m_AttributeDescription.clear();
        m_AttributeDescription.resize(layout.VertexElements.size());

        for (int i = 0; i < m_AttributeDescription.size(); i++)
        {
            const auto& componentType = layout.VertexElements[i].ComponentType;
            const auto& attributeType = layout.VertexElements[i].AttributeType;
            const auto& offset = layout.VertexElements[i].Offset;

            m_AttributeDescription[i].binding = 0; //Because we have interleaved vertex data, we use binding 0
            //Binding 0 is used when we use one vertex buffer with interleaved data
            m_AttributeDescription[i].location = i;
            m_AttributeDescription[i].format = Utils::GetVkFormat(componentType, attributeType);
            m_AttributeDescription[i].offset = offset;
        }
    }
}