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
		// ������. Ŀ�ǵ�������κ��� ���� �Ķ���͸� �Ľ� �� �� ������ �ʱ�ȭ���� ���� ���·� ����.
		explicit Application(DvContext* pContext);
		virtual ~Application();

		// ���� �ʱ�ȭ ������ ȣ��. �� ������ ���� �Ķ���͸� ���� ����.
		virtual void Setup() {}
		
		// ���� �ʱ�ȭ ��, ���� ���� ���� �� ����.
		virtual void Start() {}
		
		// ���� ���� �� Ŭ����, ���ø����̼��� ȣ��.
		virtual void Stop() {}

		// ���� �ʱ�ȭ �� ���� ���� ����.
		int Run();
		// ���� �޽����� ���. ���� ������ �����ϰ�, ���� ���� �ڵ带 ����.
		void ErrorExit(const std::string& message = std::string());

		void OnLogMessage(const Variant& data);

	protected:
		// Dive ����
		DvEngine* m_pEngine;
		// ���� �Ķ���� ��
		VariantMap m_EngineParameters;

		// ���ø����̼� ���� �ڵ�
		int m_ExitCode;
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
	auto pContext = new Dive::DvContext();	\
	auto pApplication = std::make_shared<className>(pContext);	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())
}