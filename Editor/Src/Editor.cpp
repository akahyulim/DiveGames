#include "Editor.h"
#include "Core/DiveEngine.h"

Editor::Editor()
{
	m_pEngine = std::make_unique<Dive::Engine>();
}

Editor::~Editor()
{
}

bool Editor::Initialize()
{
	if (m_pEngine->Initialize())
		return false;

	return true;
}

void Editor::Update()
{
	m_pEngine->Update();

	auto timer = m_pEngine->GetSystemManager()->GetSystem<Dive::Timer>();
	if (!timer)
		return;

	APP_INFO("Elapsed Time: {0:f} / Smooth Elapsed Time: {1:f}", timer->GetDeltaTime(), timer->GetSmoothDeltaTime());
}
