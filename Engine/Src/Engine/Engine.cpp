#include "divepch.h"
#include "Engine.h"
#include "EngineDef.h"
#include "EngineEvents.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceCache.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive
{
	Engine::Engine(Context* pContext)
		: Object(pContext),
		m_bInitialized(false),
		m_bExiting(false),
		m_DeltaTime(0.0f),
		m_MaxFps(0)
	{
		// �⺻ subsystem ����: input
		m_pContext->RegisterSubsystem(std::make_shared<Log>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<Time>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<ResourceCache>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<FileSystem>(pContext));

		SUBSCRIBE_EVENT(eEventType::ExitRequested, EVENT_HANDLER_PARAM(OnExitRequested));
	}

	Engine::~Engine()
	{
		DV_LOG_ENGINE_DEBUG("Engine �Ҹ��� ȣ��");

		// Context shut down??
		// �� �͵� �ϴ� �־���� ������... �� �ʿ��� �� ���� �ϴ�.
		EventSystem::GetInstance().Clear();
	}
	
	bool Engine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem �� graphics, renderer ����
		m_pContext->RegisterSubsystem(std::make_shared<Graphics>(m_pContext));
		m_pContext->RegisterSubsystem(std::make_shared<Renderer>(m_pContext));

		// start logging
		GetSubsystem<Log>()->Initialize("Dive.log");

		// add resource path
		// �Ķ���͸� �޴´�.
		if (!intializeResourceCache())
			return false;

		// �׷��Ƚ� �ʱ�ȭ
		{
			auto pGraphics = m_pContext->GetSubsystem<Graphics>();
			pGraphics->SetWindowTitle(parameters.title);
			// icon
			//pGraphics->SetPosition(parameters.positionX, parameters.positionY);

			if (!pGraphics->SetMode(
				parameters.width,
				parameters.height,
				parameters.bFullscreen,
				parameters.bBorderless,
				parameters.bReSizable,
				parameters.bVSync,
				parameters.bTripleBuffer,
				parameters.multiSample,
				parameters.refreshRate))
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

		DV_LOG_ENGINE_INFO("���� �ʱ�ȭ ����");
		
		m_bInitialized = true;

		return true;
	}
	
	void Engine::RunFrame()
	{
		if (IsExiting())
			return;

		auto* pTime = GetSubsystem<Time>();

		pTime->BeginFrame(m_DeltaTime);

		// temp: �ӽñ� ������ ������ ����ϴ�...
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
		float deltaTime = m_DeltaTime;

		PreUpdateEvent preUdateEvent;
		preUdateEvent.SetDeltaTime(deltaTime);
		FIRE_EVENT(preUdateEvent);

		UpdateEvent updateEvent;
		updateEvent.SetDeltaTime(deltaTime);
		FIRE_EVENT(updateEvent);

		PostUpdateEvent postUpdateEvent;
		postUpdateEvent.SetDeltaTime(deltaTime);
		FIRE_EVENT(postUpdateEvent);

		// �Ʒ� �� �̺�Ʈ�� �ǹ̰� �ָ��ϴ�.
		RenderUpdateEvent renderUpdateEvent;
		renderUpdateEvent.SetDeltaTime(deltaTime);
		FIRE_EVENT(renderUpdateEvent);

		PostRenderUpdateEvent postRenderUpdateEvent;
		postRenderUpdateEvent.SetDeltaTime(deltaTime);
		FIRE_EVENT(postRenderUpdateEvent);
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

	// �Ķ���Ϳ� �õ� ����� �� ���� �޴´�.
	// �ϴ� �⺻ ��θ� �������.
	bool Engine::intializeResourceCache()
	{
		auto* pCache = GetSubsystem<ResourceCache>();

		//pCache->AddResourceDir("CoreData/RenderPaths");

		pCache->AddResourceDir("Assets/Textures");
		pCache->AddResourceDir("Assets/Models");

		return true;
	}
}
