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

		VertexBuffer* GetVertexBuffer(size_t index) const;
		bool SetNumVertexBuffers(size_t num);
		bool SetVertexBuffer(size_t index, VertexBuffer* pBuffer);

		bool SetDrawRange(D3D11_PRIMITIVE_TOPOLOGY primitiveType, unsigned int indexStart, unsigned int indexCount);
		bool SetDrawRange(D3D11_PRIMITIVE_TOPOLOGY primitiveType, unsigned int vertexStart, unsigned int vertexCount, unsigned int indexStart, unsigned int indexCount);

		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }
		void SetIndexBuffer(IndexBuffer* pBuffer);

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return m_PrimitiveType; }
		void SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY type) { m_PrimitiveType = type; }

		unsigned int GetVertexCount() const { return m_VertexCount; }
		unsigned int GetVertexStart() const { return m_VertexStart; }
		unsigned int GetIndexCount() const { return m_IndexCount; }
		unsigned int GetIndexStart() const { return m_IndexStart; }

	private:
	private:
		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType;
		std::vector<VertexBuffer*> m_VertexBuffers;
		IndexBuffer* m_pIndexBuffer;
		unsigned int m_VertexStart;
		unsigned int m_VertexCount;
		unsigned int m_IndexStart;
		unsigned int m_IndexCount;
	};
}