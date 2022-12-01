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

		m_Count = vertexCount;
		m_Elements = elements;
		m_bDynamic = bDynamic;

		unsigned int elementSizes = 0;
		m_ElementsHash = 0;
		for (auto it = m_Elements.begin(); it != m_Elements.end(); ++it)
		{
			it->m_Offset = elementSizes;
			elementSizes += ELEMENT_TYPESIZES[it->m_Type];

			m_ElementsHash <<= 3;
			m_ElementsHash += ((static_cast<int>(it->m_Semantic) + 1) + it->m_Index);
		}

		// 직렬화를 위한 elementMask 설정 부분도 존재한다.

		m_Stride = elementSizes;

		if (m_pData)
			DV_DELETE_ARRAY(m_pData);
		m_pData = new unsigned char[m_Count * m_Stride];
		memset(static_cast<void*>(m_pData), 0, m_Count * m_Stride);

		m_pBuffer.Reset();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = (UINT)(m_Count * m_Stride);

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

		if (!m_Stride)
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::SetData - 정점 구성요소가 정의되지 않아, 정점 버퍼 데이터를 설정 할 수 없습니다.");
			return false;
		}

		if (m_pData && m_pData != pData)
			memcpy_s(m_pData, m_Count * m_Stride, pData, m_Count * m_Stride);

		if (m_pBuffer)
		{
			if (m_bDynamic)
			{
				void* pDest = Map();
				if (!pDest)
					return false;

				memcpy_s(pDest, m_Count * m_Stride, pData, m_Count * m_Stride);

				Unmap();
			}
			else
			{
				D3D11_BOX destBox;
				destBox.left = 0;
				destBox.right = m_Count * m_Stride;
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
		if (m_pBuffer || m_bDynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			if (FAILED(m_pGraphics->GetDeviceContext()->Map(
				static_cast<ID3D11Resource*>(m_pBuffer.Get()),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedSubresource)))
			{
				DV_LOG_ENGINE_ERROR("VertexBuffer::Map - 인덱스 버퍼 Map에 실패하였습니다.");
				return nullptr;
			}

			return mappedSubresource.pData;
		}

		return nullptr;
	}

	void VertexBuffer::Unmap()
	{
		if (!m_pBuffer || !m_bDynamic)
			return;

		m_pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer.Get()), 0);
	}

	bool VertexBuffer::create(const void* pData)
	{
		if (!m_Count)
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::create - 잘못된 크기를 전달받아 버퍼를 생성할 수 없습니다.");
			return false;
		}

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
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

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, m_pBuffer.ReleaseAndGetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("VertexBuffer::create - 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}