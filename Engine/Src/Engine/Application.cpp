#include "DivePch.h"
#include "Application.h"
#include "Engine.h"
#include "Core/Log.h"


namespace Dive
{
	Application::Application()
	{
		// app setting or ȯ�� ���� �Ľ�
		m_engine = std::make_shared<Engine>();
	}

	// error code�� �����Ѵ�.
	int Application::Run()
	{
		// app setup ��
		Setup();
		// engine intialize: �����ڿ��� �Ľ��� ������ ���� �ʿ�
		if (!m_engine->Initialize())
			return 0;
		APP_TRACE("Setup Application & Initialize Engine!");

		// app start ��
		Start();
		// while engine run
		// �̷��� �����ϸ� imgui�� ����� �� ����...
		// => virtual�� ��������Ƿ� ������ ����������.
		// ������ �� ��� ������ �κ��� ���� �����;� �Ѵ�.
		// exiting�� true���� ������ Ż���ϴµ� �� ����� �𸣰ڴ�.
		while (!m_engine->IsExiting())
			m_engine->RunFrame();
		APP_TRACE("Start Application & Run Engine");

		Stop();
		APP_TRACE("Stop Application & Destroy Engine");

		return 1;
	}
}