#include "Editor.h"

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	try {
		Dive::Editor editor;
		editor.Run();
	} catch (const std::exception& e) {
		DV_LOG(Main, critical, "Editor 초기화 실패: {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
