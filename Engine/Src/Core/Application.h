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

		// ���� ���� �� �ʱ�ȭ ������ ȣ��
		virtual void Setup() {}
		// ���� �ʱ�ȭ ����, ���� ������ ȣ��
		virtual void Start() {}
		// ���� ������ Ż���� �� ȣ��
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