#include "divepch.h"
#include "Engine.h"
#include "Log.h"

namespace Dive
{
	void Engine::Initialize()
	{
		Log::Initialize();
		m_Time.Initialize();

		// renderer�� �ϴ� ���� �� ���� ������ �����͸� �޾� �ʱ�ȭ�ϴ°ɷ�...?
	}

	void Engine::Shutdown()
	{
	}
}