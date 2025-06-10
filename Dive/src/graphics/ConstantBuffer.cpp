#include "stdafx.h"
#include "ConstantBuffer.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	ConstantBuffer::~ConstantBuffer()
	{
		Release();
	}

	bool ConstantBuffer::Create(UINT32 stride)
	{
		Release();

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = m_Stride;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_Buffer)))
		{
			DV_LOG(ConstantBuffer, err, "ConstantBuffer 생성에 실패하였습니다.");
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
		DV_ASSERT(ConstantBuffer, m_Buffer);

		D3D11_MAPPED_SUBRESOURCE mappedData{};
		if (FAILED(Graphics::GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_Buffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_LOG(ConstantBuffer, err, "ConstantBuffer의 Map에 실패");
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
