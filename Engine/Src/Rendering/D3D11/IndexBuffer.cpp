#include "DivePch.h"
#include "IndexBuffer.h"
#include "Core/Log.h"
#include "RenderDevice.h"

namespace Dive
{
	IndexBuffer::IndexBuffer(RenderDevice* device)
	{
		if (!device || !device->GetD3DDevice())
		{
			CORE_ERROR("IndexBuffer::IndexBuffer>> RenderDevice 객체가 유효하지 않습니다.");
			return;
		}

		m_device = device;
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	bool IndexBuffer::Create(const std::vector<unsigned int>& indices)
	{
		if (indices.empty())
		{
			CORE_ERROR("IndexBuffer::Create>> 잘못된 인자를 전달받았습니다.");
			return false;
		}

		unsigned int size = static_cast<unsigned int>(indices.size());

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = static_cast<unsigned int>(sizeof(unsigned int)) * size;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = indices.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		auto buffer_ptr = m_buffer.get();
		if (FAILED(m_device->GetD3DDevice()->CreateBuffer(&desc, &data, &buffer_ptr)))
		{
			CORE_ERROR("IndexBuffer::Create>> Index Buffer 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool IndexBuffer::CreateDynamic(unsigned int size)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = static_cast<unsigned int>(sizeof(unsigned int)) * size;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;

		auto buffer_ptr = m_buffer.get();
		if (FAILED(m_device->GetD3DDevice()->CreateBuffer(&desc, nullptr, &buffer_ptr)))
		{
			CORE_ERROR("IndexBuffer::CreateDynamic>> Index Buffer 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	void * IndexBuffer::Map()
	{
		if(!m_buffer)
		{
			CORE_ERROR("IndexBuffer::Map>> Buffer가 유효하지 않습니다.");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubRsc;
		if (FAILED(m_device->GetImmediateContext()->Map(static_cast<ID3D11Resource*>(m_buffer.get()), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRsc)))
		{
			CORE_ERROR("IndexBuffer::Map>> Index Buffer Map에 실패하였습니다.");
			return nullptr;
		}

		return mappedSubRsc.pData;
	}

	bool IndexBuffer::Unmap()
	{
		if(m_buffer)
		{
			CORE_ERROR("IndexBuffer::Unmap >> Buffer가 유효하지 않습니다.");
			return false;
		}

		m_device->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer.get()), 0);

		return true;
	}
}