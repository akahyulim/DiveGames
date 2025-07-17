#include "Editor.h"

int main(int argc, char* argv[])
{
	//std::locale::global(std::locale(""));  // 로케일 설정
	//std::wcout.imbue(std::locale(""));     // 유니코드 출력 지원
	SetConsoleOutputCP(CP_UTF8);

	Dive::Editor editor;
	editor.Run();

	return 0;
}
