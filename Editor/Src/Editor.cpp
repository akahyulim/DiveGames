#include "Editor.h"
#include "ImGUI/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "Widgets/MenuBar.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Editor::Editor()
	: m_bInitialized(false),
	m_bExiting(false)
{
	Log::Initialize();

	m_context = new Context;
	m_context->RegisterSubsystem<Time>();
	m_context->RegisterSubsystem<Input>();
	m_context->RegisterSubsystem<Scene>();
}

Editor::~Editor()
{
	SAFE_DELETE(m_context);
}

LRESULT Editor::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	auto graphics = m_context->GetSubsystem<Graphics>();
	if (graphics)
		return CallWindowProc(graphics->GetWindow()->GetBaseWndProc(), hWnd, msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
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

	auto renderer = m_context->GetSubsystem<Renderer>();
	renderer->Initialize();


	graphics->GetWindow()->SetTitle(L"Sandbox");
	graphics->GetWindow()->Show(true);


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

	// EndFrame
	graphics->EndFrame();
}
