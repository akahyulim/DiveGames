#include "divepch.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	VertexBuffer::VertexBuffer(const void* pData, uint32_t stride, uint32_t count)
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

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("VertexBuffer 생성에 실패하였습니다.");
	}
	
	VertexBuffer::~VertexBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	VertexBuffer* VertexBuffer::Create(const void* pData, uint32_t stride, uint32_t count)
	{
		return new VertexBuffer(pData, stride, count);
	}
}