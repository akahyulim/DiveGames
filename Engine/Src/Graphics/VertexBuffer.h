#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class Graphics;

	class VertexBuffer : public Object
	{
		DIVE_OBJECT(VertexBuffer, Object)

	public:
		VertexBuffer(Context* pContext);
		~VertexBuffer() override;

		// ������
		void Release();

		// ���� ���� �� ����.
		bool CreateBuffer(unsigned int vertexCount, const std::vector<VertexElement>& elements, bool bDynamic = false);
		// ���ۿ� ������ ����.
		bool SetData(void* pData);
		// ��
		void* Map();
		// ���
		void Unmap();

		// ���� ���� ����.
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		// ���� ���� ����.
		unsigned int GetVertexCount() const { return m_VertexCount; }
		// ���� ũ�� ����.
		unsigned int GetVertexStride() const { return m_VertexStride; }
		// ���� ��� ���� ����.
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		// ���̳��� ���� ���� ����.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// �׷��� �ý��� ������.
		Graphics* m_pGraphics;
		// ���� ������.
		ID3D11Buffer* m_pBuffer;
		// ���� ����.
		unsigned int m_VertexCount;
		// ���� ũ��.
		unsigned int m_VertexStride;
		// ���� ���� ���.
		std::vector<VertexElement> m_Elements;
		// ���̳��� ���� ����.
		bool m_bDynamic;
	};
}