#pragma once
#include <unordered_map>
#include <vector>
#include <functional>

#define DV_EVENT_HANDLER(function)				[this](const Dive::DvEvent& e) {function(e);}
#define DV_EVENT_HANDLER_STATIC(function)		[](const Dive::DvEvent& e) { function(e);}

#define DV_SUBSCRIBE_EVENT(type, function)		Dive::DvEventSystem::GetInstance().Subscribe(type, function);
#define DV_UNSUBSCRIBE_EVENT(type, function)	Dive::DvEventSystem::GetInstance().Unsubscribe(type, function);

#define DV_FIRE_EVENT(e)						Dive::DvEventSystem::GetInstance().Fire(e);

namespace Dive
{
	enum class eDvEventType
	{
		// Core
		BeginFrame, Update, PostUpdate, RenderUpdate, PostRenderUpdate, EndFrame,

		// Graphics
		WindowEvent, ScreenMode, BeginRender, EndRender,

		// IO
		LogMessage,	ExitRequested,
	};

#define DV_EVENT_CLASS_TYPE(type) \
public:	\
	static eDvEventType GetStaticType() { return eDvEventType::type; }	\
	virtual eDvEventType GetType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

	class DvEvent
	{
	public:
		virtual ~DvEvent() = default;

		virtual eDvEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

	class DvEventSystem
	{
		using Subscriber = std::function<void(const DvEvent&)>;

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

		void Fire(const DvEvent& e)
		{
			if (m_Subscribers.find(e.GetType()) == m_Subscribers.end())
				return;

			for (const auto& subscriber : m_Subscribers[e.GetType()])
			{
				subscriber(e);
			}
		}

	private:
		std::unordered_map<eDvEventType, std::vector<Subscriber>> m_Subscribers;
	};
}