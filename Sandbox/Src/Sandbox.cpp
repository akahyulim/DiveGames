#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox(Dive::DvContext* pContext)
	: Dive::Application(pContext)
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Setup()
{
	// 엔진 생성 전에 호출
	// ini 파일 등을 읽어오는 편이 낫다.

	// resource paths?
}

void Sandbox::Start()
{
	DV_LOG_CLIENT_DEBUG("Sandbox::Start 호출");

	// 엔진 초기화 후 호출
	// Scene을 구성한다.
}

void Sandbox::Stop()
{
	DV_LOG_CLIENT_DEBUG("Sandbox::Stop 호출");
	
	// Application Cleanup
}
