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

		bool SetNumVertexBuffers(unsigned int num);
		bool SetVertexBuffer(unsigned int index, VertexBuffer* pBuffer);
		VertexBuffer* GetVertexBuffer(unsigned int index) const;

		// ����, �ε����� ����, ��������
		// SetDrawRange��� �Լ��� ���� �����ȴ�.
		// �̴� Model���� �����ϴ� ��?

		void SetIndexBuffer(IndexBuffer* pBuffer);
		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }

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