#include "DvEditor.h"

#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
#include "imguizmo/ImGuizmo.h"

// temp
#include <CommCtrl.h>
#pragma comment(lib, "ComCtl32.lib") 

DEFINE_APPLICATION_MAIN(Editor::DvEditor)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK EditorSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR RefData)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_DESTROY:
	{
		DV_LOG_CLIENT_DEBUG("서브클래싱 프로시져 종료 메시지");

		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	}

	//return DefWindowProc(hWnd, msg, wParam, lParam);
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

namespace Editor
{
	DvEditor::DvEditor(Dive::DvContext* pContext)
		: Dive::Application(pContext)
	{
	}

	DvEditor::~DvEditor()
	{
	}
	
	void DvEditor::Setup()
	{
		// Engine Mode 설정 가능
		m_EngineParameters.title = L"Editor";
		m_EngineParameters.width = 1600;
		m_EngineParameters.height = 900;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
	}
	
	void DvEditor::Start()
	{
		DV_LOG_CLIENT_DEBUG("call Editor's Start()");
		// UI 설정

		auto* pGraphics = m_pContext->GetSubsystem<Dive::DvGraphics>();
		auto hWnd = pGraphics->GetWindowHandle();

		::SetWindowSubclass(hWnd, EditorSubclassProc, 1, (DWORD_PTR)this);
	}
	
	void DvEditor::Stop()
	{
		DV_LOG_CLIENT_DEBUG("call Editor's Stop()");

		// 이 위치는 잘못됐다??
		auto* pGraphics = m_pContext->GetSubsystem<Dive::DvGraphics>();
		auto hWnd = pGraphics->GetWindowHandle();

		::RemoveWindowSubclass(hWnd, EditorSubclassProc, (DWORD_PTR)this);
	}
}
