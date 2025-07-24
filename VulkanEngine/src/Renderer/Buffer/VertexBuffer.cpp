#include "VertexBuffer.h"
#include "Platform/Vulkan/Buffer/VulkanVertexBuffer.h"

namespace CHIKU
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create()
	{
		ZoneScoped;
		return std::make_shared<VulkanVertexBuffer>();
	}
}