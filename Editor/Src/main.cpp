#include "Editor.h"

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	auto app = std::make_unique<Dive::Editor>();
	return app->Run();
}
