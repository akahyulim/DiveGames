#include "divepch.h"
#include "Engine.h"
#include "DiveCore.h"
#include "Time.h"
#include "Log.h"

namespace Dive
{
	Engine* Engine::s_pInstance = nullptr;

	Engine::Engine()
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;

		// sub system 생성은 이 곳이 맞다.
		Log::Initialize();

		// spartan은 생성과 초기화 역시 구분했다.
		// 다만 초기화에 필요한 데이터는 매개변수가 아닌
		// Engine을 직접 호출하여 얻었다.

		CORE_TRACE("Create Engine");
	}

	Engine::~Engine()
	{
		// sub system 종료
		CORE_TRACE("Destroy Engine");
	}

	void Engine::Tick()
	{
	}
}