#include "Renderer/Renderer.h"
#include "Vulkan/Renderer/Swapchain.h"
#include "Vulkan/Renderer/Commands.h"
#include "Vulkan/VulkanHeader.h"

namespace CHIKU
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer();

		void Init(GLFWwindow* window) override;
		void CleanUp() override;
		void Wait() override;

		static VkDevice GetVulkanDevice() { return (VkDevice)s_Instance->GetDevice(); }
		static VkPhysicalDevice GetVulkanPhysicalDevice() { return (VkPhysicalDevice)s_Instance->GetPhysicalDevice(); }
		static VkCommandBuffer GetVulkanCommandBuffer() { return (VkCommandBuffer)s_Instance->GetCommandBuffer(); }
		static VkRenderPass GetVulkanRenderPass() { return (VkRenderPass)s_Instance->GetRenderPass(); }

		virtual void BeginFrame() override { s_Instance->PrivateBeginFrame(); }
		virtual void EndFrame() override { s_Instance->PrivateEndFrame(); }

		virtual void* GetRenderPass() override { return s_Instance->m_Swapchain.GetRenderPass(); }
		virtual void* GetCommandBuffer() override { return s_Instance->m_Commands.GetCommandBuffer(s_Instance->m_CurrentFrame); }
		virtual void* GetDevice() override { return s_Instance->m_LogicalDevice; }
		virtual void* GetPhysicalDevice() override { return s_Instance->m_PhysicalDevice; }
		
		virtual void* BeginSingleTimeCommands() override { return m_Commands.BeginSingleTimeCommands(); }
		virtual void EndSingleTimeCommands(void* commandBuffer) override { m_Commands.EndSingleTimeCommands((VkCommandBuffer)commandBuffer); }

		static const inline uint32_t GetCurrentFrame() noexcept { return s_Instance->m_CurrentFrame; }
		static const inline VkCommandBuffer BeginRecordingSingleTimeCommands() noexcept { return (VkCommandBuffer)s_Instance->BeginSingleTimeCommands(); }
		static const inline void EndRecordingSingleTimeCommands(VkCommandBuffer commandBuffer) noexcept { return s_Instance->EndSingleTimeCommands(commandBuffer); }

	private:
		void PrivateBeginFrame();
		void PrivateEndFrame();

		void BeginRecordingCommands(const VkCommandBuffer& commandBuffer);
		void EndRecordingCommands(const VkCommandBuffer& commandBuffer);
		
		void GetRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

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

	private:
		static VulkanRenderer* s_Instance;
		static uint32_t m_CurrentFrame;

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

		std::vector<const char*> m_Extension;

		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}