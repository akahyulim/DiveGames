#include "DivePch.h"
#include "Engine/Engine.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Context.h"
#include "Core/Timer.h"
#include "Core/Settings.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"


namespace Dive
{
	// temp
	Engine* g_engine = nullptr;

	// event�� �����ų� �������� Subsystem�� ����ؾ� �Ѵ�.
	// �ƴϸ� object�� ���� �ű��, ��� class�� �̸� ������ �����ؾ� �Ѵ�.
	Engine::Engine()
		: m_bInitialized(false),
		m_bExiting(false)
	{
		m_context = std::make_shared<Context>();
		m_context->RegisterSubsystem<Timer>();
		m_context->RegisterSubsystem<Input>();

		Log::Initialize();

		g_engine = this;
	}

	Engine::~Engine()
	{
		CORE_TRACE("Call Engine's Destructor ========================");
	}
	
	// setting data�� ������ subsystem���� �ʱ�ȭ�Ѵ�.
	bool Engine::Initialize()
	{
		if (m_bInitialized)
			return true;

		m_context->RegisterSubsystem<Graphics>();
		m_context->RegisterSubsystem<Renderer>();

		//= ���� Subsystem �ʱ�ȭ =========================================
		auto timer = m_context->GetSubsystem<Timer>();
		timer->Initialize();


		auto graphics = m_context->GetSubsystem<Graphics>();
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

		// �� �κп��� exiting�� �Ǿ�� �Ѵ�.
		// ���� �ǽɽ����� �κ��� headless�̴�.
		if (!m_context->GetSubsystem<Graphics>()->IsInitialized())
			m_bExiting = true;

		if (m_bExiting)
			return;

		Update();

		Render();
	}

	void Engine::Update()
	{
		// Update, Post Update, Render, Post Render�� �̺�Ʈ�� ������.

		// test
		auto timer = m_context->GetSubsystem<Timer>();
		timer->Update();
	}

	void Engine::Render()
	{
		// Begin Frame
		auto graphics = m_context->GetSubsystem<Graphics>();
		if (!graphics->BeginFrame())
			return;

		// Renderer Render
		// UI Render: �� ���� imGUI�� ���� �� ���� ������?
		// EndFrame
		graphics->EndFrame();
	}
}
