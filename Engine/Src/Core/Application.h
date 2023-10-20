#pragma once
#include "Object.h"
#include "CoreDefs.h"

namespace Dive
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application();

		// 엔진 생성 및 초기화 이전에 호출
		virtual void Setup() {}
		// 엔진 초기화 이후, 실행 이전에 호출
		virtual void Start() {}
		// 엔진 루프를 탈출한 후 호출
		virtual void Stop() {}

		int Run();
	};
}

#define DEFINE_MAIN(function)	\
int main()	\
{	\
	return function;	\
}

#define DEFINE_APPLICATION_MAIN(className)	\
int RunApplication()	\
{	\
	Dive::Log::Initialize();	\
	auto pApplication = std::make_unique<className>();	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())