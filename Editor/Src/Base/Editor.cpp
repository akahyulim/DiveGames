#include "Editor.h"
#include "DiveEngine.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Dive
{
    Editor::Editor()
    {
        // load ini 
    }

    Editor::~Editor()
    {
        DV_DELETE(m_pEngine);
    }

    void Editor::Tick()
    {
    }

    void Editor::OnWindowMessage(WindowData& data)
    {
        if (!m_bInitialized)
        {
            m_pEngine = new Engine;
            DV_ASSERT(m_pEngine);

            // sub system 가져오기?

            // imgui 초기화

            m_bInitialized = true;
        }
        else
        {
            ImGui_ImplWin32_WndProcHandler(data.hWnd, data.msg, data.wParam, data.lParam);

            // engine의 backbuffer resize

            m_pEngine->SetWindowData(data);
        }
    }
}