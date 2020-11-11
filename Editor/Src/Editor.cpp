#include "Editor.h"
#include "Core/DiveEngine.h"

Editor::Editor()
{
	m_Engine = std::make_unique<Dive::Engine>();
}

Editor::~Editor()
{
}

// 해상도는 수시로 갱신될 수 있으므로 한 번의 전달로는 부족하다.
// 그래서 spartan은 wndpoc을 통해 윈도우 메시지가 처리될 때 마다 이를 갱신한다.
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

	//APP_INFO("Elapsed Time: {0:f} / Smooth Elapsed Time: {1:f}", timer->GetDeltaTime(), timer->GetSmoothDeltaTime());
}
