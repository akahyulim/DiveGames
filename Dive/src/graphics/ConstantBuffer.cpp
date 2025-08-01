#include "stdafx.h"
#include "ConstantBuffer.h"
#include "Graphics.h"
#include "Common.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(ID3D11Device* device, eVSConstantBufferSlot slot, uint32_t stride)
		: m_vsSlot(slot)
		, m_shaderStage(eShaderStage::VertexShader)
	{
		assert(device);
		assert(slot < eVSConstantBufferSlot::Count);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(stride);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
		if (FAILED(hr))	DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	ConstantBuffer::ConstantBuffer(ID3D11Device* device, ePSConstantBufferSlot slot, uint32_t stride)
		: m_psSlot(slot)
		, m_shaderStage(eShaderStage::PixelShader)
	{
		assert(device);
		assert(slot < ePSConstantBufferSlot::Count);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(stride);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
		if (FAILED(hr))	DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	void ConstantBuffer::Bind(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);

		switch (m_shaderStage)
		{
		case eShaderStage::VertexShader:
			deviceContext->VSSetConstantBuffers(static_cast<UINT>(m_vsSlot), 1, m_buffer.GetAddressOf());
			break;
		case eShaderStage::PixelShader:
			deviceContext->PSSetConstantBuffers(static_cast<UINT>(m_psSlot), 1, m_buffer.GetAddressOf());
			break;
		default:
			assert(false && "지원하지 않는 셰이더 스테이지");
			break;
		}
	}
}
