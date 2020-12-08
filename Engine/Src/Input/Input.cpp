#include "DivePch.h"
#include "Input.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/EventSystem.h"


namespace Dive
{
	Input::Input(Context* context)
		: Dive_Object(context)
	{
		EventDispatcher::GetInstance().SubscribeEvent(this, &Input::OnUpdate);
	}

	Input::~Input()
	{
		EventDispatcher::GetInstance().UnsubscibeEvent<Input, E_UPDATE>(this);
	}

	void Input::OnUpdate(const E_UPDATE* evnt)
	{
		CORE_TRACE("Input' deltaTime: {:f} MS", evnt->GetDeltaTime());
	}
}