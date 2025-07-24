#include "Renderer.h"
#ifdef RENDERER_VULKAN
#include "Vulkan/Renderer/VulkanRenderer.h"
#endif

namespace CHIKU
{
	std::shared_ptr<Renderer> Renderer::Create()
	{
		ZoneScoped;
		return std::make_shared<VulkanRenderer>();
	}
}