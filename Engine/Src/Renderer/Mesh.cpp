#include "divepch.h"
#include "Mesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/GraphicsDefs.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

// 1. VertexBuffer를 왜 배열로 관리했을까?
// 2. Picking 관련 함수도 이 곳에 존재한다.

namespace Dive
{
	Mesh::Mesh(Context* pContext)
		: Object(pContext)
	{
	}

	Mesh::~Mesh()
	{
		DV_DELETE(m_pVertexBuffer);
		DV_DELETE(m_pIndexBuffer);

		DV_LOG_ENGINE_INFO("Mesh 소멸 완료");
	}

	void Mesh::Draw(Graphics* pGraphics)
	{
		if (m_pIndexBuffer && m_IndexCount > 0)
		{
			pGraphics->SetVertexBuffer(m_pVertexBuffer);
			pGraphics->SetIndexBuffer(m_pIndexBuffer);
			pGraphics->DrawIndexed(m_PrimitiveType, m_IndexCount, m_IndexStart, m_VertexStart);
		}
		else if(m_VertexCount > 0)
		{
			pGraphics->SetVertexBuffer(m_pVertexBuffer);
			pGraphics->Draw(m_PrimitiveType, m_VertexCount, m_VertexStart);
		}
	}

	bool Mesh::SetDrawRange(unsigned int vertexStart, unsigned int vertexCount, unsigned int indexStart, unsigned int indexCount)
	{
		if (m_pVertexBuffer->GetCount() < vertexStart + vertexCount)
		{
			DV_LOG_ENGINE_ERROR("Mesh::SetDrawRange - 잘못된 범위를 전달받았습니다.");
			return false;
		}

		m_VertexStart = vertexStart;
		m_VertexCount = vertexCount;

		if (m_pIndexBuffer) 
		{
			if (m_pIndexBuffer->GetCount() < indexStart + indexCount)
			{
				DV_LOG_ENGINE_ERROR("Mesh::SetDrawRange - 잘못된 범위를 전달받았습니다.");
				return false;
			}

			m_IndexStart = indexStart;
			m_IndexCount = indexCount;

		}

		m_IndexStart = 0;
		m_IndexCount = 0;

		return true;
	}

	void Mesh::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		if (pBuffer)
		{
			m_VertexStart = 0;
			m_VertexCount = pBuffer->GetCount();
		}
		else
		{
			m_VertexStart = 0;
			m_VertexCount = 0;
		}

		m_pVertexBuffer = pBuffer;
	}
	
	void Mesh::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (pBuffer)
		{
			m_IndexStart = 0;
			m_IndexCount = pBuffer->GetCount();
		}
		else
		{
			m_IndexStart = 0;
			m_IndexCount = 0;
		}

		m_pIndexBuffer = pBuffer;
	}

	void Mesh::SetLodDistance(float distance)
	{
		if (distance < 0.0f)
			distance = 0.0f;

		m_LodDistance = distance;
	}
}