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

        // create window
        m_pAppWnd = new AppWindow(hInstance, m_WindowProps);
        DV_ASSERT(m_pAppWnd);
        m_pAppWnd->SetEventCallback(DV_BIND_EVENT_FN(Engine::OnEvent));

        // create d3d11 device
        createDeviceD3D();
    }

    Editor::~Editor()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        cleanupDeviceD3D();
        m_pAppWnd->Destroy();
        DV_DELETE(m_pAppWnd);
    }

    void Editor::Initialize()
    {
        Engine::Initialize();

        // intialize imGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;
        //io.ConfigViewportsNoDefaultParent = true;
        //io.ConfigDockingAlwaysTabBar = true;
        //io.ConfigDockingTransparentPayload = true;
        //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
        //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(m_pAppWnd->GetWinodwHandle());
        ImGui_ImplDX11_Init(m_pD3dDevice, m_pD3dDeviceContext);

        m_pAppWnd->Show();
    }

    void Editor::Run()
    {
        if (!m_pAppWnd)
            return;

        while (m_pAppWnd->Run())
        {
            Update();

            beginFrame();

            Render();

            endFrame();
        }
    }

    void Editor::createDeviceD3D()
    {
        DV_ASSERT(m_pAppWnd);
        HWND hWnd = m_pAppWnd->GetWinodwHandle();

        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount                          = 2;
        sd.BufferDesc.Width                     = 0;
        sd.BufferDesc.Height                    = 0;
        sd.BufferDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator     = 60;
        sd.BufferDesc.RefreshRate.Denominator   = 1;
        sd.Flags                                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage                          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow                         = hWnd;
        sd.SampleDesc.Count                     = 1;
        sd.SampleDesc.Quality                   = 0;
        sd.Windowed                             = TRUE;
        sd.SwapEffect                           = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        auto hResult = D3D11CreateDeviceAndSwapChain(
            NULL, 
            D3D_DRIVER_TYPE_HARDWARE, 
            NULL, 
            createDeviceFlags, 
            featureLevelArray, 
            2, 
            D3D11_SDK_VERSION, 
            &sd, 
            &m_pSwapChain, 
            &m_pD3dDevice, 
            &featureLevel, 
            &m_pD3dDeviceContext);
       
        DV_ASSERT(hResult == S_OK);

        createRenderTarget();
    }

    void Editor::cleanupDeviceD3D()
    {
        cleanupRenderTarget();

        if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = NULL; }
        if (m_pD3dDeviceContext) { m_pD3dDeviceContext->Release(); m_pD3dDeviceContext = NULL; }
        if (m_pD3dDevice) { m_pD3dDevice->Release(); m_pD3dDevice = NULL; }
    }

    void Editor::createRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer = nullptr;
        m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        m_pD3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pMainRenderTargetView);
        pBackBuffer->Release();
    }

    void Editor::cleanupRenderTarget()
    {
        if (m_pMainRenderTargetView) { m_pMainRenderTargetView->Release(); m_pMainRenderTargetView = NULL; }
    }

    void Editor::beginFrame()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }
    
    void Editor::endFrame()
    {
        const float clear_color_with_alpha[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
            //{ clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();

        // rendering
        ImGui::Render();
        m_pD3dDeviceContext->OMSetRenderTargets(1, &m_pMainRenderTargetView, NULL);
        m_pD3dDeviceContext->ClearRenderTargetView(m_pMainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        m_pSwapChain->Present(1, 0); // Present with vsync
        //m_pSwapChain->Present(0, 0); // Present without vsync
    }
}