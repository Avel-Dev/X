#include "VulkanRenderer.h"
#include "DescriptorPool.h"
#include "Vulkan/Buffer/VulkanUniformBuffer.h"
#include "Vulkan/Buffer/VulkanVertexBuffer.h"
#include "Vulkan/Buffer/VulkanIndexBuffer.h"

#include <iostream>
#include <cstring>

namespace CHIKU
{
	uint32_t VulkanRenderer::m_CurrentFrame;
	QueueFamilyIndices VulkanRenderer::m_QueueFamilyIndices;

	VulkanRenderer::VulkanRenderer()
	{
		ZoneScoped;
		m_Window = nullptr;
		
		m_Instance = VK_NULL_HANDLE;
		m_Surface = VK_NULL_HANDLE;
		m_PhysicalDevice = VK_NULL_HANDLE;
		m_PipelineLayout = VK_NULL_HANDLE;
		m_GraphicsPipeline = VK_NULL_HANDLE;

		m_DebugMessenger = VK_NULL_HANDLE;
		m_LogicalDevice = VK_NULL_HANDLE;
		m_GraphicsQueue = VK_NULL_HANDLE;
		m_PresentQueue = VK_NULL_HANDLE;
	}

	void VulkanRenderer::mInit(RendererData* data)
	{
		ZoneScoped;
		m_Window = data->window;

		if (m_Window == nullptr)
		{
			LOG_ERROR("GLFWwindow is required for Vulkan Engine");
		}

		GetRequiredExtensions();
		CreateInstance();
		CreateSurface();
		CreatePhysicalDevice();
		CreateLogicalDevice();
		CreateSyncObjects();
		DescriptorPool::Init();

		m_Commands.Init(m_GraphicsQueue, m_LogicalDevice, m_PhysicalDevice, m_Surface);
		m_Swapchain.Init(m_Window, m_PhysicalDevice, m_LogicalDevice, m_Surface);
	}

