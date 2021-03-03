#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	// dx11에서는 skinning을 compute shader에서 수행한다.
	// 문제는 compute shader 방법을 모른다는 것과
	// animation을 어떻게 shader에 전달하느냐 하는 것이다. 여전히 constant buffer일까?

	struct DIVE_VERTEX
	{
		// 생성자 정도는 만들어 두는게 낫다.

		float positions[3];
		float colors[4];
		float texCoords[2];
		float normals[3];
		float tangnts[3];
		float boneWeights[4];			// 굳이 4개를 가질 필요가 없다. 1.0 - 3개면 나머지 값이 나오기 때문이다.
		unsigned int boneIndices[4];
	};

	// 아에 mesh부터 rigid와 skinned로 구분하는건 어떨까?
	// 아니면 적어도 둘 중 하나라는 표식은 달고 있어야 한다.
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

	// 여전히 prefab과 헷갈린다. 다른 resource들과는 달리 game object를 직접 생성하기 때문이다.
	// 그런데 프리셋 모델 혹은 하드 코딩된 mesh를 사용할거라면 prefab과 구분되어야 한다.
	// 이는 load from file에서 확인할 수 있지 않을까?

	// 일단 하나로 만들었지만 추후 rigid와 skinned로 나눠져야 할 수 있다.
	// 현재 언듯 보기엔 버퍼 생성 부분은 override하는 derrived로 만들면 될 것 같다.
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