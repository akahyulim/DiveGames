#include "DivePch.h"
#include "Input.h"
#include "Core/Dive_Context.h"
#include "Core/Log.h"


namespace Dive
{
	Input::Input(Dive_Context* context)
		: Dive_Object(context)
	{
		SubscribeEvent(0, DIVE_HANDLER(Input, OnUpdate));
	}

	// 사실 이벤트는 eventType과 variantMap을 무조건 받아야 한다.
	void Input::OnUpdate(size_t eventType)
	{
		CORE_TRACE("Input - Received Event!!!");
	}
}