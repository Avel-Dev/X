#pragma once
#include "EngineHeader.h"
#include "Renderer/Renderer.h"
#include "Vulkan/Renderer/Swapchain.h"
#include "Vulkan/Renderer/Commands.h"
#include "Vulkan/Utils/VulkanRendererUtility.h"
#include "OpenXR.h"

namespace CHIKU
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool isComplete()
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer();

		void mInit(RendererData* data) override;
		void mCleanUp() override;
		void mWait() override;

		static uint32_t GetGraphicsQueueFamilyIndex() { return m_QueueFamilyIndices.GraphicsFamily.value(); }
		static uint32_t GetPresentQueueFamilyIndex() { return m_QueueFamilyIndices.PresentFamily.value(); }
		static VkInstance GetVulkanInstance() { return (VkInstance)s_Instance->GetInstance(); }	
		static VkDevice GetVulkanDevice() { return (VkDevice)s_Instance->GetDevice(); }
		static VkPhysicalDevice GetVulkanPhysicalDevice() { return (VkPhysicalDevice)s_Instance->GetPhysicalDevice(); }
		static VkCommandBuffer GetVulkanCommandBuffer() { return (VkCommandBuffer)s_Instance->GetCommandBuffer(); }
		static VkRenderPass GetVulkanRenderPass() { return (VkRenderPass)s_Instance->GetRenderPass(); }

		static const inline uint32_t GetCurrentFrame() noexcept { return m_CurrentFrame; }
		static const inline VkCommandBuffer BeginRecordingSingleTimeCommands() noexcept { return (VkCommandBuffer)s_Instance->BeginSingleTimeCommands(); }
		static const inline void EndRecordingSingleTimeCommands(VkCommandBuffer commandBuffer) noexcept { return s_Instance->EndSingleTimeCommands(commandBuffer); }

	private:

		void LoadPFN_XrFunctions()
		{
			auto m_xrInstance = OpenXR::GetInstance();
			OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsRequirementsKHR.");
			OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanInstanceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanInstanceExtensionsKHR.");
			OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanDeviceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanDeviceExtensionsKHR.");
			OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsDeviceKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsDeviceKHR.");
		}

		std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId)
		{
			uint32_t extensionNamesSize = 0;
			OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

			std::vector<char> extensionNames(extensionNamesSize);
			OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

			std::stringstream streamData(extensionNames.data());
			std::vector<std::string> extensions;
			std::string extension;
			while (std::getline(streamData, extension, ' ')) {
				extensions.push_back(extension);
			}
			return extensions;
		}

		virtual void* mGetGraphicsBinding() override;
		virtual void* mGetInstance() override { return m_Instance; }
		virtual void* mGetRenderPass() override { return m_Swapchain.GetRenderPass(); }
		virtual void* mGetCommandBuffer() override { return m_Commands.GetCommandBuffer(m_CurrentFrame); }
		virtual void* mGetDevice() override { return m_LogicalDevice; }
		virtual void* mGetPhysicalDevice() override { return m_PhysicalDevice; }
		
		virtual void mRecreateSwapChain() override;
		virtual void* mBeginSingleTimeCommands() override { return m_Commands.BeginSingleTimeCommands(); }
		virtual void mEndSingleTimeCommands(void* commandBuffer) override { m_Commands.EndSingleTimeCommands((VkCommandBuffer)commandBuffer); }

	private:
		void mBeginFrame();
		void mEndFrame();

		void BeginRecordingCommands(const VkCommandBuffer& commandBuffer);
		void EndRecordingCommands(const VkCommandBuffer& commandBuffer);
		
		void GetRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		std::vector<std::string> GetDeviceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId);

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool CheckValidationLayerSupport();

		void CreateInstance();
		void CreateSurface();
		void CreatePhysicalDevice();

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);

		void SetupDebugMessenger();
		void CreateLogicalDevice();
		void CreateSyncObjects();

		void CreateGraphicsBinding();

	private:
		static uint32_t m_CurrentFrame;
		static QueueFamilyIndices m_QueueFamilyIndices;

		std::vector<const char*> activeInstanceLayers{};
		std::vector<const char*> activeInstanceExtensions{};
		std::vector<const char*> activeDeviceLayer{};
		std::vector<const char*> activeDeviceExtensions{};

		uint32_t queueFamilyIndex = 0xFFFFFFFF;
		uint32_t queueIndex = 0xFFFFFFFF;
		VkQueue queue{};
		VkFence fence{};

		VkCommandPool cmdPool{};
		VkCommandBuffer cmdBuffer{};
		VkDescriptorPool descriptorPool;

		GLFWwindow* m_Window = nullptr;

		std::vector<VkSemaphore> m_ImageAvailableSemaphore;
		std::vector<VkSemaphore> m_RenderFinishedSemaphore;
		std::vector<VkFence> m_InFlightFence;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice;
		Commands m_Commands;

		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkDevice m_LogicalDevice;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		Swapchain m_Swapchain;
		uint32_t m_ImageIndex = 0;

		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR;
		PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR;
		PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR;
		PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR;
		XrGraphicsBindingVulkanKHR m_GraphicsBinding;
	};
}