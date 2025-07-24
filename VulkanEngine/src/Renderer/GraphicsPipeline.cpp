#include "GraphicsPipeline.h"
#include "Vulkan/Renderer/VulkanGraphicsPipeline.h"

namespace CHIKU
{
	std::unique_ptr<GraphicsPipeline> GraphicsPipeline::s_Instance = GraphicsPipeline::Create();

	std::unique_ptr<GraphicsPipeline> GraphicsPipeline::Create()
	{
		return std::make_unique<VulkanGraphicsPipeline>();
	}
}