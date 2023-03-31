#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
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

	{
		Dive::View* pView = new Dive::View;
		Dive::Renderer::SetView(0, pView);

		{
			auto* pModel = Dive::ResourceCache::LoadFromFile<Dive::Model>("Assets/Models/silly_dancing.fbx");
				//Dive::ResourceCache::LoadFromFile<Dive::Model>("Assets/Models/sponza.obj");
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
	if (Dive::Input::KeyDown(DIK_W))
		DV_CORE_INFO("W Å°¸¦ ´­·¶½À´Ï´Ù.");
	else if (Dive::Input::KeyDown(DIK_A))
		DV_CORE_INFO("A Å°¸¦ ´­·¶½À´Ï´Ù.");
	else if (Dive::Input::KeyDown(DIK_S))
		DV_CORE_INFO("S Å°¸¦ ´­·¶½À´Ï´Ù.");
	else if (Dive::Input::KeyDown(DIK_D))
		DV_CORE_INFO("D Å°¸¦ ´­·¶½À´Ï´Ù.");
}
