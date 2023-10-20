#include "DivePch.h"
#include "VertexBuffer.h"
#include "Core/CoreDefs.h"
#include "Graphics.h"
#include "Core/Log.h"

namespace Dive
{
	VertexBuffer::VertexBuffer()
		: m_pBuffer(nullptr),
		m_Stride(0),
		m_Count(0)
	{}

	VertexBuffer::~VertexBuffer()
	{
		Destroy();
	}

	void VertexBuffer::Destroy()
	{
		DV_RELEASE(m_pBuffer);
	}

	void* VertexBuffer::Map()
	{
		DV_CORE_ASSERT(m_pBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(Graphics::GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pBuffer),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource)))
		{
			DV_CORE_ERROR("VertexBuffer Map에 실패하였습니다.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	void VertexBuffer::Unmap()
	{
		DV_CORE_ASSERT(m_pBuffer);

		Graphics::GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}

	bool VertexBuffer::createBuffer(const void* pVertices)
	{
		const bool bDynamic = pVertices == nullptr;

		Destroy();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = static_cast<UINT>(m_Stride * m_Count);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pVertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&desc, bDynamic ? nullptr : &data, &m_pBuffer)))
		{
			DV_CORE_ERROR("VertexBuffer 구성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}