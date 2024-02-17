#include "divepch.h"
#include "ViewScreen.h"
#include "Material.h"
#include "Mesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/ConstantBuffer.h"
#include "Core/CoreDefs.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Renderable.h"
#include "Scene/Components/Light.h"


namespace Dive
{
	ViewScreen::ViewScreen(Scene* pScene, Camera* pCamera, eRenderPath renderPath)
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

				auto pLightCom = pGameObject->GetComponent<Light>();
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
			Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
			Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());
			Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);
		}

		// urho ������ �� �κе� Batch::Prepare()�� ���Ѵ�.
		// ��, Batch���� ȣ��ǹǷ� �ߺ� üũ�� Ȯ���ϴ� ���� �¾ƺ��δ�.
		{
			// ����Ʈ
			// �ϴ� ���ܽ������� ī�޶� Ư�� �κ��� ���� �� �־�� �Ѵ�.
			//Graphics::SetViewport(m_pCamera->GetViewport());

			{
				// ���۸� Renderer�� �����Ѵٸ� cpu���۴� �� ���� �ƴ϶� GraphcisDevice�� �����ϴ� ���� �´�.
				// �ٸ� �� ������ Graphics�� �����ϱ� ���ؼ�
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
				auto pCameraBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Camera);
				pCameraBuffer->Update((void*)&m_cpuCameraBuffer);
				pCameraBuffer->Bind();
			}

			// states
			// ������ Renderer���� �� �س��� ����
			// �ᱹ RenderPath�� ���� � State�� ��� ������ ����ϴ��ĸ� �ľ��ؾ� �Ѵ�.
			// �׸��� urho�� ���� �ʿ� ���� �ϴ� �ϵ� �ڵ����� ��� state�� �����ϴ� ���� �¾� ���δ�.
			{
				Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
				Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite));
				//Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Addictive));
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

				case Light::eLightType::Directional:
					m_cpuLightBuffer.options = (1U << 0);
					m_cpuLightBuffer.direction = light->GetDir();
					break;
				case Light::eLightType::Point:
					m_cpuLightBuffer.options = (1U << 1);
					break;
				case Light::eLightType::Spot:
					m_cpuLightBuffer.options = (1U << 2);
					break;
				default:
					break;
				}

				m_cpuLightBuffer.color = light->GetColor();

				auto pLightBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Light);
				pLightBuffer->Update((void*)&m_cpuLightBuffer);
				pLightBuffer->Bind();

				if(i >= 1)
					Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Addictive));

				// draw opaque
				{
					for (auto& renderableBatch : m_RenderableBatches)
					{
						// urho�� Batch::Draw()
						{
							// urho�� Batch::Prepare()
							{
								m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(renderableBatch.worldTransform);

								auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
								pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
								pFrameBuffer->Bind();	// ���̴� Ÿ�Ժ� ���ε� �� ���� ���������� �� ����� �� ���ϴ�...

								auto pShader = renderableBatch.pMaterial->GetShader();
								Graphics::SetShader(pShader);

								m_cpuMaterialBuffer.diffuseColor = renderableBatch.pMaterial->GetDiffuseColor();
								m_cpuMaterialBuffer.properties = 0;
								m_cpuMaterialBuffer.properties |= renderableBatch.pMaterial->HasTexture(eTextureUnit::Diffuse) ? (1U << 0) : 0;

								auto pMaterialBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Material);
								pMaterialBuffer->Update((void*)&m_cpuMaterialBuffer);
								pMaterialBuffer->Bind();

								Graphics::SetTexture(eTextureUnit::Diffuse, renderableBatch.pMaterial->GetTexture(eTextureUnit::Diffuse));
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