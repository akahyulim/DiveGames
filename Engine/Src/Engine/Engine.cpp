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


		DV_LOG_ENGINE_TRACE("Engine 소멸 완료");
	}
	
	bool Engine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		m_pContext->RegisterSubsystem<Graphics>();
		//m_pContext->RegisterSubsystem<Renderer>();

		GetSubsystem<Log>()->Initialize("Dive.log");

		// add resource path
		// 파라미터를 받는다.
		if (!intializeResourceCache())
			return false;

		// 그래픽스 초기화
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

		// 오디오 초기화

		// 이닛 프레임

		// 이닛 인풋
		auto pInput = GetSubsystem<Input>();
		if (!pInput->Initialize(
			pGraphics->GetWindowInstance(), 
			pGraphics->GetWindowHandle(), 
			pGraphics->GetWidth(), 
			pGraphics->GetHeight()))
		{
			return false;
		}

		m_pContext->RegisterSubsystem<Renderer>();	// 임시 위치(생성자 확인)

		DV_LOG_ENGINE_TRACE("Engine 초기화 성공");
		
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
		// 시간이 다 같지는 않을텐데...
		float timeStep = m_DeltaTime;

		//FIRE_EVENT(PreUpdateEvent(timeStep));
		FIRE_EVENT(UpdateEvent(timeStep));
		FIRE_EVENT(PostUpdateEvent(timeStep));

		// 아래 두 이벤트는 의미가 애매하다.
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

	bool Engine::intializeResourceCache()
	{
		auto* pCache = GetSubsystem<ResourceCache>();

		// 리소스 경로 설정
		// 현재 ResourceCache에 관련 함수들을 모두 제거하였다.

		// 기본적으로 사용하는 리소스들 등록

		return true;
	}
}
