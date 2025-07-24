#pragma once
#include "Vulkan/VulkanHeader.h"
#include "Renderer/Buffer/VertexBuffer.h"

namespace CHIKU
{
	namespace Utils
	{
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        
        size_t GetAttributeSize(const VertexComponentType& componentType,const VertexAttributeType& type);
        void FinalizeLayout(VertexBufferLayout& layout);
		VkFormat GetVkFormat(const VertexComponentType& component, const VertexAttributeType& attribute);
	}
}