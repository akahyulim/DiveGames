#include "divepch.h"
#include "DvEngine.h"
#include "DvEngineDef.h"
#include "Core/DvContext.h"
#include "Core/DvEventSystem.h"
#include "Graphics/DvGraphics.h"
#include "IO/DvLog.h"

namespace Dive
{
	DvEngine::DvEngine(DvContext* pContext)
		: DvObject(pContext),
		m_bInitialized(false),
		m_bExiting(false)
	{
		// �⺻ subsystem ����: timer, loger, resource cache, input, filesystem
		m_pContext->RegisterSubsystem(std::make_shared<DvLog>(pContext));

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

		// ���� subsystem �ʱ�ȭ
		auto pLog = m_pContext->GetSubsystem<DvLog>();
		pLog->Initialize("Dive.log");

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

			// �׷��Ƚ��� ��Ÿ ����
		}

		DV_LOG_ENGINE_INFO("���� �ʱ�ȭ ����");
		
		m_bInitialized = true;

		return true;
	}
	
	void DvEngine::RunFrame()
	{
		if (IsExiting())
			return;

		// temp
		// �ӽ��̱� ������ ������ ����ϴ�...
		auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
		if (!pGraphics->RunWindow())
		{
			m_bExiting = true;
			return;
		}

		Update();

		Render();
	}
	
	void DvEngine::Update()
	{
		// timestep��
		// Update, Post_, Render_, PostRender_Update �� ������ ���� �̺�Ʈ�� ȣ���Ѵ�.
		// ���⿡�� Render_Update�� Rendering�� �϶�� �̺�Ʈ�� �ƴ� �� �ϴ�.
	}
	
	void DvEngine::Render()
	{
		auto pGraphics = m_pContext->GetSubsystem<DvGraphics>();
		if (!pGraphics->BeginFrame())
			return;

		// renderer
		// ui

		pGraphics->EndFrame();
	}

	// �׷��Ƚ��� �����츦 Ŭ�����ϰ� exit flag�� �����Ѵܴ�.
	// �����δ� doExit()���� ����ȴ�.
	// Application�� ErrorExit()���� ȣ��ȴ�.
	void DvEngine::Exit()
	{
		doExit();
	}

	void DvEngine::OnExitRequested()
	{
		doExit();
	}

	bool DvEngine::HasParameter(const VariantMap& parameters, const std::string& parameter)
	{
		return parameters.find(StringHash(parameter)) != parameters.end();
	}

	void DvEngine::doExit()
	{
		auto* pGraphics = GetSubsystem<DvGraphics>();
		if (pGraphics)
		{
			// Graphics::Close()
			// Graphics�� �����ϴ� �� �ƴϰ�
			// �����츸 �����Ų��.
			// ������ ���� �� �𸣰ڴ�.
		}

		m_bExiting = true;
	}
}
