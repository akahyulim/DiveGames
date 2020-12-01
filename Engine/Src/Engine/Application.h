#pragma once
#include "Core/Main.h"

namespace Dive
{
	class Engine;

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		virtual void Setup() {}
		virtual void Start() {}
		virtual void Stop() {}

		virtual int Run();

	protected:
		std::shared_ptr<Engine> m_engine;
	};
}


// urho의 경우 context를 생성한 후 app을 생성했다. 
// 사실 모든 class가 object를 상속하고 생성자에서 context를 전달받는다.
// main function macro
#define DIVE_APPLICATION_MAIN(className)		\
int RunApplication()							\
{												\
	className app;								\
	return app.Run();							\
}												\
DIVE_DEFINE_MAIN(RunApplication())