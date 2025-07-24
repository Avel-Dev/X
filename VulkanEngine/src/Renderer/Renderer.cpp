#include "Renderer.h"
#ifdef RENDERER_VULKAN
#include "Vulkan/Renderer/VulkanRenderer.h"
#endif

namespace CHIKU
{
	std::unique_ptr<Renderer> Renderer::s_Instance = Renderer::Create();

	std::unique_ptr<Renderer> Renderer::Create()
	{
		ZoneScoped;
		return std::make_unique<VulkanRenderer>();
	}
}