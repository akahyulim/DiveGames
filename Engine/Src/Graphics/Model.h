#pragma once
#include "Resource/Resource.h"
#include "D3D11/VertexTypes.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"


namespace Dive
{
	class Context;
	class Mesh;


	// 단일 메시, 버퍼를 가진다.
	// 따라서 계층구조를 구현해야 한다.
	class Model : public Resource
	{
		DIVE_OBJECT(Model, Resource);

	public:
		Model(Context* context);
		~Model();

		void Clear();

		// 일단 단일 정보만 전달하자.
		void SetGeometry(std::vector<Vertex_PosTexNorTan>& vertices, std::vector<unsigned int>& indices);
		bool SetAndCreateGeometry(std::vector<Vertex_PosTexNorTan>& vertices, std::vector<unsigned int>& indices);

		// 일단 직접 전달하자.
		const ID3D11Buffer* GetVertexBuffer()	const { return m_vertexBuffer->GetBuffer(); }
		const ID3D11Buffer* GetIndexBuffer()	const { return m_indexBuffer->GetBuffer(); }

	private:
		bool createGeometryBuffers();

	private:
		// 버퍼를 직접 절달할 거라면 unique_ptr이 더 어울린다.
		// 다만 그렇게 할 경우 mesh와 같이 clear함수를 구현해야 한다.
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::unique_ptr<Mesh> m_mesh;
	
	};
}