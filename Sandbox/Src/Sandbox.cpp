#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox(Dive::DvContext* pContext)
		: Dive::Application(pContext)
	{
	}

	Sandbox::~Sandbox()
	{
	}

	// ���� ���� ���� ȣ��
	void Sandbox::Setup()
	{
		// ini ���� ���� �о���� ���� ����.
		m_EngineParameters.title = L"Sandbox";
		m_EngineParameters.width = 1600;
		m_EngineParameters.height = 900;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = false;

		// resource paths?
	}

	// ���� �ʱ�ȭ �� ȣ��
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Start ȣ��");

		// create scene content
		// scene�� ���� ������ �� cache�� �̿��Ͽ� object�� ����
		// camera�� ��� ������ ���� ����

		// create ui content

		// setup viewport
		// 1. viewport�� ������ �� renderer�� set
		// viewport�� scene, camera �׸��� �ʿ��ϴٸ� render path�� �����Ͽ� ����
		// 2. render path�� ���ٸ� cache�κ��� load�� �� viewport�� set
		auto pView = std::make_shared<Dive::DvView>(m_pContext);
		GetSubsystem<Dive::DvRenderer>()->SetView(0, pView);

		// subscirbe events
		// update handle�Լ����� camera move �Լ� ȣ��
		// post render update handle �Լ����� debug geometry ȣ��
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Stop ȣ��");

		// Application Cleanup
	}
}