#include "stdafx.h"
#include "ConstantBuffer.h"
#include "core/CoreDefs.h"
#include "core/Engine.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer()
		: m_pBuffer(nullptr)
		, m_Stride(0)
	{
	}

	ConstantBuffer::ConstantBuffer(const std::string& name, uint32_t stride)
		: m_pBuffer(nullptr)
		, m_Name(name)
		, m_Stride(stride)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		Release();
	}

	bool ConstantBuffer::GenerateBuffer()
	{
		Release();

		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = m_Stride;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(GEngine->GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuffer)))
		{
			DV_LOG(ConstantBuffer, err, "ConstantBuffer({:s}) ������ �����Ͽ����ϴ�.", GetName());
			return false;
		}

		return true;
	}

	void ConstantBuffer::Release()
	{
		DV_RELEASE(m_pBuffer);
	}

	void* ConstantBuffer::Map()
	{
		DV_ASSERT(ConstantBuffer, m_pBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (FAILED(GEngine->GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pBuffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_LOG(ConstantBuffer, err, "ConstantBuffer({:s})�� Map�� �����Ͽ����ϴ�.", GetName());
			return nullptr;
		}

		return mappedData.pData;
	}

	void ConstantBuffer::Unmap()
	{
		GEngine->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}
}