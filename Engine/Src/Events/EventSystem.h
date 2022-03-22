#pragma once
#include <memory>
#include <unordered_map>

// event handle macro
#define EVENT_HANDLER(function)				[this](const Dive::Event& e) {function(e);}
#define EVENT_HANDLER_STATIC(function)		[](const Dive::Event& e) { function(e);}

#define SUBSCRIBE_EVENT(type, function)		Dive::EventDispatcher::GetInstance().Subscribe(type, function);
#define UNSUBSCRIBE_EVENT(type, function)	Dive::EventDispatcher::GetInstance().Unsubscribe(type, function);

#define FIRE_EVENT(e)						Dive::EventDispatcher::GetInstance().Fire(e);

enum class eEventType
{
	WindowData,
	SceneResolve, GameObjectModify,
};

// event type macro
#define EVENT_CLASS_TYPE(type)	static eEventType GetStaticType() { return eEventType::type; }	\
								virtual eEventType GetType() const override { return GetStaticType(); } \
								virtual const char* GetName() const override { return #type; }

namespace Dive
{
	// event interface
	class Event
	{
	public:
		virtual ~Event() = default;

		virtual eEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
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

		void Subscribe(eEventType type, Subscriber&& function)
		{
			m_Subscribers[type].push_back(std::forward<Subscriber>(function));
		}

		void Unsubscribe(eEventType type, Subscriber&& function)
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
		std::unordered_map<eEventType, std::vector<Subscriber>> m_Subscribers;
	};
}