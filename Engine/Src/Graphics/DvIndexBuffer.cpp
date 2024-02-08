#include "divepch.h"
#include "DvIndexBuffer.h"
#include "GraphicsDevice.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	DvIndexBuffer::DvIndexBuffer(uint16_t* pIndices, uint32_t count)
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

		if(FAILED(GraphicsDevice::GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
	}

	DvIndexBuffer::DvIndexBuffer(uint32_t* pIndices, uint32_t count)
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

		if(FAILED(GraphicsDevice::GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
	}

	DvIndexBuffer::~DvIndexBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	DvIndexBuffer* DvIndexBuffer::Create(uint16_t* pIndices, uint32_t count)
	{
		return new DvIndexBuffer(pIndices, count);
	}

	DvIndexBuffer* DvIndexBuffer::Create(uint32_t* pIndices, uint32_t count)
	{
		return new DvIndexBuffer(pIndices, count);
	}
}