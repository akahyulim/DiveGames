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

	// 버퍼는 생성하지 않는다. 버퍼의 정보 저장용.
	// => 이게 굳이 필요한 가 싶다. 결국은 버퍼까지 생성하는 편이 나을 것 같다.
	bool VertexBuffer::SetSize(unsigned int vertexCount, const std::vector<VertexElement>& elements)
	{
		if (!vertexCount)
		{
			DV_LOG_ENGINE_ERROR("잘못된 정점 개수(:d)를 전달받았습니다.", vertexCount);
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
			DV_LOG_ENGINE_WARN("정점 개수를 잘못 전달받아 버퍼를 생성할 수 없습니다.");
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
			DV_LOG_ENGINE_ERROR("정점 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool VertexBuffer::SetData(void* pData)
	{
		if (!pData)
		{
			DV_LOG_ENGINE_ERROR("정점 버퍼 데이터로 널 포인터를 전달받았습니다.");
			return false;
		}

		if (!m_VertexSize)
		{
			DV_LOG_ENGINE_ERROR("정점 구성요소가 정의되지 않아, 정점 버퍼 데이터를 설정 할 수 없습니다.");
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
				DV_LOG_ENGINE_ERROR("정점 버퍼 Map에 실패하였습니다.");
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