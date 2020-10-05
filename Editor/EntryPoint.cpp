
#include <DiveEngine.h>

int main()
{
	AllocConsole();

	Dive::Log::Initialize();


	Dive::Engine engine;
	APP_TRACE("Engine 생성 성공");

	if (engine.Initialize())
	{
		APP_TRACE("Engine 초기화 성공");
		engine.Update();
		APP_TRACE("Engine Update 성공");
	}

	APP_TRACE("Engine 종료");

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