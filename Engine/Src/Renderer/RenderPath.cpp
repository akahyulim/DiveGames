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
		// active scene과 camera가 필요하다.
	}

	RenderPath::~RenderPath()
	{
	}

	// 매 프레임 Visibilities를 분류하는 것은 비효율적이다.
	// 이 곳은 말 그래돌 Udpate 구문으로 채우고
	// VisibilityUpdate는 Event 처리로 할 수도 있다.
	void RenderPath::Update(float delta)
	{
		// scene, camera 설정
		m_MainVisibilities;

		Renderer::UpdateVisibility(m_MainVisibilities);
	}
	
	void RenderPath::Render()
	{
		// 이전 state 비교용
		CommandList cl;

		// sampler, global cbuffer

		// bind까지 미리 하는 것도 생각해볼 수 있다.
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		passDefault(&cl, view, proj);
	}

	// 스파르탄은 RenderObject Type을 전달받는다.
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

			// 카메라에 맞춰 Viewport도 bind해야 한다...

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
					// 이건 계층구조로 인해 누적된 행렬이다.
					// 계층구조로 구성된 오브젝트들을 어떻게 다뤄야할 지 결정해야 한다.
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