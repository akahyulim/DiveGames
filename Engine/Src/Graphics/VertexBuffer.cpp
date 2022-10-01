#include "divepch.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	VertexBuffer::VertexBuffer(Context* pContext)
		: Object(pContext),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_pShadowData(nullptr),
		m_pBuffer(nullptr),
		m_VertexCount(0),
		m_VertexSize(0),
		m_ElementHash(0),
		m_bDynamic(false)
	{
		DV_ASSERT(m_pGraphics);
	}

	VertexBuffer::~VertexBuffer()
	{
		ReleaseBuffer();
	}

	void VertexBuffer::ReleaseBuffer()
	{
		DV_RELEASE(m_pBuffer);
		DV_DELETE_ARRAY(m_pShadowData);
	}

	// ���۴� �������� �ʴ´�. ������ ���� �����.
	// => �̰� ���� �ʿ��� �� �ʹ�. �ᱹ�� ���۱��� �����ϴ� ���� ���� �� ����.
	bool VertexBuffer::SetSize(unsigned int vertexCount, const std::vector<VertexElement>& elements)
	{
		if (!vertexCount)
		{
			DV_LOG_ENGINE_ERROR("�߸��� ���� ����(:d)�� ���޹޾ҽ��ϴ�.", vertexCount);
			return false;
		}

		m_VertexCount = vertexCount;
		m_Elements = elements;

		unsigned int elementOffset = 0;
		for (auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
		{
			it->m_Offset = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[it->m_Type];
		}
		m_VertexSize = elementOffset;

		if (m_pShadowData)
			DV_DELETE_ARRAY(m_pShadowData);
		m_pShadowData = new unsigned char[m_VertexCount * m_VertexSize];

		return true;
	}

	bool VertexBuffer::CreateBuffer(unsigned int vertexCount, const std::vector<VertexElement>& elements, const void* pData)
	{
		if (!vertexCount)
		{
			DV_LOG_ENGINE_WARN("���� ������ �߸� ���޹޾� ���۸� ������ �� �����ϴ�.");
			return false;
		}

		m_VertexCount = vertexCount;
		m_Elements = elements;
		m_ElementHash = 0;
		m_bDynamic = pData == nullptr;

		unsigned int elementOffset = 0;
		for (auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
		{
			it->m_Offset = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[it->m_Type];
			m_ElementHash << 6;
			m_ElementHash += (((int)it->m_Type + 1) * ((int)it->m_Semantic + 1) + (int)it->m_Index);
		}
		m_VertexSize = elementOffset;

		ReleaseBuffer();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = (UINT)(m_VertexCount * m_VertexSize);

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
			DV_LOG_ENGINE_ERROR("���� ���� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	bool VertexBuffer::SetData(void* pData)
	{
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("���� ���� �����ͷ� �� �����͸� ���޹޾ҽ��ϴ�.");
			return false;
		}

		if (!m_VertexSize)
		{
			DV_LOG_ENGINE_ERROR("���� ������Ұ� ���ǵ��� �ʾ�, ���� ���� �����͸� ���� �� �� �����ϴ�.");
			return false;
		}

		if (m_pShadowData && m_pShadowData != pData)
			memcpy(m_pShadowData, pData, m_VertexCount * m_VertexSize);

		if (m_pBuffer)
		{
			if (m_bDynamic)
			{
				void* pDest = Map();
				if (!pDest)
					return false;

				memcpy_s(pDest, m_VertexCount * m_VertexSize, pData, m_VertexCount * m_VertexSize);

				Unmap();
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = 0;
				destBox.right = m_VertexCount * m_VertexSize;
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
					0
				);
			}
		}

		return true;
	}

	void* VertexBuffer::Map()
	{
		if (m_pBuffer && m_bDynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if (FAILED(m_pGraphics->GetDeviceContext()->Map(
				(ID3D11Buffer*)m_pBuffer,
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedResource)))
			{
				DV_LOG_ENGINE_ERROR("���� ���� Map�� �����Ͽ����ϴ�.");
				return nullptr;
			}

			return mappedResource.pData;
		}
		else if (m_pShadowData)
			return m_pShadowData;

		return nullptr;
	}

	void VertexBuffer::Unmap()
	{
		if (m_pBuffer)
		{
			m_pGraphics->GetDeviceContext()->Unmap((ID3D11Buffer*)m_pBuffer, 0);
		}
	}
}