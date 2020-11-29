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

		m_renderDevice = device;
	}

	ConstantBuffer::~ConstantBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	void * ConstantBuffer::Map()
	{
		if (!m_buffer)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(m_renderDevice->GetImmediateContext()->Map(static_cast<ID3D11Resource*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool ConstantBuffer::Unmap()
	{
		if (!m_buffer)
		{
			CORE_ERROR("");
			return false;
		}

		m_renderDevice->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer), 0);

		return true;
	}
}