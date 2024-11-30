#include "stdafx.h"
#include "EventDispatcher.h"

namespace Dive
{
	std::unordered_map<eEventType, std::vector<EventListener>> EventDispatcher::s_Listeners;

	void EventDispatcher::AddListener(eEventType type, EventListener listener)
	{
		s_Listeners[type].emplace_back(listener);
	}

	void EventDispatcher::Dispatch(eEventType type, EventData data)
	{
		auto& typeListener = s_Listeners[type];
		for (auto& listener : typeListener)
		{
			listener(data);
		}
	}
}