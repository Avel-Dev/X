#pragma once
#include "Renderer/GraphicsPipeline.h"

namespace CHIKU
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		virtual void Init() override;
		virtual void CleanUp() override;

		virtual PipelineData GetPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset) override;

		virtual PipelineData CreatePipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset) override;

		virtual void BindPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset) override;

		PipelineData CreatePipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset, 
			const VkVertexInputBindingDescription& bindingDescription, 
			const std::vector<VkVertexInputAttributeDescription>& attributeDescription);

	private:
		std::unordered_map<PipelineKey, PipelineData> m_Pipelines;
		std::array<VkDescriptorSetLayout, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT> m_GlobalDescriptorSetLayouts; //Key is the set Index>
		std::array<std::array<VkDescriptorSet, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT>, MAX_FRAMES_IN_FLIGHT> m_GlobalDescriptorSetsChache;
	};
}