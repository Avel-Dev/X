#include "GraphicsPipeline.h"

#include <Vulkan/Renderer/VulkanGraphicsPipeline.h>
#include <Vulkan/Renderer/VulkanGraphicsPipelineData.h>

namespace CHIKU
{
	std::unique_ptr<GraphicsPipeline> GraphicsPipeline::s_Instance = GraphicsPipeline::Create();

	std::unique_ptr<GraphicsPipeline> GraphicsPipeline::Create()
	{
		return std::make_unique<VulkanGraphicsPipeline>();
	}

	PipelineData GraphicsPipeline::GetPipeline(
		const std::shared_ptr<MaterialAsset>& materialAsset,
		const std::shared_ptr<MeshAsset>& meshAsset)
	{
		return s_Instance->mGetPipeline(materialAsset, meshAsset);
	}

	PipelineData GraphicsPipeline::CreatePipeline(
		const std::shared_ptr<MaterialAsset>& materialAsset,
		const std::shared_ptr<MeshAsset>& meshAsset)
	{
		return s_Instance->mCreatePipeline(materialAsset, meshAsset);
	}

	void GraphicsPipeline::BindPipeline(
		const std::shared_ptr<MaterialAsset>& materialAsset,
		const std::shared_ptr<MeshAsset>& meshAsset)
	{
		return s_Instance->mBindPipeline(materialAsset, meshAsset);
	}
}