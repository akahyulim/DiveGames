#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	// dx11������ skinning�� compute shader���� �����Ѵ�.
	// ������ compute shader ����� �𸥴ٴ� �Ͱ�
	// animation�� ��� shader�� �����ϴ��� �ϴ� ���̴�. ������ constant buffer�ϱ�?

	struct DIVE_VERTEX
	{
		// ������ ������ ����� �δ°� ����.

		float positions[3];
		float colors[4];
		float texCoords[2];
		float normals[3];
		float tangnts[3];
		float boneWeights[4];			// ���� 4���� ���� �ʿ䰡 ����. 1.0 - 3���� ������ ���� ������ �����̴�.
		unsigned int boneIndices[4];
	};

	// �ƿ� mesh���� rigid�� skinned�� �����ϴ°� ���?
	// �ƴϸ� ��� �� �� �ϳ���� ǥ���� �ް� �־�� �Ѵ�.
	class Dive_Mesh
	{
	public:
		Dive_Mesh();
		~Dive_Mesh();

		void Clear();

		void AppendVertice(const std::vector<DIVE_VERTEX>& vertices, unsigned int* offset);
		void AppendIndices(const std::vector<unsigned int>& indices, unsigned int* offset);

		std::vector<DIVE_VERTEX>& GetVertices() { return m_vertices; }
		void SetVertices(const std::vector<DIVE_VERTEX>& vertices) { m_vertices = vertices; }

		std::vector<unsigned int>& GetIndices() { return m_indices; }
		void SetIndices(const std::vector<unsigned int>& indices) { m_indices = indices; }

		unsigned int GetVertexCount() const;
		unsigned int GetIndexCount() const;
		unsigned int GetTriangleCount() const;

	private:
		std::vector<DIVE_VERTEX> m_vertices;
		std::vector<unsigned int> m_indices;

	};

	// ������ prefab�� �򰥸���. �ٸ� resource����� �޸� game object�� ���� �����ϱ� �����̴�.
	// �׷��� ������ �� Ȥ�� �ϵ� �ڵ��� mesh�� ����ҰŶ�� prefab�� ���еǾ�� �Ѵ�.
	// �̴� load from file���� Ȯ���� �� ���� ������?

	// �ϴ� �ϳ��� ��������� ���� rigid�� skinned�� �������� �� �� �ִ�.
	// ���� ��� ���⿣ ���� ���� �κ��� override�ϴ� derrived�� ����� �� �� ����.
	class Dive_MeshFilter : public Resource
	{
		DIVE_OBJECT(Dive_MeshFilter, Resource);

	public:
		Dive_MeshFilter(Context* context);
		~Dive_MeshFilter();

		bool LoadFromFile(const std::string& filepath) override;
		bool SaveToFile(const std::string& filepath) override;

		void AppendGeometry(const std::vector<DIVE_VERTEX>& vertices, const std::vector<unsigned int>& indices,
			unsigned int* vertexOffset = nullptr, unsigned int* indexOffset = nullptr);

		const VertexBuffer* GetVertexBuffer() const { return m_vertexBuffer.get(); }
		const IndexBuffer* GetIndexBuffer() const { return m_indexBuffer.get(); }

	private:
		bool createBuffers();

	private:
		std::shared_ptr<Dive_Mesh> m_mesh;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};
}