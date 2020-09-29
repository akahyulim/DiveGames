
#include <DiveEngine.h>

int main()
{
	Dive::Log::Initialize();

	CORE_TRACE("CORE TRACE");
	CORE_INFO("CORE INFO");
	CORE_ERROR("CORE ERROR");
	CORE_WARN("CORE WARN");

	APP_TRACE("START");
	APP_INFO("START - INFO");
	APP_ERROR("START - ERROR");
	APP_WARN("START - WARN");
//	APP_FATAL("START - FATAL");
	
	APP_INFO("RUN");
	APP_INFO("RUN - INFO");
	APP_ERROR("RUN - ERROR");
	APP_WARN("RUN - WARN");
	
	
	APP_INFO("END");
	APP_INFO("END - INFO");
	APP_ERROR("END - ERROR");
	APP_WARN("END - WARN");
	

	return 0;
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	return 0;
}
*/