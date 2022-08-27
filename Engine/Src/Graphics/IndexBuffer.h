#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;

	class IndexBuffer : public Object
	{
		DIVE_OBJECT(IndexBuffer, Object)
	
	public:
		explicit IndexBuffer(Context* pContext);
		~IndexBuffer() override;

		// ������
		void Release();
		
		// ���� ���� �� ����.
		bool CreateBuffer(unsigned int count, bool bLargeIndices, bool bDynamic = false);
		// ���ۿ� ������ ����.
		bool SetData(void* pData);
		// ��.
		void* Map();
		// ���.
		void Unmap();

		// �ε��� ���� ����.
		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		// �ε��� ���� ����.
		unsigned int GetCount() const { return m_Count; }
		// �ε��� ũ�� ����.
		unsigned int GetStride() const { return m_Stride; }
		// ���̳��� ���� ���� ����.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// �׷��Ƚ� �ý��� ������.
		Graphics* m_pGraphics;
		// ���� ������.
		ID3D11Buffer * m_pBuffer;
		// �ε��� ����.
		unsigned int m_Count;
		// �ε��� ũ��.
		unsigned int m_Stride;
		// ���̳��� ���� ����.
		bool m_bDynamic;
	};
}