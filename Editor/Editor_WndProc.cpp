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
            // 이 곳에서 ini를 갱신하는 게 더 맞는 것 같다.
        case WM_SIZE:
            // 일단은 ResizeBuffer를 호출하는게 맞겠지?
            // 그리고 나중엔 Renderer에서 이를 랩핑하고 RenderTarget을 재생성해야 할 거다.
            // 그런데 Editor의 경우 Backbuffer와 Renderer의 RenderTarget 크기가 다를 수 있다.
            // => 현재 Editor 객체를 가져올 방법이 없다.
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