#include "VertexBuffer.h"
#include "Renderer.h"
#include "Log.h"

namespace Dive
{
	void* VertexBuffer::Map()
	{
		auto immediateContext = Renderer::GetInstance().GetDevice()->GetImmediateContext();
		assert(immediateContext);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(immediateContext->Map(static_cast<ID3D11Resource*>(m_buffer.Get()), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedResource.pData;
	}

	void VertexBuffer::Unmap()
	{
		auto immediateContext = Renderer::GetInstance().GetDevice()->GetImmediateContext();
		assert(immediateContext);

		immediateContext->Unmap(static_cast<ID3D11Resource*>(m_buffer.Get()), 0);
	}

	bool VertexBuffer::createBuffer(const void* datas)
	{
		if(!m_bDynamic)	
			assert(datas != nullptr);

		auto device = Renderer::GetInstance().GetDevice()->GetDevice();
		assert(device != nullptr);

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = m_stride * m_vertexCount;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = datas;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		
		if (FAILED(device->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}