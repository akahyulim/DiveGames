#include "DivePch.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	Renderer::Renderer(Context* context)
		: Object(context),
		m_bInitialized(false)
	{
	}

	Renderer::~Renderer()
	{
		CORE_TRACE("Call Renderer's Desctructor =====================");
	}

	void Renderer::Initialize()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
			return;
		m_graphics = graphics;

		// 뭘로 채워나가나...

		m_bInitialized = true;
	}

	void Renderer::Render()
	{
		//CORE_TRACE("Rendering...");
	}
}
