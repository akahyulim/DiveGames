
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

		APP_TRACE("Engine 초기화 성공");

		for (auto i = 0; i != 22; i++)
		{	
			Sleep(1000);
			engine.Update();
			APP_TRACE("Engine Update 성공, {:f}초 경과", timer->GetDeltaTime());
		}

		APP_TRACE("Engine 종료, {:f}초 동안 실행", timer->GetRunningTime());
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