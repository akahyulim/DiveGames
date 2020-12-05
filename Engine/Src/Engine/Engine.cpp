#include "DivePch.h"
#include "Engine/Engine.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Dive_Context.h"
#include "Core/Timer.h"
#include "Core/Settings.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"


namespace Dive
{
	// temp
	Engine* g_engine = nullptr;

	// event를 보내거나 받으려면 Subsystem을 상속해야 한다.
	// 아니면 object로 전부 옮기고, 모든 class가 이를 상속토록 변경해야 한다.
	Engine::Engine()
		: m_bInitialized(false),
		m_bExiting(false)
	{
		m_dive_context = new Dive_Context;
		m_dive_context->RegisterSubsystem<Timer>();
		m_dive_context->RegisterSubsystem<Input>();

		Log::Initialize();

		g_engine = this;
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

		m_dive_context->RegisterSubsystem<Graphics>();
		m_dive_context->RegisterSubsystem<Renderer>();

		//= 이하 Subsystem 초기화 =========================================
		auto timer = m_dive_context->GetSubsystem<Timer>();
		timer->Initialize();


		auto graphics = m_dive_context->GetSubsystem<Graphics>();
		if (!graphics->SetScreenMode())
			return false;

		m_bInitialized = true;

		CORE_TRACE("Initialized Engine.");

		return true;
	}

	void Engine::DoExit()
	{
		// graphic close

		m_bExiting = true;
	}

	void Engine::RunFrame()
	{
		assert(m_bInitialized);

		// 이 부분에서 exiting이 되어야 한다.
		// 지금 의심스러운 부분은 headless이다.
		if (!m_dive_context->GetSubsystem<Graphics>()->IsInitialized())
			m_bExiting = true;

		if (m_bExiting)
			return;

		Update();

		Render();
	}

	void Engine::Update()
	{
		// Update, Post Update, Render, Post Render를 이벤트로 날린다.

		auto timer = m_dive_context->GetSubsystem<Timer>();
		timer->Update();
	}

	void Engine::Render()
	{
		// Begin Frame
		auto graphics = m_dive_context->GetSubsystem<Graphics>();
		if (!graphics->BeginFrame())
			return;

		// Renderer Render
		// UI Render: 이 곳에 imGUI를 넣을 수 있지 않을까?
		// EndFrame
		graphics->EndFrame();
	}
}
