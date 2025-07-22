#include "stdafx.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	// 결국 indices를 std::vector<uint32_t>로 관리한 게 문제인 듯하다.
	bool IndexBuffer::Create(const void* data, uint32_t indexCount, uint32_t vertexCount)
	{
		m_Format = DXGI_FORMAT_R32_UINT;// (vertexCount <= 65535) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		uint32_t stride = sizeof(uint32_t);// m_Format == DXGI_FORMAT_R16_UINT ? sizeof(uint16_t) : sizeof(uint32_t);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = static_cast<UINT>(stride * indexCount);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = data;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &m_Buffer))) 
		{
			DV_LOG(IndexBuffer, err, "버퍼 생성 실패");
			return false;
		}

		m_Count = indexCount;

		return true;
	}
}
