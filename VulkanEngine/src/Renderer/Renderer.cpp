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

	void Renderer::InsertExtension(const char** extensions, uint32_t count)
	{
		for (uint32_t i = 0; i < count; ++i)
		{
			bool found = false;
			for (const auto& ext : m_Extension)
			{
				if (strcmp(ext, extensions[i]) == 0)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				m_Extension.push_back(extensions[i]);
			}
		}
	}
}