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

		// ���� ������.
		void ReleaseBuffer();

		// ���� ũ�� ����.
		bool SetSize(unsigned int indexCount, bool bLargeIndices);

		// ���� ����.
		bool CreateBuffer(unsigned int indexCount, bool bLargeIndices, const void* pData = nullptr);
		// ���� ����.
		bool SetData(const void* pData);
		// ��.
		void* Map();
		// ���.
		void Unmap();

		// �ε��� ������ �迭 ������ ����.
		unsigned char* GetShadowData() const { return m_pShadowData; }

		// �ε��� ���� ����.
		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		// �ε��� ���� ����.
		unsigned int GetIndexCount() const { return m_IndexCount; }
		// �ε��� ũ�� ����.
		unsigned int GetIndexSize() const { return m_IndexSize; }
		// �ε��� ���� ����.
		DXGI_FORMAT GetFormat() const { return m_IndexSize == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		// ���̳��� ���� ���� ����.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// �׷��Ƚ� �ý��� ������.
		Graphics* m_pGraphics;
		// �ε��� ������ �迭.
		unsigned char* m_pShadowData;
		// ���� ������.
		ID3D11Buffer * m_pBuffer;
		// �ε��� ����.
		unsigned int m_IndexCount;
		// �ε��� ũ��.
		unsigned int m_IndexSize;
		// ���̳��� ���� ����.
		bool m_bDynamic;
	};
}