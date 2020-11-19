#include "DivePch.h"
#include "IndexBuffer.h"
#include "RenderDevice.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	IndexBuffer::IndexBuffer(RenderDevice* device)
	{
		if (!device || !device->GetD3dDevice())
		{
			CORE_ERROR("");
			return;
		}

		m_Device = device;
	}

	IndexBuffer::~IndexBuffer()
	{
		SAFE_RELEASE(m_Buffer);
	}

	bool IndexBuffer::Create(const std::vector<unsigned int>& indices)
	{
		if (indices.empty())
		{
			CORE_ERROR("");
			return false;
		}

		SAFE_RELEASE(m_Buffer);

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

		if (FAILED(m_Device->GetD3dDevice()->CreateBuffer(&desc, &data, &m_Buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	bool IndexBuffer::CreateDynamic(unsigned int size)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth				= static_cast<unsigned int>(sizeof(unsigned int)) * size;
		desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;
		desc.Usage					= D3D11_USAGE_DYNAMIC;

		if (FAILED(m_Device->GetD3dDevice()->CreateBuffer(&desc, nullptr, &m_Buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	void * IndexBuffer::Map()
	{
		if(!m_Buffer)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubRsc;
		if (FAILED(m_Device->GetImmediateContext()->Map(static_cast<ID3D11Resource*>(m_Buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRsc)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedSubRsc.pData;
	}

	bool IndexBuffer::Unmap()
	{
		if(!m_Buffer)
		{
			CORE_ERROR("");
			return false;
		}

		m_Device->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_Buffer), 0);

		return true;
	}
}