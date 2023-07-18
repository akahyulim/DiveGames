#pragma once
#include "Mesh.h"

namespace Dive
{
	class SkinnedMesh : public IMesh
	{
	public:
		SkinnedMesh() = default;
		SkinnedMesh(const std::string & name, std::vector<VertexSkinned>&vertices, std::vector<uint32_t>&indices);
		SkinnedMesh(const std::string& nodeName, const std::string& name, std::vector<VertexSkinned>& vertices, std::vector<uint32_t>& indices);
		virtual ~SkinnedMesh();

		void CreateVertexBuffer() override;
		void CreateIndexBuffer() override;

		uint32_t GetNumVertices() const override { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetNumIndices() const override { return static_cast<uint32_t>(m_Indices.size()); }

	private:
	private:
		std::vector<VertexSkinned> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}