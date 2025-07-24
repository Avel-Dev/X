#pragma once
#include "EngineHeader.h"

namespace CHIKU
{
	class Commands
	{
	public:
		void Init(VkQueue graphicsQueue, VkDevice device, const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface);
		void CleanUp();

		const inline std::vector<VkCommandBuffer>& GetCommandBuffers() const { return m_CommandBuffers; }
		const inline VkCommandBuffer& GetCommandBuffer(const uint32_t& index) const { return m_CommandBuffers[index]; }

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	private:
		VkQueue m_GraphicsQueue;
		VkDevice m_LogicalDevice;
		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		void CreateCommandBuffer();
		void CreateCommandPool(const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface);
	};
}