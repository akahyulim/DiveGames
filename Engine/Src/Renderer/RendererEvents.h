#pragma once
#include "Core/EventSystem.h"

namespace Dive
{
	class RenderTargetUpdate : public Event
	{
		EVENT_CLASS_TYPE(RenderTargetUpdate)
			
	public:
		RenderTargetUpdate() = default;
		~RenderTargetUpdate() override = default;
	};
}