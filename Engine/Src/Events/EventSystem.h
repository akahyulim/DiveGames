#pragma once
#include <memory>
#include <unordered_map>

#define EVENT_HANDLER(function)				[this](const Event& e) {function(e);}
#define EVENT_HANDLER_STATIC(function)		[](const Event& e) { function(e);}

#define SUBSCRIBE_EVENT(type, function)		Dive::EventDispatcher::GetInstance().Subscribe(type, function);
#define UNSUBSCRIBE_EVENT(type, function)	Dive::EventDispatcher::GetInstance().Unsubscribe(type, function);

#define FIRE_EVENT(e)						Dive::EventDispatcher::GetInstance().Fire(e);

namespace Dive
{
	// event interface
	class Event
	{
	public:
		virtual ~Event() = default;

		using EventType = const char*;

		virtual EventType GetType() const = 0;
	};

	class EventDispatcher
	{
		using Subscriber = std::function<void(const Event&)>;

	public:
		static EventDispatcher& GetInstance()
		{
			static EventDispatcher inst;
			return inst;
		}

		void Clear()
		{
			m_Subscribers.clear();
		}

		void Subscribe(const Event::EventType& type, Subscriber&& function)
		{
			m_Subscribers[type].push_back(std::forward<Subscriber>(function));
		}

		void Unsubscribe(const Event::EventType& type, Subscriber&& function)
		{
			auto& subscribers = m_Subscribers[type];
			const size_t functionAddress = *(reinterpret_cast<long*>(reinterpret_cast<char*>(&function)));

			for (auto it = subscribers.begin(); it != subscribers.end(); it++)
			{
				const size_t subscriberAddress = *(reinterpret_cast<long*>(reinterpret_cast<char*>(&(*it))));
				if (subscriberAddress == functionAddress)
				{
					it = subscribers.erase(it);
					return;
				}
			}
		}

		void Fire(const Event& e)
		{
			if (m_Subscribers.find(e.GetType()) == m_Subscribers.end())
				return;

			for (const auto& subscriber : m_Subscribers[e.GetType()])
			{
				subscriber(e);
			}
		}

	private:
		std::unordered_map<Event::EventType, std::vector<Subscriber>> m_Subscribers;
	};
}