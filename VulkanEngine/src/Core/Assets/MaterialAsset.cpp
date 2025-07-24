#include "MaterialAsset.h"
#include "AssetManager.h"
#include <nlohmann/json.hpp>
#include <fstream>

#ifdef RENDERER_VULKAN
#include "Vulkan/Assets/VulkanMaterialAsset.h"
#endif // RENDERER_VULKAN



namespace CHIKU
{
    
    std::shared_ptr<MaterialAsset> MaterialAsset::Create()
    {
        ZoneScoped;
        return std::make_shared<VulkanMaterialAsset>();
	}

    std::shared_ptr<MaterialAsset> MaterialAsset::Create(AssetHandle handle)
    {
        ZoneScoped;
        return std::make_shared<VulkanMaterialAsset>(handle);
    }

    std::shared_ptr<MaterialAsset> MaterialAsset::Create(AssetHandle handle, AssetPath path)
    {
        ZoneScoped;
        return std::make_shared<VulkanMaterialAsset>(handle,path);
    }

    Material MaterialAsset::LoadMaterialFromFile(const AssetPath& filePath)
    {
        ZoneScoped;

        std::string filePathStr = SOURCE_DIR + filePath;
        std::ifstream file(filePathStr);
        if (!file)
        {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        nlohmann::json j;
        file >> j;

        Material mat;
        mat.name = j["name"];
        mat.shader = j["shader"];

        const auto& cfg = j["Config"];
        mat.config.depthTest = cfg["depthTest"];
        mat.config.depthWrite = cfg["depthWrite"];
        mat.config.blendEnabled = cfg["blendEnabled"];
        mat.config.cullMode = cfg["cullMode"];
        mat.config.frontFace = cfg["frontFace"];
        mat.config.polygonMode = cfg["polygonMode"];
        mat.config.topology = cfg["topology"];

        float r = j["baseColorFactor"][0];
        float g = j["baseColorFactor"][1];
        float b = j["baseColorFactor"][2];
        float w = j["baseColorFactor"][3];

		mat.config.baseColor = glm::vec4(r,g,b,w);

        return mat;
    }
}