#pragma once
#include "Vulkan/VulkanHeader.h"
#include "EngineHeader.h"

namespace CHIKU
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class Swapchain
	{
	public:
		void Init(GLFWwindow* window, const VkPhysicalDevice& physicalDevice,const VkDevice& logicalDevice,const VkSurfaceKHR& surface);
		void CleanUp();
		void RecreateSwapchain(GLFWwindow* window, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
		void CreateDepthResources(const VkPhysicalDevice& physicalDevice);
		VkResult AcquireNextImageInSwapchain(const VkDevice& device, const VkSemaphore& semaphore, uint32_t* pImageIndex);
		void BeginRenderPass(const VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void EndRenderPass(const VkCommandBuffer& commandBuffer);
		
		const VkSwapchainKHR& GetSwapchain() const { return m_SwapChain; }
		const VkRenderPass& GetRenderPass() const { return m_RenderPass; }

	private:
		SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
		VkFormat FindDepthFormat(const VkPhysicalDevice& physicalDevice);

		void CreateRenderpass(const VkPhysicalDevice& physicalDevice);
		void CreateSwapchain(GLFWwindow* window, const VkPhysicalDevice& physicalDevice,const VkSurfaceKHR& surface);
		void CreateImageViews();
		void CreateFrameBuffers();

	private:
		VkSwapchainKHR m_SwapChain;
		VkExtent2D m_SwapChainExtent;
		VkDevice m_LogicalDevice;
		VkRenderPass m_RenderPass;
		VkFormat m_SwapChainImageFormat;

		VkImage m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView m_DepthImageView;

		std::vector<VkFramebuffer> SwapChainFramebuffers;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
	};
}