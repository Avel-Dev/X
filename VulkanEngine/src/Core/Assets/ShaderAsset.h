#pragma once
#include "Asset.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer/UniformBuffer.h"
#include "Renderer/Buffer/VertexBuffer.h"
#include "Vulkan/Assets/VulkanShaderStagesData.h"

#define SHADER_STAGE_VERTEX "Vertex"
#define SHADER_STAGE_TESSELATION "Tesselation"
#define SHADER_STAGE_GEOMETRY "Geometry"
#define SHADER_STAGE_FRAGMENT "Fragment"
#define SHADER_STAGE_COMPUTE "Compute"

#define SHADER_LIT "Lit"
#define SHADER_UNLIT "Unlit"
#define SHADER_DEFAULT_LIT "Defaultlit"
#define SHADER_PBR "PBR"
#define SHADER_MASKED_PBR "MaskedPBR"
#define SHADER_TRANSPARENT_PBR "TransparentPBR"

namespace CHIKU
{

	class ShaderAsset : public Asset
    {
    public:
        ShaderAsset() : Asset(AssetType::Shader) {}
        ShaderAsset(AssetHandle handle) : Asset(handle, AssetType::Shader) {}

        virtual void CreateShader(const std::vector<AssetPath>& shaderCodes) = 0;
        virtual bool CreateShaderProgram(const AssetPath& ID) = 0;

        const UniformBufferDescription& GetBufferDescription() const 
        { 
            return m_UniformBufferDescription; 
        }

        virtual void CleanUp() = 0;
        virtual ~ShaderAsset() = default;

		const std::unordered_map<ShaderStages,ShaderStageData>& GetShaderStage() { return m_ShaderStage; }
        ReadableHandle GetShaderHandle() const { return m_ShaderHandle; }

        static SHARED<ShaderAsset> Create();
        static SHARED<ShaderAsset> Create(AssetHandle handle);

    protected:
		std::vector<AssetPath> m_ShaderCodes;
		std::vector<AssetPath> m_ShaderSPIRVs;

		std::bitset<ATTR_COUNT> m_InputAttributes;
        UniformBufferDescription m_UniformBufferDescription;
        ReadableHandle m_ShaderHandle = "";
        std::unordered_map<ShaderStages, ShaderStageData> m_ShaderStage;
    };
}