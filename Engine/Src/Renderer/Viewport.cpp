#include "divepch.h"
#include "Viewport.h"
#include "Renderer.h"
#include "RenderPath.h"
#include "View.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Viewport::Viewport(Context* pContext)
		: Object(pContext),
		m_pScene(nullptr),
		m_pView(nullptr),
		m_Rect({0, 0, 0, 0})
	{
		SetRenderPath(nullptr);

		DV_LOG_ENGINE_TRACE("Viewport ����");
	}

	Viewport::Viewport(Context* pContext, Scene* pScene, RenderPath* pRenderPath) //+ camera
		: Object(pContext),
		m_pScene(pScene),
		m_pView(nullptr),
		m_Rect({ 0, 0, 0, 0 })
	{
		SetRenderPath(pRenderPath);
	}

	Viewport::Viewport(Context* pContext, Scene* pScene, RenderPath* pRenderPath, const RECT& rect) // + camera
		: Object(pContext),
		m_pScene(pScene),
		m_pView(nullptr),
		m_Rect(rect)
	{
		SetRenderPath(pRenderPath);
	}

	Viewport::~Viewport()
	{
		DV_DELETE(m_pView);

		DV_LOG_ENGINE_TRACE("Viewport �Ҹ� �Ϸ�");
	}

	void Viewport::SetRenderPath(RenderPath* pRenderPath)
	{
		if (pRenderPath)
			m_pRenderPath = pRenderPath;
		else
		{
			Renderer* pRenderer = GetSubsystem<Renderer>();
			m_pRenderPath = pRenderer->GetDefaultRenderPath();
		}
	}

	// Renderer�� ȣ��.
	void Viewport::AllocateView()
	{
		DV_DELETE(m_pView);
		m_pView = new View(m_pContext);
	}
}