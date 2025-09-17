#include "stdafx.h"
#include "ConstantBuffer.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(uint32_t stride)
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(stride);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = Graphics::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
		if (FAILED(hr))	DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	void ConstantBuffer::BindVS(eCBufferSlotVS slot)
	{
		assert(m_buffer);
		assert(slot < eCBufferSlotVS::Count);

		Graphics::GetDeviceContext()->VSSetConstantBuffers(static_cast<UINT>(slot), 1, m_buffer.GetAddressOf());
	}
	
	void ConstantBuffer::BindPS(eCBufferSlotPS slot)
	{
		assert(m_buffer);
		assert(slot < eCBufferSlotPS::Count);

		Graphics::GetDeviceContext()->PSSetConstantBuffers(static_cast<UINT>(slot), 1, m_buffer.GetAddressOf());
	}
}
