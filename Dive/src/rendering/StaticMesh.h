#pragma once
#include "resource/Resource.h"
#include "graphics/Vertex.h"

namespace Dive
{
	class Buffer;
	class BoundingBox;
	class Shader;

	enum class eSourceType
	{
		None,
		File,
		Factory
	};

	enum class eMeshType
	{
		None,
		Static,
		Skinned
	};

	class StaticMesh : public Resource
	{
	public:
		StaticMesh();
		~StaticMesh() override;

		bool CreateBuffers(ID3D11Device* device);
		void Clear();

		void Draw(ID3D11DeviceContext* deviceContext);

		void ComputeBouingBox();
		
		void AddVertices(const std::vector<LitVertex>& vertices, uint32_t* outOffset = nullptr);
		void AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset = nullptr);

		std::vector<LitVertex> GetVertices() { return m_vertices; }
		std::vector<uint32_t> GetIndices() { return m_indices; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_indices.size()); }

		BoundingBox* GetBoundingBox() const { return m_boundingBox.get(); }

		eSourceType GetSourceType() const { return m_sourceType; }
		void SetSourceType(eSourceType type) { m_sourceType = type; }

		std::filesystem::path GetFilepath() const { return m_filepath; }

		eMeshType GetMeshType() const { return m_type; }

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return m_primitiveTopology; }
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) { m_primitiveTopology; }

		static constexpr eResourceType GetType() { return eResourceType::StaticMesh; }

	private:
		eSourceType m_sourceType = eSourceType::None;
		std::filesystem::path m_filepath;	// 좀 애매해다. 

		eMeshType m_type = eMeshType::None;

		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::vector<LitVertex> m_vertices;
		std::vector<uint32_t> m_indices;

		std::shared_ptr<Buffer> m_vb;
		std::shared_ptr<Buffer> m_ib;
		
		std::shared_ptr<BoundingBox> m_boundingBox;
	};
}
