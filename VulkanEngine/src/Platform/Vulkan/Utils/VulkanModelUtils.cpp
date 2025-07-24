#include "VulkanModelUtils.h"
#include "VulkanBufferUtils.h"
#include "Assets/AssetManager.h"
#include "Assets/ShaderAsset.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace CHIKU
{
    namespace Utils
    {
        void FinalizeLayout(GLTFVertexBufferLayout& layout)
        {
            ZoneScoped;

            uint32_t offset = 0;
            for (auto& attr : layout.VertexElements)
            {
                attr.Offset = offset;
                attr.size = static_cast<uint32_t>(GetAttributeSize(attr.ComponentType, attr.AttributeType));
                offset += attr.size;
            }
            layout.Stride = offset;
        }

        std::vector<uint32_t> CreateIndices(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
        {
			std::vector<uint32_t> indices;
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            const size_t byteOffset = bufferView.byteOffset + indexAccessor.byteOffset;
            const unsigned char* dataPtr = buffer.data.data() + byteOffset;

            for (size_t i = 0; i < indexAccessor.count; ++i)
            {
                uint32_t index = 0;

                switch (indexAccessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    index = reinterpret_cast<const uint16_t*>(dataPtr)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    index = reinterpret_cast<const uint32_t*>(dataPtr)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    index = reinterpret_cast<const uint8_t*>(dataPtr)[i];
                    break;
                default:
                    std::cerr << "Unsupported index component type!" << std::endl;
                    break;
                }

                //std::cout << "Index[" << i << "] = " << index << std::endl;

				indices.push_back(index);
            }

			return indices;
        }

        std::string SelectShaderFromMaterial(const tinygltf::Material& mat)
        {
            ZoneScoped;

            // 1. If it's explicitly marked as unlit
            if (mat.extensions.find("KHR_materials_unlit") != mat.extensions.end()) {
                return SHADER_LIT;
            }

            // 2. Check alphaMode for transparency
            std::string alphaMode = mat.alphaMode; // May be empty
            std::transform(alphaMode.begin(), alphaMode.end(), alphaMode.begin(), ::toupper);

            if (alphaMode == "BLEND") {
                return SHADER_TRANSPARENT_PBR;
            }
            else if (alphaMode == "MASK") {
                return SHADER_MASKED_PBR;
            }

            // 3. Check for PBR-related textures
            const auto& pbr = mat.pbrMetallicRoughness;
            bool hasBaseColor = pbr.baseColorTexture.index >= 0;
            bool hasMetalRough = pbr.metallicRoughnessTexture.index >= 0;
            bool hasNormalMap = mat.normalTexture.index >= 0;
            bool hasEmissive = mat.emissiveTexture.index >= 0;

            if (hasBaseColor || hasMetalRough || hasNormalMap || hasEmissive) {
                return SHADER_PBR;
            }

            // 4. Fallback
            return SHADER_DEFAULT_LIT;
        }

        AssetHandle CreateMaterials(int index,const tinygltf::Model& model, const tinygltf::Material& mat, const AssetPath& outputPath)
        {
            ZoneScoped;

            nlohmann::json materialJson;
            materialJson["name"] = mat.name.empty() ? "Material" + std::to_string(index) : mat.name;
            materialJson["shader"] = SelectShaderFromMaterial(mat); // Decide shader based on presence of textures, etc.

            // Hardcoded render config
            nlohmann::json config;
            config["depthTest"] = true;
            config["depthWrite"] = true;
            config["blendEnabled"] = false;
            config["cullMode"] = "VK_CULL_MODE_BACK_BIT";
            config["frontFace"] = "VK_FRONT_FACE_COUNTER_CLOCKWISE";
            config["polygonMode"] = "VK_POLYGON_MODE_FILL";
            config["topology"] = "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST";
            materialJson["Config"] = config;

            // Add material factors
            if (mat.values.contains("baseColorFactor"))
            {
                const auto& color = mat.values.at("baseColorFactor").ColorFactor();
                materialJson["baseColorFactor"] = { color[0], color[1], color[2], color[3] };
            }

            if (mat.values.contains("metallicFactor"))
                materialJson["metallicFactor"] = mat.values.at("metallicFactor").Factor();

            if (mat.values.contains("roughnessFactor"))
                materialJson["roughnessFactor"] = mat.values.at("roughnessFactor").Factor();

            // Textures section
            nlohmann::json textures;

            auto setTextureIfValid = [&](const char* name, const tinygltf::Parameter& param) {
                if (param.TextureIndex() >= 0)
                {
                    int texIndex = param.TextureIndex();
                    if (texIndex >= 0 && texIndex < model.textures.size())
                    {
                        int imageIndex = model.textures[texIndex].source;
                        if (imageIndex >= 0 && imageIndex < model.images.size())
                            textures[name] = model.images[imageIndex].uri;
                    }
                }
                };

            if (mat.values.contains("baseColorTexture"))
                setTextureIfValid("albedo", mat.values.at("baseColorTexture"));

            if (mat.values.contains("metallicRoughnessTexture"))
                setTextureIfValid("metallicRoughness", mat.values.at("metallicRoughnessTexture"));

            if (mat.additionalValues.contains("normalTexture"))
                setTextureIfValid("normal", mat.additionalValues.at("normalTexture"));

            if (mat.additionalValues.contains("emissiveTexture"))
                setTextureIfValid("emissive", mat.additionalValues.at("emissiveTexture"));

            if (mat.additionalValues.contains("occlusionTexture"))
                setTextureIfValid("occlusion", mat.additionalValues.at("occlusionTexture"));

            if (!textures.empty())
                materialJson["Textures"] = textures;

            // Write to disk
            std::string fileName = materialJson["name"].get<std::string>() + ".json";
            std::string outputPathStr = outputPath + "/" + fileName;

            std::ofstream file(SOURCE_DIR + outputPathStr);
            if (!file.is_open())
            {
                throw std::runtime_error("Failed to write material file: " + outputPathStr);
                return {};
            }

            file << materialJson.dump(4);
            file.close();

            return AssetManager::AddMaterial(outputPathStr);
        }


        bool ProcessModel(const tinygltf::Model& model, std::unordered_map<AssetHandle, AssetHandle>& meshMaterial)
        {
            ZoneScoped;
            GLTFVertexBufferMetaData layout;
            AssetHandle materialHandle{};
            AssetHandle meshHandle;
            std::vector<uint8_t> data;

            std::unordered_map<int, AssetHandle> materialCache;

            bool success = true;

            for (const auto& node : model.meshes)
            {
                //if (node.mesh < 0)
                //{
                //    continue;
                //}

                for (const auto& primitive : node.primitives)
                {
                    int materialIndex = primitive.material;

                    if (materialIndex >= 0 && materialIndex < model.materials.size()) 
                    {
                        if (materialCache.find(materialIndex) == materialCache.end()) 
                        {
                            materialCache[materialIndex] = CreateMaterials(materialIndex, model, model.materials[materialIndex], "Materials/");
                        }
                        materialHandle = materialCache[materialIndex];
                    }
                    auto it = primitive.attributes.find(std::string(VertexAttributesArray[0])); // POSITION
                    if (it == primitive.attributes.end()) continue;

                    std::vector<uint32_t> indices;
                    if (primitive.indices >= 0)
                    {
                        indices = CreateIndices(model, primitive);
                    }

                    layout.Count = model.accessors[it->second].count;
                    layout.Layout = CreateBufferLayout(model, primitive);
                    FinalizeLayout(layout.Layout);
                    
                    if (!CreateVertexData(layout, data)) // fill the data vector with vertex data
                    {
                        success = false;
                    }
					//Utils::PrintVertexData(data, layout); // Print vertex data for debugging

                    meshHandle = AssetManager::AddMesh(Utils::ConvertGLTFInfoToVertexInfo(layout), data, indices); // add the mesh to the asset manager   

                    data.clear();
                    data.shrink_to_fit();
                    meshMaterial[meshHandle] = materialHandle;
                }

                if (!success)
                    break;
            }

            return success;
        }


        GLTFVertexBufferLayout CreateBufferLayout(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
        {
            ZoneScoped;

            GLTFVertexBufferLayout layout;
            auto& mask = layout.mask;

            int i = 0;
            layout.VertexElements.resize(primitive.attributes.size());

            for (auto& attrname : VertexAttributesArray)
            {
                auto it = primitive.attributes.find(std::string(attrname));
                if (it == primitive.attributes.end()) 
                {
                    continue;
                }

                auto& vertexElements = layout.VertexElements[i];
                mask.set(static_cast<size_t>(i)); // if order matches ATTR enum

                int accessorIndex = it->second;

                const tinygltf::Accessor& accessor = model.accessors[accessorIndex];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];
                vertexElements.Data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    vertexElements.ComponentType = VertexComponentType::Float;
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    vertexElements.ComponentType = VertexComponentType::Short;
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    vertexElements.ComponentType = VertexComponentType::Int;
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    vertexElements.ComponentType = VertexComponentType::Byte;
                }

                if (accessor.type == TINYGLTF_TYPE_SCALAR)
                {
                    vertexElements.AttributeType = VertexAttributeType::SCALAR;
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC2)
                {
                    vertexElements.AttributeType = VertexAttributeType::Vec2;
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC3)
                {
                    vertexElements.AttributeType = VertexAttributeType::Vec3;
                }
                else if (accessor.type == TINYGLTF_TYPE_VEC4)
                {
                    vertexElements.AttributeType = VertexAttributeType::Vec4;
                }

                //std::cout << "  Count: " << accessor.count << "\n";
                //std::cout << "  Stride: " << (bufferView.byteStride ? bufferView.byteStride : tinygltf::GetNumComponentsInType(accessor.type) * tinygltf::GetComponentSizeInBytes(accessor.componentType)) << "\n";
                //std::cout << "  Offset in BufferView: " << accessor.byteOffset << "\n";
                //std::cout << "  BufferView Offset: " << bufferView.byteOffset << "\n";
                //std::cout << "  Buffer Index: " << bufferView.buffer << "\n";
                //std::cout << "---------------------------\n";

                i++;
            }

            return layout;
        }

        bool CreateVertexData(const GLTFVertexBufferMetaData& infoData, std::vector<uint8_t>& outBuffer)
        {
            ZoneScoped;

            outBuffer.resize(infoData.Count * infoData.Layout.Stride);

            for (int i = 0; i < infoData.Count; ++i)
            {
                uint8_t* dstVertex = outBuffer.data() + i * infoData.Layout.Stride;

                for (const auto& attrib : infoData.Layout.VertexElements)
                {
                    const uint8_t* src = attrib.Data + i * attrib.size;
                    uint8_t* dst = dstVertex + attrib.Offset;
                    std::memcpy(dst, src, attrib.size);
                }
            }

            return true;
        }

        const char* ToString(VertexComponentType compType,VertexAttributeType attribType)
        {
            ZoneScoped;

            switch (attribType)
            {
            case VertexAttributeType::SCALAR: return "Scalar";
            case VertexAttributeType::Vec2 : return "Vec2";
            case VertexAttributeType::Vec3: return "Vec3";
            case VertexAttributeType::Vec4: return "Vec4";
                // Add others as needed
            default: return "Unknown";
            }

            switch (compType)
            {
            case VertexComponentType::Float:    return "Float";
            case VertexComponentType::Byte:     return "Byte";
            case VertexComponentType::Int:      return "Int";
            case VertexComponentType::Short:    return "Short";
                            // Add others as needed
            default: return "Unknown";
            }
        }

        void PrintVertexData(const std::vector<uint8_t>& buffer, const GLTFVertexBufferMetaData& infoData)
        {
            ZoneScoped;

            for (int i = 0; i < infoData.Count; ++i)
            {
                const uint8_t* vertexPtr = buffer.data() + i * infoData.Layout.Stride;
                std::cout << "Vertex " << i << ": ";

                for (const auto& attrib : infoData.Layout.VertexElements)
                {
                    const uint8_t* attrPtr = vertexPtr + attrib.Offset;

                    std::cout << "[" << ToString(attrib.ComponentType,attrib.AttributeType) << "] ";

                    // Print each component as float (assuming 4-byte floats)
                    for (int j = 0; j < attrib.size / sizeof(float); ++j)
                    {
                        float value;
                        std::memcpy(&value, attrPtr + j * sizeof(float), sizeof(float));
                        std::cout << value << " ";
                    }

                    std::cout << " | ";
                }

                std::cout << "\n";
            }
        }

        VertexBufferMetaData ConvertGLTFInfoToVertexInfo(const GLTFVertexBufferMetaData& gltfInfo)
        {
            ZoneScoped;

            VertexBufferMetaData result;
            result.Count = gltfInfo.Count;
            result.Layout.Stride = gltfInfo.Layout.Stride;
            result.Layout.Mask = gltfInfo.Layout.mask;

            result.Layout.VertexElements.reserve(gltfInfo.Layout.VertexElements.size());
            for (const auto& attr : gltfInfo.Layout.VertexElements)
            {
                VertexAttribute convertedAttr;
                convertedAttr.Offset = attr.Offset;
                convertedAttr.size = attr.size;
                convertedAttr.ComponentType = attr.ComponentType;
                convertedAttr.AttributeType = attr.AttributeType;

                result.Layout.VertexElements.push_back(convertedAttr);
            }

            return result;
        }

        bool IsGLTFFormat(const AssetPath& path)
        {
            ZoneScoped;

			auto path_ = std::filesystem::path(path);
            std::string ext = path_.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return ext == ".gltf" || ext == ".glb";
        }

        AssetPath ConvertToGLTF(const AssetPath& modelAsset)
        {
            ZoneScoped;
            if (Utils::IsGLTFFormat(modelAsset))
            {
                return modelAsset;
            }


			auto modelpath = std::filesystem::path(modelAsset);
            std::string stem = modelpath.stem().string();
            AssetPath gltfPath = SOURCE_DIR + "Models/" + (stem + ".gltf");

            // Build the command
            std::string command = (SOURCE_DIR + "tools/FBX2glTF.exe")
                + " --input \"" + SOURCE_DIR + modelAsset + "\""
                + " --output \"" + gltfPath + "\"";

            int result = std::system(command.c_str());

            if (result != 0)
            {
                std::cerr << "FBX2glTF failed with exit code: " << result << std::endl;
                return "";
            }

            if (std::filesystem::exists(gltfPath))
            {
                return gltfPath;
            }
            else
            {
                std::cerr << "Output file not found after conversion." << std::endl;
                return "";
            }
        }
    }
}