#include "divepch.h"
#include "Engine.h"
#include "EngineDef.h"
#include "EngineEvents.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceCache.h"
#include "Input/Input.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive
{
	Engine::Engine(Context* pContext)
		: Object(pContext),
		m_bInitialized(false),
		m_bExiting(false),
		m_DeltaTime(0.0f),
		m_MaxFPS(0)
	{
		m_pContext->RegisterSubsystem<Log>();
		m_pContext->RegisterSubsystem<Time>();
		m_pContext->RegisterSubsystem<ResourceCache>();
		m_pContext->RegisterSubsystem<FileSystem>();
		m_pContext->RegisterSubsystem<Input>();

		SUBSCRIBE_EVENT(eEventType::ExitRequested, EVENT_HANDLER_PARAM(OnExitRequested));
	}

	Engine::~Engine()
	{
		EventSystem::GetInstance().Clear();


		DV_LOG_ENGINE_TRACE("Engine �Ҹ� �Ϸ�");
	}
	
	bool Engine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		m_pContext->RegisterSubsystem<Graphics>();
		//m_pContext->RegisterSubsystem<Renderer>();

		GetSubsystem<Log>()->Initialize("Dive.log");

		// add resource path
		// �Ķ���͸� �޴´�.
		if (!intializeResourceCache())
			return false;

		// �׷��Ƚ� �ʱ�ȭ
		auto pGraphics = m_pContext->GetSubsystem<Graphics>();
		{
			pGraphics->SetWindowTitle(parameters.Title);
			// icon
			//pGraphics->SetPosition(parameters.positionX, parameters.positionY);

			if (!pGraphics->SetMode(
				parameters.Width,
				parameters.Height,
				parameters.bFullscreen,
				parameters.bBorderless,
				parameters.bReSizable,
				parameters.bVSync,
				parameters.bTripleBuffer,
				parameters.MultiSample,
				parameters.RefreshRate))
			{
				return false;
			}

			// graphics set
			// set shader cach dir
			// begin dump shaders

			// renderer set
			// set default render path
			// set draw shdows
			// set shadow quality
			// set texture quality
			// set texture filter mode
			// set texture anisotropy
		}

		// ����� �ʱ�ȭ

		// �̴� ������

		// �̴� ��ǲ
		auto pInput = GetSubsystem<Input>();
		if (!pInput->Initialize(
			pGraphics->GetWindowInstance(), 
			pGraphics->GetWindowHandle(), 
			pGraphics->GetWidth(), 
			pGraphics->GetHeight()))
		{
			return false;
		}

		m_pContext->RegisterSubsystem<Renderer>();	// �ӽ� ��ġ(������ Ȯ��)

		DV_LOG_ENGINE_TRACE("Engine �ʱ�ȭ ����");
		
		m_bInitialized = true;

		return true;
	}
	
	void Engine::RunFrame()
	{
		if (IsExiting())
			return;

		auto* pTime = GetSubsystem<Time>();

		pTime->BeginFrame(m_DeltaTime);

		if (!GetSubsystem<Graphics>()->RunWindow())
		{
			m_bExiting = true;
			return;
		}

		Update();

		Render();

		updateFrameTimer();

		pTime->EndFrame();
	}
	
	void Engine::Update()
	{
		// �ð��� �� ������ �����ٵ�...
		float timeStep = m_DeltaTime;

		//FIRE_EVENT(PreUpdateEvent(timeStep));
		FIRE_EVENT(UpdateEvent(timeStep));
		FIRE_EVENT(PostUpdateEvent(timeStep));

		// �Ʒ� �� �̺�Ʈ�� �ǹ̰� �ָ��ϴ�.
		FIRE_EVENT(RenderUpdateEvent(timeStep));
		FIRE_EVENT(PostRenderUpdateEvent(timeStep));
	}
	
	void Engine::Render()
	{
		auto pGraphics = GetSubsystem<Graphics>();
		if (!pGraphics->BeginFrame())
			return;

		GetSubsystem<Renderer>()->Render();
		
		// ui

		pGraphics->EndFrame();
	}

	void Engine::Exit()
	{
		doExit();
	}

	void Engine::OnExitRequested(const Event& e)
	{
		doExit();
	}

	void Engine::updateFrameTimer()
	{
		if (!m_bInitialized)
			return;

		// max frame �̻��̸� sleep
		// �ƴϸ� �׳� ����
		// �̸� ���ؼ� fps�� ���� ����ؾ� �Ѵ�.

		m_DeltaTime = static_cast<float>(m_FrameTimer.GetMSec(false) * 0.0001f);
	}

	void Engine::doExit()
	{
		auto* pGraphics = GetSubsystem<Graphics>();
		if (pGraphics)
		{
			// ������� ������ ������
			// �����츦 ���� �����Ѵٴ� �� �� �̻��ϴ�.
			pGraphics->CloseWindow();
		}

		m_bExiting = true;
	}

	bool Engine::intializeResourceCache()
	{
		auto* pCache = GetSubsystem<ResourceCache>();

		// ���ҽ� ��� ����
		// ���� ResourceCache�� ���� �Լ����� ��� �����Ͽ���.

		// �⺻������ ����ϴ� ���ҽ��� ���

		return true;
	}
}
