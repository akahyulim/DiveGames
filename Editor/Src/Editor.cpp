#include "Editor.h"
#include "Dive.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "Widgets/MenuBar.h"


Editor::Editor(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed)
{
	// Engine
	m_engine = std::make_unique<Dive::Engine>(hInstance, hWnd, width, height, windowed);
	if (!m_engine->IsInitialized())
	{
		return;
	}
	m_systemManager = m_engine->GetSystemManager();

	// ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	auto renderer = m_engine->GetSystemManager()->GetSystem<Dive::Renderer>();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(renderer->GetRenderDevice()->GetD3dDevice(), renderer->GetRenderDevice()->GetImmediateContext());

	// Widgets
	{
		m_widgets.emplace_back(std::make_shared<MenuBar>(this));
	}

	m_bInitialize = true;
}

Editor::~Editor()
{
	m_widgets.clear();

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

	m_engine->Update();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// draw widget
	for (auto& widget : m_widgets)
	{
		widget->Begin();
		widget->Tick();
		widget->End();
	}

	ImGui::Render();

	// ��ġ�� �� �޶�����?
	{
		float clear_color[4]{ 0.1f, 0.1f, 0.1f, 1.0f };

		auto renderer = m_engine->GetSystemManager()->GetSystem<Dive::Renderer>();
		auto renderTarget = renderer->GetRenderDevice()->GetRenderTargetView();
		renderer->GetRenderDevice()->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, nullptr);
		renderer->GetRenderDevice()->GetImmediateContext()->ClearRenderTargetView(renderTarget, (float*)&clear_color);
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// ȣ�� ������ ���ݵ�...
	m_engine->GetSystemManager()->GetSystem<Dive::Renderer>()->GetRenderDevice()->Present();
}

void Editor::OnResize(int width, int height)
{
	if (!m_engine->IsInitialized())
		return;

	APP_TRACE("Change Winodw Size: {0:d}, {1:d}", width, height);

	// ���� �Ʒ� �ּ�ó�� ���� �����ϴ� ���� ��������...
	Dive::WindowResizeEvent evnt(width, height);
	Dive::EventSystem::GetInstance().Fire(&evnt);
	//m_engine->GetSystemManager()->GetSystem<Dive::Renderer>()->OnResize(width, height);

	// �̰� ����Ǵµ�
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}