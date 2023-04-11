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

		m_pMainCamera = Dive::Scene::CreateGameObject("MainCamera")->AddComponent<Dive::Camera>();
		m_pMainCamera->GetGameObject()->GetComponent<Dive::Transform>()->SetPosition(0.0f, 0.0f, -10.0f);
		pView->SetCamera(m_pMainCamera);

		Dive::Renderer::SetView(0, pView);

		{
			auto* pModel = //Dive::ResourceCache::LoadFromFile<Dive::Model>("Assets/Models/dancing-stormtrooper/source/silly_dancing.fbx");
				//Dive::ResourceCache::LoadFromFile<Dive::Model>("Assets/Models/sponza-master/sponza.obj");
				Dive::ResourceCache::GetResourceByPath<Dive::Model>("Assets/Models/pilot-avatar/source/Pilot_LP_Animated.fbx");

			//auto* pTexture2D = Dive::ResourceCache::GetResourceByPath<Dive::Texture2D>("Assets/Models/Stormtrooper_D.png");
			//Dive::Graphics::SetTexture(0, static_cast<Dive::Texture*>(pTexture2D));
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
}
