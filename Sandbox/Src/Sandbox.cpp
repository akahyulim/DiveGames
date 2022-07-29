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

	void Sandbox::Setup()
	{
		// ���� ���� ���� ȣ��
		// ini ���� ���� �о���� ���� ����.
		m_EngineParameters.title = L"Sandbox";
		m_EngineParameters.width = 1600;
		m_EngineParameters.height = 900;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = false;

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
}