#include "divepch.h"
#include "Mesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/GraphicsDefs.h"
#include "Core/Context.h"
#include "IO/Log.h"

namespace Dive
{
	Mesh::Mesh(Context* pContext)
		: Object(pContext),
		m_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
		m_pIndexBuffer(nullptr),
		m_VertexStart(0),
		m_VertexCount(0),
		m_IndexStart(0),
		m_IndexCount(0)
	{
		SetNumVertexBuffers(1);
	}

	Mesh::~Mesh()
	{
		// 외부에서 생성된 버퍼를 Model과 나누어 관리하므로
		// 어디에서 제거해야 할 지 애매해졌다.
	}

	void Mesh::Draw(Graphics* pGraphics)
	{
		if (m_pIndexBuffer && m_IndexCount > 0)
		{
			pGraphics->SetVertexBuffers(m_VertexBuffers);
			pGraphics->SetIndexBuffer(m_pIndexBuffer);
			pGraphics->DrawIndexed(m_PrimitiveType, m_VertexCount, m_IndexCount, m_IndexStart);
		}
		else if(m_VertexCount > 0)
		{
			pGraphics->SetVertexBuffers(m_VertexBuffers);
			pGraphics->Draw(m_PrimitiveType, m_VertexCount, m_VertexStart);
		}
	}

	bool Mesh::SetNumVertexBuffers(size_t num)
	{
		if (num >= static_cast<size_t>(MAX_VERTEX_STREAMS))
		{
			DV_LOG_ENGINE_ERROR("정점 버퍼 배열의 크기가 너무 큽니다.");
			return false;
		}

		m_VertexBuffers.resize(num);

		return true;
	}
	
	bool Mesh::SetVertexBuffer(size_t index, VertexBuffer* pBuffer)
	{
		if (index >= m_VertexBuffers.size())
		{
			DV_LOG_ENGINE_ERROR("정점 버퍼 배열의 인덱스를 잘못 전달 받았습니다.");
			return false;
		}

		m_VertexBuffers[index] = pBuffer;

		return true;
	}
	
	VertexBuffer* Mesh::GetVertexBuffer(size_t index) const
	{
		return index < m_VertexBuffers.size() ? m_VertexBuffers[index] : nullptr;
	}
	
	void Mesh::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		m_pIndexBuffer = pBuffer;
	}
}