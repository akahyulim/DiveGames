#include "Buffer.h"
#include "Renderer.h"
#include "Log.h"

namespace Dive
{
	void* Buffer::Map()
	{
		return nullptr;
	}

	void Buffer::Unmap()
	{
	}
	
	bool Buffer::generateBuffer(const void* datas)
	{
		auto device = Renderer::GetInstance().GetDevice()->GetDevice();
		assert(device != nullptr);

		D3D11_BUFFER_DESC desc;
		switch (m_type)
		{
		case eBufferType::VertexBuffer:		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		break;
		case eBufferType::IndexBuffer:		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;		break;
		case eBufferType::ConstantBuffer:	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	break;
		default:	return false;
		}
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;

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