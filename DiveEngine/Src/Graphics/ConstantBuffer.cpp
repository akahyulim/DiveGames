#include "DivePch.h"
#include "ConstantBuffer.h"

namespace Dive 
{
	ConstantBuffer::ConstantBuffer(const std::string& name)
		: m_pBuffer(nullptr),
		m_Stride(0)
	{
		SetName(name);
	}

	ConstantBuffer::~ConstantBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	void* ConstantBuffer::Map()
	{
		DV_ASSERT(Graphics::IsInitialized());

		if(!m_pBuffer)
			return nullptr;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (FAILED(Graphics::GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pBuffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_CORE_ERROR("ConstantBuffer({:s})의 Map에 실패하였습니다.", m_Name);
			return nullptr;
		}

		return mappedData.pData;
	}

	void ConstantBuffer::Unmap()
	{
		DV_ASSERT(Graphics::IsInitialized());

		if (!m_pBuffer)
			return;

		Graphics::GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}
}