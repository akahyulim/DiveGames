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
		bool SetBuffer(unsigned int count, const std::vector<VertexElement>& elements, bool bDynamic = false);
		// 버퍼에 데이터 전달.
		bool SetData(void* pData);
		// 맵
		void* Map();
		// 언맵
		void Unmap();

		// 정점 버퍼 리턴.
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		// 정점 개수 리턴.
		unsigned int GetCount() const { return m_Count; }
		// 정점 크기 리턴.
		unsigned int GetStride() const { return m_Stride; }
		// 다이나믹 버퍼 유무 리턴.
		bool IsDynamic() const { return m_bDynamic; }
		
	private:
		// 버퍼 생성.
		bool create();

	private:
		// 그래픽 시스템 포인터.
		Graphics* m_pGraphics;
		// 버퍼 포인터.
		ID3D11Buffer* m_pBuffer;
		// 정점 개수.
		unsigned int m_Count;
		// 정점 크기.
		unsigned int m_Stride;
		// 버퍼 구성 요소.
		std::vector<VertexElement> m_Elements;
		// 다이나믹 버퍼 유무.
		bool m_bDynamic;
	};
}