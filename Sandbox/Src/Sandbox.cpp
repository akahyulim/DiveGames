#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
	: m_pMainCamera(nullptr)
{}

Sandbox::~Sandbox()
{
	DV_INFO("Sandbox ¼Ò¸êÀÚ");
}

void Sandbox::Setup()
{
	Dive::Graphics::SetWindowTitle(L"Dive Sandbox");
}

void Sandbox::Start()
{
	DV_INFO("Sandbox::Start()");

	Dive::Layer* pLayer = new Dive::Layer;
	pLayer->SetName("Default");
	//pLayer->SetRenderPath(Dive::eRenderPath::Forward);
	pLayer->SetRenderPath(Dive::eRenderPath::Deferred);

	m_pMainCamera = Dive::Scene::CreateGameObject("MainCamera")->AddComponent<Dive::Camera>();
	m_pMainCamera->GetGameObject()->GetComponent<Dive::Transform>()->SetPosition(0.0f, 10.0f, -20.0f);
	pLayer->SetCamera(m_pMainCamera);

	Dive::Renderer::SetLayer(0, pLayer);

	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
}

void Sandbox::Stop()
{
	DV_INFO("Sandbox::Stop()");
}

void Sandbox::OnUpdate(const Dive::Event& evnt)
{
}
