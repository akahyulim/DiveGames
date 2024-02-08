#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox()
		: Dive::DvApplication(L"Sandbox", 1600, 900, false)
		, m_pScene(nullptr)
		, m_pMainCam(nullptr)
	{
	}

	Sandbox::~Sandbox()
	{
	}

	void Sandbox::OnSetup()
	{
		// ����� ���� �ý��� ����
		// ���Ŀ� ���� ��θ� ���޹޾� ���� �ý����� ����
	}

	void Sandbox::OnStart()
	{
		// create scene
		{
		}

		// setup renderLayer
		{
		}

		// subscribe events
		{

		}
	}
	
	void Sandbox::OnStop()
	{
		DV_DELETE(m_pScene);
	}
}