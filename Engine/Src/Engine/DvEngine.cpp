#include "divepch.h"
#include "DvEngine.h"
#include "DvEngineDef.h"
#include "Core/DvContext.h"
#include "Core/DvEventSystem.h"
#include "Core/CoreEvents.h"
#include "Graphics/DvGraphics.h"
#include "Renderer/DvRenderer.h"
#include "IO/DvLog.h"

namespace Dive
{
	DvEngine::DvEngine(DvContext* pContext)
		: DvObject(pContext),
		m_bInitialized(false),
		m_bExiting(false),
		m_DeltaTime(0.0f),
		m_MaxFps(0)
	{
		// 기본 subsystem 생성: resource cache, input, filesystem
		m_pContext->RegisterSubsystem(std::make_shared<DvLog>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<DvTime>(pContext));

		DV_SUBSCRIBE_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
	}

	DvEngine::~DvEngine()
	{
		DV_LOG_ENGINE_DEBUG("DvEngine 소멸자 호출");
	}
	
	bool DvEngine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem 중 graphics, renderer 생성
		m_pContext->RegisterSubsystem(std::make_shared<DvGraphics>(m_pContext));
		m_pContext->RegisterSubsystem(std::make_shared<DvRenderer>(m_pContext));

		// start logging
		GetSubsystem<DvLog>()->Initialize("Dive.log");

		// 그래픽스 초기화
		{
			auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
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
	
	void DvEngine::RunFrame()
	{
		if (IsExiting())
			return;

		auto* pTime = GetSubsystem<DvTime>();

		pTime->BeginFrame(m_DeltaTime);

		// temp: 임시긴 하지만 구조상 깔끔하다...
		if (!GetSubsystem<DvGraphics>()->RunWindow())
		{
			m_bExiting = true;
			return;
		}

		Update();

		Render();

		updateFrameTimer();

		pTime->EndFrame();
	}
	
	void DvEngine::Update()
	{
		float deltaTime = m_DeltaTime;

		UpdateEvent updateEvent;
		updateEvent.SetDeltaTime(deltaTime);
		DV_FIRE_EVENT(updateEvent);

		PostUpdateEvent postUpdateEvent;
		postUpdateEvent.SetDeltaTime(deltaTime);
		DV_FIRE_EVENT(postUpdateEvent);

		RenderUpdateEvent renderUpdateEvent;
		renderUpdateEvent.SetDeltaTime(deltaTime);
		DV_FIRE_EVENT(renderUpdateEvent);

		PostRenderUpdateEvent postRenderUpdateEvent;
		postRenderUpdateEvent.SetDeltaTime(deltaTime);
		DV_FIRE_EVENT(postRenderUpdateEvent);
	}
	
	void DvEngine::Render()
	{
		auto pGraphics = GetSubsystem<DvGraphics>();
		if (!pGraphics->BeginFrame())
			return;

		GetSubsystem<DvRenderer>()->Render();
		
		// ui

		pGraphics->EndFrame();
	}

	void DvEngine::Exit()
	{
		doExit();
	}

	void DvEngine::OnExitRequested(const DvEvent& e)
	{
		doExit();
	}

	void DvEngine::updateFrameTimer()
	{
		if (!m_bInitialized)
			return;

		// max frame 이상이면 sleep
		// 아니면 그냥 실행
		// 이를 위해선 fps를 직접 계산해야 한다.

		m_DeltaTime = static_cast<float>(m_FrameTimer.GetMSec(false) * 0.0001f);
	}

	void DvEngine::doExit()
	{
		auto* pGraphics = GetSubsystem<DvGraphics>();
		if (pGraphics)
		{
			// 현재까진 문제가 없지만
			// 윈도우를 먼저 제거한다는 건 좀 이상하다.
			pGraphics->CloseWindow();
		}

		m_bExiting = true;
	}
}
