#include "divepch.h"
#include "Application.h"
#include "CoreDefs.h"
#include "EventHandler.h"
#include "CoreEvents.h"
#include "Engine.h"

#include "Timer.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceCache.h"
#include "Input/Input.h"

namespace Dive
{
	Application::Application()
		: m_Title(L"Dive")
		, m_WindowWidth(800)
		, m_WindowHeight(600)
		, m_bFullscreen(false)
	{
		m_pEngine = std::make_unique<Engine>();
	}

	Application::~Application()
	{
	}

	int Application::Run()
	{
		OnSetup();

		// ����ü�� ���� ��︰��.
		// ������ �ε��ϰų� OnSet()���� ���� �����ؾ� �ϹǷ�
		// ��� �������� �Ѵ�.
		if (!m_pEngine->Initialize(m_WindowWidth, m_WindowHeight, m_bFullscreen))
			return -1;

		OnStart();

		while (!m_pEngine->IsExit())
			m_pEngine->RunFrame();

		OnStop();

		return 0;
	}
}