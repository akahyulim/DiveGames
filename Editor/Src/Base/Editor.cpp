#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

namespace Dive
{
    Editor::Editor(HINSTANCE hInstance)
		: Runtime(hInstance, "DiveEditor")
    {
        // app system ����

        // renderer���� ������ imgui�� ������ �� �ִ�...
	}

    Editor::~Editor()
	{
        // app system ����
 	}
}