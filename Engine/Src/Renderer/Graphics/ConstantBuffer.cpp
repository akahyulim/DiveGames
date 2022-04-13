#include "divepch.h"
#include "ConstantBuffer.h"
#include "GraphicsDevice.h"
#include "Renderer/Renderer.h"
#include "Base/Base.h"


namespace Dive
{
	void ConstantBuffer::Destroy()
	{
		DV_RELEASE(m_pBuffer);
	}

	void* ConstantBuffer::Map()
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		DV_ASSERT(m_pBuffer != nullptr);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(pImmediateContext->Map(static_cast<ID3D11Resource*>(m_pBuffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			DV_CORE_ERROR("Constant Buffer Map에 실패하였습니다.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	void ConstantBuffer::Unmap()
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		DV_ASSERT(m_pBuffer != nullptr);

		pImmediateContext->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}

	bool ConstantBuffer::createBuffer()
	{
		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		Destroy();

		D3D11_BUFFER_DESC desc;
		desc.Usage					= D3D11_USAGE_DYNAMIC;
		desc.ByteWidth				= m_Stride;
		desc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;

		if (FAILED(pDevice->CreateBuffer(&desc, nullptr, &m_pBuffer)))
		{
			DV_CORE_ERROR("Constant Buffer 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}
