#include "DivePch.h"
#include "VertexBuffer.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	VertexBuffer::VertexBuffer(RenderDevice* device)
	{
		if (!device || !device->GetD3dDevice())
		{
			CORE_ERROR("");
			return;
		}

		m_Device = device;
	}

	VertexBuffer::~VertexBuffer()
	{
		SAFE_RELEASE(m_Buffer);
	}

	void * VertexBuffer::Map()
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

	bool VertexBuffer::Unmap()
	{
		if (!m_Buffer)
		{
			CORE_ERROR("");
			return false;
		}

		m_Device->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_Buffer), 0);

		return true;
	}

	bool VertexBuffer::createBuffer(const void * vertices)
	{
		if (!m_bDynamic)
		{
			if (!vertices || m_VertexCount == 0)
			{
				CORE_ERROR("");
				return false;
			}
		}

		SAFE_RELEASE(m_Buffer);

		D3D11_BUFFER_DESC desc;
		desc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth				= m_Stride * m_VertexCount;
		desc.CPUAccessFlags			= m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage					= m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem			= vertices;
		data.SysMemPitch		= 0;
		data.SysMemSlicePitch	= 0;

		if (FAILED(m_Device->GetD3dDevice()->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_Buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}