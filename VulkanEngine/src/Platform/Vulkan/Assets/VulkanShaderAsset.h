#pragma once
#include "Assets/ShaderAsset.h"

namespace CHIKU
{
	class VulkanShaderAsset : public ShaderAsset
	{
	public:
		VulkanShaderAsset() : ShaderAsset() {}
		VulkanShaderAsset(AssetHandle handle) : ShaderAsset(handle) {}
		
		virtual void CreateShader(const std::vector<AssetPath>& shaderCodes) override;
		virtual bool CreateShaderProgram(const AssetPath& ID) override;
		
		virtual void CleanUp() override;
		virtual ~VulkanShaderAsset();

	private:
		void GetShaderNameAndStage(const AssetPath& data, ReadableHandle& name, ShaderStages& stage) const;
		void CreateUniformBufferDescription(const std::vector<AssetPath>& shaderCodes);
		bool CreateSPIRV(const std::string& a_ShaderPath, const std::string& a_OutPutPath) const;

		ShaderStageData CreateShaderModule(const std::vector<char>& code) const;
		std::vector<char> ReadFile(const std::string& filename) const;
	};
}