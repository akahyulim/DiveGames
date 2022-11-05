#pragma once
#include "EngineDef.h"
#include "Core/Object.h"
#include "Core/Context.h"
#include "Core/Variant.h"

namespace Dive
{
	class Engine;
	class Event;

	class Application : public Object
	{
		DIVE_OBJECT(Application, Object)

	public:
		explicit Application(Context* pContext);
		virtual ~Application() = default;

		// 엔진 초기화 이전에 호출됩니다.
		virtual void Setup() {}
		
		// 엔진 초기화 후, 메인 루프 실행 전 호출됩니다.
		virtual void Start() {}
		
		// 메인 루프 종류 후 호출됩니다.
		virtual void Stop() {}

		int Run();

		void ErrorExit(const std::string& message = std::string());
		
		void OnLogMessage(const Event& e);

	protected:
		std::unique_ptr<Engine> m_pEngine;
		// 엔진 파라미터.
		EngineParameters m_EngineParameters;

		int m_ExitCode;
		std::string m_ErrorMessage;
	};

/*
#define DEFINE_MAIN(function) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) \
{ \
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \				// 메모리 누수 탐지?
	return function; \
}
*/

#define DEFINE_MAIN(function)	\
int main()	\
{	\
	return function;	\
}

#define DEFINE_APPLICATION_MAIN(className)	\
int RunApplication()	\
{	\
	auto pContext = std::make_unique<Dive::Context>();	\
	auto pApplication = std::make_unique<className>(pContext.get());	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())
}