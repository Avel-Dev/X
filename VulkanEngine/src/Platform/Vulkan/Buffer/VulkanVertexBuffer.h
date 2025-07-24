#pragma once
#include "Renderer/Buffer/VertexBuffer.h"
#include "Vulkan/VulkanHeader.h"

namespace CHIKU
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        void CreateVertexBuffer(const std::vector<uint8_t>& vertices);
        void Bind() const;
        void CleanUp();

        void SetBinding(uint32_t binding) { m_Binding = binding; }
		virtual void SetMetaData(const VertexBufferMetaData& metaData) override
        {
            VertexBuffer::SetMetaData(metaData);

            PrepareBindingDescription();
            PrepareAttributeDescriptions();
        }

        inline VkVertexInputBindingDescription GetBindingDescription() const { return m_BindingDescription; }
        inline std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() const { return m_AttributeDescription; }

        inline VkDeviceMemory GetBufferMemory() const { return m_VertexBufferMemory; }

    private:
        void PrepareBindingDescription();
        void PrepareAttributeDescriptions();

    private:
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        VkVertexInputBindingDescription m_BindingDescription;
        std::vector<VkVertexInputAttributeDescription> m_AttributeDescription;
    };
}