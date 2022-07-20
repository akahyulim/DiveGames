#pragma once
#include <unordered_map>
#include <vector>
#include <functional>

#include "Variant.h"

#define DV_EVENT_HANDLER(function) [this](const Dive::Variant& var) { function();}
#define DV_EVENT_HANDLER_STATIC(function) [](const Dive::Variant& var) { function();}

#define DV_EVENT_HANDLER_VARIANT(function) [this](const Dive::Variant& var) { function(var);}
#define DV_EVENT_HANDLER_STATIC_VARIANT(function) [](const Dive::Variant& var) { function(var);}

#define DV_EVENT_FIRE(eventType) Dive::DvEventSystem::GetInstance().Fire(eventType)
#define DV_EVENT_FIRE_PARAM(eventType, data) Dive::DvEventSystem::GetInstance().Fire(eventType, data)

#define DV_SUBSCRIBE_TO_EVENT(eventType, function) Dive::DvEventSystem::GetInstance().Subscribe(eventType, function)
#define DV_UNSUBSCRIBE_FROM_EVENT(eventType, function) Dive::DvEventSystem::GetInstance().Unsubscribe(eventType, function)

namespace Dive
{
	enum class eDvEventType
	{
		// IO
		LogMessage,	ExitRequested,
	};

	using Subscriber = std::function<void(const Variant&)>;

	class DvEventSystem
	{
	public:
		static DvEventSystem& GetInstance()
		{
			static DvEventSystem instance;
			return instance;
		}

		void Subscribe(const eDvEventType eventType, Subscriber&& function)
		{
			m_Subscribers[eventType].emplace_back(std::forward<Subscriber>(function));
		}

		void Unsubscribe(const eDvEventType eventType, Subscriber&& function)
		{
			const size_t functionAddr = *reinterpret_cast<long*>(reinterpret_cast<char*>(&function));
			auto& subscribers = m_Subscribers[eventType];

			for (auto it = subscribers.begin(); it != subscribers.end();)
			{
				const size_t subscriberAddr = *reinterpret_cast<long*>(reinterpret_cast<char*>(&(*it)));
				if (functionAddr == subscriberAddr)
				{
					++it = subscribers.erase(it);
					return;
				}
				else
					++it;
			}
		}

		void Fire(const eDvEventType eventType, const Variant& data = 0)
		{
			if (m_Subscribers.find(eventType) == m_Subscribers.end())
				return;

			for (const auto& subscriber : m_Subscribers[eventType])
			{
				subscriber(data);
			}
		}

	private:
		std::unordered_map<eDvEventType, std::vector<Subscriber>> m_Subscribers;
	};
}