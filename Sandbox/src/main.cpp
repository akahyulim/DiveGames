#include "Sandbox.h"

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	auto app = std::make_unique<Dive::Sandbox>();
	return app->Run();
}
