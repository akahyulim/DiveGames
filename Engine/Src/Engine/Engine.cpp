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
		// 기본 subsystem 생성: input
		m_pContext->RegisterSubsystem(std::make_shared<Log>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<Time>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<ResourceCache>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<FileSystem>(pContext));

		SUBSCRIBE_EVENT(eEventType::ExitRequested, EVENT_HANDLER_PARAM(OnExitRequested));
	}

	Engine::~Engine()
	{
		DV_LOG_ENGINE_DEBUG("Engine 소멸자 호출");

		// Context shut down??
		// 이 것두 일단 넣어놓긴 했지만... 뭐 필요한 거 같긴 하다.
		EventSystem::GetInstance().Clear();
	}
	
	bool Engine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem 중 graphics, renderer 생성
		m_pContext->RegisterSubsystem(std::make_shared<Graphics>(m_pContext));
		m_pContext->RegisterSubsystem(std::make_shared<Renderer>(m_pContext));

		// start logging
		GetSubsystem<Log>()->Initialize("Dive.log");

		// add resource path
		// 파라미터를 받는다.
		if (!intializeResourceCache())
			return false;

		// 그래픽스 초기화
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

		// 오디오 초기화

		// 이닛 프레임

		// 이닛 인풋

		DV_LOG_ENGINE_INFO("엔진 초기화 성공");
		
		m_bInitialized = true;

		return true;
	}
	
	void Engine::RunFrame()
	{
		if (IsExiting())
			return;

		auto* pTime = GetSubsystem<Time>();

		pTime->BeginFrame(m_DeltaTime);

		// temp: 임시긴 하지만 구조상 깔끔하다...
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
		// 시간이 다 같지는 않을텐데...
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

		// 아래 두 이벤트는 의미가 애매하다.
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

		// max frame 이상이면 sleep
		// 아니면 그냥 실행
		// 이를 위해선 fps를 직접 계산해야 한다.

		m_DeltaTime = static_cast<float>(m_FrameTimer.GetMSec(false) * 0.0001f);
	}

	void Engine::doExit()
	{
		auto* pGraphics = GetSubsystem<Graphics>();
		if (pGraphics)
		{
			// 현재까진 문제가 없지만
			// 윈도우를 먼저 제거한다는 건 좀 이상하다.
			pGraphics->CloseWindow();
		}

		m_bExiting = true;
	}

	// 파라미터와 올드 지우기 두 개를 받는다.
	// 일단 기본 경로만 등록하자.
	bool Engine::intializeResourceCache()
	{
		auto* pCache = GetSubsystem<ResourceCache>();

		//pCache->AddResourceDir("CoreData/RenderPaths");

		pCache->AddResourceDir("Assets/Textures");
		pCache->AddResourceDir("Assets/Models");

		return true;
	}
}
