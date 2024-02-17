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
		// 임시: create constant buffer 
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

		// 임시
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
			DV_CORE_ERROR("정의되지 않은 렌더패스는 수행할 수 없습니다.");
			return;
		}
	}

	// 일단 한 곳에서 쭉 구성해보고
	// 추후엔 draw call별로 함수화 시켜야할 듯 하다.
	// command list를 적용할 것이라면 더 그래야 한다.
	void ViewScreen::forwardRender()
	{
		// set & clear rendertarget
		{
			Graphics::SetRenderTargetView(0, m_pCamera->GetRenderTargetView());
			Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());
			Graphics::ClearViews(eClearFlags::Color | eClearFlags::Depth, m_pCamera->GetBackgroundColor(), 1.0f, 0);
		}

		// urho 구조상 이 부분도 Batch::Prepare()에 속한다.
		// 즉, Batch별로 호출되므로 중복 체크를 확인하는 편이 맞아보인다.
		{
			// 뷰포트
			// 일단 제외시켰지만 카메라가 특정 부분을 비출 수 있어야 한다.
			//Graphics::SetViewport(m_pCamera->GetViewport());

			{
				// 버퍼를 Renderer가 관리한다면 cpu버퍼는 이 곳이 아니라 GraphcisDevice가 관리하는 것이 맞다.
				// 다만 이 곳에서 Graphics에 전달하기 위해선
				// 1. 버퍼의 종류, 2. 적절한 데이터, 3. offset이 필요해 보인다.
				// urho의 경우 SetShaderParamter라는 함수가 이를 수행하는데
				// 버퍼 이름과 타입별 데이터를 전달하면
				// 내부에선 해당 이름의 버퍼를 가져와 데이터를 집어넣고 더티체크를 하는 듯 하다.
				// 이 경우 데이터별 offset이 중요해진다.
				m_cpuFrameBuffer.view = DirectX::XMMatrixTranspose(m_pCamera->GetViewMatrix());
				m_cpuFrameBuffer.projection = DirectX::XMMatrixTranspose(m_pCamera->GetProjectionMatrix());
			}

			{
				m_cpuCameraBuffer.position = m_pCamera->GetPosition();

				// 애매하지만 드로우콜마다 한 번만 해도 되는 작업이다.
				auto pCameraBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Camera);
				pCameraBuffer->Update((void*)&m_cpuCameraBuffer);
				pCameraBuffer->Bind();
			}

			// states
			// 생성은 Renderer에서 다 해놓은 상태
			// 결국 RenderPath에 맞춰 어떤 State를 어느 시점에 사용하느냐를 파악해야 한다.
			// 그리고 urho를 따라갈 필요 없이 일단 하드 코딩으로 사용 state를 전달하는 것이 맞아 보인다.
			{
				Graphics::SetRasterizerState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
				Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite));
				//Graphics::SetBlendState(Renderer::GetBlendState(eBlendState::Addictive));
			}
		}

		// bind lights
		// forward light는 light x mesh
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
						// urho의 Batch::Draw()
						{
							// urho의 Batch::Prepare()
							{
								m_cpuFrameBuffer.world = DirectX::XMMatrixTranspose(renderableBatch.worldTransform);

								auto pFrameBuffer = Renderer::GetConstantBuffer(eConstantBuffer::Frame);
								pFrameBuffer->Update((void*)&m_cpuFrameBuffer);
								pFrameBuffer->Bind();	// 셰이더 타입별 바인드 및 슬롯 관리때문에 이 방법이 더 편하다...

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

							// urho의 Geometry::Draw()
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