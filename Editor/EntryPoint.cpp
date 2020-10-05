
#include <DiveEngine.h>

int main()
{
	AllocConsole();

	Dive::Log::Initialize();

	CORE_TRACE("INITIALIZE");
	CORE_INFO("INITIALIZE");
	CORE_ERROR("INITIALIZE");
	CORE_WARN("INITIALIZE");

	APP_TRACE("START");
	APP_INFO("START");
	APP_ERROR("START");
	APP_WARN("START");
	
	APP_INFO("RUN");
	APP_INFO("RUN");
	APP_ERROR("RUN");
	APP_WARN("RUN");
	
	
	APP_INFO("END");
	APP_INFO("END");
	APP_ERROR("END");
	APP_WARN("END");
	
	CORE_TRACE("TERMINATE");
	CORE_INFO("TERMINATE");
	CORE_ERROR("TERMINATE");
	CORE_WARN("TERMINATE");

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