
namespace Dive
{
	__declspec(dllimport) void Print();
}

//#pragma comment(lib, "Engine.lib")
//#include "../Engine/Test.h"

void main()
{
	Dive::Print();
}