#include "Editor.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

namespace Dive
{
    Editor::~Editor()
	{
        // app system ����
 	}

//	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
//		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//			return true;

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Runtime* CreateApplication(HINSTANCE hInstance)
	{
		auto pEditor = new Editor;
		DV_ASSERT(pEditor);

		// ���� �� �ʱ�ȭ
		// load ini

		auto AppWindow = pEditor->GetAppWindow();
		AppWindow->SetInstance(hInstance);
		AppWindow->SetTitle("DiveEditor");
		AppWindow->SetWidth(1280);
		AppWindow->SetHeight(720);
		// wndproc�� ��ü�ؾ� �Ѵ�.

		// intialize editor
		pEditor->Initialize();

		return pEditor;
	}
}