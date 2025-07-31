#include "stdafx.h"
#include "Buffer.h"

namespace Dive
{
	Buffer::Buffer(ID3D11Device* device, eBufferType type, const void* rawData, uint32_t stride, uint32_t count)
		: m_type(type)
		, m_stride(stride)
		, m_count(count)
	{
		assert(device);
		assert(rawData);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = static_cast<UINT>(stride * count);
		bufferDesc.BindFlags = type == eBufferType::VertexBuffer ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = rawData;

		HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, &m_buffer);
		if (FAILED(hr))	DV_LOG(VertexBuffer, err, "[::Buffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	Buffer::~Buffer()
	{
		m_buffer.Reset();
	}

	void Buffer::Bind(ID3D11DeviceContext* deviceContext) const
	{
		assert(deviceContext);

		if (m_type == eBufferType::VertexBuffer)
		{
			UINT offsets = 0;
			deviceContext->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), &m_stride, &offsets);
		}
		else if (m_type == eBufferType::IndexBuffer)
		{
			deviceContext->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
	}
}