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
		// �⺻ subsystem ����: resource cache, input, filesystem
		m_pContext->RegisterSubsystem(std::make_shared<DvLog>(pContext));
		m_pContext->RegisterSubsystem(std::make_shared<DvTime>(pContext));

		DV_SUBSCRIBE_TO_EVENT(eDvEventType::ExitRequested, DV_EVENT_HANDLER(OnExitRequested));
	}

	DvEngine::~DvEngine()
	{
		DV_LOG_ENGINE_DEBUG("DvEngine �Ҹ��� ȣ��");
	}
	
	bool DvEngine::Initialize(const EngineParameters& parameters)
	{
		if(IsInitialized())
			return true;

		// subsystem �� graphics, renderer ����
		m_pContext->RegisterSubsystem(std::make_shared<DvGraphics>(m_pContext));
		m_pContext->RegisterSubsystem(std::make_shared<DvRenderer>(m_pContext));

		// start logging
		GetSubsystem<DvLog>()->Initialize("Dive.log");

		// �׷��Ƚ� �ʱ�ȭ
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

		// ����� �ʱ�ȭ

		// �̴� ������

		// �̴� ��ǲ

		DV_LOG_ENGINE_INFO("���� �ʱ�ȭ ����");
		
		m_bInitialized = true;

		return true;
	}
	
	void DvEngine::RunFrame()
	{
		if (IsExiting())
			return;

		auto* pTime = GetSubsystem<DvTime>();

		pTime->BeginFrame(m_TimeStep);

		// temp: �ӽñ� ������ ������ ����ϴ�...
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

	// frame ���߱�
	void DvEngine::ApplyFrameLimit()
	{
		if (!m_bInitialized)
			return;

		// max frame �̻��̸� sleep

		// 
	}

	void DvEngine::doExit()
	{
		auto* pGraphics = GetSubsystem<DvGraphics>();
		if (pGraphics)
		{
			// ������� ������ ������
			// �����츦 ���� �����Ѵٴ� �� �� �̻��ϴ�.
			pGraphics->CloseWindow();
		}

		m_bExiting = true;
	}
}
