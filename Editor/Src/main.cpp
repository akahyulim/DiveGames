#include "Editor.h"
#include "dive.h"

int main(int argc, char* argv[])
{
	auto DiveEditor = std::make_unique<Dive::Editor>();
	return DiveEditor->Run();
}