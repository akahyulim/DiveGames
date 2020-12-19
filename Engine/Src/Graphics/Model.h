#pragma once
#include "Resource/Resource.h"


namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class Mesh;

	struct Vertex_PosTexNorTan;

	// ���� �޽�, ���۸� ������.
	// ���� ���������� �����ؾ� �Ѵ�.
	class Model : public Resource
	{
		DIVE_OBJECT(Model, Resource);

	public:
		Model(Context* context);
		~Model();

		// �ϴ� ���� ������ ��������.
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