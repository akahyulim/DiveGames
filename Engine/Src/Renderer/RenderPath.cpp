#include "divepch.h"
#include "ConstantBuffers.h"	// 이름이 좀 에바다.
#include "RenderPath.h"
#include "PipelineState.h"
#include "CommandList.h"
#include "Graphics/ConstantBuffer.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/SpriteRenderable.h"
#include "Scene/Component/MeshRenderable.h"
#include "Model.h"
#include "LegacyMaterial.h"

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
	// 이 곳은 이름 그대로 Udpate 구문으로 채우고
	// VisibilityUpdate는 Event 처리로 할 수도 있다.
	void RenderPath::Update(float delta)
	{
		// scene, camera 설정
		m_MainVisibilities;

		Renderer::UpdateVisibility(m_MainVisibilities);
	}
	
	void RenderPath::Render()
	{
		CommandList cl;

		// viewport?
		// sampler, global cbuffer

		// bind까지 미리 하는 것도 생각해볼 수 있다.
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();

		// 임시
		DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX proj = DirectX::XMMatrixIdentity();

		// FrameBuffer
		{
			auto pCbFrame = Renderer::GetCbFrame();

			// map & unmap
			auto pPtr = static_cast<FrameBuffer*>(pCbFrame->Map());
			pPtr->view = DirectX::XMMatrixTranspose(view);
			pPtr->proj = DirectX::XMMatrixTranspose(proj);
			// Camera Position을 추가해야 한다.
			pCbFrame->Unmap();

			cl.SetConstantBuffer(Scope_Vertex, eConstantBufferSlot::Frame, pCbFrame);
		}

		passDefault(&cl);
	}

	// 스파르탄은 RenderObject Type을 전달받는다.
	void RenderPath::passDefault(CommandList* pCl)
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
			ps.renderTargetViews[0] = Renderer::GetSampleTexture()->GetRenderTargetView();
			ps.pViewport			= Renderer::GetSampleTexture()->GetViewport();
			ps.pDepthStencilView	= Renderer::GetDepthStencilTexture()->GetDepthStencilView();
			
			pCl->BindPipelineState(ps);

			for(auto pGameObject : m_MainVisibilities.visibleSpriteRenderables)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pSpriteRenderable = pGameObject->GetComponent<SpriteRenderable>();

				// 버퍼가 없을 수 있다.
				if (!pSpriteRenderable->GetVertexBuffer() || !pSpriteRenderable->GetIndexBuffer())
					continue;

				// Sprite Buffer
				{
					auto pCbSprite = Renderer::GetCbSprite();

					// map & unmap
					auto pPtr = static_cast<SpriteBuffer*>(pCbSprite->Map());
					pPtr->world			= DirectX::XMMatrixTranspose(pTransform->GetMatrix());
					pPtr->materialColor = pSpriteRenderable->GetColor();
					pPtr->options		= 0;
					pPtr->options		|= pSpriteRenderable->HasSprite() ? (1U << 0) : 0;
					pPtr->options		|= pSpriteRenderable->IsFlipX() ? (1U << 1) : 0;
					pPtr->options		|= pSpriteRenderable->IsFlipY() ? (1U << 2) : 0;
					pCbSprite->Unmap();

					pCl->SetConstantBuffer(Scope_Vertex | Scope_Pixel, eConstantBufferSlot::Sprite, pCbSprite);

					if (pSpriteRenderable->HasSprite())
					{
						auto pShaderResourceView = pSpriteRenderable->GetSprite()->GetShaderResourceView();
						pImmediateContext->PSSetShaderResources(1, 1, &pShaderResourceView);
					}
				}
				
				
				pCl->SetVertexBuffer(pSpriteRenderable->GetVertexBuffer());
				pCl->SetIndexBuffer(pSpriteRenderable->GetIndexBuffer());

				// count는 임시
				pImmediateContext->DrawIndexed(6, 0, 0);
			}
		}

		if(!m_MainVisibilities.visibleMeshRenderables.empty())
		{
			// pipeline은 il, shader가 다르다.
			PipelineState ps;
			ps.primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;//D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			ps.pInputLayout			= Renderer::GetShader(eShaderType::Mesh)->pInputLayout;
			ps.pVertexShader		= Renderer::GetShader(eShaderType::Mesh)->pVertexShader;
			ps.pRasterizerState		= Renderer::GetRasterizerState(eRasterizerStateType::CullBackSolid);
			ps.pPixelShader			= Renderer::GetShader(eShaderType::Mesh)->pPixelShader;
			ps.pDepthStencilState	= Renderer::GetDepthStencilState(eDepthStencilStateType::DepthOnStencilOn);
			ps.renderTargetViews[0] = Renderer::GetSampleTexture()->GetRenderTargetView();
			ps.pViewport			= Renderer::GetSampleTexture()->GetViewport();
			ps.pDepthStencilView	= Renderer::GetDepthStencilTexture()->GetDepthStencilView();

			pCl->BindPipelineState(ps);

			for (auto pGameObject : m_MainVisibilities.visibleMeshRenderables)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pMeshRenderable = pGameObject->GetComponent<MeshRenderable>();

				// renderable로부터 model 획득
				// buffer는 Model로부터 획득
				// bind시 stride가 필요하다. offset은 그냥 0인듯
				auto pModel = pMeshRenderable->GetModel();

				// 이것두 버퍼가 없을 수 있나...?
				if (!pModel->GetVertexBuffer() || !pModel->GetIndexBuffer())
					continue;
				
				pCl->SetVertexBuffer(pModel->GetVertexBuffer());
				pCl->SetIndexBuffer(pModel->GetIndexBuffer());

				// UberBuffer
				{
					auto pMaterial = pMeshRenderable->GetMaterial();
					if (pMaterial)
					{
						auto pCbUber = Renderer::GetCbUber();

						// map & unmap
						auto pPtr = static_cast<UberBuffer*>(pCbUber->Map());
						pPtr->world				= DirectX::XMMatrixTranspose(pTransform->GetMatrix());
						pPtr->materialColor		= pMaterial->GetAlbedoColor();
						pPtr->materialTextures	= 0;
						pPtr->materialTextures	|= pMaterial->HasTexture(eMaterialMapType::Albedo) ? (1U << 0) : 0;
						pPtr->materialTextures	|= pMaterial->HasTexture(eMaterialMapType::Normal) ? (1U << 1) : 0;
						pCbUber->Unmap();
						
						pCl->SetConstantBuffer(Scope_Vertex | Scope_Pixel, eConstantBufferSlot::Uber, pCbUber);

						// 텍스쳐 유무에 따라 available 값을 전달하는 방법을 생각해 볼 수 있다.
						auto pAlbedoTex = pMaterial->GetMap(eMaterialMapType::Albedo);
						auto pSrv = pAlbedoTex ? pAlbedoTex->GetShaderResourceView() : nullptr;
						pImmediateContext->PSSetShaderResources(1, 1, &pSrv);
					}
				}

				// draw시 index count, index offset, vertex offset이 필요하다.
				// 이들은 renderable이 관리한다.
				auto indexCount = pMeshRenderable->IndexCount();
				auto indexOffset = pMeshRenderable->IndexOffset();
				auto vertexOffset = pMeshRenderable->VertexOffset();
				pImmediateContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
			}
		}
	}
}