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
		pView->SetRenderPath(Dive::eRenderPath::Deferred);
		//pView->SetRenderPath(Dive::eRenderPath::Forward);

		m_pMainCamera = Dive::Scene::CreateGameObject("MainCamera")->AddComponent<Dive::Camera>();
		m_pMainCamera->GetGameObject()->GetComponent<Dive::Transform>()->SetPosition(0.0f, 10.0f, -20.0f);
		pView->SetCamera(m_pMainCamera);

		Dive::Renderer::SetView(0, pView);

		// Objects
		{
			auto* pModel = //Dive::ResourceCache::LoadFromFile<Dive::Model>("../Assets/Models/dancing-stormtrooper/source/silly_dancing.fbx");
				//Dive::ResourceCache::LoadFromFile<Dive::Model>("../Assets/Models/sponza-master/sponza.obj");
				Dive::ResourceCache::GetResourceByPath<Dive::Model>("../Assets/Models/pilot-avatar/source/Pilot_LP_Animated.fbx");
		}

		// Lights
		{	
			auto pDirLight = Dive::Scene::CreateGameObject("Directional Light")->AddComponent<Dive::Light>();
			pDirLight->SetType(Dive::eLightType::Directional);
			pDirLight->SetColor(1.f, 1.0f, 1.0f);
			pDirLight->SetDir(1.0f, -1.0f, 1.0f);
			
			auto pPointLightRed = Dive::Scene::CreateGameObject("Point Light Red")->AddComponent<Dive::Light>();
			pPointLightRed->SetType(Dive::eLightType::Point);
			pPointLightRed->SetColor(1.0f, 0.0f, 0.0f);
			pPointLightRed->SetRange(50.0f);
			pPointLightRed->GetGameObject()->GetTransform()->SetPosition(0.0f, 10.0f, -10.0f);
			
			auto pPointLightGreen = Dive::Scene::CreateGameObject("Point Light Green")->AddComponent<Dive::Light>();
			pPointLightGreen->SetType(Dive::eLightType::Point);
			pPointLightGreen->SetColor(0.0f, 1.0f, 0.0f);
			pPointLightGreen->SetRange(50.0f);
			pPointLightGreen->GetGameObject()->GetTransform()->SetPosition(-10.0f, 10.0f, 0.0f);

			auto pPointLightBlue = Dive::Scene::CreateGameObject("Point Light Blue")->AddComponent<Dive::Light>();
			pPointLightBlue->SetType(Dive::eLightType::Point);
			pPointLightBlue->SetColor(0.0f, 0.0f, 1.0f);
			pPointLightBlue->SetRange(50.0f);
			pPointLightBlue->GetGameObject()->GetTransform()->SetPosition(10.0f, 10.0f, 0.0f);
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
		auto* pModel = Dive::ResourceCache::GetResourceByPath<Dive::Model>("../Assets/Models/pilot-avatar/source/Pilot_LP_Animated.fbx");
		pModel->GetRootGameObject()->GetTransform()->Rotate(0.0f, 1.0f, 0.0f);
	}
}
