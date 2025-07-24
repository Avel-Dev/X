#pragma once
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Renderer/Buffer/VertexBuffer.h"
#ifdef RENDERER_VULKAN
	#include "Vulkan/Renderer/VulkanGraphicsPipelineData.h"
#endif

namespace CHIKU
{
	struct PipelineKey
	{
		AssetHandle MaterialAssetHandle;
		VertexBufferMetaData VertexBufferMetaData;

		bool operator==(const PipelineKey& other) const
		{
			return MaterialAssetHandle == other.MaterialAssetHandle &&
				VertexBufferMetaData == other.VertexBufferMetaData;
		}
	};
}

namespace std {
	template <>
	struct hash<CHIKU::PipelineKey>
	{
		std::size_t operator()(const CHIKU::PipelineKey& key) const
		{
			std::size_t seed = 0;
			CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::AssetHandle>()(key.MaterialAssetHandle));
			CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::VertexBufferMetaData>()(key.VertexBufferMetaData));
			return seed;
		}
	};
}

namespace CHIKU
{
	class GraphicsPipeline
	{
	public:
		static std::unique_ptr<GraphicsPipeline> s_Instance;

	public:
		GraphicsPipeline() = default;
		
		virtual void Init() = 0;
		virtual void CleanUp() = 0;

		virtual PipelineData GetPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset, 
			const std::shared_ptr<MeshAsset>& meshAsset) = 0;
		
		virtual PipelineData CreatePipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset, 
			const std::shared_ptr<MeshAsset>& meshAsset) = 0;

		virtual void BindPipeline(const std::shared_ptr<MaterialAsset>& materialAsset, 
			const std::shared_ptr<MeshAsset>& meshAsset) = 0;
		
		static std::unique_ptr<GraphicsPipeline> Create();

	protected:
		std::array<UniformSetStorage, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT> m_GlobalUniformSetStorage; //Key is the set Index>
	};
}
