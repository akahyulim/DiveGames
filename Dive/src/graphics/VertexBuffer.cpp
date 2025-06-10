#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	VertexBuffer::~VertexBuffer()
	{
		Release();
	}

	bool VertexBuffer::Create(const void* data, UINT32 stride, UINT32 count)
	{
		Release();

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = stride * count;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = data;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &m_Buffer)))
		{
			return false;
		}

		m_Stride = stride;
		m_Count = count;

		return true;
	}

	void VertexBuffer::Release()
	{
		DV_RELEASE(m_Buffer);
	}

	VertexBuffer* VertexBuffer::Generate(const void* data, UINT32 stride, UINT32 count)
	{
		auto vertexBuffer = new VertexBuffer();
		if (!vertexBuffer->Create(data, stride, count))
			DV_DELETE(vertexBuffer);

		return vertexBuffer;
	}
}
