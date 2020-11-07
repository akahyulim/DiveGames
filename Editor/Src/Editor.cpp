#include "Editor.h"
#include "Core/DiveEngine.h"

Editor::Editor()
{
	m_Engine = std::make_unique<Dive::Engine>();
}

Editor::~Editor()
{
}

bool Editor::Initialize(HWND hWnd, int width, int height, bool vsync, bool windowed)
{
	Dive::WindowData windowData;
	windowData.hWnd				= hWnd;
	windowData.ResolutionWidth	= width;
	windowData.ResolutionHeight = height;
	windowData.bVSync			= vsync;
	windowData.bWindowed		= windowed;

	if (!m_Engine->Initialize(windowData))
	{
		APP_ERROR("Editor Initialize에 실패하였습니다.");
		return false;
	}

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
