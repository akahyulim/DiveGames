#include "DivePch.h"
#include "Graphics.h"
#include "Core/Context.h"


namespace Dive
{
	Graphics::Graphics(Context* context)
		: Subsystem(context),
		m_bInitialized(false)

	{
	}

	Graphics::~Graphics()
	{

	}
}
