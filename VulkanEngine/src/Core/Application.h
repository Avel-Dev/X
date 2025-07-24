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
		void Init();
		void Run();
		void Render();
		void CleanUp();

	private:
		Window m_Window;
		std::shared_ptr<Renderer> m_Renderer;
		std::shared_ptr<ModelAsset> m_Model;
	};
}