#include "Renderer.h"

#include <Vulkan/Renderer/VulkanRenderer.h>

namespace CHIKU
{
	Renderer* Renderer::s_Instance = nullptr;

	Renderer* Renderer::Create(const RendererData* data)
	{
		return new VulkanRenderer();
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