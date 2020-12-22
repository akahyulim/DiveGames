#pragma once
#include "Resource/Resource.h"
#include "D3D11/VertexTypes.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"


namespace Dive
{
	class Context;
	class Mesh;


	// ���� �޽�, ���۸� ������.
	// ���� ���������� �����ؾ� �Ѵ�.
	class Model : public Resource
	{
		DIVE_OBJECT(Model, Resource);

	public:
		Model(Context* context);
		~Model();

		void Clear();

		// �ϴ� ���� ������ ��������.
		void SetGeometry(std::vector<Vertex_PosTexNorTan>& vertices, std::vector<unsigned int>& indices);
		bool SetAndCreateGeometry(std::vector<Vertex_PosTexNorTan>& vertices, std::vector<unsigned int>& indices);

		// �ϴ� ���� ��������.
		const ID3D11Buffer* GetVertexBuffer()	const { return m_vertexBuffer->GetBuffer(); }
		const ID3D11Buffer* GetIndexBuffer()	const { return m_indexBuffer->GetBuffer(); }

	private:
		bool createGeometryBuffers();

	private:
		// ���۸� ���� ������ �Ŷ�� unique_ptr�� �� ��︰��.
		// �ٸ� �׷��� �� ��� mesh�� ���� clear�Լ��� �����ؾ� �Ѵ�.
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<Mesh> m_mesh;
	
	};
}