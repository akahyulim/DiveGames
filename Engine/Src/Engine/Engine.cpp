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
	
	// ����������
	// ini ������ ���ϸ��� ���� �޴´�. �׸��� ����ü�� �ִ´�.
	// ������ ���ٸ� �� ����ü�� ����Ѵ�.
	// �ش� �����͸� �̿��� ���� subsystem���� �ʱ�ȭ�Ѵ�.
	bool Engine::Initialize()
	{
		if (m_bInitialized)
			return true;

		m_context->RegisterSubsystem<Graphics>();
		m_context->RegisterSubsystem<Renderer>();

		//= Graphics & Renderer
		auto graphics = m_context->GetSubsystem<Graphics>();
		// ������ setting�� �����ͼ� ���ϸ� �����Ѵ�.
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
		// Update, Post Update, Render, Post Render�� �̺�Ʈ�� ������.

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
