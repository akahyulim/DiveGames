
#include <DiveEngine.h>

int main()
{
	AllocConsole();

	Dive::Log::Initialize();


	Dive::Engine engine;
	APP_TRACE("Engine ���� ����");

	if (engine.Initialize())
	{
		APP_TRACE("Engine �ʱ�ȭ ����");
		engine.Update();
		APP_TRACE("Engine Update ����");
	}

	APP_TRACE("Engine ����");

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