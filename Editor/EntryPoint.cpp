
#include <DiveEngine.h>

int main()
{
	AllocConsole();

	Dive::Log::Initialize();


	Dive::Engine engine;
	APP_TRACE("Engine ���� ����");

	if (engine.Initialize())
	{
		auto timer = engine.GetSystemManager()->GetSystem<Dive::Timer>();

		APP_TRACE("Engine �ʱ�ȭ ����");

		for (auto i = 0; i != 22; i++)
		{	
			Sleep(1000);
			engine.Update();
			APP_TRACE("Engine Update ����, {:f}�� ���", timer->GetDeltaTime());
		}

		APP_TRACE("Engine ����, {:f}�� ���� ����", timer->GetRunningTime());
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