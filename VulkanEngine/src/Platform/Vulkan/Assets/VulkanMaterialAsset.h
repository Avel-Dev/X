#pragma once
#include "Assets/MaterialAsset.h"

namespace CHIKU
{
	class VulkanMaterialAsset : public MaterialAsset
	{
	public:
		VulkanMaterialAsset() : MaterialAsset() {}
		VulkanMaterialAsset(AssetHandle handle) : MaterialAsset(handle) {}
		VulkanMaterialAsset(AssetHandle handle, AssetPath path) : MaterialAsset(handle, path)
		{
			CreateMaterial();
		}

		virtual void CreateMaterial() override;
		virtual void CreateUniformBuffer() override;

		virtual void CleanUp() override;

		~VulkanMaterialAsset();

		virtual void UpdateUniformBuffer(uint32_t currentFrame) override;

		std::vector<VkDescriptorSet> GetDescriptorSets(uint32_t frameCount) const { return m_DescriptorSetsChache[frameCount]; };
		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts() const;

	private:
		std::array< std::vector<VkDescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_DescriptorSetsChache;
	};

}