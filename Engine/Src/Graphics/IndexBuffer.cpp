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
		m_pShadowData(nullptr),
		m_pBuffer(nullptr),
		m_IndexCount(0),
		m_IndexSize(0),
		m_bDynamic(false)
	{
		DV_ASSERT(m_pGraphics);
	}

	IndexBuffer::~IndexBuffer()
	{
		ReleaseBuffer();

		DV_DELETE_ARRAY(m_pShadowData);
	}
	
	void IndexBuffer::ReleaseBuffer()
	{
		DV_RELEASE(m_pBuffer);
	}

	bool IndexBuffer::SetSize(unsigned int indexCount, bool bLargeIndices)
	{
		if (!indexCount)
		{
			DV_LOG_ENGINE_ERROR("잘못된 인덱스 카운트({:d})를 전달받았습니다.", indexCount);
			return false;
		}

		m_IndexCount = indexCount;
		m_IndexSize = static_cast<unsigned int>(bLargeIndices ? sizeof(unsigned int) : sizeof(unsigned short));

		if (m_pShadowData)
			DV_DELETE_ARRAY(m_pShadowData);
		m_pShadowData = new unsigned char[m_IndexCount * m_IndexSize];
		memset(static_cast<void*>(m_pShadowData), NULL, m_IndexCount * m_IndexSize);

		return true;
	}

	bool IndexBuffer::CreateBuffer(unsigned int indexCount, bool bLargeIndices, const void* pData)
	{
		if (!indexCount)
		{
			DV_LOG_ENGINE_ERROR("잘못된 인덱스 카운트({:d})를 전달받아 인덱스 버퍼를 생성할 수 없습니다.", indexCount);
			return false;
		}

		m_IndexCount = indexCount;
		m_IndexSize = static_cast<unsigned int>(bLargeIndices ? sizeof(unsigned int) : sizeof(unsigned short));
		m_bDynamic = pData == nullptr;

		ReleaseBuffer();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = (UINT)(m_IndexCount * m_IndexSize);

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(
			&desc, 
			m_bDynamic ? nullptr : &data, 
			(ID3D11Buffer**)&m_pBuffer)))
		{
			DV_RELEASE(m_pBuffer);
			DV_LOG_ENGINE_ERROR("인덱스 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
	
	bool IndexBuffer::SetData(const void* pData)
	{
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("인덱스 버퍼 데이터로 널 포인터를 전달받았습니다.");
			return false;
		}

		if (m_pShadowData && m_pShadowData != pData)
			memcpy_s(m_pShadowData, m_IndexCount * m_IndexSize, pData, m_IndexCount * m_IndexSize);

		if (m_pBuffer)
		{
			if (m_bDynamic)
			{
				void* pDest = Map();
				if (!pDest)
					return false;

				memcpy_s(pDest, m_IndexCount * m_IndexSize, pData, m_IndexCount * m_IndexSize);

				Unmap();
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = 0;
				destBox.right = m_IndexCount * m_IndexSize;
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
		else if (m_pShadowData)
			return m_pShadowData;

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