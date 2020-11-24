#include "DivePch.h"
#include "ConstantBuffer.h"
#include "RenderDevice.h"


namespace Dive
{
	ConstantBuffer::ConstantBuffer(const std::shared_ptr<RenderDevice>& device)
	{
		if (!device->IsInitialized())
		{
			CORE_ERROR("");
			return;
		}

		m_Device = device;
	}

	ConstantBuffer::~ConstantBuffer()
	{
		SAFE_RELEASE(m_Buffer);
	}

	void * ConstantBuffer::Map()
	{
		if (!m_Buffer)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(m_Device->GetImmediateContext()->Map(static_cast<ID3D11Resource*>(m_Buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool ConstantBuffer::Unmap()
	{
		if (!m_Buffer)
		{
			CORE_ERROR("");
			return false;
		}

		m_Device->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_Buffer), 0);

		return true;
	}
}