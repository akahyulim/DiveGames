#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox(Dive::DvContext* pContext)
	: Dive::Application(pContext)
{
	std::cout << "Sandbox ������" << std::endl;
}

Sandbox::~Sandbox()
{
	std::cout << "Sandbox �Ҹ���" << std::endl;
}

void Sandbox::Setup()
{
	std::cout << "Sandbox Setup" << std::endl;

	// ���� ���� ���� ȣ��
	// ini ���� ���� �о���� ���� ����.
	// �� ���� ����Ʈ �����Ͱ� �����ϴ� ���� ����.

	// ���� �Ķ���͸� ���� �� �� �ִ�.
	m_EngineParameters[Dive::EP_WINDOW_TITLE] = "Sandbox"s;
	// �α� ����?
	m_EngineParameters[Dive::EP_FULL_SCREEN] = false;
	// headless?
	// sound?

	// resource paths?
}

void Sandbox::Start()
{
	std::cout << "Sandbox Start" << std::endl;

	// ���� �ʱ�ȭ �� ȣ��
	// Scene�� �����Ѵ�.
}
