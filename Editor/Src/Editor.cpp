#include "Editor.h"
#include "Core/DiveEngine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "Widgets/MenuBar.h"


Editor::Editor(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed)
{
	// Engine
	m_Engine = std::make_unique<Dive::Engine>(hInstance, hWnd, width, height, windowed);
	if (!m_Engine->IsInitialized())
	{
		return;
	}
	m_SystemManager = m_Engine->GetSystemManager();

	// ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	auto renderer = m_Engine->GetSystemManager()->GetSystem<Dive::Renderer>();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(renderer->GetRenderDevice()->GetD3dDevice(), renderer->GetRenderDevice()->GetImmediateContext());

	// Widgets
	{
		m_Widgets.emplace_back(std::make_shared<MenuBar>(this));
	}

	m_bInitialize = true;
}

Editor::~Editor()
{
	m_Widgets.clear();

	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void Editor::Update()
{
	if (!m_bInitialize)
		return;

	m_Engine->Update();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// draw widget
	for (auto& widget : m_Widgets)
	{
		widget->Begin();
		widget->Tick();
		widget->End();
	}

	ImGui::Render();

	// 위치가 좀 달라졌다?
	{
		float clear_color[4]{ 0.1f, 0.1f, 0.1f, 1.0f };

		auto renderer = m_Engine->GetSystemManager()->GetSystem<Dive::Renderer>();
		auto renderTarget = renderer->GetRenderDevice()->GetRenderTargetView();
		renderer->GetRenderDevice()->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, nullptr);
		renderer->GetRenderDevice()->GetImmediateContext()->ClearRenderTargetView(renderTarget, (float*)&clear_color);
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// 호출 과정이 에반데...
	m_Engine->GetSystemManager()->GetSystem<Dive::Renderer>()->GetRenderDevice()->Present();
}

void Editor::OnResize(int width, int height)
{
	if (!m_Engine->IsInitialized())
		return;

	APP_TRACE("Change Winodw Size: {0:d}, {1:d}", width, height);

	// 차라리 아래 주석처럼 직접 전달하는 편이 나을지도...
	Dive::WindowResizeEvent evnt(width, height);
	Dive::EventSystem::GetInstance().Fire(&evnt);
	//m_Engine->GetSystemManager()->GetSystem<Dive::Renderer>()->OnResize(width, height);

	// 이건 적용되는듯
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}