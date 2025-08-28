#include "stdafx.h"
#include "Buffer.h"
#include "Graphics.h"

namespace Dive
{
	Buffer::Buffer(eBufferType type, const void* rawData, uint32_t stride, uint32_t count)
		: m_type(type)
		, m_stride(stride)
		, m_count(count)
	{
		assert(rawData);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = static_cast<UINT>(stride * count);
		bufferDesc.BindFlags = type == eBufferType::VertexBuffer ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = rawData;

		HRESULT hr = Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &m_buffer);
		if (FAILED(hr))	DV_LOG(VertexBuffer, err, "[::Buffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	Buffer::~Buffer()
	{
		m_buffer.Reset();
	}

	void Buffer::Bind() const
	{
		if (m_type == eBufferType::VertexBuffer)
		{
			UINT offsets = 0;
			Graphics::GetDeviceContext()->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), &m_stride, &offsets);
		}
		else if (m_type == eBufferType::IndexBuffer)
		{
			Graphics::GetDeviceContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
	}
}