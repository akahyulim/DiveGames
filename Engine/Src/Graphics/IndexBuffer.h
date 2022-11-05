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

		bool SetSize(unsigned int indexCount, bool bLargeIndices, bool bDynamic = false);
		bool SetData(const void* pData);

		void* Map();
		void Unmap();

		unsigned char* GetData() const { return m_pData; }

		ID3D11Buffer* GetBuffer() { return m_pBuffer.Get(); }
		unsigned int GetIndexCount() const { return m_IndexCount; }
		unsigned int GetIndexSize() const { return m_IndexSize; }
		DXGI_FORMAT GetFormat() const { return m_IndexSize == sizeof(unsigned short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }
		bool IsDynamic() const { return m_bDynamic; }

	private:
		Graphics* m_pGraphics = nullptr;
		unsigned char* m_pData = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
		unsigned int m_IndexCount = 0;
		unsigned int m_IndexSize = 0;
		bool m_bDynamic = false;
	};
}