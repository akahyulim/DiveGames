#include "DivePch.h"
#include "VertexBuffer.h"
#include "Graphics/Graphics.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	VertexBuffer::VertexBuffer(Context* context)
		: Object(context),
		m_buffer(nullptr),
		m_bDynamic(false),
		m_stride(0),
		m_vertexCount(0),
		m_vertexType(eVertexType::Unknown)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	void * VertexBuffer::Map()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || !m_buffer)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(graphics->GetRHIContext()->Map(static_cast<ID3D11Resource*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool VertexBuffer::Unmap()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || !m_buffer)
		{
			CORE_ERROR("");
			return false;
		}

		graphics->GetRHIContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer), 0);

		return true;
	}

	bool VertexBuffer::createBuffer(const void * vertices)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || !m_bDynamic)
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

		if (FAILED(graphics->GetRHIDevice()->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}