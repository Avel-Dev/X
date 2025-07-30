#pragma once
#include "Asset.h"
#include "ShaderAsset.h"
#include <glm/glm.hpp>
#include "Vulkan/Buffer/VulkanUniformBuffer.h"

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

		SHARED<ShaderAsset> GetShader() const { return m_Shader; }

		virtual void CreateMaterial() = 0;
		virtual void CreateUniformBuffer() = 0;
		
		virtual Material LoadMaterialFromFile(const AssetPath& filePath) final;

		virtual void CleanUp() = 0;

		virtual ~MaterialAsset() = default;

		Material GetMaterial() const { return m_Material; }
		virtual void UpdateUniformBuffer(uint32_t currentFrame) = 0;

		static SHARED<MaterialAsset> Create();
		static SHARED<MaterialAsset> Create(AssetHandle handle);
		static SHARED<MaterialAsset> Create(AssetHandle handle, AssetPath path);

	protected:
		SHARED<ShaderAsset> m_Shader = nullptr;	
		std::map<uint32_t, UniformSetStorage> m_UniformSetStorage; //Key is the set Index
		Material m_Material;
	};
}