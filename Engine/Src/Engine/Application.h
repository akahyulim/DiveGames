#pragma once
#include "EngineDef.h"
#include "Core/Object.h"

namespace Dive
{
	class Engine;
	class Event;

	// ���� ������ �����ϴ� ���ø����̼� ���̽� Ŭ����.
	class Application : public Object
	{
		DIVE_OBJECT(Application, Object)

	public:
		explicit Application(Context* pContext);
		virtual ~Application() = default;

		// ���� �ʱ�ȭ ������ ȣ��.
		virtual void Setup() {}
		// ���� �ʱ�ȭ ��, ���� ���� ���� �� ȣ��.
		virtual void Start() {}
		// ���� ���� ���� �� ȣ��.
		virtual void Stop() {}

		int Run();

		void ErrorExit(const std::string& message = std::string());
		
		void OnLogMessage(const Event& e);

	protected:
		std::unique_ptr<Engine> m_pEngine;
		EngineParameters m_EngineParams;
		int m_ExitCode;
		std::string m_ErrorMessage;
	};

/*
#define DEFINE_MAIN(function) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) \
{ \
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \				// �޸� ���� Ž��?
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