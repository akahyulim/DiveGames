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
		DV_LOG_ENGINE_TRACE("IndexBuffer �Ҹ��� ȣ��");

		DV_DELETE_ARRAY(m_pData);
	}

	bool IndexBuffer::SetSize(unsigned int indexCount, bool bLargeIndices, bool bDynamic)
	{
		if (!indexCount)
		{
			DV_LOG_ENGINE_ERROR("IndexBuffer::SetSize - �߸��� �ε��� ī��Ʈ({:d})�� ���޹޾ҽ��ϴ�.", indexCount);
			return false;
		}

		m_IndexCount = indexCount;
		m_IndexSize = static_cast<unsigned int>(bLargeIndices ? sizeof(unsigned int) : sizeof(unsigned short));
		m_bDynamic = bDynamic;

		if (m_pData)
			DV_DELETE_ARRAY(m_pData);
		m_pData = new unsigned char[m_IndexCount * m_IndexSize];
		memset(static_cast<void*>(m_pData), 0, m_IndexCount * m_IndexSize);

		m_pBuffer.Reset();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(m_IndexCount * m_IndexSize);

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(
			&desc,
			nullptr,
			m_pBuffer.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("IndexBuffer::SetSize - �ε��� ���� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	bool IndexBuffer::SetData(const void* pData)
	{
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("IndexBuffer::SetData - �ε��� ���� �����ͷ� �� �����͸� ���޹޾ҽ��ϴ�.");
			return false;
		}

		if (m_pData && m_pData != pData)
			memcpy_s(m_pData, m_IndexCount * m_IndexSize, pData, m_IndexCount * m_IndexSize);

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
				destBox.right = (m_IndexCount * m_IndexSize);
				destBox.top = 0;
				destBox.bottom = 1;
				destBox.front = 0;
				destBox.back = 1;

				m_pGraphics->GetDeviceContext()->UpdateSubresource(
					static_cast<ID3D11Resource*>(m_pBuffer.Get()),
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
				static_cast<ID3D11Resource*>(m_pBuffer.Get()),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedSubresource)))
			{
				DV_LOG_ENGINE_ERROR("IndexBuffer::Map - �ε��� ���� Map�� �����Ͽ����ϴ�.");
				return nullptr;
			}

			return mappedSubresource.pData;
		}
		else if (m_pData)
			return m_pData;

		return nullptr;
	}
	
	void IndexBuffer::Unmap()
	{
		if (m_pBuffer)
		{
			m_pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer.Get()), 0);
		}
	}
}