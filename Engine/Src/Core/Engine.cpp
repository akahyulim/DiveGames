#include "DivePch.h"
#include "Engine.h"
#include "DiveDefs.h"
#include "Core/Log.h"
#include "Core/Context.h"
#include "Core/Timer.h"
#include "Core/Settings.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"


namespace Dive
{
	Engine::Engine()
		: m_bInitialized(false),
		m_bExiting(false)
	{
		m_context = std::make_shared<Context>();
		m_context->RegisterSubsystem<Timer>();

		Log::Initialize();
	}

	Engine::~Engine()
	{
		CORE_TRACE("Call Engine's Destructor ========================");
	}
	
	// setting data를 가져와 subsystem들을 초기화한다.
	bool Engine::Initialize()
	{
		if (m_bInitialized)
			return true;

		m_context->RegisterSubsystem<Graphics>();
		m_context->RegisterSubsystem<Renderer>();

		//= 이하 Subsystem 초기화 =========================================
		auto timer = m_context->GetSubsystem<Timer>();
		timer->Initialize();

		m_bInitialized = true;

		return true;
	}

	void Engine::RunFrame()
	{
		assert(m_bInitialized);

		// graphic이 initialized인가
		if (!m_context->GetSubsystem<Graphics>()->IsInitialized())
			m_bExiting = true;

		if (m_bExiting)
			return;

		Update();

		Render();
	}

	void Engine::Update()
	{
		// Update, Post Update, Render, Post Render를 이벤트로 날린다.
	}

	void Engine::Render()
	{
		// Begin Frame
		// Renderer Render
		// UI Render: 이 곳에 imGUI를 넣을 수 있지 않을까?
		// EndFrame
	}
}
