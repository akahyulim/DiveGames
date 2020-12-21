#include "Sandbox.h"

DIVE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
	SAFE_DELETE(m_scene);
}

void Sandbox::Setup()
{
	m_scene = new Scene(const_cast<Context*>(m_engine->GetContext()), "Test Scene");
	m_scene->CreateGameObject()->SetName("Knave");
}

void Sandbox::Start()
{
	auto graphics = m_engine->GetSubsystem<Graphics>();
	// 아이콘 변경
	graphics->GetWindow()->SetTitle(L"Sandbox");
	graphics->GetWindow()->Show(true);
	APP_TRACE("Sandbox Start~");

	Shader shader(const_cast<Context*>(m_engine->GetContext()));
	shader.CreateShader(eVertexType::PositionColor, L"../Assets/Shaders/old/Color.hlsl");

}

void Sandbox::Stop()
{
}
