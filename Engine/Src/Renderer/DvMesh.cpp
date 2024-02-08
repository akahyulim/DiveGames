#include "DivePch.h"
#include "DvMesh.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DvVertexBuffer.h"
#include "Graphics/DvIndexBuffer.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	DvMesh::DvMesh(DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer)
		: m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
	}
	
	DvMesh::~DvMesh()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);
	}
	
	// 현재 urho3d의 Geometry::Draw와 완전히 같다.
	void DvMesh::Draw(D3D11_PRIMITIVE_TOPOLOGY topology)
	{	
		if (m_pIndexBuffer && m_pIndexBuffer->GetCount() > 0)
		{
			GraphicsDevice::SetVertexBuffer(m_pVertexBuffer);
			GraphicsDevice::SetIndexBuffer(m_pIndexBuffer);
			GraphicsDevice::DrawIndexed(topology, m_pIndexBuffer->GetCount(), 0);
		}
		else if (m_pVertexBuffer->GetCount() > 0)
		{
			GraphicsDevice::SetVertexBuffer(m_pVertexBuffer);
			GraphicsDevice::Draw(topology, m_pVertexBuffer->GetCount(), 0);
		}
	}

	DvMesh* DvMesh::Create(DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer)
	{
		return new DvMesh(pVertexBuffer, pIndexBuffer);
	}
};

