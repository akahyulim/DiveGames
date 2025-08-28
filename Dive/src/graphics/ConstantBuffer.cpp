#include "stdafx.h"
#include "ConstantBuffer.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(eVSConstantBufferSlot slot, uint32_t stride)
		: m_vsSlot(slot)
		, m_shaderStage(eShaderStage::VertexShader)
	{
		if (slot == eVSConstantBufferSlot::Count)
		{
			DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] 잘못된 버퍼 슬롯으로 초기화");
			return;
		}

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(stride);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = Graphics::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
		if (FAILED(hr))	DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	ConstantBuffer::ConstantBuffer(ePSConstantBufferSlot slot, uint32_t stride)
		: m_psSlot(slot)
		, m_shaderStage(eShaderStage::PixelShader)
	{
		if (slot == ePSConstantBufferSlot::Count)
		{
			DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] 잘못된 버퍼 슬롯으로 초기화");
			return;
		}

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(stride);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = Graphics::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
		if (FAILED(hr))	DV_LOG(ConstantBuffer, err, "[::ConstantBuffer] CreateBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
	}

	void ConstantBuffer::Bind()
	{
		if (!m_buffer)
		{
			DV_LOG(ConstantBuffer, err, "[::Bind] 버퍼 미생성");
			return;
		}

		switch (m_shaderStage)
		{
		case eShaderStage::VertexShader:
			Graphics::GetDeviceContext()->VSSetConstantBuffers(static_cast<UINT>(m_vsSlot), 1, m_buffer.GetAddressOf());
			break;
		case eShaderStage::PixelShader:
			Graphics::GetDeviceContext()->PSSetConstantBuffers(static_cast<UINT>(m_psSlot), 1, m_buffer.GetAddressOf());
			break;
		default:
			DV_LOG(ConstantBuffer, err, "[::Bind] 잘못된 셰이더 스테이지 적용");
			break;
		}
	}
}
