#include "DivePch.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	Renderer::Renderer(Context* context)
		: Object(context)
	{
	}

	Renderer::~Renderer()
	{
		CORE_TRACE("Call Renderer's Desctructor =====================");
	}
}
