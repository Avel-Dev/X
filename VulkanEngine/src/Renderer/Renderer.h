#pragma once
#include "Window.h"

namespace CHIKU
{
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		static void Init(GLFWwindow* window) { s_Instance->mInit(window); }
		static void CleanUp() { s_Instance->mCleanUp(); }
		static void Wait() { s_Instance->mWait(); }

		static void BeginFrame() { s_Instance->mBeginFrame(); }
		static void EndFrame() { s_Instance->mEndFrame(); }

		static void* GetRenderPass() { return s_Instance->mGetRenderPass(); }
		static void* GetCommandBuffer() { return s_Instance->mGetCommandBuffer(); }
		static void* GetDevice() { return s_Instance->mGetDevice(); }
		static void* GetPhysicalDevice() { return s_Instance->mGetPhysicalDevice(); }

		static void* BeginSingleTimeCommands() { return s_Instance->mBeginSingleTimeCommands(); }
		static void EndSingleTimeCommands(void* cmdBuffer) { return s_Instance->mEndSingleTimeCommands(cmdBuffer); }

		static std::unique_ptr<Renderer> Create();

	protected:
		virtual void mInit(GLFWwindow* window) = 0;
		virtual void mCleanUp() = 0;
		virtual void mWait() = 0;

		virtual void mBeginFrame() = 0;
		virtual void mEndFrame() = 0;

		virtual void* mGetRenderPass() = 0;
		virtual void* mGetCommandBuffer() = 0;
		virtual void* mGetDevice() = 0;
		virtual void* mGetPhysicalDevice() = 0;

		virtual void* mBeginSingleTimeCommands() = 0;
		virtual void mEndSingleTimeCommands(void* cmdBuffer) = 0;

	protected:
		static std::unique_ptr<Renderer> s_Instance;
		GLFWwindow* m_Window = nullptr;
	};
}