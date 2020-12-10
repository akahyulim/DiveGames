#include "DivePch.h"
#include "Application.h"
#include "Engine.h"
#include "Core/Log.h"


namespace Dive
{
	Application::Application()
	{
		// app setting or 환경 변수 파싱
		m_engine = std::make_shared<Engine>();
	}

	// error code를 리턴한다.
	int Application::Run()
	{
		APP_TRACE("Setup Application & Initialize Engine!");
	
		// app setup 후
		Setup();
		// engine intialize: 생성자에서 파싱한 데이터 전달 필요
		if (!m_engine->Initialize())
			return 0;

		APP_TRACE("Start Application & Run Engine");
		// app start 후
		Start();
		// while engine run
		// 이렇게 구현하면 imgui를 사용할 수 없다...
		// => virtual로 만들었으므로 변경이 가능해졌다.
		// 하지만 이 경우 렌더링 부분을 직접 가져와야 한다.
		while (!m_engine->IsExiting())
			m_engine->RunFrame();

		Stop();
		APP_TRACE("Stop Application & Destroy Engine");

		return 1;
	}
}