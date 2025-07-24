#include "Vulkan/Renderer/VulkanRenderer.h"
#include "VulkanBufferUtils.h"
#include "VulkanRendererUtility.h"
#include <iostream>

namespace CHIKU
{
	namespace Utils
	{
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
            ZoneScoped;

			auto commandBuffer = VulkanRenderer::BeginRecordingSingleTimeCommands();
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			VulkanRenderer::EndRecordingSingleTimeCommands(commandBuffer);
		}

		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
            ZoneScoped;

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(VulkanRenderer::GetVulkanDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(VulkanRenderer::GetVulkanDevice(), buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = Utils::FindMemoryType(VulkanRenderer::GetVulkanPhysicalDevice(), memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(VulkanRenderer::GetVulkanDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate buffer memory!");
			}

			vkBindBufferMemory(VulkanRenderer::GetVulkanDevice(), buffer, bufferMemory, 0);
		}

        size_t GetAttributeSize(const VertexComponentType& componentType, const VertexAttributeType& type)
        {
            ZoneScoped;

            short byteSize = 0;

            switch (componentType)
            {
            case VertexComponentType::Float:    byteSize = 4; break;
            case VertexComponentType::Int:      byteSize = 4; break;
            case VertexComponentType::Byte:     byteSize = 1; break;
            case VertexComponentType::Short:    byteSize = 2; break;
            }

            switch (type)
            {
            case VertexAttributeType::SCALAR:      return byteSize;
            case VertexAttributeType::Vec2:      return byteSize * 2;
            case VertexAttributeType::Vec3:      return byteSize * 3;
            case VertexAttributeType::Vec4:      return byteSize * 4;
            }

			return -1; // Invalid type
        }

        void FinalizeLayout(VertexBufferLayout& layout)
        {
            ZoneScoped;

            uint32_t offset = 0;
            for (auto& attr : layout.VertexElements)
            {
                attr.Offset = offset;
                attr.size = static_cast<uint32_t>(GetAttributeSize(attr.ComponentType, attr.AttributeType));
                offset += attr.size;
            }
            layout.Stride = offset;
        }

        VkFormat GetVkFormat(const VertexComponentType& component, const VertexAttributeType& attribute)
        {
            ZoneScoped;

            using VCT = VertexComponentType;
            using VAT = VertexAttributeType;

            switch (component)
            {
            case VCT::Float:
                switch (attribute) {
                case VAT::SCALAR: return VK_FORMAT_R32_SFLOAT;
                case VAT::Vec2:   return VK_FORMAT_R32G32_SFLOAT;
                case VAT::Vec3:   return VK_FORMAT_R32G32B32_SFLOAT;
                case VAT::Vec4:   return VK_FORMAT_R32G32B32A32_SFLOAT;
                }
                break;

            case VCT::Int:
                switch (attribute) {
                case VAT::SCALAR: return VK_FORMAT_R32_SINT;
                case VAT::Vec2:   return VK_FORMAT_R32G32_SINT;
                case VAT::Vec3:   return VK_FORMAT_R32G32B32_SINT;
                case VAT::Vec4:   return VK_FORMAT_R32G32B32A32_SINT;
                }
                break;

            case VCT::Byte:
                if (attribute == VAT::Vec4)
                    return VK_FORMAT_R8G8B8A8_UNORM; // Common for vertex color
                break;

            case VCT::Short:
                switch (attribute) {
                case VAT::Vec2: return VK_FORMAT_R16G16_SINT;
                case VAT::Vec4: return VK_FORMAT_R16G16B16A16_SINT;
                }
                break;
            }

            // Unsupported combination
            return VK_FORMAT_UNDEFINED;
        }

	}
}