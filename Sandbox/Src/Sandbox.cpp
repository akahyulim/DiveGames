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
	// ���� ���� ���� ȣ��
	// ini ���� ���� �о���� ���� ����.

	// resource paths?
}

void Sandbox::Start()
{
	DV_LOG_CLIENT_DEBUG("Sandbox::Start ȣ��");

	// ���� �ʱ�ȭ �� ȣ��
	// Scene�� �����Ѵ�.
}

void Sandbox::Stop()
{
	DV_LOG_CLIENT_DEBUG("Sandbox::Stop ȣ��");
	
	// Application Cleanup
}
