#pragma once
#include "Renderer/GraphicsPipeline.h"

namespace CHIKU
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{

	public:
		virtual void mInit() override;
		virtual void mCleanUp() override;

		virtual PipelineData mGetPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset) override;

		virtual PipelineData mCreatePipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset) override;

		virtual void mBindPipeline(
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