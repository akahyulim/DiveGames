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

		// 버퍼 릴리즈.
		void ReleaseBuffer();

		// 버퍼 크기 설정.
		bool SetSize(unsigned int indexCount, bool bLargeIndices);

		// 버퍼 생성.
		bool CreateBuffer(unsigned int indexCount, bool bLargeIndices, const void* pData = nullptr);
		// 버퍼 갱신.
		bool SetData(const void* pData);
		// 맵.
		void* Map();
		// 언맵.
		void Unmap();

		// 인덱스 데이터 배열 포인터 리턴.
		unsigned char* GetShadowData() const { return m_pShadowData; }

		// 인덱스 버퍼 리턴.
		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		// 인덱스 개수 리턴.
		unsigned int GetIndexCount() const { return m_IndexCount; }
		// 인덱스 크기 리턴.
		unsigned int GetIndexSize() const { return m_IndexSize; }
		// 인덱스 포멧 리턴.
		DXGI_FORMAT GetFormat() const { return m_IndexSize == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		// 다이나믹 버퍼 여부 리턴.
		bool IsDynamic() const { return m_bDynamic; }

	private:
		// 그래픽스 시스템 포인터.
		Graphics* m_pGraphics;
		// 인덱스 데이터 배열.
		unsigned char* m_pShadowData;
		// 버퍼 포인터.
		ID3D11Buffer * m_pBuffer;
		// 인덱스 개수.
		unsigned int m_IndexCount;
		// 인덱스 크기.
		unsigned int m_IndexSize;
		// 다이나믹 버퍼 유무.
		bool m_bDynamic;
	};
}