	void VulkanRenderer::mCleanUp()
	{
		ZoneScoped;

		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphore[i], nullptr);
			vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphore[i], nullptr);

			vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFence[i], VK_TRUE, UINT64_MAX);
			vkDestroyFence(m_LogicalDevice, m_InFlightFence[i], nullptr);
		}

		vkDeviceWaitIdle(m_LogicalDevice);  // Or vkQueueWaitIdle(queue)

		DescriptorPool::CleanUp();
		m_Commands.CleanUp();
		m_Swapchain.CleanUp();
		vkQueueWaitIdle(m_GraphicsQueue);
		vkQueueWaitIdle(m_PresentQueue);
		vkDestroyDevice(m_LogicalDevice, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanRenderer::mWait()
	{
		ZoneScoped;

		vkQueueWaitIdle(m_GraphicsQueue);
		vkQueueWaitIdle(m_PresentQueue);
	}

	void VulkanRenderer::mRecreateSwapChain()
	{
		m_Swapchain.RecreateSwapchain(m_Window,m_PhysicalDevice,m_Surface);
	}

	void VulkanRenderer::mBeginFrame()
	{
		ZoneScoped;

		vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFence[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		VkResult result = m_Swapchain.AcquireNextImageInSwapchain(m_LogicalDevice, m_ImageAvailableSemaphore[m_CurrentFrame], &m_ImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_Swapchain.RecreateSwapchain(m_Window, m_PhysicalDevice, m_Surface);
		}
		else if (result != VK_SUCCESS)
		{
			LOG_ERROR("failed to present swap chain image!");
		}
		VkCommandBuffer commandBuffer = m_Commands.GetCommandBuffer(m_CurrentFrame);

		vkResetFences(m_LogicalDevice, 1, &m_InFlightFence[m_CurrentFrame]);
		vkResetCommandBuffer(commandBuffer, 0);
		BeginRecordingCommands(commandBuffer);
	}

	void VulkanRenderer::mEndFrame()
	{
		ZoneScoped;

		EndRecordingCommands(m_Commands.GetCommandBuffer(m_CurrentFrame));
		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore[m_CurrentFrame] };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_Commands.GetCommandBuffer(m_CurrentFrame);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(m_LogicalDevice, 1, &m_InFlightFence[m_CurrentFrame]);
		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence[m_CurrentFrame]) != VK_SUCCESS)
		{
			LOG_ERROR("failed to submit draw command buffer!");
		}

		VkSwapchainKHR swapChains[] = { m_Swapchain.GetSwapchain() };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(m_PresentQueue, &presentInfo);

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::BeginRecordingCommands(const VkCommandBuffer& commandBuffer)
	{
		ZoneScoped;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			LOG_ERROR("failed to begin recording command buffer!");
		}

		m_Swapchain.BeginRenderPass(commandBuffer, m_ImageIndex);
	}

	void VulkanRenderer::EndRecordingCommands(const VkCommandBuffer& commandBuffer)
	{
		ZoneScoped;

		m_Swapchain.EndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			LOG_ERROR("failed to record command buffer!");
		}
	}

	void VulkanRenderer::GetRequiredExtensions()
	{
		ZoneScoped;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		
		InsertExtension(glfwExtensions, glfwExtensionCount);
		
#ifdef ENABLE_VALIDATION_LAYERS
		auto ext = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
		InsertExtension(&ext, 1);
#endif
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		ZoneScoped;

		// Severity handling
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			LOG_ERROR(FMT_STRING("validation layer (ERROR): {}"), pCallbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			LOG_WARN(FMT_STRING("validation layer (WARNING): {}"), pCallbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			LOG_INFO(FMT_STRING("validation layer (INFO): {}"), pCallbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
			LOG_TRACE(FMT_STRING("validation layer (VERBOSE): {}"), pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	void VulkanRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		ZoneScoped;

		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	bool VulkanRenderer::CheckValidationLayerSupport()
	{
		ZoneScoped;

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	void VulkanRenderer::CreateInstance()
	{
		ZoneScoped;

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "NOGAME";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "CHIKU";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_Extension.size());
		createInfo.ppEnabledExtensionNames = m_Extension.data();
		createInfo.enabledLayerCount = 0;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#ifdef ENABLE_VALIDATION_LAYERS
		if (!CheckValidationLayerSupport())
		{
			LOG_ERROR("validation layers requested, but not available!");
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			LOG_ERROR("failed to create Vulkan instance!");
		}
	}

	void VulkanRenderer::CreateSurface()
	{
		ZoneScoped;

		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
		{
			LOG_ERROR("failed to create window surface!");
		}
	}

	void VulkanRenderer::CreatePhysicalDevice()
	{
		ZoneScoped;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			LOG_ERROR("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());
		// Selection logic can be added here

		for (const auto& device : devices)
		{
			if (Utils::IsDeviceSuitable(device, m_Surface, m_DeviceExtensions))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			LOG_ERROR("failed to find a suitable GPU!");
		}

		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int score = Utils::RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0)
		{
			m_PhysicalDevice = candidates.rbegin()->second;
			VkPhysicalDeviceProperties deviceProperties;

			vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
			std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
		}
		else
		{
			LOG_ERROR("failed to find a suitable GPU!");
		}
	}

	VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		ZoneScoped;

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		ZoneScoped;

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanRenderer::SetupDebugMessenger()
	{
		ZoneScoped;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			LOG_ERROR("failed to set up debug messenger!");
		}
	}

	void VulkanRenderer::CreateLogicalDevice()
	{
		ZoneScoped;

#ifdef ENABLE_VALIDATION_LAYERS
		SetupDebugMessenger();
#endif

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		float queuePriority = 1.0f;

		m_QueueFamilyIndices = Utils::FindQueueFamilies(m_PhysicalDevice, m_Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_QueueFamilyIndices.GraphicsFamily.value(), m_QueueFamilyIndices.PresentFamily.value() };

		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledLayerCount = 0;
		deviceFeatures.samplerAnisotropy = VK_TRUE;

#ifdef ENABLE_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#endif // ENABLE_VALIDATION_LAYERS


		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			LOG_ERROR("failed to create logical device!");
		}

		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.PresentFamily.value(), 0, &m_PresentQueue);
	}

	void VulkanRenderer::CreateSyncObjects()
	{
		ZoneScoped;

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		m_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFence[i]) != VK_SUCCESS)
			{
				LOG_ERROR("failed to create semaphores!");
			}
		}
	}
}