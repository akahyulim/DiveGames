#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
	: m_pMainCamera(nullptr)
{}

Sandbox::~Sandbox()
{
	DV_TRACE("Sandbox ¼Ò¸êÀÚ");
}

void Sandbox::Setup()
{
	Dive::Graphics::SetWindowTitle(L"Dive Sandbox");
}

void Sandbox::Start()
{
	DV_TRACE("Sandbox::Start()");

	Dive::View* pView = new Dive::View;
	pView->SetName("MainView");
	//pView->SetRenderPath(Dive::eRenderPath::Forward);
	pView->SetRenderPath(Dive::eRenderPath::Deferred);

	m_pMainCamera = Dive::Scene::CreateGameObject("MainCamera")->AddComponent<Dive::Camera>();
	m_pMainCamera->GetGameObject()->GetComponent<Dive::Transform>()->SetPosition(0.0f, 10.0f, -20.0f);
	pView->SetCamera(m_pMainCamera);

	Dive::Renderer::SetView(0, pView);

	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
}

void Sandbox::Stop()
{
	DV_TRACE("Sandbox::Stop()");
}

void Sandbox::OnUpdate(const Dive::Event& evnt)
{
	const auto& e = dynamic_cast<const Dive::UpdateEvent&>(evnt);
}
