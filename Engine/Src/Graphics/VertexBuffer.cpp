#include "divepch.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	VertexBuffer::VertexBuffer()
		: m_pBuffer(nullptr)
		, m_Stride(0)
		, m_Offset(0)
		, m_Count(0)
	{
	}
	
	VertexBuffer::~VertexBuffer()
	{
		Destroy();
	}

	bool VertexBuffer::CreateBuffer(const void* pData, uint32_t stride, uint32_t count)
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

		if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
		{
			DV_ERROR("VertexBuffer 생성에 실패하였습니다.");
			return false;
		}

		m_Stride = stride;
		m_Count = count;

		return true;
	}

	void VertexBuffer::Destroy()
	{
		DV_RELEASE(m_pBuffer);
	}

	VertexBuffer* VertexBuffer::Create(const void* pData, uint32_t stride, uint32_t count)
	{
		auto pVertexBuffer = new VertexBuffer();
		if (!pVertexBuffer->CreateBuffer(pData, stride, count))
			DV_DELETE(pVertexBuffer);

		return pVertexBuffer;
	}
}