#include "VulkanRenderer.h"
#include "DescriptorPool.h"
#include <Vulkan/Buffer/VulkanUniformBuffer.h>
#include <Vulkan/Buffer/VulkanVertexBuffer.h>
#include <Vulkan/Buffer/VulkanIndexBuffer.h>
#include <Vulkan/Utils/OpenXRUtils/OpenXRUtils.h>
#include <iostream>
#include <cstring>


#define VULKAN_CHECK(x, y)                                                                         \
    {                                                                                              \
        VkResult result = (x);                                                                     \
        if (result != VK_SUCCESS) {                                                                \
            std::cout << "ERROR: VULKAN: " << std::hex << "0x" << result << std::dec << std::endl; \
            std::cout << "ERROR: VULKAN: " << y << std::endl;                                      \
        }                                                                                          \
    }


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

		auto m_xrInstance = OpenXR::GetInstance();
		auto systemId = OpenXR::GetSystemID();

		CreateInstance();
		CreateSurface();
		CreatePhysicalDevice();
		CreateLogicalDevice();
		CreateDeviceQueue();

		CreateSyncObjects();
		DescriptorPool::Init();

		m_Commands.Init(m_GraphicsQueue, m_LogicalDevice, m_PhysicalDevice, m_Surface);
		m_Swapchain.Init(m_Window, m_PhysicalDevice, m_LogicalDevice, m_Surface);
		CreateGraphicsBinding();
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

	void* VulkanRenderer::mGetGraphicsBinding()
	{
		return &m_GraphicsBinding;
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
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "NOGAME";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "CHIKU";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = OpenXR::GetAPIVersion();

		std::vector<std::string> OpenXRStringExt = OpenXR::GetVulkanRequiredExtensions();
		std::vector<const char*> OpenXRExt;

		for (const auto& ext : OpenXRStringExt)
		{
			OpenXRExt.push_back(ext.c_str());  // Safe: ext refers to elements in OpenXRStringExt which are still alive
		}

		InsertExtension(OpenXRExt.data(), OpenXRExt.size());

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = static_cast<uint32_t>(activeInstanceLayers.size());
		createInfo.ppEnabledLayerNames = activeInstanceLayers.data();
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
		if (deviceCount == 0) { LOG_ERROR("failed to find GPUs with Vulkan support!"); }
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		auto OpenXRDevice = OpenXR::GetXRPhysicalDevice();
		auto physicalDeviceFromXR_it = std::find(devices.begin(), devices.end(), OpenXRDevice);

		if (physicalDeviceFromXR_it != devices.end()) {
			m_PhysicalDevice = *physicalDeviceFromXR_it;
		}
		else {
			std::cout << "ERROR: Vulkan: Failed to find PhysicalDevice for OpenXR." << std::endl;
			// Select the first available device.
			m_PhysicalDevice = devices[0];
		}

		if (!Utils::IsDeviceSuitable(m_PhysicalDevice, m_Surface, m_DeviceExtensions))
		{
			LOG_ERROR("Device Not suitable with Surface and extensions!");
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			LOG_ERROR("failed to find a suitable GPU!");
		}

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
		std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
	}

	void VulkanRenderer::CreateDeviceQueue()
	{
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.QueueFamilyIndicesArray[GRAPHICS_FAMILY].value(), GRAPHICS_FAMILY, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.QueueFamilyIndicesArray[PRESENT_FAMILY].value(), PRESENT_FAMILY, &m_PresentQueue);
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

//#ifdef ENABLE_VALIDATION_LAYERS
//		SetupDebugMessenger();
//#endif

		// Device
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		uint32_t queueFamilyPropertiesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertiesCount, nullptr);
		queueFamilyProperties.resize(queueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCIs;
		std::vector<std::vector<float>> queuePriorities;
		queuePriorities.resize(queueFamilyProperties.size());
		deviceQueueCIs.resize(queueFamilyProperties.size());
		for (size_t i = 0; i < deviceQueueCIs.size(); i++) {
			for (size_t j = 0; j < queueFamilyProperties[i].queueCount; j++)
				queuePriorities[i].push_back(1.0f);

			deviceQueueCIs[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCIs[i].pNext = nullptr;
			deviceQueueCIs[i].flags = 0;
			deviceQueueCIs[i].queueFamilyIndex = static_cast<uint32_t>(i);
			deviceQueueCIs[i].queueCount = queueFamilyProperties[i].queueCount;
			deviceQueueCIs[i].pQueuePriorities = queuePriorities[i].data();

			if (!m_QueueFamilyIndices.QueueFamilyIndicesArray[GRAPHICS_FAMILY].has_value() && Utils::BitwiseCheck(queueFamilyProperties[i].queueFlags, VkQueueFlags(VK_QUEUE_GRAPHICS_BIT))) 
			{
				m_QueueFamilyIndices.QueueFamilyIndicesArray[GRAPHICS_FAMILY] = static_cast<uint32_t>(i);
			}

			if (!m_QueueFamilyIndices.QueueFamilyIndicesArray[PRESENT_FAMILY].has_value())
			{
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupport);

				if (presentSupport) {
					m_QueueFamilyIndices.QueueFamilyIndicesArray[PRESENT_FAMILY] = static_cast<uint32_t>(i);
				}
			}
		}

		uint32_t deviceExtensionCount = 0;
		VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, 0, &deviceExtensionCount, 0), "Failed to enumerate DeviceExtensionProperties.");
		std::vector<VkExtensionProperties> deviceExtensionProperties;
		deviceExtensionProperties.resize(deviceExtensionCount);

		VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, 0, &deviceExtensionCount, deviceExtensionProperties.data()), "Failed to enumerate DeviceExtensionProperties.");
		const std::vector<std::string>& openXrDeviceExtensionNames = OpenXR::GetDeviceExtensionsForOpenXR();
		for (const std::string& requestExtension : openXrDeviceExtensionNames) {
			for (const VkExtensionProperties& extensionProperty : deviceExtensionProperties) {
				if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
					continue;
				else
					activeDeviceExtensions.push_back(requestExtension.c_str());
				break;
			}
		}

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &features);

		VkDeviceCreateInfo deviceCI;
		deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCI.pNext = nullptr;
		deviceCI.flags = 0;
		deviceCI.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCIs.size());
		deviceCI.pQueueCreateInfos = deviceQueueCIs.data();
		deviceCI.enabledLayerCount = 0;
		deviceCI.ppEnabledLayerNames = nullptr;
		deviceCI.enabledExtensionCount = static_cast<uint32_t>(activeDeviceExtensions.size());
		deviceCI.ppEnabledExtensionNames = activeDeviceExtensions.data();
		deviceCI.pEnabledFeatures = &features;
		VULKAN_CHECK(vkCreateDevice(m_PhysicalDevice, &deviceCI, nullptr, &m_LogicalDevice), "Failed to create Device.");

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

	void VulkanRenderer::CreateGraphicsBinding()
	{
		m_GraphicsBinding = { XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR };
		m_GraphicsBinding.instance = m_Instance;
		m_GraphicsBinding.physicalDevice = m_PhysicalDevice;
		m_GraphicsBinding.device = m_LogicalDevice;
		m_GraphicsBinding.queueFamilyIndex = m_QueueFamilyIndices.QueueFamilyIndicesArray[GRAPHICS_FAMILY].value();
		m_GraphicsBinding.queueIndex = GRAPHICS_FAMILY;
	}
}