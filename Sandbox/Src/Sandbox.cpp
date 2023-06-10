#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
	: m_pMainCamera(nullptr)
{}

Sandbox::~Sandbox()
{
	DV_TRACE("Sandbox 소멸자");
}

void Sandbox::Setup()
{
	Dive::Graphics::SetWindowTitle(L"Dive Sandbox");
}

void Sandbox::Start()
{
	DV_TRACE("Sandbox::Start()");

	{
		Dive::View* pView = new Dive::View;
		pView->SetName("MainView");
		//pView->SetRenderPath(Dive::eRenderPath::Forward);
		pView->SetRenderPath(Dive::eRenderPath::Deferred);

		m_pMainCamera = Dive::Scene::CreateGameObject("MainCamera")->AddComponent<Dive::Camera>();
		m_pMainCamera->GetGameObject()->GetComponent<Dive::Transform>()->SetPosition(0.0f, 10.0f, -20.0f);
		pView->SetCamera(m_pMainCamera);

		Dive::Renderer::SetView(0, pView);

		// Objects
		{
			{
				m_pPilot = Dive::ResourceCache::GetResourceByPath<Dive::Model>("Assets/Models/pilot/Pilot_LP_Animated.fbx");
				auto* pTransform = m_pPilot->GetRootGameObject()->GetTransform();
				pTransform->SetPosition(5.0f, 0.0f, 0.0f);
				pTransform->SetScale(0.01f, 0.01f, 0.01f);

				for (auto* pChild : pTransform->GetChildren())
				{
					auto* pDrawable = pChild->GetGameObject()->GetComponent<Dive::Drawable>();
					if (pDrawable)
					{
						pDrawable->SetMaterial("Assets/Materials/Pilot/pilot.yaml");
						break;
					}
				}
			}

			{
				m_pStormTrooper = Dive::ResourceCache::LoadFromFile<Dive::Model>("Assets/Models/stormtrooper/silly_dancing.fbx");
				auto* pTransform = m_pStormTrooper->GetRootGameObject()->GetTransform();
				pTransform->SetPosition(-5.0f, 0.0f, 0.0f);
				pTransform->SetScale(5.0f, 5.0f, 5.0f);

				for (auto* pChild : pTransform->GetChildren())
				{
					auto* pDrawable = pChild->GetGameObject()->GetComponent<Dive::Drawable>();
					if (pDrawable)
					{
						pDrawable->SetMaterial("Assets/Materials/StormTrooper/StormTrooper.yaml");
						break;
					}
				}
			}
		}

		// 현재 광원 적용이 이상하다.
		// DeferredPoint를 안만든 듯 하다.
		// Lights
		{	
			auto pDirLight = Dive::Scene::CreateGameObject("Directional Light")->AddComponent<Dive::Light>();
			pDirLight->SetType(Dive::eLightType::Directional);
			pDirLight->SetColor(1.f, 1.0f, 1.0f);
			pDirLight->SetDir(1.0f, -1.0f, 1.0f);
			
			/*
			auto pPointLightRed = Dive::Scene::CreateGameObject("Point Light Red")->AddComponent<Dive::Light>();
			pPointLightRed->SetType(Dive::eLightType::Point);
			pPointLightRed->SetColor(1.0f, 0.0f, 0.0f);
			pPointLightRed->SetRange(10.0f);
			pPointLightRed->GetGameObject()->GetTransform()->SetPosition(0.0f, 10.0f, 0.0f);
			
			auto pPointLightGreen = Dive::Scene::CreateGameObject("Point Light Green")->AddComponent<Dive::Light>();
			pPointLightGreen->SetType(Dive::eLightType::Point);
			pPointLightGreen->SetColor(0.0f, 1.0f, 0.0f);
			pPointLightGreen->SetRange(10.0f);
			pPointLightGreen->GetGameObject()->GetTransform()->SetPosition(-5.0f, 5.0f, 0.0f);

			auto pPointLightBlue = Dive::Scene::CreateGameObject("Point Light Blue")->AddComponent<Dive::Light>();
			pPointLightBlue->SetType(Dive::eLightType::Point);
			pPointLightBlue->SetColor(0.0f, 0.0f, 1.0f);
			pPointLightBlue->SetRange(10.0f);
			pPointLightBlue->GetGameObject()->GetTransform()->SetPosition(5.0f, 0.0f, 0.0f);
			*/
		}
	}

	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
}

void Sandbox::Stop()
{
	DV_TRACE("Sandbox::Stop()");
}

void Sandbox::OnUpdate(const Dive::Event& evnt)
{
	auto pTransform = m_pMainCamera->GetGameObject()->GetTransform();

	// xmfloat3와 xmvector 전부 speed와 delta를 곱할 수가 없다.
	if (Dive::Input::KeyPress(DIK_W))
	{
		pTransform->Translate(0.0f, 0.0f, 1.0f);
	}
	if (Dive::Input::KeyPress(DIK_S))
	{
		pTransform->Translate(0.0f, 0.0f, -1.0f);
	}
	if (Dive::Input::KeyPress(DIK_A))
	{
		pTransform->Translate(-1.0f, 0.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_D))
	{
		pTransform->Translate(1.0f, 0.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_C))
	{
		pTransform->Translate(0.0f, -1.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_SPACE))
	{
		pTransform->Translate(0.0f, 1.0f, 0.0f);
	}

	if (Dive::Input::KeyPress(DIK_Q))
	{
		pTransform->Rotate(0.0f, 0.0f, -1.0f);
	}
	if (Dive::Input::KeyPress(DIK_E))
	{
		pTransform->Rotate(0.0f, 0.0f, 1.0f);
	}
	if (Dive::Input::KeyPress(DIK_LEFT))
	{
		pTransform->Rotate(0.0f, -1.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_RIGHT))
	{
		pTransform->Rotate(0.0f, 1.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_UP))
	{
		pTransform->Rotate(-1.0f, 0.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_DOWN))
	{
		pTransform->Rotate(1.0f, 0.0f, 0.0f);
	}

	{
		m_pStormTrooper->GetRootGameObject()->GetTransform()->Rotate(0.0f, 1.0f, 0.0f);
		m_pPilot->GetRootGameObject()->GetTransform()->Rotate(0.0f, 1.0f, 0.0f);
	}
}
