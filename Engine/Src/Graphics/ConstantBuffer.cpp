#include "divepch.h"
#include "ConstantBuffer.h"

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

		if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuffer)))
		{
			DV_ENGINE_ERROR("ConstantBuffer({:s}) 생성에 실패하였습니다.", GetName());
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
		DV_ENGINE_ASSERT(m_pBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (FAILED(Graphics::GetInstance()->GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pBuffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_ENGINE_ERROR("ConstantBuffer({:s})의 Map에 실패하였습니다.", GetName());
			return nullptr;
		}

		return mappedData.pData;
	}

	void ConstantBuffer::Unmap()
	{
		Graphics::GetInstance()->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}
}