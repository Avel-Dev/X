#include "VulkanMeshAsset.h"
#include "Vulkan/Renderer/VulkanRenderer.h"

namespace CHIKU
{
	void VulkanMeshAsset::Draw() const
	{
		ZoneScoped;
		auto commandBuffer = VulkanRenderer::GetVulkanCommandBuffer();
		m_VertexBuffer->Bind();
		if (m_IndexBuffer->GetCount() > 0)
		{
			m_IndexBuffer->Bind();
			vkCmdDrawIndexed(commandBuffer, m_IndexBuffer->GetCount(), 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_VertexBuffer->GetCount()), 1, 0, 0);
		}
	}
}