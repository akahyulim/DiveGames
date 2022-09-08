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

		// ���� ������.
		void ReleaseBuffer();

		// ���� ũ�� ����.
		bool SetSize(unsigned int vertexCount, const std::vector<VertexElement>& elements);

		// ���� ����.
		bool CreateBuffer(unsigned int vertexCount, const std::vector<VertexElement>& elements, const void* pData = nullptr);
		// ���ۿ� ������ ����.
		bool SetData(void* pData);
		// ��
		void* Map();
		// ���
		void Unmap();

		// ���� ������ �迭 ������ ����.
		unsigned char* GetShadowData() const { return m_pShadowData; }

		// ���� ���� ����.
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		// ���� ���� ����.
		unsigned int GetVertexCount() const { return m_VertexCount; }
		// ���� ũ�� ����.
		unsigned int GetVertexSize() const { return m_VertexSize; }
		// ���� ��� ���� ����.
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		// ���� ��� �ؽ� ����.
		unsigned long long GetElementHash() const { return m_ElementHash; }
		// ���� ���� �ؽ� ����.
		unsigned long long GetBufferHash(unsigned int slot) const { return m_ElementHash << (slot * 16); }
		// ���̳��� ���� ���� ����.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// �׷��� �ý��� ������.
		Graphics* m_pGraphics;
		// ���� ������ �迭 ������.
		unsigned char* m_pShadowData;
		// ���� ������.
		ID3D11Buffer* m_pBuffer;
		// ���� ����.
		unsigned int m_VertexCount;
		// ���� ũ��.
		unsigned int m_VertexSize;
		// ���� ���� ���.
		std::vector<VertexElement> m_Elements;
		// ���� ���� ��Ҹ� �̿��� ���� �ؽ�.
		unsigned long long m_ElementHash;
		// ���̳��� ���� ����.
		bool m_bDynamic;
	};
}