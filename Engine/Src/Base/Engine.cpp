#include "divepch.h"
#include "Engine.h"
#include "Log.h"

namespace Dive
{
	void Engine::Initialize(const WindowData* pData)
	{
		Log::Initialize();
		m_Time.Initialize();

		m_Renderer.Initialize(pData);

	}

	void Engine::Shutdown()
	{
		m_Renderer.Shutdow();
	}

	void Engine::Update()
	{
		m_Time.Update();
	}

	void Engine::Render()
	{
		m_Renderer.Tick();
	}
}