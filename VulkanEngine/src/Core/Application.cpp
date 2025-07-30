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
	}

	void Application::Init()
	{
		ZoneScoped;    // Profile this block
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

		AssetManager::AddShader({ "src/Shaders/Unlit/unlit.vert", "src/Shaders/Unlit/unlit.frag" });
		AssetManager::AddShader({ "src/Shaders/Defaultlit/defaultlit.vert", "src/Shaders/Defaultlit/defaultlit.frag" });

		auto assetHandle = AssetManager::AddModel({ "Models/Y Bot/Y Bot.gltf" });
		m_Model = std::dynamic_pointer_cast<ModelAsset>(AssetManager::GetAsset(assetHandle));
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
			m_Model->Draw();
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
#ifdef CHIKU_ENABLE_LOGGING
		Logger::Shutdown();
#endif
	}
}