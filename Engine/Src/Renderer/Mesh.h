#pragma once
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class IMesh
	{
	public:
		virtual ~IMesh() {}

		virtual uint32_t GetNumVertices() const = 0;
		virtual uint32_t GetNumIndices() const = 0;
	};

	class StaticMesh : public IMesh
	{
	public:
		StaticMesh();
		virtual ~StaticMesh();

		void AppendVertices(const std::vector<VertexStatic>& vertices, uint32_t* pOffset = nullptr);
		void AppendIndices(const std::vector<uint32_t>& indices, uint32_t* pOffset = nullptr);

		uint32_t GetNumVertices() const override { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetNumIndices() const override { return static_cast<uint32_t>(m_Indices.size()); }

	private:
	private:
		std::vector<VertexStatic> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}