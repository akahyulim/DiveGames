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
		m_pGraphics(GetSubsystem<Graphics>()),
		m_pBuffer(nullptr),
		m_IndexCount(0),
		m_Stride(0),
		m_bDynamic(false)
	{
		DV_ASSERT(m_pGraphics);
	}

	IndexBuffer::~IndexBuffer()
	{
		Release();
	}
	
	void IndexBuffer::Release()
	{
		DV_RELEASE(m_pBuffer);
	}
	
	bool IndexBuffer::CreateBuffer(unsigned int indexCount, bool bLargeIndices, bool bDynamic)
	{
		Release();

		if (!indexCount)
		{
			DV_LOG_ENGINE_WARN("인덱스 개수를 잘못 전달받아 버퍼를 생성할 수 없습니다.");
			return false;
		}

		m_IndexCount = indexCount;
		m_Stride = (unsigned int)(bLargeIndices ? sizeof(unsigned int) : sizeof(unsigned short));
		m_bDynamic = bDynamic;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = (UINT)(m_IndexCount * m_Stride);

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_pBuffer)))
		{
			DV_RELEASE(m_pBuffer);
			DV_LOG_ENGINE_ERROR("인덱스 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
	
	bool IndexBuffer::SetData(void* pData)
	{
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("인덱스 버퍼 데이터로 널 포인터를 전달받았습니다.");
			return false;
		}

		if (!m_pBuffer)
		{
			DV_LOG_ENGINE_ERROR("인덱스 버퍼가 존재하지 않습니다.");
			return false;
		}

		if (m_bDynamic)
		{
			void* pDest = Map();
			if (!pDest)
				return false;

			memcpy(pDest, pData, (size_t)(m_IndexCount * m_Stride));

			Unmap();
		}
		else
		{
			D3D11_BOX destBox;
			destBox.left = 0;
			destBox.right = m_IndexCount * m_Stride;
			destBox.top = 0;
			destBox.bottom = 1;
			destBox.front = 0;
			destBox.back = 1;

			m_pGraphics->GetDeviceContext()->UpdateSubresource(
				(ID3D11Buffer*)m_pBuffer,
				0,
				&destBox,
				pData,
				0,
				0);
		}

		return true;
	}
	
	void* IndexBuffer::Map()
	{
		if (m_pBuffer)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			if (FAILED(m_pGraphics->GetDeviceContext()->Map(
				(ID3D11Buffer*)m_pBuffer,
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedSubresource)))
			{
				DV_LOG_ENGINE_ERROR("인덱스 버퍼 Map에 실패하였습니다.");
				return nullptr;
			}

			return mappedSubresource.pData;
		}

		return nullptr;
	}
	
	void IndexBuffer::Unmap()
	{
		if (m_pBuffer)
		{
			m_pGraphics->GetDeviceContext()->Unmap((ID3D11Buffer*)m_pBuffer, 0);
		}
	}
}