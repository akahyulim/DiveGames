#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	bool VertexBuffer::Create(const void* data, uint32_t stride, uint32_t count)
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = static_cast<UINT>(stride * count);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = data;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &m_Buffer)))
		{
			DV_LOG(VertexBuffer, err, "Failed to create vertex buffer");
			return false;
		}

		m_Stride = stride;
		m_Count = count;

		return true;
	}
}
