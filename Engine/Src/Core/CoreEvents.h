#pragma once
#include "EventHandler.h"

namespace Dive
{
	class PreUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PreUpdate)
	};

	class UpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(Update)
	};

	class PostUpdateEvent : public Event
	{
		EVENT_CLASS_TYPE(PostUpdate)
	};
}