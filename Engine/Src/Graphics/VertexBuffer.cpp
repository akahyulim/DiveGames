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
		m_pGraphics(GetSubsystem<Graphics>())
	{
		DV_ASSERT(m_pGraphics->IsInitialized());
	}

	VertexBuffer::~VertexBuffer()
	{
		DV_LOG_ENGINE_TRACE("VertexBuffer 소멸자 호출");

		DV_DELETE_ARRAY(m_pData);
	}

	bool VertexBuffer::SetSize(unsigned int vertexCount, const std::vector<VertexElement>& elements, bool bDynamic)
	{
		if (!vertexCount)
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::SetSize - 잘못된 정점 개수(:d)를 전달받았습니다.", vertexCount);
			return false;
		}

		m_VertexCount = vertexCount;
		m_Elements = elements;
		m_bDynamic = bDynamic;

		unsigned int elementOffset = 0;
		for (auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
		{
			it->m_Offset = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[it->m_Type];
		}
		m_VertexSize = elementOffset;

		if (m_pData)
			DV_DELETE_ARRAY(m_pData);
		m_pData = new unsigned char[m_VertexCount * m_VertexSize];
		memset(static_cast<void*>(m_pData), 0, m_VertexCount * m_VertexSize);

		m_pBuffer.Reset();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = (UINT)(m_VertexCount * m_VertexSize);

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(
			&desc,
			nullptr,
			m_pBuffer.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::CreateBuffer - 정점 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool VertexBuffer::SetData(void* pData)
	{
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::SetData - 정점 버퍼 데이터로 널 포인터를 전달받았습니다.");
			return false;
		}

		if (!m_VertexSize)
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::SetData - 정점 구성요소가 정의되지 않아, 정점 버퍼 데이터를 설정 할 수 없습니다.");
			return false;
		}

		if (m_pData && m_pData != pData)
			memcpy_s(m_pData, m_VertexCount * m_VertexSize, pData, m_VertexCount * m_VertexSize);

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
					static_cast<ID3D11Resource*>(m_pBuffer.Get()),
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
				static_cast<ID3D11Resource*>(m_pBuffer.Get()),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedResource)))
			{
				DV_LOG_ENGINE_ERROR("VertexBuffer::Map - 정점 버퍼 Map에 실패하였습니다.");
				return nullptr;
			}

			return mappedResource.pData;
		}
		else if (m_pData)
			return m_pData;

		return nullptr;
	}

	void VertexBuffer::Unmap()
	{
		if (m_pBuffer)
		{
			m_pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer.Get()), 0);
		}
	}
}