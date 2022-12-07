#include "divepch.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "IO/Log.h"

namespace Dive
{
	IndexBuffer::IndexBuffer(Context* pContext)
		: Object(pContext),
		m_pGraphics(GetSubsystem<Graphics>())
	{
		DV_ASSERT(m_pGraphics->IsInitialized());
	}

	IndexBuffer::~IndexBuffer()
	{
		DV_RELEASE(m_pBuffer);

		DV_LOG_ENGINE_INFO("IndexBuffer 소멸 완료");
	}

	void* IndexBuffer::Map()
	{
		if (m_pBuffer && m_bDynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			if (FAILED(m_pGraphics->GetDeviceContext()->Map(
				static_cast<ID3D11Resource*>(m_pBuffer),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedSubresource)))
			{
				DV_LOG_ENGINE_ERROR("IndexBuffer::Map - 인덱스 버퍼 Map에 실패하였습니다.");
				return nullptr;
			}

			return mappedSubresource.pData;
		}

		return nullptr;
	}
	
	void IndexBuffer::Unmap()
	{
		if (!m_pBuffer || !m_bDynamic)
			return;
		
		m_pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
	}

	bool IndexBuffer::create(const void* pData)
	{
		if (!m_Count)
		{
			DV_LOG_ENGINE_ERROR("IndexBuffer::create - 잘못된 크기를 전달받아 버퍼를 생성할 수 없습니다.");
			return false;
		}

		DV_RELEASE(m_pBuffer);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(m_Count * m_Stride);
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_pBuffer)))
		{
			DV_LOG_ENGINE_ERROR("IndexBuffer::create - 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}