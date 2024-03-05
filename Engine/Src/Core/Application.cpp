#include "divepch.h"
#include "Application.h"
#include "CoreDefs.h"
#include "EventHandler.h"
#include "CoreEvents.h"
#include "Engine.h"

#include "Timer.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceManager.h"
#include "Input/Input.h"

namespace Dive
{
	Application::Application()
		: m_Title(L"Dive")
		, m_WindowWidth(800)
		, m_WindowHeight(600)
		, m_bFullscreen(false)
	{
	}

	Application::~Application()
	{
	}

	int Application::Run()
	{
		OnSetup();

		// 구조체가 가장 어울린다.
		// 파일을 로드하거나 OnSet()에서 설정 가능해야 하므로
		// 멤버 변수여야 한다.
		if (!Engine::Initialize(m_WindowWidth, m_WindowHeight, m_bFullscreen))
			return -1;

		OnStart();

		while (!Engine::IsExit())
			Engine::RunFrame();

		OnStop();

		Engine::Shutdown();

		return 0;
	}
}