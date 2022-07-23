#pragma once
#include "Core/DvObject.h"
#include "Core/DvContext.h"
#include "Core/Variant.h"

namespace Dive
{
	class DvEngine;

	class Application : public DvObject
	{
		DIVE_OBJECT(Application, DvObject)

	public:
		// 생성자. 커맨드라인으로부터 엔진 파라미터를 파싱 한 후 엔진을 초기화되지 않은 상태로 생성.
		explicit Application(DvContext* pContext);
		// 소멸자. 엔진 객체를 제거.
		virtual ~Application() override;

		// 엔진 초기화 이전에 호출. 이 곳에서 엔진 파라미터를 수정 가능.
		virtual void Setup() {}
		
		// 엔진 초기화 후, 메인 루프 실행 전 설정.
		virtual void Start() {}
		
		// 메인 루프 후 클린업, 어플리케이션이 호출.
		virtual void Stop() {}

		// 엔진 초기화 후 메인 루프 실행.
		int Run();
		// 에러 메시지를 출력. 메인 루프를 종료하고, 실패 종료 코드를 설정.
		void ErrorExit(const std::string& message = std::string());
		// 로그 메시지 중 에러 메시지를 저장.
		void OnLogMessage(const Variant& data);

	protected:
		// 엔진 객체 포인터.
		std::shared_ptr<DvEngine> m_pEngine;
		// 엔진 파라미터 맵.
		VariantMap m_EngineParameters;

		// 어플리케이션 종료 코드.
		int m_ExitCode;
		// 마지막 에러 로그 메시지.
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
	auto pContext = std::make_shared<Dive::DvContext>();	\
	auto pApplication = std::make_shared<className>(pContext.get());	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())
}