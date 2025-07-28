#include "EditorApplication.h"
#include "Renderer/Renderer.h"
#include "Assets/AssetManager.h"
#include "Logging/Logger.h"
#include "Renderer/GraphicsPipeline.h"

namespace Editor
{
	void EditorApplication::Init()
	{
		m_Window.SetTitle("Editor");
		m_Window.SetSize(1680, 945);

		//AssetManager::AddShader({ "Shaders/Unlit/unlit.vert", "Shaders/Unlit/unlit.frag" });
		CHIKU::AssetHandle model = CHIKU::AssetManager::AddModel("Models/Y Bot/Y Bot.gltf");

		SHARED<CHIKU::Asset> asset = CHIKU::AssetManager::GetAsset(model);
		m_Model = std::dynamic_pointer_cast<CHIKU::ModelAsset>(asset);
	}
}

namespace CHIKU
{
	Application* CreateApplication()
	{
		return new Editor::EditorApplication();
	}
}