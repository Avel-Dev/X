#pragma once
#include "Renderer/Buffer/VertexBuffer.h"
#include "Assets/Asset.h"
#include <tiny_gltf.h>

namespace CHIKU
{
	struct VertexBufferMetaData;
    
	namespace Utils
	{
        struct GLTFVertexAttribute
        {
            const uint8_t* Data;
            uint32_t Offset = 0; // byte offset within vertex struct
            uint8_t size;
            VertexComponentType ComponentType;
            VertexAttributeType AttributeType;
            //1 byte can be squeezed in here 
        };

        struct GLTFVertexBufferLayout
        {
            uint32_t Stride;
            std::bitset<ATTR_COUNT> mask;
            std::vector<GLTFVertexAttribute> VertexElements;
        };

        struct GLTFVertexBufferMetaData
        {
            uint64_t Count;            // number of vertices
            GLTFVertexBufferLayout Layout;
        };

        void FinalizeLayout(GLTFVertexBufferLayout& layout);

		std::vector<uint32_t> CreateIndices(const tinygltf::Model& model, const tinygltf::Primitive& primitive);

        std::string SelectShaderFromMaterial(const tinygltf::Material& mat);
        AssetHandle CreateMaterials(int index, const tinygltf::Model& model, const tinygltf::Material& mat, const AssetPath& outputPath);
        bool ProcessModel(const tinygltf::Model& model, std::unordered_map<AssetHandle, AssetHandle>& meshMaterial);
        
		GLTFVertexBufferLayout CreateBufferLayout(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
		
        bool CreateVertexData(const GLTFVertexBufferMetaData& infoData, std::vector<uint8_t>& outBuffer);
		void PrintVertexData(const std::vector<uint8_t>& buffer, const GLTFVertexBufferMetaData& infoData);

        VertexBufferMetaData ConvertGLTFInfoToVertexInfo(const GLTFVertexBufferMetaData& gltfInfo);
        bool IsGLTFFormat(const AssetPath& path);
        AssetPath ConvertToGLTF(const AssetPath& modelAsset);
	}
}