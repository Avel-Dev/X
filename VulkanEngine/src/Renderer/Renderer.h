#pragma once
#include "Window.h"

namespace CHIKU
{
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		virtual void Init(GLFWwindow* window) = 0;
		virtual void CleanUp() = 0;
		virtual void Wait() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void* GetRenderPass() = 0;
		virtual void* GetCommandBuffer() = 0;
		virtual void* GetDevice() = 0;
		virtual void* GetPhysicalDevice() = 0;

		virtual void* BeginSingleTimeCommands() = 0;
		virtual void EndSingleTimeCommands(void* cmdBuffer) = 0;

		static std::shared_ptr<Renderer> Create();
	protected:
		GLFWwindow* m_Window = nullptr;
	};
}