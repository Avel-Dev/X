#pragma once
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Renderer/Buffer/VertexBuffer.h"

namespace CHIKU
{
	struct PipelineKey
	{
		AssetHandle MaterialAssetHandle;
		VertexBufferMetaData PipelineVertexBufferMetaData;

		bool operator==(const PipelineKey& other) const
		{
			return MaterialAssetHandle == other.MaterialAssetHandle &&
				PipelineVertexBufferMetaData == other.PipelineVertexBufferMetaData;
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
			CHIKU::Utils::hash_combine(seed, std::hash<CHIKU::VertexBufferMetaData>()(key.PipelineVertexBufferMetaData));
			return seed;
		}
	};
}

namespace CHIKU
{
	struct PipelineData;

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() = default;

		static std::unique_ptr<GraphicsPipeline> s_Instance;
		static std::unique_ptr<GraphicsPipeline> Create();

		static void Init() { s_Instance->mInit(); }
		static void CleanUp() { s_Instance->mCleanUp(); }

		static PipelineData GetPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset);

		static PipelineData CreatePipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset);

		static void BindPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset,
			const std::shared_ptr<MeshAsset>& meshAsset);

	private:
		
		virtual void mInit() = 0;
		virtual void mCleanUp() = 0;

		virtual PipelineData mGetPipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset, 
			const std::shared_ptr<MeshAsset>& meshAsset) = 0;
		
		virtual PipelineData mCreatePipeline(
			const std::shared_ptr<MaterialAsset>& materialAsset, 
			const std::shared_ptr<MeshAsset>& meshAsset) = 0;

		virtual void mBindPipeline(const std::shared_ptr<MaterialAsset>& materialAsset, 
			const std::shared_ptr<MeshAsset>& meshAsset) = 0;
		
	protected:
		std::array<UniformSetStorage, DEFAULT_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT> m_GlobalUniformSetStorage; //Key is the set Index>
	};
}
