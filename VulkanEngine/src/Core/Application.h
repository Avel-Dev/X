#pragma once
#include "Renderer/Renderer.h"
#include "Renderer/Window.h"
#include "Assets/ModelAsset.h"

namespace CHIKU
{
	class Application
	{
	public:
		Application() = default;

		virtual void Init();
		virtual void Run();
		virtual void Render();
		virtual void CleanUp();

		virtual void RunLoopBody();
		virtual void RenderLoopBody();

	protected:
		Window m_Window;

		bool m_applicationRunning = true;
		bool m_sessionRunning = false;
		std::shared_ptr<ModelAsset> m_Model;
	};

	Application* CreateApplication();
}