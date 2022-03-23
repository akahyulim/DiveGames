#include "divepch.h"
#include "RenderPath.h"
#include "PipelineState.h"
#include "CommandList.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/SpriteRenderable.h"

namespace Dive
{
	RenderPath::RenderPath()
	{
		// active scene�� camera�� �ʿ��ϴ�.
	}

	RenderPath::~RenderPath()
	{
	}

	// �� ������ Visibilities�� �з��ϴ� ���� ��ȿ�����̴�.
	// �� ���� �� �׷��� Udpate �������� ä���
	// VisibilityUpdate�� Event ó���� �� ���� �ִ�.
	void RenderPath::Update(float delta)
	{
		// scene, camera ����
		m_MainVisibilities;

		Renderer::UpdateVisibility(m_MainVisibilities);
	}
	
	void RenderPath::Render()
	{
		// ���� state �񱳿�
		CommandList cl;

		// sampler, global cbuffer

		// bind���� �̸� �ϴ� �͵� �����غ� �� �ִ�.
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		passDefault(&cl, view, proj);
	}

	// ���ĸ�ź�� RenderObject Type�� ���޹޴´�.
	void RenderPath::passDefault(CommandList* pCl, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		if (!pImmediateContext)
			return;

		if (!m_MainVisibilities.visibleSpriteRenderables.empty())
		{
			PipelineState ps;
			ps.primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			ps.pInputLayout			= Renderer::GetShader(eShaderType::Sprite)->pInputLayout;
			ps.pVertexShader		= Renderer::GetShader(eShaderType::Sprite)->pVertexShader;
			ps.pRasterizerState		= Renderer::GetRasterizerState(eRasterizerStateType::CullBackSolid);
			ps.pPixelShader			= Renderer::GetShader(eShaderType::Sprite)->pPixelShader;
			ps.pDepthStencilState	= Renderer::GetDepthStencilState(eDepthStencilStateType::DepthOnStencilOn);
			pCl->SetPipelineState(ps);

			// ī�޶� ���� Viewport�� bind�ؾ� �Ѵ�...

			for(auto pGameObject : m_MainVisibilities.visibleSpriteRenderables)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pSpriteRenderable = pGameObject->GetComponent<SpriteRenderable>();
				
				{
					D3D11_MAPPED_SUBRESOURCE mappedResource;
					auto pMatrixBuffer = Renderer::GetMatrixBuffer();

					// map
					pImmediateContext->Map(pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
					Renderer::MatrixBufferType* pPtr = static_cast<Renderer::MatrixBufferType*>(mappedResource.pData);

					// world
					// �̰� ���������� ���� ������ ����̴�.
					// ���������� ������ ������Ʈ���� ��� �ٷ���� �� �����ؾ� �Ѵ�.
					auto matWorld = DirectX::XMMatrixTranspose(pTransform->GetMatrix());
					pPtr->world = matWorld;
					pPtr->view = DirectX::XMMatrixTranspose(view);
					pPtr->proj = DirectX::XMMatrixTranspose(proj);

					// unmap
					pImmediateContext->Unmap(pMatrixBuffer, 0);

					pImmediateContext->VSSetConstantBuffers(0, 1, &pMatrixBuffer);
				}

				auto pShaderResourceView = pSpriteRenderable->GetShaderResourceView();
				pImmediateContext->PSSetShaderResources(0, 1, &pShaderResourceView);

				auto pVertexBuffer = pSpriteRenderable->GetVertexBuffer();
				DV_ASSERT(pVertexBuffer);
				unsigned int stride = pSpriteRenderable->GetVertexStride();
				unsigned int offset = 0;
				pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

				auto pIndexBuffer = pSpriteRenderable->GetIndexBuffer();
				DV_ASSERT(pIndexBuffer);
				auto indexCount = pSpriteRenderable->GetIndexCount();
				auto indexFormat = pSpriteRenderable->GetIndexForamt();
				pImmediateContext->IASetIndexBuffer(pIndexBuffer, indexFormat, 0);

				pImmediateContext->DrawIndexed(indexCount, 0, 0);
			}
		}
	}
}