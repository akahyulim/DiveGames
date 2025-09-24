#pragma once
#include "resource/Resource.h"
#include "graphics/Vertex.h"
#include "math/BoundingBox.h"

namespace Dive
{
	class Buffer;
	class Shader;

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

		bool CreateBuffers();
		void Clear();

		void Draw();

		void AddVertices(const std::vector<StaticVertex>& vertices, uint32_t* outOffset = nullptr);
		void AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset = nullptr);

		std::vector<StaticVertex> GetVertices() { return m_Vertices; }
		std::vector<uint32_t> GetIndices() { return m_Inidices; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Inidices.size()); }

		BoundingBox GetLocalBoundingBox() const { return m_LocalBoundingBox; }

		eMeshType GetMeshType() const { return m_Type; }

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return m_PrimitiveTopology; }
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) { m_PrimitiveTopology; }

		eResourceType GetType() override { return eResourceType::StaticMesh; }
		static constexpr eResourceType StaticType() { return eResourceType::StaticMesh; }

		bool LoadFromFile(const std::filesystem::path& filepath) override;
		bool SaveToFile(const std::filesystem::path& filepath) override;

	private:
		eMeshType m_Type = eMeshType::None;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::vector<StaticVertex> m_Vertices;
		std::vector<uint32_t> m_Inidices;

		std::shared_ptr<Buffer> m_VertexBuffer;
		std::shared_ptr<Buffer> m_IndexBuffer;
		
		BoundingBox m_LocalBoundingBox;
	};
}
