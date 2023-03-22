#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Setup() {}
		virtual void Start() {}
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
	auto pApplication = std::make_unique<className>();	\
	return pApplication->Run();	\
}	\
DEFINE_MAIN(RunApplication())