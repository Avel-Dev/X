#pragma once
#include "Asset.h"
#include "Renderer/Buffer/VertexBuffer.h"
#include "Renderer/Buffer/IndexBuffer.h"
#include <tiny_gltf.h>
#include <iostream>

namespace CHIKU
{
	class MeshAsset : public Asset
	{
	public:
		MeshAsset() : Asset(AssetType::Mesh) { Init(); }
		MeshAsset(AssetHandle handle) : Asset(handle, AssetType::Mesh) { Init(); }
		MeshAsset(AssetHandle handle, AssetPath path) : Asset(handle, AssetType::Mesh, path) { Init(); }
		
		void Init() override
		{
			m_VertexBuffer = VertexBuffer::Create();
			m_IndexBuffer = IndexBuffer::Create();
		}
		void SetMetaData(VertexBufferMetaData metaData) { m_VertexBuffer->SetMetaData(metaData); }
		void SetData(const std::vector<uint8_t>& data) { m_VertexBuffer->CreateVertexBuffer(data); }
		void SetIndexData(const std::vector<uint32_t>& indices) { m_IndexBuffer->CreateIndexBuffer(indices); }

		virtual void CleanUp() override
		{
			m_VertexBuffer->CleanUp();
			m_IndexBuffer->CleanUp();
			Asset::CleanUp();
		}
		
		inline uint64_t GetVertexCount() const { return m_VertexBuffer->GetCount(); }

		inline const std::shared_ptr<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
		inline const std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
		
		inline void Bind() const { m_VertexBuffer->Bind(); };
		virtual void Draw() const = 0;

		static std::shared_ptr<MeshAsset> Create();
		static std::shared_ptr<MeshAsset> Create(AssetHandle handle);
		static std::shared_ptr<MeshAsset> Create(AssetHandle handle, AssetPath path);

	protected:
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}