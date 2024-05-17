#include "divepch.h"
#include "ConstantBuffer.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(const std::string& name, eShaderType type, uint32_t slot)
		: m_Name(name)
		, m_ShaderType(type)
		, m_Slot(slot)
		, m_pBuffer(nullptr)
		, m_Stride(0)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	void ConstantBuffer::Update(void* pData)
	{
		DV_ENGINE_ASSERT(m_pBuffer);
		DV_ENGINE_ASSERT(pData);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (FAILED(Graphics::GetInstance()->GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pBuffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedData)))
		{
			DV_ENGINE_ERROR("ConstantBuffer({:s})의 Map에 실패하였습니다.", GetName());
			return;
		}

		memcpy(mappedData.pData, pData, m_Stride);

		Graphics::GetInstance()->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}

	// start slot도 변수로 관리해야 하나?
	void ConstantBuffer::Bind()
	{
		auto pDeviceContext = Graphics::GetInstance()->GetDeviceContext();

		switch (m_ShaderType)
		{
		case eShaderType::Vertex:
			pDeviceContext->VSSetConstantBuffers(m_Slot, 1, &m_pBuffer);
			return;
		case eShaderType::Pixel:
			pDeviceContext->PSSetConstantBuffers(m_Slot, 1, &m_pBuffer);
			return;
		default:
			DV_ENGINE_ERROR("셰이더 타입을 불분명해 바인딩할 수 없습니다.");
			return;
		}
	}
}