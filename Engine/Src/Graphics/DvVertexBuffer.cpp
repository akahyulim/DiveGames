#include "divepch.h"
#include "DvVertexBuffer.h"
#include "GraphicsDevice.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	DvVertexBuffer::DvVertexBuffer(const void* pData, uint32_t stride, uint32_t count)
		: m_pBuffer(nullptr)
		, m_Stride(stride)
		, m_Offset(0)
		, m_Count(count)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = stride * count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(GraphicsDevice::GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("VertexBuffer 생성에 실패하였습니다.");
	}
	
	DvVertexBuffer::~DvVertexBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	DvVertexBuffer* DvVertexBuffer::Create(const void* pData, uint32_t stride, uint32_t count)
	{
		return new DvVertexBuffer(pData, stride, count);
	}
}