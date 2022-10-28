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
		// Renderer�κ��� DefaultRenderPath�� �����´�.
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
		DV_LOG_ENGINE_DEBUG("Viewport �Ҹ��� ȣ��");
		DV_DELETE(m_pView);
	}

	// Renderer�� ȣ��.
	void Viewport::AllocateView()
	{
		DV_DELETE(m_pView);
		m_pView = new View(m_pContext);
	}
}