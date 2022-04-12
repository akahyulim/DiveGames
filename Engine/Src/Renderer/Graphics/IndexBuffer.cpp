#include "divepch.h"
#include "IndexBuffer.h"
#include "GraphicsDevice.h"
#include "Renderer/Renderer.h"
#include "Base/Base.h"

namespace Dive
{
	IndexBuffer::~IndexBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}
	
	void* IndexBuffer::Map()
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		DV_ASSERT(m_pBuffer != nullptr);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(pImmediateContext->Map(static_cast<ID3D11Resource*>(m_pBuffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			DV_CORE_ERROR("Failed to map index buffer.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	void IndexBuffer::Unmap()
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		DV_ASSERT(m_pBuffer != nullptr);

		pImmediateContext->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}

	bool IndexBuffer::createBuffer(const void* pData)
	{
		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
		DV_ASSERT(pDevice != nullptr);

		DV_RELEASE(m_pBuffer);

		bool bDynamic = pData == nullptr;

		D3D11_BUFFER_DESC desc;
		desc.Usage					= bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth				= m_Stride * m_Count;
		desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags			= bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem			= pData;
		data.SysMemPitch		= 0;
		data.SysMemSlicePitch	= 0;

		if (FAILED(pDevice->CreateBuffer(&desc, bDynamic ? nullptr : &data, &m_pBuffer)))
		{
			DV_CORE_ERROR("Failed to create index buffer.");
			return false;
		}

		return true;
	}
}