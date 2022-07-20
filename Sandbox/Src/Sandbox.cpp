#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox(Dive::DvContext* pContext)
	: Dive::Application(pContext)
{
	std::cout << "Sandbox 생성자" << std::endl;
}

Sandbox::~Sandbox()
{
	std::cout << "Sandbox 소멸자" << std::endl;
}

void Sandbox::Setup()
{
	std::cout << "Sandbox Setup" << std::endl;

	// 엔진 생성 전에 호출
	// ini 파일 등을 읽어오는 편이 낫다.
	// 그 전에 디폴트 데이터가 존재하는 것이 좋다.

	// 엔진 파라미터를 설정 할 수 있다.
	m_EngineParameters[Dive::EP_WINDOW_TITLE] = "Sandbox"s;
	// 로그 네임?
	m_EngineParameters[Dive::EP_FULL_SCREEN] = false;
	// headless?
	// sound?

	// resource paths?
}

void Sandbox::Start()
{
	std::cout << "Sandbox Start" << std::endl;

	// 엔진 초기화 후 호출
	// Scene을 구성한다.
}
