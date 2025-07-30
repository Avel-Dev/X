#include "EditorApplication.h"
#include "Renderer/Renderer.h"
#include "Assets/AssetManager.h"
#include "Logging/Logger.h"
#include "Renderer/GraphicsPipeline.h"

namespace Editor
{
	void EditorApplication::Init()
	{
		Application::Init();

		m_Window.SetTitle("Editor");
		m_Window.SetSize(1680, 945);
	}
}

namespace CHIKU
{
	Application* CreateApplication()
	{
		return new Editor::EditorApplication();
	}
}