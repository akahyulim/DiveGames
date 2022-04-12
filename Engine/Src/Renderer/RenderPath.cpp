#include "divepch.h"
#include "ConstantBuffers.h"
#include "RenderPath.h"
#include "PipelineState.h"
#include "CommandList.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/SpriteRenderable.h"
#include "Scene/Component/MeshRenderable.h"
#include "Model.h"
#include "Material.h"

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
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			auto pFrameBuffer = Renderer::GetMatrixBuffer();

			// map
			pImmediateContext->Map(pFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			FrameBuffer* pPtr = static_cast<FrameBuffer*>(mappedResource.pData);

			pPtr->view = DirectX::XMMatrixTranspose(view);
			pPtr->proj = DirectX::XMMatrixTranspose(proj);

			// unmap
			pImmediateContext->Unmap(pFrameBuffer, 0);

			pImmediateContext->VSSetConstantBuffers(0, 1, &pFrameBuffer);
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

			// 여기는 ubber buffer가 없다...
			for(auto pGameObject : m_MainVisibilities.visibleSpriteRenderables)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pSpriteRenderable = pGameObject->GetComponent<SpriteRenderable>();

				// 슬롯이 있다.
				auto pShaderResourceView = pSpriteRenderable->GetShaderResourceView();
				pImmediateContext->PSSetShaderResources(0, 1, &pShaderResourceView);

				// UberBuffer
				// 현재 임시로 UberBuffer를 활용했다.
				// 실제로는 따로 전용 혹은 공용 Buffer를 두어야 한다.
				{
					D3D11_MAPPED_SUBRESOURCE mappedResource;
					auto pUberBuffer = Renderer::GetUberBuffer();

					// map
					pImmediateContext->Map(pUberBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
					UberBuffer* pPtr = static_cast<UberBuffer*>(mappedResource.pData);

					// 오직 이 값을 전달하기 위해 UberBuffer를 사용한다...
					// 이 구현이 싫다면 world를 전달하는 다른 constant buffer를 만들어야 한다.
					// 아니면 SpriteBuffer, MaterialBuffer 등으로 구분 구현할 수 있다.
					// 그런데 사실 Mesh에서 Quad를 만들면 SpriteRenderable 자체가 필요없어진다...
					// 물론 Sprite와 Quad Mesh가 다른 성격일 순 있다.
					pPtr->world = DirectX::XMMatrixTranspose(pTransform->GetMatrix());

					// unmap
					pImmediateContext->Unmap(pUberBuffer, 0);

					// 역시 슬롯 설정이 필요하다.
					// 그리고 WorldMatrix 때문에 VS에도 bind해야 한다.
					pImmediateContext->VSSetConstantBuffers(1, 1, &pUberBuffer);
					pImmediateContext->PSSetConstantBuffers(1, 1, &pUberBuffer);
				}
				// 전부 command list에서 bind하도록 수정해야 한다.
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

		if(!m_MainVisibilities.visibleMeshRenderables.empty())
		{
			// pipeline은 il, shader가 다르다.
			PipelineState ps;
			ps.primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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

				// UberBuffer
				{
					auto pMaterial = pMeshRenderable->GetMaterial();
					if (pMaterial)
					{
						D3D11_MAPPED_SUBRESOURCE mappedResource;
						auto pUberBuffer = Renderer::GetUberBuffer();

						// map
						pImmediateContext->Map(pUberBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
						UberBuffer* pPtr = static_cast<UberBuffer*>(mappedResource.pData);

						pPtr->world = DirectX::XMMatrixTranspose(pTransform->GetMatrix());

						pPtr->materialColor = pMaterial->GetAlbedoColor();
						pPtr->materialTextures = 0;
						pPtr->materialTextures |= pMaterial->HasTexture(eMaterialMapType::Albedo) ? (1U << 0) : 0;
						pPtr->materialTextures |= pMaterial->HasTexture(eMaterialMapType::Normal) ? (1U << 1) : 0;

						// unmap
						pImmediateContext->Unmap(pUberBuffer, 0);

						// 역시 슬롯 설정이 필요하다.
						// 그리고 WorldMatrix 때문에 VS에도 bind해야 한다.
						pImmediateContext->VSSetConstantBuffers(1, 1, &pUberBuffer);
						pImmediateContext->PSSetConstantBuffers(1, 1, &pUberBuffer);

						// 텍스쳐 유무에 따라 available 값을 전달하는 방법을 생각해 볼 수 있다.
						Texture2D* pAlbedoTex = dynamic_cast<Texture2D*>(pMaterial->GetMap(eMaterialMapType::Albedo));
						auto pSrv = pAlbedoTex ? pAlbedoTex->GetShaderResourceView() : nullptr;
						pImmediateContext->PSSetShaderResources(1, 1, &pSrv);
					}
				}

				// renderable로부터 model 획득
				// buffer는 Model로부터 획득
				// bind시 stride가 필요하다. offset은 그냥 0인듯
				auto pModel = pMeshRenderable->GetModel();

				// => 굳이 Model을 Renderable로부터 획득한 후 다시 이를 이용해 Buffer를 획득할 필요가 있나 싶다.
				// Renderable의 interface를 위해 Renderable이 직접 Buffer를 리턴해도 될 것 같다.
				auto pVertexBuffer = (ID3D11Buffer*)pModel->GetVertexBuffer();
				DV_ASSERT(pVertexBuffer);
				unsigned int stride = sizeof(VertexType);
				unsigned int offset = 0;
				pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

				auto pIndexBuffer = (ID3D11Buffer*)pModel->GetIndexBuffer();
				DV_ASSERT(pIndexBuffer);
				auto indexFormat = DXGI_FORMAT_R32_UINT;
				pImmediateContext->IASetIndexBuffer(pIndexBuffer, indexFormat, 0);

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