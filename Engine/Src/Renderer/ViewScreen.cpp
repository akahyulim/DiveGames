#include "divepch.h"
#include "ViewScreen.h"
#include "DvMaterial.h"
#include "DvMesh.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DvRenderTexture.h"
#include "Graphics/DvConstantBuffer.h"
#include "Core/CoreDefs.h"
#include "Scene/DvScene.h"
#include "Scene/DvGameObject.h"
#include "Scene/Components/DvTransform.h"
#include "Scene/Components/DvCamera.h"
#include "Scene/Components/Renderable.h"
#include "Scene/Components/DvLight.h"


namespace Dive
{
	ViewScreen::ViewScreen(DvScene* pScene, DvCamera* pCamera, eRenderPath renderPath)
		: m_pScene(pScene)
		, m_pCamera(pCamera)
		, m_RenderPath(renderPath)
	{
		// �ӽ�: create constant buffer 
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuMaterialBuffer,sizeof(m_cpuMaterialBuffer));
		ZeroMemory(&m_cpuCameraBuffer, sizeof(m_cpuCameraBuffer));
		ZeroMemory(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));
	}

	void ViewScreen::Update()
	{
		if (m_pScene->IsEmpty())
			return;

		m_Renderables.clear();
		m_Lights.clear();

		m_RenderableBatches.clear();
		m_LightBatches.clear();

		// �ӽ�
		ZeroMemory(&m_cpuFrameBuffer, sizeof(m_cpuFrameBuffer));
		ZeroMemory(&m_cpuMaterialBuffer, sizeof(m_cpuMaterialBuffer));
		ZeroMemory(&m_cpuCameraBuffer, sizeof(m_cpuCameraBuffer));
		ZeroMemory(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));

		// GetDrawables()
		{
			for (auto pGameObject : m_pScene->GetAllGameObjects())
			{
				auto pRenderableCom = pGameObject->GetComponent<Renderable>();
				if (pRenderableCom)
				{
					m_Renderables.push_back(pRenderableCom);
					continue;
				}

				auto pLightCom = pGameObject->GetComponent<DvLight>();
				if (pLightCom)
				{
					m_Lights.push_back(pLightCom);
					continue;
				}
			}

		}

		// GetBatches()
		{
			for (auto pRenderable : m_Renderables)
			{
				RenderableBatch batch;
				batch.pMesh = pRenderable->GetMesh();
				batch.pMaterial = pRenderable->GetMaterial();
				batch.worldTransform = pRenderable->GetTransform()->GetWorldMatrix();

				m_RenderableBatches.emplace_back(batch);
			}
		}
	}
	
	void ViewScreen::Render()
	{
		switch (m_RenderPath)
		{
		case eRenderPath::Forward:
			forwardRender();
			return;
		case eRenderPath::Deferred:
			deferredRender();
			return;
		default:
			DV_CORE_ERROR("���ǵ��� ���� �����н��� ������ �� �����ϴ�.");
			return;
		}
	}

	// �ϴ� �� ������ �� �����غ���
	// ���Ŀ� draw call���� �Լ�ȭ ���Ѿ��� �� �ϴ�.
	// command list�� ������ ���̶�� �� �׷��� �Ѵ�.
	void ViewScreen::forwardRender()
	{
		// set & clear rendertarget
		{
			GraphicsDevice::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
			GraphicsDevice::SetDepthStencilView(GraphicsDevice::GetDefaultDepthStencilView());
			GraphicsDevice::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);
		}

		// urho ������ �� �κе� Batch::Prepare()�� ���Ѵ�.
		// ��, Batch���� ȣ��ǹǷ� �ߺ� üũ�� Ȯ���ϴ� ���� �¾ƺ��δ�.
		{
			// ����Ʈ
			GraphicsDevice::SetViewport(m_pCamera->GetViewport());

			{
				// ���۸� Renderer�� �����Ѵٸ� cpu���۴� �� ���� �ƴ϶� GraphcisDevice�� �����ϴ� ���� �´�.
				// �ٸ� �� ������ GraphicsDevice�� �����ϱ� ���ؼ�
				// 1. ������ ����, 2. ������ ������, 3. offset�� �ʿ��� ���δ�.
				// urho�� ��� SetShaderParamter��� �Լ��� �̸� �����ϴµ�
				// ���� �̸��� Ÿ�Ժ� �����͸� �����ϸ�
				// ���ο��� �ش� �̸��� ���۸� ������ �����͸� ����ְ� ��Ƽüũ�� �ϴ� �� �ϴ�.
				// �� ��� �����ͺ� offset�� �߿�������.
				m_cpuFrameBuffer.view = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
				m_cpuFrameBuffer.projection = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
			}

			{
				m_cpuCameraBuffer.position = m_pCamera->GetPosition();

				// �ָ������� ��ο��ݸ��� �� ���� �ص� �Ǵ� �۾��̴�.
				auto pCameraBuffer = DvRenderer::GetConstantBuffer(eConstantBuffer::Camera);
				pCameraBuffer->Update((void*)&m_cpuCameraBuffer);
				pCameraBuffer->Bind();
			}

			// states
			// ������ Renderer���� �� �س��� ����
			// �ᱹ RenderPath�� ���� � State�� ��� ������ ����ϴ��ĸ� �ľ��ؾ� �Ѵ�.
			// �׸��� urho�� ���� �ʿ� ���� �ϴ� �ϵ� �ڵ����� ��� state�� �����ϴ� ���� �¾� ���δ�.
			{
				GraphicsDevice::SetRasterizerState(DvRenderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
				GraphicsDevice::SetDepthStencilState(DvRenderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite));
				//GraphicsDevice::SetBlendState(DvRenderer::GetBlendState(eBlendState::Addictive));
			}
		}

		// bind lights
		// forward light�� light x mesh
		{
			for (int i = 0; i < (int)m_Lights.size(); i++)
			{
				auto light = m_Lights[i];

				switch (light->GetType())
				{

				case DvLight::eLightType::Directional:
					m_cpuLightBuffer.options = (1U << 0);
					m_cpuLightBuffer.direction = light->GetDir();
					break;
				case DvLight::eLightType::Point:
					m_cpuLightBuffer.options = (1U << 1);
					break;
				case DvLight::eLightType::Spot:
					m_cpuLightBuffer.options = (1U << 2);
					break;
				default:
					break;
				}

				m_cpuLightBuffer.color = light->GetColor();

				auto pLightBuffer = DvRenderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				if(i >= 1)
					GraphicsDevice::SetBlendState(DvRenderer::GetBlendState(eBlendState::Addictive));

				// draw opaque
				{
					for (auto& renderableBatch : m_RenderableBatches)
					{
						// urho�� Batch::Draw()
						{
							// urho�� Batch::Prepare()
							{
								m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(renderableBatch.worldTransform);

								auto pFrameBuffer = DvRenderer::GetConstantBuffer(eConstantBuffer::Frame);
								pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
								pFrameBuffer->Bind();	// ���̴� Ÿ�Ժ� ���ε� �� ���� ���������� �� ����� �� ���ϴ�...

								auto pShader = renderableBatch.pMaterial->GetShader();
								GraphicsDevice::SetShader(pShader);

								m_cpuMaterialBuffer.diffuseColor = renderableBatch.pMaterial->GetDiffuseColor();
								m_cpuMaterialBuffer.properties = 0;
								m_cpuMaterialBuffer.properties |= renderableBatch.pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;

								auto pMaterialBuffer = DvRenderer::GetConstantBuffer(eConstantBuffer::Material);
								pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
								pMaterialBuffer->Bind();

								GraphicsDevice::SetTexture(eTextureUnit::Diffuse, renderableBatch.pMaterial->GetTexture(eTextureUnit::Diffuse));
							}

							// urho�� Geometry::Draw()
							renderableBatch.pMesh->Draw();

						}
					}
				}
			}
		}
	}
	
	void ViewScreen::deferredRender()
	{
		// set & clear g buffer

		// prepare draw(bind state, viewport)

		// draw opaque(& bind constant buffer, shader, buffer) 

		// set & clear rendertarget

		// prepare draw

		// draw light
	}
}