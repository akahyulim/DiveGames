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

		m_bInitialized = true;

		return true;
	}

	void Engine::RunFrame()
	{
		assert(m_bInitialized);

		// graphic�� initialized�ΰ�
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
	}

	void Engine::Render()
	{
		// Begin Frame
		// Renderer Render
		// UI Render: �� ���� imGUI�� ���� �� ���� ������?
		// EndFrame
	}
}
