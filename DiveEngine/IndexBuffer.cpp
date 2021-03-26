#include "IndexBuffer.h"
#include "Renderer.h"
#include "Log.h"

namespace Dive
{
	void* IndexBuffer::Map()
	{
		auto immediateContext = Renderer::GetInstance().GetDevice()->GetImmediateContext();
		assert(immediateContext != nullptr);

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		if (FAILED(immediateContext->Map(static_cast<ID3D11Resource*>(m_buffer.Get()), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedSubresource.pData;
	}

	void IndexBuffer::Unmap()
	{
		auto immediateContext = Renderer::GetInstance().GetDevice()->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->Unmap(static_cast<ID3D11Resource*>(m_buffer.Get()), 0);
	}

	// vertex buffer랑 너무 비슷하다.
	bool IndexBuffer::createBuffer(const void* datas)
	{
		if (!m_bDynamic)
			assert(datas != nullptr);

		auto device = Renderer::GetInstance().GetDevice()->GetDevice();
		assert(device != nullptr);

		// flag 빼고 전부 동일
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = m_stride * m_indexCount;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
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