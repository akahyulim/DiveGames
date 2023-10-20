#include "DivePch.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	IndexBuffer::IndexBuffer()
		: m_pBuffer(nullptr),
		m_Stride(0),
		m_Count(0)
	{
	}

	IndexBuffer::~IndexBuffer()
	{
		Destroy();
	}

	void IndexBuffer::Destroy()
	{
		DV_RELEASE(m_pBuffer);
	}

	void* IndexBuffer::Map()
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
			DV_CORE_ERROR("IndexBuffer Map에 실패하였습니다.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	void IndexBuffer::Unmap()
	{
		DV_CORE_ASSERT(m_pBuffer);

		Graphics::GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}

	bool IndexBuffer::createBuffer(const void* pIndices)
	{
		const bool bDynamic = pIndices == nullptr;

		Destroy();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = static_cast<UINT>(m_Stride * m_Count);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(Graphics::GetDevice()->CreateBuffer(&desc, bDynamic ? nullptr : &data, &m_pBuffer)))
		{
			DV_CORE_ERROR("IndexBuffer 구성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}