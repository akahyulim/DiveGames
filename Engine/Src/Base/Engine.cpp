#include "divepch.h"
#include "Engine.h"
#include "Log.h"

namespace Dive
{
	void Engine::Initialize()
	{
		Log::Initialize();
		m_Time.Initialize();

		// renderer는 일단 생성 후 따로 윈도우 데이터를 받아 초기화하는걸로...?
	}

	void Engine::Shutdown()
	{
	}
}