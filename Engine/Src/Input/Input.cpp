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

	// ��� �̺�Ʈ�� eventType�� variantMap�� ������ �޾ƾ� �Ѵ�.
	void Input::OnUpdate(size_t eventType)
	{
		CORE_TRACE("Input - Received Event!!!");
	}
}