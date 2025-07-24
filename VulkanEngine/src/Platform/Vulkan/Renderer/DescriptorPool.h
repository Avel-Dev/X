#pragma once
#include "Vulkan/VulkanHeader.h"

namespace CHIKU
{
	class DescriptorPool
	{
	public:
		static void Init();
		static void CleanUp();	

		static VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

	private:
		static VkDescriptorPool m_DescriptorPool;
	};
}