#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Setup()
{
	// Engine Setup
	Dive::Graphics::SetWindowTitle(L"ModelViewer");

	// Subscribe Events
	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
	SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(OnPostUpdate));
	SUBSCRIBE_EVENT(Dive::eEventType::WindowEvent, EVENT_HANDLER_PARAM(OnWindowEvent));
}

void Sandbox::Start()
{
	// �ϴ� ���� ���� ��ü���� �Ϲ� Ŭ���� ��ü�� �ٲ�� �Ѵ�.
	// �� �������� ���� ������ ���Ÿ� ��� ó���� ���ΰ��� �����ؾ� �Ѵ�.
	// => ���� ������ ������ ��ü ���ø����̼ǿ��� �Ѵ�.
	auto m_pCamera = Dive::Scene::CreateGameObject("MainCamera");
	Dive::Camera* pCamera = m_pCamera->AddComponent<Dive::Camera>();
	pCamera->SetViewSize(static_cast<float>(Dive::Graphics::GetWindowWidth()), static_cast<float>(Dive::Graphics::GetWindowHeight()));
	pCamera->SetBackgroundColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pCamera->GetComponent<Dive::Transform>()->SetPosition(0.0f, 0.0f, -10.0f);
}

void Sandbox::Stop()
{
}

void Sandbox::OnUpdate(const Dive::Event& e)
{
}

void Sandbox::OnPostUpdate(const Dive::Event& e)
{
}

void Sandbox::OnWindowEvent(const Dive::Event& e)
{
}
