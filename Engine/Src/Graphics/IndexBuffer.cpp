#include "divepch.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	IndexBuffer::IndexBuffer(uint16_t* pIndices, uint32_t count)
		: m_Count(count)
		, m_Format(DXGI_FORMAT_R16_UINT)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(uint16_t) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if(FAILED(Graphics::GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
	}

	IndexBuffer::IndexBuffer(uint32_t* pIndices, uint32_t count)
		: m_Count(count)
		, m_Format(DXGI_FORMAT_R32_UINT)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(uint32_t) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if(FAILED(Graphics::GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
	}

	IndexBuffer::~IndexBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	IndexBuffer* IndexBuffer::Create(uint16_t* pIndices, uint32_t count)
	{
		return new IndexBuffer(pIndices, count);
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* pIndices, uint32_t count)
	{
		return new IndexBuffer(pIndices, count);
	}
}