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
			// Camera Position�� �߰��ؾ� �Ѵ�.
			pCbFrame->Unmap();

			cl.SetConstantBuffer(Scope_Vertex, eConstantBufferSlot::Frame, pCbFrame);
		}

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
			ps.renderTargetViews[0] = Renderer::GetGbufferAlbedo()->GetRenderTargetView();
			ps.pViewport			= Renderer::GetGbufferAlbedo()->GetViewport();
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
				
				pPtr->color = pLight->GetColor();

				pPtr->options = 0;
				if (pLight->GetLightType() == eLightType::Directional)
				{
					pPtr->dir = pTransform->GetForward();
					pPtr->options |= (1 << 0);
				}
				if (pLight->GetLightType() == eLightType::Point)
				{
					// local�� �ؾ� �Ѵ�. ���� Ȯ�� �ʿ�.
					pPtr->pos = pTransform->GetLocalPosition();
					pPtr->range = 1.0f / pLight->GetRange();
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
					ps.renderTargetViews[0] = Renderer::GetGbufferAlbedo()->GetRenderTargetView();
					ps.pViewport			= Renderer::GetGbufferAlbedo()->GetViewport();
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
}