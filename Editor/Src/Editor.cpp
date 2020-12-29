#include "Editor.h"
#include "ImGUI/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "Widgets/MenuBar.h"
#include "Widgets/Hierarchy.h"


Editor* s_editor = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	auto graphics = s_editor->GetContext()->GetSubsystem<Graphics>();
	if (graphics)
		return CallWindowProc(graphics->GetWindow()->GetBaseWndProc(), hWnd, msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

Editor::Editor()
	: m_bInitialized(false),
	m_bExiting(false)
{
	Log::Initialize();

	m_context = new Context;
	m_context->RegisterSubsystem<Time>();
	m_context->RegisterSubsystem<Input>();
	m_context->RegisterSubsystem<Scene>();

	s_editor = this;
}

Editor::~Editor()
{
	SAFE_DELETE(m_context);
}

bool Editor::Initialize()
{
	if (m_bInitialized)
		return true;

	m_context->RegisterSubsystem<Graphics>();
	m_context->RegisterSubsystem<Renderer>();

	//= Graphics & Renderer
	auto graphics = m_context->GetSubsystem<Graphics>();
	// 원래는 setting을 가져와서 비교하며 전달한다.
	if (!graphics->SetMode(0, 0, false, false, true))
		return false;
	graphics->GetWindow()->SetTitle(L"Sandbox");
	graphics->GetWindow()->Show(true);
	graphics->SetWindowSubclassing((LONG_PTR)&EditorWndProc);

	auto renderer = m_context->GetSubsystem<Renderer>();
	renderer->Initialize();

	// ImGUI
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(graphics->GetWindow()->GetHandle());
		ImGui_ImplDX11_Init(graphics->GetRHIDevice(), graphics->GetRHIContext());
	}

	// Widgets
	{
		m_widgets.emplace_back(std::make_shared<MenuBar>(this));
		m_widgets.emplace_back(std::make_shared<Hierarchy>(this));
	}

	m_bInitialized = true;

	return true;
}

void Editor::DoExit()
{
	// graphic close

	m_bExiting = true;
}

void Editor::RunFrame()
{
	assert(m_bInitialized);

	if (!m_context->GetSubsystem<Graphics>()->IsInitialized())
		m_bExiting = true;

	if (m_bExiting)
		return;

	Update();
	Render();
}

void Editor::Update()
{
	auto time = m_context->GetSubsystem<Time>();
	time->Update();
}

void Editor::Render()
{
	// Begin Frame
	auto graphics = m_context->GetSubsystem<Graphics>();
	if (!graphics->BeginFrame())
	{
		DoExit();
		return;
	}

	auto renderer = m_context->GetSubsystem<Renderer>();
	renderer->Render();
	// UI Render


	// ImGUI
	{
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

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	// EndFrame
	graphics->EndFrame();
}
