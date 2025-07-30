#pragma once
#include "Window.h"

#define VULKAN_RENDERER "VulkanRenderer"
#define OPENXR_RENDERER "OpenXRVulkanRenderer"

namespace CHIKU
{
	struct RendererData
	{
		const char* type;
		GLFWwindow* window;
		std::vector<const char*> extensions;
	};

	class Renderer
	{
	public:
		Renderer() = default;

		virtual ~Renderer() = default;

		static void Init(RendererData* data) 
		{ 
			s_Instance = Renderer::Create(data);
			s_Instance->mInit(data);
		}

		static Renderer* Create(const RendererData* data);
		static void CleanUp() { s_Instance->mCleanUp(); }
		static void Wait() { s_Instance->mWait(); }

		static void BeginFrame() { s_Instance->mBeginFrame(); }
		static void EndFrame() { s_Instance->mEndFrame(); }
		static void RecreateSwapChain() { s_Instance->mRecreateSwapChain(); }

		static void* GetGraphicsBinding() { return s_Instance->mGetGraphicsBinding(); }
		static void* GetRenderPass() { return s_Instance->mGetRenderPass(); }
		static void* GetCommandBuffer() { return s_Instance->mGetCommandBuffer(); }
		static void* GetDevice() { return s_Instance->mGetDevice(); }
		static void* GetPhysicalDevice() { return s_Instance->mGetPhysicalDevice(); }
		static void* GetInstance() { return s_Instance->mGetInstance(); }

		static void AddExtions(const char** extensions, uint32_t count) { s_Instance->InsertExtension(extensions,count); }

		static void* BeginSingleTimeCommands() { return s_Instance->mBeginSingleTimeCommands(); }
		static void EndSingleTimeCommands(void* cmdBuffer) { return s_Instance->mEndSingleTimeCommands(cmdBuffer); }

	protected:
		void InsertExtension(const char** extensions, uint32_t count);

		virtual void mInit(RendererData* data) = 0;
		virtual void mCleanUp() = 0;
		virtual void mWait() = 0;

		virtual void mBeginFrame() = 0;
		virtual void mEndFrame() = 0;

		virtual void mRecreateSwapChain() = 0;

		virtual void* mGetGraphicsBinding() = 0;
		virtual void* mGetInstance() = 0;
		virtual void* mGetRenderPass() = 0;
		virtual void* mGetCommandBuffer() = 0;
		virtual void* mGetDevice() = 0;
		virtual void* mGetPhysicalDevice() = 0;

		virtual void* mBeginSingleTimeCommands() = 0;
		virtual void mEndSingleTimeCommands(void* cmdBuffer) = 0;

	protected:
		static Renderer* s_Instance;

		std::vector<const char*> m_Extension;
	};
}