#pragma once

namespace Dive
{
	// 타입을 하나로 고정해 놓았다.
	struct VertexType
	{
		VertexType() = default;
		VertexType(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT3& nor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			const DirectX::XMFLOAT3& tan = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
		)
		{
			this->position[0] = pos.x;
			this->position[1] = pos.y;
			this->position[2] = pos.z;

			this->texCoords[0] = tex.x;
			this->texCoords[1] = tex.y;

			this->normal[0] = nor.x;
			this->normal[1] = nor.y;
			this->normal[2] = nor.z;

			this->tangent[0] = tan.x;
			this->tangent[1] = tan.y;
			this->tangent[2] = tan.z;
		}

		float position[3] = { 0.0f };
		float texCoords[2] = { 0.0f };
		float normal[3] = { 0.0f };
		float tangent[3] = { 0.0f };
	};

	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() { Clear(); }

		void Clear();

		void GetGeometry(unsigned int vertexOffset, unsigned int vertexCount, std::vector<VertexType>* pOutVertices,
			unsigned int indexOffset, unsigned int indexCount, std::vector<unsigned int>* pOutIndices);

		void AddVertex(const VertexType& vertex);
		void AppendVertices(const std::vector<VertexType>& vertices, unsigned int* pOutOffset);
		std::vector<VertexType>& GetVertices() { return m_Vertices; }
		void SetVertices(const std::vector<VertexType>& vertices) { m_Vertices = vertices; }
		unsigned int GetVertexCount() const { return static_cast<unsigned int>(m_Vertices.size()); }

		void AddIndex(unsigned int index);
		void AppendIndices(const std::vector<unsigned int>& indices, unsigned int* pOutOffset);
		std::vector<unsigned int>& GetIndices() { return m_Indices; }
		void SetIndices(const std::vector<unsigned int>& indices) { m_Indices = indices; }
		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_Indices.size()); }

		unsigned int GetTriangleCount() const { return GetIndexCount() / 3; }

	private:
		std::vector<VertexType> m_Vertices;
		std::vector<unsigned int> m_Indices;
	};
}