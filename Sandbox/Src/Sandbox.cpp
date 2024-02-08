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
		// 여기는 서브 시스템 설정
		// 추후엔 파일 경로를 전달받아 서브 시스템을 설정
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