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

	// 일단 삼각형 GameObject를 구성토록 하자.
	// 이후 Graphics 혹은 Renderer가 Shader를 생성하고
	// pipeline에 GameObject의 리소스들을 연결한 후 렌더링해야 한다.
}

void Sandbox::Start()
{
	auto graphics = m_engine->GetSubsystem<Graphics>();
	// 아이콘 변경
	graphics->GetWindow()->SetTitle(L"Sandbox");
	graphics->GetWindow()->Show(true);
	APP_TRACE("Sandbox Start~");
}

void Sandbox::Stop()
{
}
