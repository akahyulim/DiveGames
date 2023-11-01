#pragma once
#include "Object.h"
#include "CoreDefs.h"

namespace Dive
{
	class Application
	{
	public:
		// 생성자는 매개인자를 받도록 하고 디폴트 값을 넣어놓는 편이 나을 것 같다.
		// 매개인자는 일단 이름과 윈도우 프로퍼티(크기, 전체 화면 유무, 싱크 유무) 정도면 된다.
		// 스파키의 경우 윈도우 프로퍼티를 윈도우와 D3DContext 객체에 전부 전달 및 저장하여 사용한다.
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