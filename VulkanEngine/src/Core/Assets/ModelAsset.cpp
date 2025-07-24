#include "ModelAsset.h"
#include "AssetManager.h"
#include "Vulkan/Utils/VulkanModelUtils.h"
#include "Renderer/GraphicsPipeline.h"

#include <nlohmann/json.hpp>
#include <iostream>

namespace CHIKU
{
	bool ModelAsset::LoadModel(const AssetPath& path)
	{
        ZoneScoped;

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;
		
		AssetPath newPath = Utils::ConvertToGLTF(SOURCE_DIR + path);

		bool ok = loader.LoadASCIIFromFile(&model, &err, &warn, newPath);
		if (!warn.empty()) std::cout << "Warn: " << warn << "\n";
		if (!err.empty()) std::cerr << "Err: " << err << "\n";

        std::filesystem::path inputPath = newPath;

		Utils::ProcessModel(model, m_MeshesMaterials);

		for (const auto& [meshHandle, materialHandle] : m_MeshesMaterials)
		{
			const auto& meshAsset = std::dynamic_pointer_cast<MeshAsset>(AssetManager::GetAsset(meshHandle));

			m_MeshesMaterialsAssets[meshAsset] = std::dynamic_pointer_cast<MaterialAsset>(AssetManager::GetAsset(materialHandle));
		}

        return ok;
	}

	void ModelAsset::Draw() const
	{
		ZoneScoped;
		for (const auto& [mesh, material] : m_MeshesMaterialsAssets)
		{
			GraphicsPipeline::s_Instance->BindPipeline(material, mesh);

			mesh->Bind();
			mesh->Draw();
		}
	}
}