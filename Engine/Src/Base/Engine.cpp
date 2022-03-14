#include "divepch.h"
#include "Engine.h"
#include "Log.h"

namespace Dive
{
	void Engine::Initialize(const WindowData* pData)
	{
		Log::Initialize();
		m_Time.Initialize();

		Renderer::Initialize(pData);
	}

	void Engine::Shutdown()
	{
		Renderer::Shutdown();
	}

	void Engine::Run()
	{
		m_Time.Tick();
	}

	void Engine::SetWindowData(const WindowData& data)
	{
		m_WindowData = data;

		// �ӽ� ó�� : �ϴ� Event�� ������� ����.
		if (data.msg == WM_SIZE && data.wParam != SIZE_MINIMIZED)
		{
			Renderer::SetResolution(data.Width, data.Height);
		}
	}
}