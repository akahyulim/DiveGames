#include "DivePch.h"
#include "Engine/Engine.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Context.h"
#include "Core/Timer.h"
#include "Core/Settings.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"


namespace Dive
{
	// temp: Window의 종료 메시지 전달을 위해 임시로 구현
	// 이벤트로 변경해야 한다.
	Engine* g_engine = nullptr;

	Engine::Engine()
		: m_bInitialized(false),
		m_bExiting(false)
	{
		m_dive_context = new Context;
		m_dive_context->RegisterSubsystem<Timer>();
		m_dive_context->RegisterSubsystem<Input>();

		Log::Initialize();

		g_engine = this;
	}

	Engine::~Engine()
	{
		CORE_TRACE("Call Engine's Destructor ========================");
	}
	
	// 설정데이터를 받아야 한다.
	bool Engine::Initialize()
	{
		if (m_bInitialized)
			return true;

		m_dive_context->RegisterSubsystem<Graphics>();
		m_dive_context->RegisterSubsystem<Renderer>();

		//= 이하 Subsystem 초기화 =========================================
		// 초기화는 단순 Initialize호출부터 각 객체에 맞춰 복잡화될 수 있다.
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
