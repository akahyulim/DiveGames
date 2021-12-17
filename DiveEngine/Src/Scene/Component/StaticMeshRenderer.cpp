#include "StaticMeshRenderer.h"
#include "../Src/Renderer/dvMesh.h"
#include "../Src/Renderer/StaticMesh.h"

namespace DiveEngine
{
	StaticMeshRenderer::StaticMeshRenderer(GameObject* pOwner)
		: dvMeshRenderer(pOwner)
	{
		m_Type = eComponentType::StaticMeshRenderer;
	}

	StaticMeshRenderer::~StaticMeshRenderer()
	{
	}
	void StaticMeshRenderer::Draw(ID3D11DeviceContext* pImmediateContext)
	{
		// 안보이기로 했다면 역시 패스

		if (!m_pMesh)
			return;

		// vertex buffer
		{
			ID3D11Buffer* pVB = m_pMesh->GetVertexBuffer();
			UINT stride = m_pMesh->GetVertexStride();
			UINT offset = 0;

			pImmediateContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		}

		// index buffer
		{
			ID3D11Buffer* pIB = m_pMesh->GetIndexBuffer();
			DXGI_FORMAT format = m_pMesh->GetIndexFormat();
			UINT offset = 0;

			pImmediateContext->IASetIndexBuffer(pIB, format, offset);
		}

		// draw
		{
			UINT indexCount = m_pMesh->GetIndexCount();

			pImmediateContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}