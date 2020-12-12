#include "Sandbox.h"

DIVE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Setup()
{
}

void Sandbox::Start()
{
	auto graphics = m_engine->GetSubsystem<Graphics>();
	// ������ ����
	graphics->GetWindow()->SetTitle(L"Sandbox");
	graphics->GetWindow()->Show(true);
	APP_TRACE("Sandbox Start~");
}

void Sandbox::Stop()
{
}
