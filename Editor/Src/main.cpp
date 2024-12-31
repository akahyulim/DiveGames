#include "Editor.h"
#include "dive.h"

int main(int argc, char* argv[])
{
	auto pEditor = std::make_unique<Dive::Editor>();
	return pEditor->Run();
}