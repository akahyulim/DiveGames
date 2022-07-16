#include "divepch.h"
#include "DvEngine.h"
#include "Core/DvContext.h"
#include "Graphics/DvGraphics.h"

namespace Dive
{
	DvEngine::DvEngine(DvContext* pContext)
		: DvObject(pContext),
		m_bInitialized(false),
		m_bExiting(false)
	{
		m_pContext->RegisterSubsystem(this);

		// 扁夯 subsystem 积己: timer, loger, resource cache, input, filesystem
	}

	DvEngine::~DvEngine()
	{
	}
	
	bool DvEngine::Initialize()
	{
		if(IsInitialized())
			return true;

		// subsystem 吝 graphics, renderer 积己

		m_bInitialized = true;

		return true;
	}
	
	void DvEngine::RunFrame()
	{
		if (IsExiting())
			return;

		update();

		render();
	}
	
	void DvEngine::update()
	{
	}
	
	void DvEngine::render()
	{
	}
}
