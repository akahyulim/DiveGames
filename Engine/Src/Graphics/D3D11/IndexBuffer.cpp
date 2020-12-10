#include "DivePch.h"
#include "IndexBuffer.h"
#include "Graphics/Graphics.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	IndexBuffer::IndexBuffer(Context* context)
		: Object(context)
	{
	}

	IndexBuffer::~IndexBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	bool IndexBuffer::Create(const std::vector<unsigned int>& indices)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || indices.empty())
		{
			CORE_ERROR("");
			return false;
		}

		SAFE_RELEASE(m_buffer);

		unsigned int size = static_cast<unsigned int>(indices.size());

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth				= static_cast<unsigned int>(sizeof(unsigned int)) * size;
		desc.CPUAccessFlags			= 0;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;
		desc.Usage					= D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem			= indices.data();
		data.SysMemPitch		= 0;
		data.SysMemSlicePitch	= 0;

		if (FAILED(graphics->GetRHIDevice()->CreateBuffer(&desc, &data, &m_buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	bool IndexBuffer::CreateDynamic(unsigned int size)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
		{
			CORE_ERROR("");
			return false;
		}

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth				= static_cast<unsigned int>(sizeof(unsigned int)) * size;
		desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;
		desc.Usage					= D3D11_USAGE_DYNAMIC;

		if (FAILED(graphics->GetRHIDevice()->CreateBuffer(&desc, nullptr, &m_buffer)))
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