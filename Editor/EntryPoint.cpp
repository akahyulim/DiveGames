
#include <DiveEngine.h>

int main()
{
	AllocConsole();

	Dive::Log::Initialize();


	Dive::Engine engine;
	APP_TRACE("Engine 생성 성공");

	if (engine.Initialize())
	{
		auto timer = engine.GetSystemManager()->GetSystem<Dive::Timer>();
		timer->SetTargetFps(60);

		APP_TRACE("Engine 초기화 성공");

		for (auto i = 0; i != 60; i++)
		{	
			//Sleep(10);
			engine.Update();
			APP_TRACE("Engine Update 성공, {0:f} : {1:f}", timer->GetDeltaTime(), timer->GetSmoothDeltaTime());
		}

		APP_TRACE("Engine 종료, {0:f}", timer->GetRunningTime());
	}

	system("pause");
	FreeConsole();

	return 0;
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	return 0;
}
*/