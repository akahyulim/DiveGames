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

		// 버퍼 릴리즈.
		void ReleaseBuffer();

		// 버퍼 크기 설정.
		bool SetSize(unsigned int vertexCount, const std::vector<VertexElement>& elements);

		// 버퍼 생성.
		bool CreateBuffer(unsigned int vertexCount, const std::vector<VertexElement>& elements, const void* pData = nullptr);
		// 버퍼에 데이터 전달.
		bool SetData(void* pData);
		// 맵
		void* Map();
		// 언맵
		void Unmap();

		// 정점 데이터 배열 포인터 리턴.
		unsigned char* GetShadowData() const { return m_pShadowData; }

		// 정점 버퍼 리턴.
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		// 정점 개수 리턴.
		unsigned int GetVertexCount() const { return m_VertexCount; }
		// 정점 크기 리턴.
		unsigned int GetVertexSize() const { return m_VertexSize; }
		// 정점 요소 벡터 리턴.
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		// 정점 요소 해시 리턴.
		unsigned long long GetElementHash() const { return m_ElementHash; }
		// 정점 버퍼 해시 리턴.
		unsigned long long GetBufferHash(unsigned int slot) const { return m_ElementHash << (slot * 16); }
		// 다이나믹 버퍼 유무 리턴.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// 그래픽 시스템 포인터.
		Graphics* m_pGraphics;
		// 정점 데이터 배열 포인터.
		unsigned char* m_pShadowData;
		// 버퍼 포인터.
		ID3D11Buffer* m_pBuffer;
		// 정점 개수.
		unsigned int m_VertexCount;
		// 정점 크기.
		unsigned int m_VertexSize;
		// 버퍼 구성 요소.
		std::vector<VertexElement> m_Elements;
		// 버퍼 구성 요소를 이용해 만든 해시.
		unsigned long long m_ElementHash;
		// 다이나믹 버퍼 유무.
		bool m_bDynamic;
	};
}