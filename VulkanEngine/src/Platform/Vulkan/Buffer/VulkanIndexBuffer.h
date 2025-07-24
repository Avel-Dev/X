#pragma once
#include "Vulkan/VulkanHeader.h"
#include "Renderer/Buffer/IndexBuffer.h"

namespace CHIKU
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		virtual void CreateIndexBuffer(const std::vector<uint32_t>& indices);
		virtual void Bind() const;
		virtual void CleanUp();

	private:
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
	};
}