#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

namespace Dive
{
    Editor::Editor(HINSTANCE hInstance)
		: Runtime(hInstance, "DiveEditor")
    {
        // app system 생성

        // renderer까지 만들어야 imgui를 생성할 수 있다...
	}

    Editor::~Editor()
	{
        // app system 제거
 	}
}