#include "Editor.h"
#include "core/DiveEngine.h"

Editor::Editor()
{
	m_engine = std::make_unique<Dive::Engine>();
	m_engine->Initialize();
}

Editor::~Editor()
{
}

void Editor::Update()
{
	m_engine->Update();

	auto timer = m_engine->GetSystemManager()->GetSystem<Dive::Timer>();
	if (!timer)
		return;

	APP_INFO("Elapsed Time: {0:f} / Smooth Elapsed Time: {1:f}", timer->GetDeltaTime(), timer->GetSmoothDeltaTime());
}
