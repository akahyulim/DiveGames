#pragma once
#include "Engine.h"

namespace Dive
{
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		virtual void Setup() {}		// 엔진 생성 및 초기화 전
		virtual void Start() {}		// 엔진 초기화 후, 런타임 수행 전
		virtual void Stop() {}		// 엔진 런타임 종료 후

		int Run();

	protected:
		std::unique_ptr<Engine> m_engine;
	};
}