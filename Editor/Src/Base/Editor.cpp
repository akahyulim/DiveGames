#include "Editor.h"
#include "AppWindow.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"

namespace Dive
{
    Editor::Editor(HINSTANCE hInstance)
    {
        // load ini

        m_pAppWnd = new AppWindow(hInstance, m_WindowProps);
        DV_ASSERT(m_pAppWnd);

        m_pAppWnd->SetEventCallback(DV_BIND_EVENT_FN(Engine::OnEvent));
    }

    Editor::~Editor()
    {
    }

    void Editor::Initialize()
    {
        Engine::Initialize();

        initializeImGui();

        m_pAppWnd->Show();
    }

    void Editor::Destroy()
    {
        destroyImGui();

        Engine::Destroy();

        m_pAppWnd->Destroy();
        DV_DELETE(m_pAppWnd);
    }

    void Editor::Tick()
    {
        if (!m_pAppWnd)
            return;

        while (m_pAppWnd->Run())
        {
            Engine::Tick();
        }
    }

    void Editor::initializeImGui()
    {
    }

    void Editor::destroyImGui()
    {
    }
}