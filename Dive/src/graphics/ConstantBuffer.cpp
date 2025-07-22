#include "stdafx.h"
#include "ConstantBuffer.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	DvConstantBuffer::DvConstantBuffer(ID3D11Device* device, eVSConstantBufferSlot slot, size_t size)
		: m_Slot(static_cast<UINT>(slot))
		, m_Stage(eShaderStage::VertexShader)
	{
		assert(device);
		assert(slot < eVSConstantBufferSlot::Count);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(size);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer)))
			DV_LOG(ConstantBuffer, err, "버퍼 생성 실패");
	}

	DvConstantBuffer::DvConstantBuffer(ID3D11Device* device, ePSConstantBufferSlot slot, size_t size)
		: m_Slot(static_cast<UINT>(slot))
		, m_Stage(eShaderStage::PixelShader)
	{
		assert(device);
		assert(slot < ePSConstantBufferSlot::Count);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(size);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer)))
			DV_LOG(ConstantBuffer, err, "버퍼 생성 실패");
	}

	void DvConstantBuffer::Bind(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);

		switch (m_Stage)
		{
		case eShaderStage::VertexShader:
			deviceContext->VSSetConstantBuffers(static_cast<UINT>(m_Slot), 1, m_Buffer.GetAddressOf());
			break;
		case eShaderStage::PixelShader:
			deviceContext->PSSetConstantBuffers(static_cast<UINT>(m_Slot), 1, m_Buffer.GetAddressOf());
			break;
		default:
			break;
		}
	}

	// ====================================================================================================================

	ConstantBuffer::~ConstantBuffer()
	{
		Release();
	}

	bool ConstantBuffer::Create(UINT32 stride)
	{
		Release();

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = m_Stride = stride;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_Buffer)))
		{
			DV_LOG(ConstantBuffer, err, "버퍼 생성 실패");
			return false;
		}

		return true;
	}

	void ConstantBuffer::Release()
	{
		DV_RELEASE(m_Buffer);
	}

	void* ConstantBuffer::Map()
	{
		assert(m_Buffer);

		D3D11_MAPPED_SUBRESOURCE mappedData{};
		if (FAILED(Graphics::GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_Buffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_LOG(ConstantBuffer, err, "버퍼 Map 실패");
			return nullptr;
		}

		return mappedData.pData;
	}

	void ConstantBuffer::Unmap()
	{
		Graphics::GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_Buffer), 0);
	}

	std::shared_ptr<ConstantBuffer> ConstantBuffer::Generate(UINT32 stride)
	{
		auto constantBuffer = std::make_shared<ConstantBuffer>();
		if (!constantBuffer->Create(stride))
			return nullptr;

		return constantBuffer;
	}
}
