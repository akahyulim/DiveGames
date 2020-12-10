#include "DivePch.h"
#include "Input.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/EventSystem.h"


namespace Dive
{
	Input::Input(Context* context)
		: Object(context)
	{
		DIVE_SUBSCRIBE_TO_EVENT(this, &Input::OnUpdate);
	}

	Input::~Input()
	{
		DIVE_UNSUBSCRIBE_FROM_EVENT(Input, E_UPDATE, this);

		CORE_TRACE("Call Input's Destructor =========================");
	}

	void Input::OnUpdate(const E_UPDATE* evnt)
	{
		CORE_TRACE("Input' deltaTime: {:f} MS", evnt->GetDeltaTime());
	}
}