#include "DivePch.h"
#include "VertexBuffer.h"
#include "RenderDevice.h"
#include "Core/Log.h"

namespace Dive
{
	VertexBuffer::VertexBuffer(RenderDevice* device)
	{
		if (!device || !device->GetD3dDevice())
		{
			CORE_ERROR("VertexBuffer::VertexBuffer>> RenderDevice ��ü�� ��ȿ���� �ʽ��ϴ�.");
			return;
		}

		m_device = device;
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	void * VertexBuffer::Map()
	{
		if (!m_buffer)
		{
			CORE_ERROR("VertexBuffer::Map>> Buffer�� ��ȿ���� �ʽ��ϴ�.");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(m_device->GetImmediateContext()->Map(static_cast<ID3D11Resource*>(m_buffer.get()), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("VertexBuffer::Map>> Vertex Buffer Map�� �����Ͽ����ϴ�.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool VertexBuffer::Unmap()
	{
		if (!m_buffer)
		{
			CORE_ERROR("VertexBuffer::Unmap>> Buffer�� ��ȿ���� �ʽ��ϴ�.");
			return false;
		}

		m_device->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer.get()), 0);

		return true;
	}

	bool VertexBuffer::createBuffer(const void * pVertices)
	{
		if (!m_is_dynamic)
		{
			if (!pVertices || m_vertex_count == 0)
			{
				CORE_ERROR("VertexBuffer::create >> �߸��� ���ڸ� ���޹޾ҽ��ϴ�.");
				return false;
			}
		}

		//SAFE_RELEASE(m_pBuffer);

		D3D11_BUFFER_DESC descBuffer;
		descBuffer.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
		descBuffer.ByteWidth			= m_stride * m_vertex_count;
		descBuffer.CPUAccessFlags		= m_is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		descBuffer.Usage				= m_is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		descBuffer.MiscFlags			= 0;
		descBuffer.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem			= pVertices;
		data.SysMemPitch		= 0;
		data.SysMemSlicePitch	= 0;

		auto buffer_ptr = m_buffer.get();
		if (FAILED(m_device->GetD3dDevice()->CreateBuffer(&descBuffer, m_is_dynamic ? nullptr : &data, &buffer_ptr)))
		{
			CORE_ERROR("VertexBuffer::Create>> Vertex Buffer ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}
}