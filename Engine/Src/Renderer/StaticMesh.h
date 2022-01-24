#pragma once
#include "dvMesh.h"
#include "VertexTypes.h"

namespace DiveEngine
{
	class StaticMesh : public dvMesh
	{
	public:
		StaticMesh();
		~StaticMesh();

		std::vector<Vertex_StaticMesh>& GetVertices() { return m_vertices; }

		void SetVertices(const std::vector<Vertex_StaticMesh>& vertices);

		// override
		bool CreateBuffers(ID3D11Device* pDevice) override;
		void Clear() override;

		uint32_t GetVertexCount() override { return m_vertices.size(); }

		size_t GetVertexStride() override { return sizeof(Vertex_StaticMesh); }

		DXGI_FORMAT GetIndexFormat() override { return m_vertices.size() > 65535 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT; }
		size_t GetIndexStride() override { return (GetIndexFormat() == DXGI_FORMAT_R32_UINT) ? sizeof(uint32_t) : sizeof(uint16_t); }
	
	private:

	private:
		std::vector<Vertex_StaticMesh> m_vertices;
	};
}