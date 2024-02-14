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
		: Resource(eResourceType::DvMesh)
		, m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
	}

	DvMesh::DvMesh(const std::string& name, DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer)
		: Resource(eResourceType::DvMesh)
		, m_pVertexBuffer(pVertexBuffer)
		, m_pIndexBuffer(pIndexBuffer)
	{
		SetName(name);
	}
	
	DvMesh::~DvMesh()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);
	}
	
	// ���� urho3d�� Geometry::Draw�� ������ ����.
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

	DvMesh* DvMesh::Create(const std::string& name, DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer)
	{
		return new DvMesh(name, pVertexBuffer, pIndexBuffer);
	}
};

