#include "DivePch.h"
#include "Input.h"
#include "Core/Context.h"
#include "Core/Log.h"


namespace Dive
{
	Input::Input(Context* context)
		: Subsystem(context)
	{
		SubscribeEvent(0, DIVE_HANDLER(Input, Update));
	}

	void Input::Update(size_t eventType)
	{
		CORE_TRACE("Input - Received Event!!!");
	}
}