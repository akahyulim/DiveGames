#include "DivePch.h"
#include "Engine/Engine.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Context.h"
#include "Core/Time.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Scene/Scene.h"


namespace Dive
{
	Engine::Engine()
		: m_bInitialized(false),
		m_bExiting(false)
	{
		Log::Initialize();
	
		m_context = new Context;
		m_context->RegisterSubsystem<Time>();
		m_context->RegisterSubsystem<Input>();
		m_context->RegisterSubsystem<Scene>();
	}

	Engine::~Engine()
	{
		SAFE_DELETE(m_context);

		CORE_TRACE("Call Engine's Destructor ========================");
	}
	
	// 설정데이터
	// ini 포멧의 파일명을 전달 받는다. 그리고 구조체에 넣는다.
	// 파일이 없다면 빈 구조체를 사용한다.
	// 해당 데이터를 이용해 각종 subsystem들을 초기화한다.
	bool Engine::Initialize()
	{
		if (m_bInitialized)
			return true;

		m_context->RegisterSubsystem<Graphics>();
		m_context->RegisterSubsystem<Renderer>();

		//= Graphics & Renderer
		auto graphics = m_context->GetSubsystem<Graphics>();
		// 원래는 setting을 가져와서 비교하며 전달한다.
		if (!graphics->SetMode(1024, 768, false, false, true))
			return false;

		auto renderer = m_context->GetSubsystem<Renderer>();
		renderer->Initialize();

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

		auto time = m_context->GetSubsystem<Time>();
		time->Update();
	}

	void Engine::Render()
	{
		// Begin Frame
		auto graphics = m_context->GetSubsystem<Graphics>();
		if (!graphics->BeginFrame())
		{
			DoExit();
			return;
		}

		GetSubsystem<Renderer>()->Render();
		// UI Render
			
		// EndFrame
		graphics->EndFrame();
	}
}
