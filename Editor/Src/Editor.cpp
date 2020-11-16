#include "Editor.h"
#include "Core/DiveEngine.h"


Editor::Editor(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	m_Engine = std::make_unique<Dive::Engine>(hInstance, hWnd, width, height);

	// spratan�� ��� SystemManager raw pointer�� ��� ������ �� �� GetSystemManager()�� �����Ͽ� System ��ü�� ������ �Ѵ�. 

	// ���� ImGUI
}

Editor::~Editor()
{
}

void Editor::Update()
{
	if (!m_Engine)
		return;

	m_Engine->Update();

	// ȣ�� ������ ���ݵ�...
//	m_Engine->GetSystemManager()->GetSystem<Dive::Renderer>()->GetSwapChain()->Present();
}

void Editor::OnResize(int width, int height)
{
	if (!m_Engine)
		return;

	// RenderSystem�� �����Ѵ�.

	APP_TRACE("Change Winodw Size: {0:d}, {1:d}", width, height);
}