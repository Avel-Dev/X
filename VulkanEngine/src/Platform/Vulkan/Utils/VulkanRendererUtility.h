#pragma once
#include "EngineHeader.h"
#include <optional>

namespace CHIKU
{
	struct QueueFamilyIndices;
	
	namespace Utils
	{
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,uint32_t typeFilter, VkMemoryPropertyFlags properties);
		CHIKU::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

		int RateDeviceSuitability(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
	}
}