#include "Assets/AssetManager.h"
#include "VulkanMaterialAsset.h"
#include "Vulkan/Utils/VulkanShaderUtils.h"
#include "Vulkan/Utils/VulkanBufferUtils.h"
#include "Vulkan/Renderer/VulkanRenderer.h"
#include "Vulkan/Renderer/DescriptorPool.h"    
#include "Vulkan/Buffer/VulkanUniformBuffer.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>

namespace CHIKU
{

    void VulkanMaterialAsset::CreateMaterial()
    {
        ZoneScoped;

        m_Material = LoadMaterialFromFile(m_SourcePath);
        auto shaderAssetHandle = AssetManager::GetShaderAssetHandle(m_Material.shader);

        if (shaderAssetHandle == Asset::InvalidHandle)
        {
            throw std::runtime_error("Invalid Shader Asset Handle that means shader does not exist");
        }

        m_Shader = std::dynamic_pointer_cast<ShaderAsset>(AssetManager::GetAsset(shaderAssetHandle));

        if (!m_Shader)
        {
            throw std::runtime_error("Shader asset not found: " + m_Material.shader);
        }

        CreateUniformBuffer();
    }

    void VulkanMaterialAsset::CreateUniformBuffer()
    {
        ZoneScoped;

        const auto& bufferDescription = m_Shader->GetBufferDescription();
        m_UniformSetStorage = Utils::CreateDescriptorSetLayout(bufferDescription);
        Utils::CreateDescriptorSets(bufferDescription, m_UniformSetStorage);

        if (m_UniformSetStorage.empty())
            return;

        for (int frameNumber = 0; frameNumber < MAX_FRAMES_IN_FLIGHT; frameNumber++)
        {
            m_DescriptorSetsChache[frameNumber].resize(m_UniformSetStorage.size());

            int i = 0;
            for (const auto& [setIndex, storage] : m_UniformSetStorage)
            {
                m_DescriptorSetsChache[frameNumber][i] = storage.DescriptorSets[frameNumber];
                i++;
            }
        }

    }

    void VulkanMaterialAsset::UpdateUniformBuffer(uint32_t currentFrame)
    {
        ZoneScoped;

        if (m_UniformSetStorage.find(1) == m_UniformSetStorage.end())
        {
            throw std::runtime_error("Uniform Set 1 not found, it should contain base color");
        }

        auto& color = m_UniformSetStorage[1].BindingStorage[0].UniformBuffersMapped[currentFrame];
        memcpy(color, &m_Material.config.baseColor, sizeof(glm::vec4));
    }

    void VulkanMaterialAsset::CleanUp()
    {
        ZoneScoped;

        Asset::CleanUp();

        for (auto& [set, storage] : m_UniformSetStorage)
        {
            vkDestroyDescriptorSetLayout(VulkanRenderer::GetVulkanDevice(), storage.DescriptorSetLayout, nullptr);

            for (auto& [bindingIndex, bindingStorage] : storage.BindingStorage)
            {
                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                {
                    if (bindingStorage.UniformBuffersMapped[i])
                    {
                        vkUnmapMemory(VulkanRenderer::GetVulkanDevice(), bindingStorage.UniformBuffersMemory[i]);
                        bindingStorage.UniformBuffersMapped[i] = nullptr;
                    }

                    vkDestroyBuffer(VulkanRenderer::GetVulkanDevice(), bindingStorage.UniformBuffers[i], nullptr);
                    vkFreeMemory(VulkanRenderer::GetVulkanDevice(), bindingStorage.UniformBuffersMemory[i], nullptr);
                }
            }
        }

        m_UniformSetStorage.clear();
    }

    VulkanMaterialAsset::~VulkanMaterialAsset()
    {
        ZoneScoped;

        CleanUp();
    }

    std::vector<VkDescriptorSetLayout> VulkanMaterialAsset::GetDescriptorSetLayouts() const
    {
        ZoneScoped;

        std::vector<VkDescriptorSetLayout> layouts;

        for (const auto& [setIndex, storage] : m_UniformSetStorage)
        {
            layouts.push_back(storage.DescriptorSetLayout);
        }

        return layouts;
    }
}