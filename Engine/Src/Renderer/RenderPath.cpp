#include "divepch.h"
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
		// active scene�� camera�� �ʿ��ϴ�.
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
			ps.renderTargetViews[0] = Renderer::GetSampleTexture()->GetRenderTargetView();
			ps.pViewport			= Renderer::GetSampleTexture()->GetViewport();
			ps.pDepthStencilView	= Renderer::GetDepthStencilTexture()->GetDepthStencilView();
			
			pCl->BindPipelineState(ps);

			for(auto pGameObject : m_MainVisibilities.visibleSpriteRenderables)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				auto pSpriteRenderable = pGameObject->GetComponent<SpriteRenderable>();
				
				{
					D3D11_MAPPED_SUBRESOURCE mappedResource;
					auto pMatrixBuffer = Renderer::GetMatrixBuffer();

					// map
					pImmediateContext->Map(pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
					Renderer::FrameBuffer* pPtr = static_cast<Renderer::FrameBuffer*>(mappedResource.pData);

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

				// ������ �ִ�.
				auto pShaderResourceView = pSpriteRenderable->GetShaderResourceView();
				pImmediateContext->PSSetShaderResources(0, 1, &pShaderResourceView);

				// ���� command list���� bind�ϵ��� �����ؾ� �Ѵ�.
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
			// pipeline�� il, shader�� �ٸ���.
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

				// �̰� ���� frame �� �� �� update �ǵ��� ������ �ʿ��ϴ�.
				// FrameBuffer
				{
					D3D11_MAPPED_SUBRESOURCE mappedResource;
					auto pMatrixBuffer = Renderer::GetMatrixBuffer();

					// map
					pImmediateContext->Map(pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
					Renderer::FrameBuffer* pPtr = static_cast<Renderer::FrameBuffer*>(mappedResource.pData);

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

				// material test
				{
					auto pMaterial = pMeshRenderable->GetMaterial();
					if (pMaterial)
					{
						D3D11_MAPPED_SUBRESOURCE mappedResource;
						auto pUberBuffer = Renderer::GetUberBuffer();

						// map
						pImmediateContext->Map(pUberBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
						Renderer::UberBuffer* pPtr = static_cast<Renderer::UberBuffer*>(mappedResource.pData);

						pPtr->materialColor = pMaterial->GetAlbedoColor();
						
						// unmap
						pImmediateContext->Unmap(pUberBuffer, 0);

						// ���� ���� ������ �ʿ��ϴ�.
						pImmediateContext->PSSetConstantBuffers(1, 1, &pUberBuffer);

						// �ؽ��� ������ ���� available ���� �����ϴ� ����� ������ �� �� �ִ�.
						Texture2D* pAlbedoTex = dynamic_cast<Texture2D*>(pMaterial->GetMap(eMaterialMapType::Albedo));
						auto pSrv = pAlbedoTex ? pAlbedoTex->GetShaderResourceView() : nullptr;
						pImmediateContext->PSSetShaderResources(1, 1, &pSrv);
					}

					// ObjectBuffer
					{
						// world
						// wvp�� world�� FrameBuffer�� vp�� ���Ѵ�.
					}
				}

				// renderable�κ��� model ȹ��
				// buffer�� Model�κ��� ȹ��
				// bind�� stride�� �ʿ��ϴ�. offset�� �׳� 0�ε�
				auto pModel = pMeshRenderable->GetModel();

				// => ���� Model�� Renderable�κ��� ȹ���� �� �ٽ� �̸� �̿��� Buffer�� ȹ���� �ʿ䰡 �ֳ� �ʹ�.
				// Renderable�� interface�� ���� Renderable�� ���� Buffer�� �����ص� �� �� ����.
				auto pVertexBuffer = (ID3D11Buffer*)pModel->GetVertexBuffer();
				DV_ASSERT(pVertexBuffer);
				unsigned int stride = sizeof(VertexType);
				unsigned int offset = 0;
				pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

				auto pIndexBuffer = (ID3D11Buffer*)pModel->GetIndexBuffer();
				DV_ASSERT(pIndexBuffer);
				auto indexFormat = DXGI_FORMAT_R32_UINT;
				pImmediateContext->IASetIndexBuffer(pIndexBuffer, indexFormat, 0);

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