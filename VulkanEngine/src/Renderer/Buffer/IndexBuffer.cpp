#include "IndexBuffer.h"
#include "Platform/Vulkan/Buffer/VulkanIndexBuffer.h"

namespace CHIKU
{
	std::shared_ptr<IndexBuffer> IndexBuffer::Create()
	{
		ZoneScoped;
		return std::make_shared<VulkanIndexBuffer>();
	}
}