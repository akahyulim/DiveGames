#include "Editor.h"
#include "External/ImGui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Editor
{
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
            // �� ������ ini�� �����ϴ� �� �� �´� �� ����.
        case WM_SIZE:
            // �ϴ��� ResizeBuffer�� ȣ���ϴ°� �°���?
            // �׸��� ���߿� Renderer���� �̸� �����ϰ� RenderTarget�� ������ؾ� �� �Ŵ�.
            // �׷��� Editor�� ��� Backbuffer�� Renderer�� RenderTarget ũ�Ⱑ �ٸ� �� �ִ�.
            // => ���� Editor ��ü�� ������ ����� ����.
        /*
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            */
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}