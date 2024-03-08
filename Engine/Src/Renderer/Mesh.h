#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh : public Resource
	{
		DV_OBJECT(Mesh, Resource);

	public:
		Mesh() = default;
		Mesh(VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);
		Mesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);
		~Mesh() override;

		bool LoadFromFile(const std::string& fileName) override;
		bool SaveToFile(const std::string& fileName) override;

		VertexBuffer* GetVertexBuffer() const { return m_pVertexBuffer; }		
		void SetVertexBuffer(VertexBuffer* pBuffer) { m_pVertexBuffer = pBuffer; }

		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }
		void SetIndexBuffer(IndexBuffer* pBuffer) { m_pIndexBuffer = pBuffer; }

		void Draw(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	private:
		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;
	};

	Mesh* CreateMesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);
}