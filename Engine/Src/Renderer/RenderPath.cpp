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
#include "Material.h"
#include "Model.h"

namespace Dive
{
	RenderPath::RenderPath()
	{
		// camera가 필요하다.

		SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
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
			// PerpectiveValue를 추가해야 한다.
			// Camera Position을 추가해야 한다.
			// => 둘 다 Editor의 Path Render에선 추가했다.
			pCbFrame->Unmap();

			cl.SetConstantBuffer(Scope_Vertex, eConstantBufferSlot::Frame, pCbFrame);
		}

		// 일단 하드 코딩
		auto pPointSampler = Renderer::GetSamplerState(eSamplerStateType::Point);
		auto pLinearSampler = Renderer::GetSamplerState(eSamplerStateType::Linear);
		pImmediateContext->PSSetSamplers(0, 1, &pPointSampler);
		pImmediateContext->PSSetSamplers(1, 1, &pLinearSampler);

		passDefault(&cl);
	}

	void RenderPath::OnActiveScene(const Event& e)
	{
		const auto& evnt = dynamic_cast<const CreateSceneEvent&>(e);
		m_pActiveScene = const_cast<Scene*>(evnt.GetCreatedScene());
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
			ps.renderTargetViews[0] = Renderer::GetGBufferAlbedo()->GetRenderTargetView();
			ps.pViewport			= Renderer::GetGBufferAlbedo()->GetViewport();
			ps.pDepthStencilView	= Renderer::GetDepthStencilTexture()->GetDepthStencilView();
			
			pCl->BindPipelineState(ps);

			for(auto pGameObject : m_MainVisibilities.visibleSpriteRenderables)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pSpriteRenderable = pGameObject->GetComponent<SpriteRenderable>();

				if (!pSpriteRenderable->HasSprite())
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

					auto pShaderResourceView = pSpriteRenderable->GetSprite()->GetShaderResourceView();
					pImmediateContext->PSSetShaderResources(1, 1, &pShaderResourceView);
				}
				
				pCl->SetVertexBuffer(pSpriteRenderable->GetVertexBuffer());
				pCl->SetIndexBuffer(pSpriteRenderable->GetIndexBuffer());

				// count는 임시
				pImmediateContext->DrawIndexed(6, 0, 0);
			}
		}

		if(!m_MainVisibilities.visibleLights.empty())
		{ 
			// forward light 이므로 Light * object 만큼 draw call이 발생한다.
			// 그리고 현재 구현이 잘못되어 두 번째 Light부터 적용이 되지 않는다.
			// blend state에서 처리해야 할 것 같다. 아직 반투명 그리기도 안된다.
			for (auto pGameObject : m_MainVisibilities.visibleLights)
			{
				auto pLight = pGameObject->GetComponent<Light>();
				if (pLight == nullptr || !pLight->IsEnabled())
					continue;

				auto pTransform = pGameObject->GetComponent<Transform>();

				// 현재 테스트에선 cb만 만들어서 등록하면 될 듯
				// map & unmap
				auto pCbLight = Renderer::GetCbLight();
				auto pPtr = static_cast<LightBuffer*>(pCbLight->Map());
				
				pPtr->pos		= pTransform->GetLocalPosition();
				pPtr->rangeRcp	= 1.0f / pLight->GetRange();
				pPtr->dir		= pTransform->GetForward();
				pPtr->spotAngle = cosf(pLight->GetSpotAngleRadian());
				pPtr->color		= pLight->GetColor();
				pPtr->options	= 0;
				if (pLight->GetLightType() == eLightType::Directional)
				{					
					pPtr->options |= (1 << 0);
				}
				if (pLight->GetLightType() == eLightType::Point)
				{
					pPtr->options |= (1 << 1);
				}
				if (pLight->GetLightType() == eLightType::Spot)
				{
					pPtr->options |= (1 << 2);
				}
				pCbLight->Unmap();

				pCl->SetConstantBuffer(Scope_Vertex | Scope_Pixel, eConstantBufferSlot::Light, pCbLight);

				if (!m_MainVisibilities.visibleMeshRenderables.empty())
				{
					// pipeline은 il, shader가 다르다.
					PipelineState ps;
					ps.primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;//D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
					ps.pInputLayout			= Renderer::GetShader(eShaderType::Mesh)->pInputLayout;
					ps.pVertexShader		= Renderer::GetShader(eShaderType::Mesh)->pVertexShader;
					ps.pRasterizerState		= Renderer::GetRasterizerState(eRasterizerStateType::CullBackSolid);
					ps.pPixelShader			= Renderer::GetShader(eShaderType::Mesh)->pPixelShader;
					ps.pDepthStencilState	= Renderer::GetDepthStencilState(eDepthStencilStateType::DepthOnStencilOn);
					ps.renderTargetViews[0] = Renderer::GetGBufferAlbedo()->GetRenderTargetView();
					ps.pViewport			= Renderer::GetGBufferAlbedo()->GetViewport();
					ps.pDepthStencilView	= Renderer::GetDepthStencilTexture()->GetDepthStencilView();

					// 불투명 + 멀티 라이트를 위한 add다.
					// 어떻게 하는지 아직 모르겠다.
					// 책에서 bind하는 순서를 분석해야 할 듯 하다.
					//if (pLight->GetLightType() == eLightType::Directional)
						ps.pBlendState = Renderer::GetBlendState(eBlendStateType::Disabled);
					//else
					//	ps.pBlendState = Renderer::GetBlendState(eBlendStateType::Addtive);

					pCl->BindPipelineState(ps);

					for (auto pGameObject : m_MainVisibilities.visibleMeshRenderables)
					{
						auto pTransform = pGameObject->GetComponent<Transform>();
						auto pMeshRenderable = pGameObject->GetComponent<MeshRenderable>();

						// renderable로부터 model 획득
						// buffer는 Model로부터 획득
						// bind시 stride가 필요하다. offset은 그냥 0인듯
						auto pModel = pMeshRenderable->GetModel();
						if (pModel == nullptr)
							continue;

						// 이것두 버퍼가 없을 수 있다.
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
								pPtr->world = DirectX::XMMatrixTranspose(pTransform->GetMatrix());
								pPtr->materialColor = pMaterial->GetAlbedoColor();
								pPtr->materialTextures = 0;
								pPtr->materialTextures |= pMaterial->HasMap(eMaterialMapType::Albedo) ? (1U << 0) : 0;
								pPtr->materialTextures |= pMaterial->HasMap(eMaterialMapType::Normal) ? (1U << 1) : 0;
								pCbUber->Unmap();

								pCl->SetConstantBuffer(Scope_Vertex | Scope_Pixel, eConstantBufferSlot::Uber, pCbUber);

								// 이 부분도 CommandList로 옮겨야 한다.
								// slot은 eMaterialMapType으로 전달할 수 있을 것 같다.
								auto pAlbedoTex = pMaterial->GetMap(eMaterialMapType::Albedo);
								auto pSrv = pAlbedoTex ? pAlbedoTex->GetShaderResourceView() : nullptr;
								pImmediateContext->PSSetShaderResources(1, 1, &pSrv);
								auto pNormalTex = pMaterial->GetMap(eMaterialMapType::Normal);
								pSrv = pNormalTex ? pNormalTex->GetShaderResourceView() : nullptr;
								pImmediateContext->PSSetShaderResources(2, 1, &pSrv);
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
	}

	void RenderPath::passGBuffer(CommandList* pCl)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		if (!pImmediateContext)
			return;

		// 일단 직접 연결
		PipelineState ps;

		// PreRender
		auto pGBufferAlbedo			= Renderer::GetGBufferAlbedo()->GetRenderTargetView();
		auto pGBufferNormal			= Renderer::GetGBufferNormal()->GetRenderTargetView();
		auto pGBufferMaterial		= Renderer::GetGBufferMaterial()->GetRenderTargetView();
		auto pGBufferDepthStencil	= Renderer::GetDepthStencilTexture()->GetDepthStencilView();
		auto pDepthStencilState		= Renderer::GetDepthStencilState(eDepthStencilStateType::DepthOnStencilOn);

		// clear
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		pImmediateContext->ClearRenderTargetView(pGBufferAlbedo, clearColor);
		pImmediateContext->ClearRenderTargetView(pGBufferNormal, clearColor);
		pImmediateContext->ClearRenderTargetView(pGBufferMaterial, clearColor);
		pImmediateContext->ClearDepthStencilView(pGBufferDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// set
		ID3D11RenderTargetView* rt[3] = { pGBufferAlbedo, pGBufferNormal, pGBufferMaterial };
		pImmediateContext->OMSetRenderTargets(3, rt, pGBufferDepthStencil);
		pImmediateContext->OMSetDepthStencilState(pDepthStencilState, 1);

		// render scene
		if (!m_MainVisibilities.visibleMeshRenderables.empty())
		{
			// sampler state : 일단 제외
			// blend state : 일단 제외
			pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pImmediateContext->IASetInputLayout(Renderer::GetShader(eShaderType::Deferred)->pInputLayout);
			pImmediateContext->VSSetShader(Renderer::GetShader(eShaderType::Deferred)->pVertexShader, NULL, 0);
			pImmediateContext->PSSetShader(Renderer::GetShader(eShaderType::Deferred)->pPixelShader, NULL, 0);
			pImmediateContext->RSSetViewports(1, Renderer::GetGBufferAlbedo()->GetViewport());
			pImmediateContext->RSSetState(Renderer::GetRasterizerState(eRasterizerStateType::CullBackSolid));

			for (auto pGameObject : m_MainVisibilities.visibleMeshRenderables)
			{	
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pMeshRenderable = pGameObject->GetComponent<MeshRenderable>();

				auto pModel = pMeshRenderable->GetModel();
				if (pModel == nullptr)
					continue;
				if (!pModel->GetVertexBuffer() || !pModel->GetIndexBuffer())
					continue;

				// 굳이 바꿀 필요가 없다.
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
						pPtr->world = DirectX::XMMatrixTranspose(pTransform->GetMatrix());
						pPtr->materialColor = pMaterial->GetAlbedoColor();
						pPtr->materialTextures = 0;
						pPtr->materialTextures |= pMaterial->HasMap(eMaterialMapType::Albedo) ? (1U << 0) : 0;
						pPtr->materialTextures |= pMaterial->HasMap(eMaterialMapType::Normal) ? (1U << 1) : 0;
						pCbUber->Unmap();

						//pCl->SetConstantBuffer(Scope_Vertex | Scope_Pixel, eConstantBufferSlot::Uber, pCbUber);
						auto pBuffer = pCbUber->GetBuffer();
						pImmediateContext->VSSetConstantBuffers(static_cast<unsigned int>(eConstantBufferSlot::Uber), 1, &pBuffer);
						pImmediateContext->PSSetConstantBuffers(static_cast<unsigned int>(eConstantBufferSlot::Uber), 1, &pBuffer);

						// 이 부분도 CommandList로 옮겨야 한다.
						// slot은 eMaterialMapType으로 전달할 수 있을 것 같다.
						auto pAlbedoTex = pMaterial->GetMap(eMaterialMapType::Albedo);
						auto pSrv = pAlbedoTex ? pAlbedoTex->GetShaderResourceView() : nullptr;
						pImmediateContext->PSSetShaderResources(1, 1, &pSrv);
						auto pNormalTex = pMaterial->GetMap(eMaterialMapType::Normal);
						pSrv = pNormalTex ? pNormalTex->GetShaderResourceView() : nullptr;
						pImmediateContext->PSSetShaderResources(2, 1, &pSrv);
					}
				}

				auto indexCount = pMeshRenderable->IndexCount();
				auto indexOffset = pMeshRenderable->IndexOffset();
				auto vertexOffset = pMeshRenderable->VertexOffset();
				pImmediateContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
			}
		}
	}

	void RenderPath::passLighting(CommandList* pCl)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		if (!pImmediateContext)
			return;

		// GBuffer의 값을 이용해 light를 적용
		// 즉, render target view가 달라진다.

		// 일단 선형 깊이값을 출력토록 하자.
		// 현재 깊이값은 비 선형값으로 저장된 상태다.
		// 따라서 셰이더에서 계산한 후 출력해야 한다.

		// PostRender
		ID3D11RenderTargetView* rt[3] = { NULL, NULL, NULL };
		auto pDepthStencilViewReadOnly = Renderer::GetDepthStencilTexture()->GetDepthStencilViewReadOnly();
		pImmediateContext->OMSetRenderTargets(3, rt, pDepthStencilViewReadOnly);

		// 괜히 두 번 한다...
		auto pRenderTargetView = Renderer::GetFrameOutput()->GetRenderTargetView();
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColor);
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilViewReadOnly);


		auto pDepthStencilShaderResourceView = Renderer::GetDepthStencilTexture()->GetShaderResourceView();
		auto pAlbedoShaderResourceView = Renderer::GetGBufferAlbedo()->GetShaderResourceView();
		auto pNormalShaderResourceView = Renderer::GetGBufferNormal()->GetShaderResourceView();
		auto pMaterialShaderResourceView = Renderer::GetGBufferMaterial()->GetShaderResourceView();
		
		pImmediateContext->PSSetShaderResources(0, 1, &pDepthStencilShaderResourceView);
		pImmediateContext->PSSetShaderResources(1, 1, &pAlbedoShaderResourceView);
		pImmediateContext->PSSetShaderResources(2, 1, &pNormalShaderResourceView);
		pImmediateContext->PSSetShaderResources(3, 1, &pMaterialShaderResourceView);

		pImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pImmediateContext->IASetInputLayout(Renderer::GetShader(eShaderType::Light)->pInputLayout);
		pImmediateContext->VSSetShader(Renderer::GetShader(eShaderType::Light)->pVertexShader, nullptr, 0);
		pImmediateContext->PSSetShader(Renderer::GetShader(eShaderType::Light)->pPixelShader, nullptr, 0);

		pImmediateContext->Draw(4, 0);
	}
}