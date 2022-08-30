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
		bool CreateBuffer(unsigned int indexCount, bool bLargeIndices, bool bDynamic = false);
		// ���ۿ� ������ ����.
		bool SetData(void* pData);
		// ��.
		void* Map();
		// ���.
		void Unmap();

		// �ε��� ���� ����.
		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		// �ε��� ���� ����.
		unsigned int GetIndexCount() const { return m_IndexCount; }
		// �ε��� ũ�� ����.
		unsigned int GetStride() const { return m_Stride; }
		// �ε��� ���� ����.
		DXGI_FORMAT GetFormat() const { return m_Stride == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		// ���̳��� ���� ���� ����.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// �׷��Ƚ� �ý��� ������.
		Graphics* m_pGraphics;
		// ���� ������.
		ID3D11Buffer * m_pBuffer;
		// �ε��� ����.
		unsigned int m_IndexCount;
		// �ε��� ũ��.
		unsigned int m_Stride;
		// ���̳��� ���� ����.
		bool m_bDynamic;
	};
}