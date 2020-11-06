#include "Editor.h"
#include "Core/DiveEngine.h"

Editor::Editor()
{
	m_Engine = std::make_unique<Dive::Engine>();
}

Editor::~Editor()
{
}

bool Editor::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	if (m_Engine->Initialize(hInstance, hWnd))
		return false;

	return true;
}

void Editor::Update()
{
	m_Engine->Update();

	auto timer = m_Engine->GetSystemManager()->GetSystem<Dive::Timer>();
	if (!timer)
		return;

	APP_INFO("Elapsed Time: {0:f} / Smooth Elapsed Time: {1:f}", timer->GetDeltaTime(), timer->GetSmoothDeltaTime());
}
