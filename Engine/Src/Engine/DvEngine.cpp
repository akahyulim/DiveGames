#include "divepch.h"
#include "DvEngine.h"
#include "DvEngineDef.h"
#include "Core/DvContext.h"
#include "Core/DvEventSystem.h"
#include "Graphics/DvGraphics.h"
#include "Renderer/DvRenderer.h"
#include "IO/DvLog.h"

namespace Dive
{
	DvEngine::DvEngine(DvContext* pContext)
		: DvObject(pContext),
		m_bInitialized(false),
		m_bExiting(false),
		m_TimeStep(0.0f)
	{
		// 기본 subsystem 생성: resource cache, input, filesystem
		m_pContext->RegisterSubsystem(std::make_shared<DvLog>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<DvTime>(pContext));

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
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
			pGraphics->SetTitle(parameters.title);
			// icon
			pGraphics->SetPosition(parameters.positionX, parameters.positionY);

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

		pTime->BeginFrame(m_TimeStep);

		// temp: 임시긴 하지만 구조상 깔끔하다...
		if (!GetSubsystem<DvGraphics>()->RunWindow())
		{
			m_bExiting = true;
			return;
		}

		Update();

		Render();

		ApplyFrameLimit();

		pTime->EndFrame();
	}
	
	void DvEngine::Update()
	{
		float deltaTime = m_TimeStep;

		DV_EVENT_FIRE_PARAM(eDvEventType::Update, deltaTime);
		DV_EVENT_FIRE_PARAM(eDvEventType::PostUpdate, deltaTime);
		DV_EVENT_FIRE_PARAM(eDvEventType::RenderUpdate, deltaTime);
		DV_EVENT_FIRE_PARAM(eDvEventType::PostRenderUpdate, deltaTime);
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

	void DvEngine::OnExitRequested()
	{
		doExit();
	}

	// frame 맞추기
	void DvEngine::ApplyFrameLimit()
	{
		if (!m_bInitialized)
			return;

		// max frame 이상이면 sleep

		// 
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
