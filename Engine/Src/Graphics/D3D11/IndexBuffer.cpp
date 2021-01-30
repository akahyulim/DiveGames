#include "DivePch.h"
#include "IndexBuffer.h"


namespace Dive
{
	IndexBuffer::IndexBuffer(Context * context)
		: Object(context),
		m_buffer(nullptr),
		m_bDynamic(false),
		m_stride(0),
		m_count(0)
	{
	}

	IndexBuffer::~IndexBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	DXGI_FORMAT IndexBuffer::GetFormat() const
	{
		if (m_stride == sizeof(unsigned short))		return DXGI_FORMAT_R16_UINT;
		else if (m_stride == sizeof(unsigned int))	return DXGI_FORMAT_R32_UINT;
		else										return DXGI_FORMAT_UNKNOWN;
	}

	bool IndexBuffer::createBuffer(const void * indices)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
		{
			CORE_ERROR("");
			return false;
		}

		SAFE_RELEASE(m_buffer);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth				= m_stride * m_count;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;
		desc.Usage					= m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags			= m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem			= indices;
		data.SysMemPitch		= 0;
		data.SysMemSlicePitch	= 0;

		if (FAILED(graphics->GetRHIDevice()->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	void * IndexBuffer::Map()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || !m_buffer)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubRsc;
		if (FAILED(graphics->GetRHIContext()->Map(static_cast<ID3D11Resource*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRsc)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedSubRsc.pData;
	}

	bool IndexBuffer::Unmap()
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
}