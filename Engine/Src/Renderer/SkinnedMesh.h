#pragma once
#include "Mesh.h"

namespace Dive
{
	class SkinnedMesh : public IMesh
	{
	public:
		SkinnedMesh();
		virtual ~SkinnedMesh();

		void AppendVertices(const std::vector<VertexDynamic>& vertices, uint32_t* pOffset = nullptr);
		void AppendIndices(const std::vector<uint32_t>& indices, uint32_t* pOffset = nullptr);

		uint32_t GetNumVertices() const override { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetNumIndices() const override { return static_cast<uint32_t>(m_Indices.size()); }

	private:
	private:
		std::vector<VertexDynamic> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}