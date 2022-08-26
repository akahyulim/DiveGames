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
		bool SetBuffer(unsigned int count, const std::vector<VertexElement>& elements, bool bDynamic = false);
		// ���ۿ� ������ ����.
		bool SetData(void* pData);
		// ��
		void* Map();
		// ���
		void Unmap();

		// ���� ���� ����.
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		// ���� ���� ����.
		unsigned int GetCount() const { return m_Count; }
		// ���� ũ�� ����.
		unsigned int GetStride() const { return m_Stride; }
		// ���̳��� ���� ���� ����.
		bool IsDynamic() const { return m_bDynamic; }
		
	private:
		// ���� ����.
		bool create();

	private:
		// �׷��� �ý��� ������.
		Graphics* m_pGraphics;
		// ���� ������.
		ID3D11Buffer* m_pBuffer;
		// ���� ����.
		unsigned int m_Count;
		// ���� ũ��.
		unsigned int m_Stride;
		// ���� ���� ���.
		std::vector<VertexElement> m_Elements;
		// ���̳��� ���� ����.
		bool m_bDynamic;
	};
}