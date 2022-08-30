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

		// 릴리즈
		void Release();

		// 버퍼 설정 후 생성.
		bool CreateBuffer(unsigned int vertexCount, const std::vector<VertexElement>& elements, bool bDynamic = false);
		// 버퍼에 데이터 전달.
		bool SetData(void* pData);
		// 맵
		void* Map();
		// 언맵
		void Unmap();

		// 정점 버퍼 리턴.
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		// 정점 개수 리턴.
		unsigned int GetVertexCount() const { return m_VertexCount; }
		// 정점 크기 리턴.
		unsigned int GetVertexStride() const { return m_VertexStride; }
		// 정점 요소 벡터 리턴.
		const std::vector<VertexElement>& GetElements() const { return m_Elements; }
		// 다이나믹 버퍼 유무 리턴.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// 그래픽 시스템 포인터.
		Graphics* m_pGraphics;
		// 버퍼 포인터.
		ID3D11Buffer* m_pBuffer;
		// 정점 개수.
		unsigned int m_VertexCount;
		// 정점 크기.
		unsigned int m_VertexStride;
		// 버퍼 구성 요소.
		std::vector<VertexElement> m_Elements;
		// 다이나믹 버퍼 유무.
		bool m_bDynamic;
	};
}