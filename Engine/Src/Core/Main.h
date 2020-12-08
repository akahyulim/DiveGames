#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef _MSC_VER
#include <crtdbg.h>
#endif


#if defined (_DEBUG) && defined(_MSC_VER)
#define DIVE_DEFINE_MAIN(function)																\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)	\
{																								\
	AllocConsole();																				\
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);								\
	int result = function;																		\
	system("pause");																			\
	FreeConsole();																				\
	return result;																				\
}	
#else
#define DIVE_DEFINE_MAIN(function)																\
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)	\
{																								\
	return function;																			\
}
#endif
