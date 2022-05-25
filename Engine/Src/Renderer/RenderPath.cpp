#include "divepch.h"
#include "ConstantBuffers.h"	// �̸��� �� ���ٴ�.
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
		// camera�� �ʿ��ϴ�.

		SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
	}

	RenderPath::~RenderPath()
	{
	}

	// �� ������ Visibilities�� �з��ϴ� ���� ��ȿ�����̴�.
	// �� ���� �̸� �״�� Udpate �������� ä���
	// VisibilityUpdate�� Event ó���� �� ���� �ִ�.
	void RenderPath::Update(float delta)
	{
		// scene, camera ����
		m_MainVisibilities;

		Renderer::UpdateVisibility(m_MainVisibilities);
	}
	
	void RenderPath::Render()
	{
		CommandList cl;

		// viewport?
		// sampler, global cbuffer

		// bind���� �̸� �ϴ� �͵� �����غ� �� �ִ�.
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();

		// �ӽ�
		DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX proj = DirectX::XMMatrixIdentity();

		// FrameBuffer
		{
			auto pCbFrame = Renderer::GetCbFrame();

			// map & unmap
			auto pPtr = static_cast<FrameBuffer*>(pCbFrame->Map());
			pPtr->view = DirectX::XMMatrixTranspose(view);
			pPtr->proj = DirectX::XMMatrixTranspose(proj);
			// PerpectiveValue�� �߰��ؾ� �Ѵ�.
			// Camera Position�� �߰��ؾ� �Ѵ�.
			// => �� �� Editor�� Path Render���� �߰��ߴ�.
			pCbFrame->Unmap();

			cl.SetConstantBuffer(Scope_Vertex, eConstantBufferSlot::Frame, pCbFrame);
		}

		// �ϴ� �ϵ� �ڵ�
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

	// ���ĸ�ź�� RenderObject Type�� ���޹޴´�.
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

				// count�� �ӽ�
				pImmediateContext->DrawIndexed(6, 0, 0);
			}
		}

		if(!m_MainVisibilities.visibleLights.empty())
		{ 
			// forward light �̹Ƿ� Light * object ��ŭ draw call�� �߻��Ѵ�.
			// �׸��� ���� ������ �߸��Ǿ� �� ��° Light���� ������ ���� �ʴ´�.
			// blend state���� ó���ؾ� �� �� ����. ���� ������ �׸��⵵ �ȵȴ�.
			for (auto pGameObject : m_MainVisibilities.visibleLights)
			{
				auto pLight = pGameObject->GetComponent<Light>();
				if (pLight == nullptr || !pLight->IsEnabled())
					continue;

				auto pTransform = pGameObject->GetComponent<Transform>();

				// ���� �׽�Ʈ���� cb�� ���� ����ϸ� �� ��
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
					// pipeline�� il, shader�� �ٸ���.
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

					// ������ + ��Ƽ ����Ʈ�� ���� add��.
					// ��� �ϴ��� ���� �𸣰ڴ�.
					// å���� bind�ϴ� ������ �м��ؾ� �� �� �ϴ�.
					//if (pLight->GetLightType() == eLightType::Directional)
						ps.pBlendState = Renderer::GetBlendState(eBlendStateType::Disabled);
					//else
					//	ps.pBlendState = Renderer::GetBlendState(eBlendStateType::Addtive);

					pCl->BindPipelineState(ps);

					for (auto pGameObject : m_MainVisibilities.visibleMeshRenderables)
					{
						auto pTransform = pGameObject->GetComponent<Transform>();
						auto pMeshRenderable = pGameObject->GetComponent<MeshRenderable>();

						// renderable�κ��� model ȹ��
						// buffer�� Model�κ��� ȹ��
						// bind�� stride�� �ʿ��ϴ�. offset�� �׳� 0�ε�
						auto pModel = pMeshRenderable->GetModel();
						if (pModel == nullptr)
							continue;

						// �̰͵� ���۰� ���� �� �ִ�.
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

								// �� �κе� CommandList�� �Űܾ� �Ѵ�.
								// slot�� eMaterialMapType���� ������ �� ���� �� ����.
								auto pAlbedoTex = pMaterial->GetMap(eMaterialMapType::Albedo);
								auto pSrv = pAlbedoTex ? pAlbedoTex->GetShaderResourceView() : nullptr;
								pImmediateContext->PSSetShaderResources(1, 1, &pSrv);
								auto pNormalTex = pMaterial->GetMap(eMaterialMapType::Normal);
								pSrv = pNormalTex ? pNormalTex->GetShaderResourceView() : nullptr;
								pImmediateContext->PSSetShaderResources(2, 1, &pSrv);
							}
						}

						// draw�� index count, index offset, vertex offset�� �ʿ��ϴ�.
						// �̵��� renderable�� �����Ѵ�.
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

		// �ϴ� ���� ����
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
			// sampler state : �ϴ� ����
			// blend state : �ϴ� ����
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

				// ���� �ٲ� �ʿ䰡 ����.
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

						// �� �κе� CommandList�� �Űܾ� �Ѵ�.
						// slot�� eMaterialMapType���� ������ �� ���� �� ����.
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

		// GBuffer�� ���� �̿��� light�� ����
		// ��, render target view�� �޶�����.

		// �ϴ� ���� ���̰��� ������ ����.
		// ���� ���̰��� �� ���������� ����� ���´�.
		// ���� ���̴����� ����� �� ����ؾ� �Ѵ�.

		// PostRender
		ID3D11RenderTargetView* rt[3] = { NULL, NULL, NULL };
		auto pDepthStencilViewReadOnly = Renderer::GetDepthStencilTexture()->GetDepthStencilViewReadOnly();
		pImmediateContext->OMSetRenderTargets(3, rt, pDepthStencilViewReadOnly);

		// ���� �� �� �Ѵ�...
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