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
		// �Ҹ���. ���� ��ü�� ����.
		virtual ~Application() override;

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
		// �α� �޽��� �� ���� �޽����� ����.
		void OnLogMessage(const Variant& data);

	protected:
		// ���� ��ü ������.
		std::shared_ptr<DvEngine> m_pEngine;
		// ���� �Ķ���� ��.
		VariantMap m_EngineParameters;

		// ���ø����̼� ���� �ڵ�.
		int m_ExitCode;
		// ������ ���� �α� �޽���.
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
	auto pContext = std::make_shared<Dive::DvContext>();	\
	auto pApplication = std::make_shared<className>(pContext.get());	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())
}