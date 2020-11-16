#include "Editor.h"
#include "Core/DiveEngine.h"


Editor::Editor(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	m_Engine = std::make_unique<Dive::Engine>(hInstance, hWnd, width, height);

	// spratan의 경우 SystemManager raw pointer를 멤버 변수로 둔 후 GetSystemManager()로 전달하여 System 객체를 사용토록 한다. 

	// 이하 ImGUI
}

Editor::~Editor()
{
}

void Editor::Update()
{
	if (!m_Engine)
		return;

	m_Engine->Update();

	// 호출 과정이 에반데...
//	m_Engine->GetSystemManager()->GetSystem<Dive::Renderer>()->GetSwapChain()->Present();
}

void Editor::OnResize(int width, int height)
{
	if (!m_Engine)
		return;

	// RenderSystem에 전달한다.

	APP_TRACE("Change Winodw Size: {0:d}, {1:d}", width, height);
}