#include "Application.h"
#include "Vulkan/Utils/VulkanShaderUtils.h"
#include "Assets/AssetManager.h"
#include "Renderer/GraphicsPipeline.h"

namespace CHIKU
{
	ApplicationData Application::s_Data;
	EventBus Application::m_EventBus;

	Application::Application()
	{
		m_Window.SetTitle("Chiku Editor");
		m_Window.SetSize(1280, 720);
		m_Window.Create();

#ifdef CHIKU_ENABLE_LOGGING
		Logger::Init("VulkanEngine");
#endif // CHIKU_ENABLE_LOGGING

		Renderer::Init(m_Window.GetWindow());
		AssetManager::Init();
		GraphicsPipeline::Init();

		s_Data.eventHandler = [this](Event& event) -> void
		{
			Publish(event);
		};

		CHIKU::AssetManager::AddShader({ "src/Shaders/Unlit/unlit.vert", "src/Shaders/Unlit/unlit.frag" });
		CHIKU::AssetManager::AddShader({ "src/Shaders/Defaultlit/defaultlit.vert", "src/Shaders/Defaultlit/defaultlit.frag" });
	}

	void Application::Init()
	{
		ZoneScoped;    // Profile this block
	}

	void Application::Run()
	{
		ZoneScoped;    // Profile this block
		while (!m_Window.WindowShouldClose())
		{
			FrameMark;
			m_Window.WindowPoolEvent();
		}
	}

	//Renderer Thread Process
	void Application::Render()
	{
		ZoneScoped;
		while (!m_Window.WindowShouldClose())
		{
			FrameMark;
			m_Window.WindowPoolEvent();
			if (s_Data.framebufferResized)
			{
				Renderer::RecreateSwapChain();
				s_Data.framebufferResized = false;
			}
			Renderer::BeginFrame();
			Renderer::EndFrame();
		}
	}

	void Application::CleanUp()
	{
		ZoneScoped;

		Renderer::Wait();
		AssetManager::CleanUp();
		GraphicsPipeline::s_Instance->CleanUp();
		Renderer::CleanUp();
	}
}