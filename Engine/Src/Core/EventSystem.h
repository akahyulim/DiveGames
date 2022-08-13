#pragma once
#include <unordered_map>
#include <vector>
#include <functional>

#define EVENT_HANDLER(function)				[this](const Dive::Event& e) {function(e);}
#define EVENT_HANDLER_STATIC(function)		[](const Dive::Event& e) { function(e);}

#define SUBSCRIBE_EVENT(type, function)		Dive::EventSystem::GetInstance().Subscribe(type, function);
#define UNSUBSCRIBE_EVENT(type, function)	Dive::EventSystem::GetInstance().Unsubscribe(type, function);

#define FIRE_EVENT(e)						Dive::EventSystem::GetInstance().Fire(e);

namespace Dive
{
	enum class eEventType
	{
		// Core
		BeginFrame, Update, PostUpdate, RenderUpdate, PostRenderUpdate, EndFrame,

		// Graphics
		WindowEvent, ScreenMode, BeginRender, EndRender,

		// Scene
		SceneUpdate, 

		// IO
		LogMessage,	ExitRequested,
	};

#define EVENT_CLASS_TYPE(type) \
public:	\
	static eEventType GetStaticType() { return eEventType::type; }	\
	virtual eEventType GetType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

	class Event
	{
	public:
		virtual ~Event() = default;

		virtual eEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

	class EventSystem
	{
		using Subscriber = std::function<void(const Event&)>;

	public:
		static EventSystem& GetInstance()
		{
			static EventSystem instance;
			return instance;
		}

		void Subscribe(const eEventType eventType, Subscriber&& function)
		{
			m_Subscribers[eventType].emplace_back(std::forward<Subscriber>(function));
		}

		void Unsubscribe(const eEventType eventType, Subscriber&& function)
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