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

		// 릴리즈
		void Release();
		
		// 버퍼 설정 후 생성.
		bool CreateBuffer(unsigned int indexCount, bool bLargeIndices, bool bDynamic = false);
		// 버퍼에 데이터 전달.
		bool SetData(void* pData);
		// 맵.
		void* Map();
		// 언맵.
		void Unmap();

		// 인덱스 버퍼 리턴.
		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		// 인덱스 개수 리턴.
		unsigned int GetIndexCount() const { return m_IndexCount; }
		// 인덱스 크기 리턴.
		unsigned int GetStride() const { return m_Stride; }
		// 인덱스 포멧 리턴.
		DXGI_FORMAT GetFormat() const { return m_Stride == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		// 다이나믹 버퍼 여부 리턴.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// 그래픽스 시스템 포인터.
		Graphics* m_pGraphics;
		// 버퍼 포인터.
		ID3D11Buffer * m_pBuffer;
		// 인덱스 개수.
		unsigned int m_IndexCount;
		// 인덱스 크기.
		unsigned int m_Stride;
		// 다이나믹 버퍼 유무.
		bool m_bDynamic;
	};
}