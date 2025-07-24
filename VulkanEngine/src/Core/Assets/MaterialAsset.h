#pragma once
#include "Asset.h"
#include "ShaderAsset.h"
#ifdef  RENDERER_VULKAN
#include "Vulkan/Buffer/VulkanUniformBuffer.h"
#endif //  RENDERER_VULKAN

#include <glm/glm.hpp>

namespace CHIKU
{
	struct Config
	{
		bool depthTest;
		bool depthWrite;
		bool blendEnabled;
		std::string cullMode;
		std::string frontFace;
		std::string polygonMode;
		std::string topology;
		glm::vec4 baseColor;
	};

	struct Material
	{
		ReadableHandle name;
		ReadableHandle shader;
		Config config;
	};

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() : Asset(AssetType::Material) {}
		MaterialAsset(AssetHandle handle) : Asset(handle, AssetType::Material) {}
		MaterialAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Material, path) {}

		std::shared_ptr<ShaderAsset> GetShader() const { return m_Shader; }

		virtual void CreateMaterial() = 0;
		virtual void CreateUniformBuffer() = 0;
		
		virtual Material LoadMaterialFromFile(const AssetPath& filePath) final;

		virtual void CleanUp() = 0;

		virtual ~MaterialAsset() = default;

		Material GetMaterial() const { return m_Material; }
		virtual void UpdateUniformBuffer(uint32_t currentFrame) = 0;

		static std::shared_ptr<MaterialAsset> Create();
		static std::shared_ptr<MaterialAsset> Create(AssetHandle handle);
		static std::shared_ptr<MaterialAsset> Create(AssetHandle handle, AssetPath path);

	protected:
		std::shared_ptr<ShaderAsset> m_Shader = nullptr;	
		std::map<uint32_t, UniformSetStorage> m_UniformSetStorage; //Key is the set Index
		Material m_Material;
	};
}