#pragma once
#include "Resource/Resource.h"


namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class Mesh;

	struct Vertex_PosTexNorTan;

	// 단일 메시, 버퍼를 가진다.
	// 따라서 계층구조를 구현해야 한다.
	class Model : public Resource
	{
		DIVE_OBJECT(Model, Resource);

	public:
		Model(Context* context);
		~Model();

		// 일단 단일 정보만 전달하자.
		void SetGeometry(std::vector<Vertex_PosTexNorTan>& vertices, std::vector<unsigned int>& indices);

		const std::shared_ptr<VertexBuffer>& GetVertexBuffer()	const { return m_vertexBuffer; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer()	const { return m_indexBuffer; }

	private:
		bool createGeometryBuffers();

	private:
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<Mesh> m_mesh;
	
	};
}