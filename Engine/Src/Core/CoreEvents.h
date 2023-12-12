#pragma once
#include "EventHandler.h"

namespace Dive
{
	class PreUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PreUpdate)
	};

	class PostUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PostUpdate)
	};

	class PreRenderEvent : public Event
	{
		EVENT_CLASS_TYPE(PreRender)
	};

	class PostRenderEvent : public Event
	{
		EVENT_CLASS_TYPE(PostRender)
	};
}