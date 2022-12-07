#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;
	class VertexBuffer;
	class IndexBuffer;

	class Mesh : public Object
	{
		DIVE_OBJECT(Mesh, Object)

	public:
		explicit Mesh(Context* pContext);
		~Mesh() override;

		void Draw(Graphics* pGraphics);

		bool SetDrawRange(unsigned int vertexStart, unsigned int vertexCount, unsigned int indexStart, unsigned int indexCount);

		VertexBuffer* GetVertexBuffer() const { return m_pVertexBuffer; }
		void SetVertexBuffer(VertexBuffer* pBuffer);

		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }
		void SetIndexBuffer(IndexBuffer* pBuffer);

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return m_PrimitiveType; }
		void SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY type) { m_PrimitiveType = type; }

		unsigned int GetVertexCount() const { return m_VertexCount; }
		unsigned int GetVertexStart() const { return m_VertexStart; }
		unsigned int GetIndexCount() const { return m_IndexCount; }
		unsigned int GetIndexStart() const { return m_IndexStart; }

		float GetLodDistance() const { return m_LodDistance; }
		void SetLodDistance(float distance);

	private:
	private:
		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		VertexBuffer* m_pVertexBuffer = nullptr;
		IndexBuffer* m_pIndexBuffer = nullptr;
		unsigned int m_VertexStart = 0;
		unsigned int m_VertexCount = 0;
		unsigned int m_IndexStart = 0;
		unsigned int m_IndexCount = 0;
		float m_LodDistance = 0.0f;
	};
}