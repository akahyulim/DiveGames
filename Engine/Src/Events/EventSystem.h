#pragma once
#include <memory>
#include <unordered_map>

#define DV_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Dive
{
	enum class eEventType
	{
		None = 0,
		WindowResize,
		AppUpdate, AppRender,
	};

#define EVENT_CLASS_TYPE(type) static eEventType GetStaticType() { return eEventType::type; } \
								virtual eEventType GetEventType() const override { return GetStaticType(); } \
								virtual const char* GetName() const override { return #type; }

	// event interface
	class Event
	{
	public:
		virtual ~Event() = default;

		virtual eEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

#define EVENT_HANDLE(function) [this](Event& e) {function(e);}

	using Subscriber = std::function<void(Event&)>;

	class EventDispatcher
	{
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

		template<typename T>
		void Subscribe(Subscriber&& function)
		{
			m_Subscribers[T::GetStaticType()].push_back(std::forward<Subscriber>(function));
		}

		template<typename T>
		void Unsubscribe(Subscriber&& function)
		{
			auto& subscribers = m_Subscribers[T::GetStaticType()];
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

		void Fire(Event& e)
		{
			if (m_Subscribers.find(e.GetEventType()) == m_Subscribers.end())
				return;

			for (const auto& subscriber : m_Subscribers[e.GetEventType()])
			{
				subscriber(e);
			}
		}

	private:
		std::unordered_map<eEventType, std::vector<Subscriber>> m_Subscribers;
	};
}