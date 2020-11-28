#include "DivePch.h"
#include "VertexBuffer.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	VertexBuffer::VertexBuffer(const std::shared_ptr<RenderDevice>& device)
	{
		if (!device->IsInitialized())
		{
			CORE_ERROR("");
			return;
		}

		m_renderDevice = device;
	}

	VertexBuffer::~VertexBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	void * VertexBuffer::Map()
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

	bool VertexBuffer::Unmap()
	{
		if (!m_buffer)
		{
			CORE_ERROR("");
			return false;
		}

		m_renderDevice->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer), 0);

		return true;
	}

	bool VertexBuffer::createBuffer(const void * vertices)
	{
		if (!m_bDynamic)
		{
			if (!vertices || m_vertexCount == 0)
			{
				CORE_ERROR("");
				return false;
			}
		}

		SAFE_RELEASE(m_buffer);

		D3D11_BUFFER_DESC desc;
		desc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth				= m_stride * m_vertexCount;
		desc.CPUAccessFlags			= m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage					= m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem			= vertices;
		data.SysMemPitch		= 0;
		data.SysMemSlicePitch	= 0;

		if (FAILED(m_renderDevice->GetD3dDevice()->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}