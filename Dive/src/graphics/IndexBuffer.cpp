#include "stdafx.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	IndexBuffer::~IndexBuffer()
	{
		Release();
	}

	bool IndexBuffer::Create(const void* data, UINT32 count)
	{
		Release();

		const bool use32BitFormat = count > 65535;

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = (use32BitFormat ? sizeof(UINT32) : sizeof(UINT16)) * count;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = data;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &m_Buffer))) 
		{
			DV_LOG(IndexBuffer, err, "IndexBuffer 생성에 실패하였습니다.");
			return false;
		}

		m_Format = use32BitFormat ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		m_Count = count;

		return true;
	}

	void IndexBuffer::Release()
	{
		DV_RELEASE(m_Buffer);
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Generate(const void* data, UINT32 count)
	{
		auto indexBuffer = std::make_shared<IndexBuffer>();
		if (!indexBuffer->Create(data, count)) 
			return nullptr;

		return indexBuffer;
	}
}
