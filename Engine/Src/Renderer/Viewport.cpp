#include "divepch.h"
#include "Viewport.h"
#include "View.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Viewport::Viewport(Context* pContext)
		: Object(pContext)
	{
		m_Rect = { 0 ,0, 0, 0 };
		// Renderer로부터 DefaultRenderPath를 가져온다.
	}

	Viewport::Viewport(Context* pContext, Scene* pScene, eRenderPath renderPath) //+ camera
		: Object(pContext),
		m_pScene(pScene),
		m_RenderPath(renderPath)
	{
		m_Rect = { 0 ,0, 0, 0 };
	}

	Viewport::Viewport(Context* pContext, Scene* pScene, const RECT& rect, eRenderPath renderPath) // + camera
		: Object(pContext),
		m_pScene(pScene),
		m_Rect(rect),
		m_RenderPath(renderPath)
	{

	}

	Viewport::~Viewport()
	{
		DV_LOG_ENGINE_DEBUG("Viewport 소멸자 호출");
		DV_DELETE(m_pView);
	}

	// Renderer가 호출.
	void Viewport::AllocateView()
	{
		DV_DELETE(m_pView);
		m_pView = new View(m_pContext);
	}
}