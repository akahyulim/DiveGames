#include "DivePch.h"
#include "EventHandler.h"

namespace Dive
{
	static std::unordered_map<eEventType, std::vector<Function>> s_Subscribers;

	void EventHandler::Destroy()
	{
		s_Subscribers.clear();
	}

	void EventHandler::Subscribe(const eEventType eventType, Function&& function)
	{
		s_Subscribers[eventType].emplace_back(std::forward<Function>(function));
	}

	void EventHandler::Fire(const Event& e)
	{
		if (s_Subscribers.find(e.GetType()) == s_Subscribers.end())
			return;

		for (const auto& handler : s_Subscribers[e.GetType()])
		{
			handler(e);
		}
	}
}