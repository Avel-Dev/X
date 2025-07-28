#include "VulkanShaderUtils.h"
#include "Assets/ShaderAsset.h"
#include "Vulkan/Renderer/VulkanRenderer.h"
#include "Vulkan/Renderer/DescriptorPool.h"
#include "Vulkan/Utils/VulkanBufferUtils.h"

#include <fstream>
#include <iostream>

namespace CHIKU
{
	namespace Utils
	{
        UniformPlainDataType GetSPIRVDataType(const SpvReflectBlockVariable& member)
        {
            ZoneScoped;

            const SpvReflectTypeDescription* type = member.type_description;
            if (!type)
                throw std::runtime_error("Missing type description");

            const auto flags = type->type_flags;

            if (flags & SPV_REFLECT_TYPE_FLAG_MATRIX)
            {
                if (type->traits.numeric.matrix.column_count == 2) return UniformPlainDataType::Mat2;
                if (type->traits.numeric.matrix.column_count == 3) return UniformPlainDataType::Mat3;
                if (type->traits.numeric.matrix.column_count == 4) return UniformPlainDataType::Mat4;
            }

            if (flags & SPV_REFLECT_TYPE_FLAG_VECTOR)
            {
                uint32_t count = type->traits.numeric.vector.component_count;

                if (flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
                {
                    if (count == 2) return UniformPlainDataType::Vec2;
                    if (count == 3) return UniformPlainDataType::Vec3;
                    if (count == 4) return UniformPlainDataType::Vec4;
                }
                if (flags & SPV_REFLECT_TYPE_FLAG_INT)
                {
                    bool isSigned = type->traits.numeric.scalar.signedness != 0;

                    if (isSigned)
                    {
                        if (count == 2) return UniformPlainDataType::IVec2;
                        if (count == 3) return UniformPlainDataType::IVec3;
                        if (count == 4) return UniformPlainDataType::IVec4;
                    }
                    else
                    {
                        if (count == 2) return UniformPlainDataType::UVec2;
                        if (count == 3) return UniformPlainDataType::UVec3;
                        if (count == 4) return UniformPlainDataType::UVec4;
                    }
                }
                if (flags & SPV_REFLECT_TYPE_FLAG_BOOL)
                {
                    if (count == 2) return UniformPlainDataType::BVec2;
                    if (count == 3) return UniformPlainDataType::BVec3;
                    if (count == 4) return UniformPlainDataType::BVec4;
                }
            }

            if (flags & SPV_REFLECT_TYPE_FLAG_FLOAT) return UniformPlainDataType::Float;
            if (flags & SPV_REFLECT_TYPE_FLAG_INT) 
            {
                bool isSigned = type->traits.numeric.scalar.signedness != 0;
                
                if (isSigned) return UniformPlainDataType::Int;
                else return UniformPlainDataType::UInt;
            }
            if (flags & SPV_REFLECT_TYPE_FLAG_BOOL)  return UniformPlainDataType::Bool;

            throw std::runtime_error("Unknown SPIR-V type");
        }

        void PrintUniformSetLayout(const UniformBufferDescription& uniform)
        {
            ZoneScoped;

            for (const auto& [setNumber,uniformSet] : uniform) 
            {
                for (const auto& [binding, uniformBuffer] : uniformSet)
                {
                    std::cout << "Uniform Set : " << setNumber << "\n";
					std::cout << "Uniform Buffer Size : " << uniformBuffer.Size << "\n";
                    if (uniformBuffer.isUBO())
                    {
                        for (const auto& member : uniformBuffer.UniformBufferInfo)
                        {
                            std::cout << "  Type   : " << ToString(member.Type) << "\n";
                            std::cout << "  Offset : " << member.Offset << "\n";
                            std::cout << "  Size   : " << member.Size << "\n";
                            std::cout << "  -----------\n";
                        }
                    }
                    else if (uniformBuffer.isSampler())
                    {
                        std::cout << uniformBuffer.BufferSamplerInfo.Count << "\n";
                        std::cout << ToString(uniformBuffer.BufferSamplerInfo.DescriptorType) << "\n";
                    }
                }
            }
        }

        UniformOpaqueDataType ConvertToOpaqueType(const SpvReflectDescriptorBinding* binding)
        {
            ZoneScoped;

            if (!binding) return UniformOpaqueDataType::none;

            if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            {
                switch (binding->binding)
                {
                case 0:  return UniformOpaqueDataType::DIFFUSE_TEXTURE;
                case 1:  return UniformOpaqueDataType::NORMAL_TEXTURE;
                case 2:  return UniformOpaqueDataType::SPECULAR_TEXTURE;
                case 3:  return UniformOpaqueDataType::EMISSIVE_TEXTURE;
                case 4:  return UniformOpaqueDataType::HEIGHT_TEXTURE;
                case 5:  return UniformOpaqueDataType::SHADOW_MAP_TEXTURE;
                case 6:  return UniformOpaqueDataType::SHADOW_CUBE_MAP_TEXTURE;
                default: return UniformOpaqueDataType::DIFFUSE_TEXTURE; // Fallback
                }
            }

            return UniformOpaqueDataType::none;
        }

        bool IsVertexShader(const AssetPath& shaderPath)
        {
            ZoneScoped;

            if(shaderPath.find("vert") != std::string::npos || shaderPath.find("Vertex") != std::string::npos)
            {
                return true;
			}

            return false;
        }

        void ProcessSPIRV(const std::vector<AssetPath>& shaderCodes, UniformBufferDescription& uniformSets, std::bitset<ATTR_COUNT>& inputAttribute)
        {
            ZoneScoped;

            for (auto& shaderCode : shaderCodes)
            {
                std::ifstream file(SOURCE_DIR + shaderCode, std::ios::binary | std::ios::ate);

                if (!file.is_open())
                {
                    throw std::runtime_error("Failed to open SPIR-V file");
                }

                size_t size = file.tellg();
                file.seekg(0);
                std::vector<uint32_t> spirv(size / sizeof(uint32_t));
                file.read(reinterpret_cast<char*>(spirv.data()), size);

                SpvReflectShaderModule module;
                SpvReflectResult result = spvReflectCreateShaderModule(spirv.size() * sizeof(uint32_t), spirv.data(), &module);
                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    throw std::runtime_error("Failed to reflect shader");
                }

				GetUniformDescription(uniformSets, module);

                if(module.shader_stage & SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
                {
                    GetInputAttributes(inputAttribute, module);
				}

                spvReflectDestroyShaderModule(&module);
            }

            PrintUniformSetLayout(uniformSets);
        }

        ShaderStages MapReflectStage(SpvReflectShaderStageFlagBits stage)
        {
            ZoneScoped;

            switch (stage)
            {
            case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT: return ShaderStages::Stage_Vertex;
            case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return ShaderStages::Stage_Tessellation;
            case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT: return ShaderStages::Stage_Geometry;
            case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT: return ShaderStages::Stage_Fragment;
            case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT: return ShaderStages::Stage_Compute;
            default: return ShaderStages::Stage_None;
            }
        }

        void GetUniformDescription(UniformBufferDescription& uniformBufferSet,const SpvReflectShaderModule& spirv)
        {
            ZoneScoped;

            uint32_t setCount = 0;
            spvReflectEnumerateDescriptorSets(&spirv, &setCount, nullptr);
            std::vector<SpvReflectDescriptorSet*> sets(setCount);
            spvReflectEnumerateDescriptorSets(&spirv, &setCount, sets.data());

            for (const SpvReflectDescriptorSet* set : sets)
            {
                for (uint32_t i = 0; i < set->binding_count; ++i)
                {
                    const SpvReflectDescriptorBinding* binding = set->bindings[i];
                    uint32_t bindingIndex = binding->binding;

                    if (set->set == 0 && binding->binding == 0)
                    {
						continue; // skip the reserved binding 0 in set 0
                    }

                    if(uniformBufferSet[set->set].find(bindingIndex) != uniformBufferSet[set->set].end())
                    {
                        uniformBufferSet[set->set][bindingIndex].Stages.set(MapReflectStage(spirv.shader_stage));
                        continue;
					}

                    auto& uniformBuffer = uniformBufferSet[set->set][bindingIndex];

                    if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                    {
                        const SpvReflectBlockVariable& block = binding->block;

                        std::vector<UniformMemberInfo> members;
                        for (uint32_t j = 0; j < block.member_count; ++j)
                        {
                            const SpvReflectBlockVariable& member = block.members[j];

                            UniformMemberInfo info;
                            info.Offset = member.offset;
                            info.Size = member.size;
                            info.Type = GetSPIRVDataType(member);

							uniformBuffer.Size += info.Size;
                            members.push_back(info);
                        }

                        uniformBuffer.UniformBufferInfo.insert(uniformBuffer.UniformBufferInfo.end(), members.begin(), members.end());
                    }
                    else if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                    {
                        SamplerInfo sampler;
                        sampler.Count = binding->count;
                        sampler.DescriptorType = ConvertToOpaqueType(binding);

                        uniformBuffer.BufferSamplerInfo = std::move(sampler);
                    }

                    uniformBuffer.Stages.set(MapReflectStage(spirv.shader_stage));
                }
            }
        }

        void GetInputAttributes(std::bitset<ATTR_COUNT>& attribute, const SpvReflectShaderModule& spirv)
        {
            ZoneScoped;

            // 3) Enumerate all input variables
            uint32_t varCount = 0;
            SpvReflectResult result = spvReflectEnumerateInputVariables(&spirv, &varCount, nullptr);
            if (result != SPV_REFLECT_RESULT_SUCCESS) {
                std::cerr << "Failed to get input variable count\n";
				throw std::runtime_error("Failed to enumerate input variables");
            }

            std::vector<SpvReflectInterfaceVariable*> inputs(varCount);
            result = spvReflectEnumerateInputVariables(&spirv, &varCount, inputs.data());
            if (result != SPV_REFLECT_RESULT_SUCCESS) {
                std::cerr << "Failed to enumerate input variables\n";
				throw std::runtime_error("Failed to enumerate input variables");
            }

            // 4) Print them out
            std::cout << "Vertex-input attributes:\n";
            for (auto* var : inputs) {

                if (var->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
                    continue; // skip built-in like gl_VertexID

                // var->location is your `layout(location = X)`
                // var->name     is the GLSL variable name
                // var->format   is a VkFormat enum of how you'd declare the attribute

                attribute.set(var->location, true);

                std::cout
                    << "  Location " << var->location
                    << " : " << var->name
                    << " (format = " << var->format << ")\n";
            }
        }

        VkShaderStageFlags MapToVulkanShaderStageFlags(const ShaderStageBits& stageBits)
        {
            ZoneScoped;

            VkShaderStageFlags flags = 0;

            if (stageBits.test(Stage_Vertex))        flags |= VK_SHADER_STAGE_VERTEX_BIT;
            if (stageBits.test(Stage_Tessellation))  flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            if (stageBits.test(Stage_Geometry))      flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
            if (stageBits.test(Stage_Fragment))      flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            if (stageBits.test(Stage_Compute))       flags |= VK_SHADER_STAGE_COMPUTE_BIT;

            return flags;
        }


        std::map<uint32_t, UniformSetStorage>  CreateDescriptorSetLayout(const UniformBufferDescription& bufferDescription)
        {
            ZoneScoped;
            std::map<uint32_t, UniformSetStorage> setStorage;

            for (const auto& [setIndex, set] : bufferDescription)
            {
                for (const auto& [bindingIndex, uniformBuffer] : set)
                {
                    if (!uniformBuffer.isUBO())
                        continue;

                    UniformBufferStorage storage;

                    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                    {
                        Utils::CreateBuffer(uniformBuffer.Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storage.UniformBuffers[i], storage.UniformBuffersMemory[i]);

                        vkMapMemory(VulkanRenderer::GetVulkanDevice(), storage.UniformBuffersMemory[i], 0, uniformBuffer.Size, 0, &storage.UniformBuffersMapped[i]);
                    }

                    setStorage[setIndex].BindingStorage[bindingIndex] = storage;
                }
            }

            for (const auto& [setIndex, set] : bufferDescription)
            {
                std::vector<VkDescriptorSetLayoutBinding> bindings;
                for (const auto& [bindingIndex, uniformBuffer] : set)
                {
                    if (!uniformBuffer.isValid())
                        continue;

                    VkDescriptorSetLayoutBinding layoutBinding{};
                    layoutBinding.binding = bindingIndex;
                    layoutBinding.descriptorCount = 1; // or uniformBuffer.SamplerInfo.Count
                    layoutBinding.pImmutableSamplers = nullptr;
                    layoutBinding.stageFlags = Utils::MapToVulkanShaderStageFlags(uniformBuffer.Stages);

                    if (uniformBuffer.isUBO())
                        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    else if (uniformBuffer.isSampler())
                        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    else
                        continue;

                    bindings.push_back(layoutBinding);
                }

                if (bindings.empty())
                    continue;

                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
                layoutInfo.pBindings = bindings.data();

                VkDescriptorSetLayout layout;
                if (vkCreateDescriptorSetLayout(VulkanRenderer::GetVulkanDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create descriptor set layout!");
                }

                setStorage[setIndex].DescriptorSetLayout = layout;
            }

            return setStorage;
        }

        void CreateDescriptorSets(const UniformBufferDescription& bufferDescription, std::map<uint32_t, UniformSetStorage>& setStorage)
        {
            ZoneScoped;

            for (auto& [setIndex, layout] : setStorage)
            {
                std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout.DescriptorSetLayout);

                VkDescriptorSetAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocInfo.descriptorPool = DescriptorPool::GetDescriptorPool();
                allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
                allocInfo.pSetLayouts = layouts.data();

                if (vkAllocateDescriptorSets(VulkanRenderer::GetVulkanDevice(), &allocInfo, layout.DescriptorSets.data()) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to allocate descriptor sets!");
                }

                for (const auto& [bindingIndex, uniformBuffer] : bufferDescription.at(setIndex))
                {
                    if (!uniformBuffer.isValid())
                        continue;

                    VkWriteDescriptorSet descriptorWrites[MAX_FRAMES_IN_FLIGHT]{};
                    VkDescriptorBufferInfo bufferInfos[MAX_FRAMES_IN_FLIGHT]{};
                    VkDescriptorImageInfo imageInfos[MAX_FRAMES_IN_FLIGHT]{};

                    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                    {
                        VkWriteDescriptorSet descriptorWrite{};
                        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        descriptorWrite.dstSet = layout.DescriptorSets[i];
                        descriptorWrite.dstBinding = bindingIndex;
                        descriptorWrite.dstArrayElement = 0;
                        descriptorWrite.descriptorCount = 1;

                        if (uniformBuffer.isUBO())
                        {
                            auto& bufferStorage = layout.BindingStorage.at(bindingIndex);
                            bufferInfos[i] = {
                                bufferStorage.UniformBuffers[i],
                                0,
                                uniformBuffer.Size
                            };

                            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            descriptorWrite.pBufferInfo = &bufferInfos[i];
                        }
                        else if (uniformBuffer.isSampler())
                        {
                            throw std::runtime_error("textures are not implemented yet");
                            //TODO :: ADD Textures

                           /* imageInfos[i] = {
                                textureSampler,
                                textureImageView,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                                };

                            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            descriptorWrite.pImageInfo = &imageInfos[i];*/
                        }

                        descriptorWrites[i] = descriptorWrite;
                    }

                    vkUpdateDescriptorSets(VulkanRenderer::GetVulkanDevice(), MAX_FRAMES_IN_FLIGHT, descriptorWrites, 0, nullptr);
                }
            }
        }

	}
}