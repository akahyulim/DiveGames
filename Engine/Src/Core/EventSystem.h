#pragma once
#include <unordered_map>
#include <vector>
#include <functional>

#define EVENT_HANDLER(function)					[this](const Dive::Event& e) { function(); }
#define EVENT_HANDLER_STATIC(function)			[](const Dive::Event& e) { function(); }

#define EVENT_HANDLER_PARAM(function)			[this](const Dive::Event& e) { function(e); }
#define EVENT_HANDLER_PARAM_STATIC(function)	[](const Dive::Event& e) { function(e); }

#define SUBSCRIBE_EVENT(type, function)			Dive::EventSystem::GetInstance().Subscribe(type, function)
#define UNSUBSCRIBE_EVENT(type, id)				Dive::EventSystem::GetInstance().Unsubscribe(type, id)

#define FIRE_EVENT(e)							Dive::EventSystem::GetInstance().Fire(e)

namespace Dive
{
	enum class eEventType
	{
		// Engine
		BeginFrame, PreUpdate, Update, PostUpdate, RenderUpdate, PostRenderUpdate, EndFrame,

		// Graphics
		WindowEvent, ScreenMode, BeginRender, EndRender,

		// Renderer
		RenderTargetUpdate,

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

	using Subscriber = std::function<void(const Event&)>;


	class EventSystem
	{
	public:
		static EventSystem& GetInstance()
		{
			static EventSystem instance;
			return instance;
		}

		unsigned int Subscribe(const eEventType eventType, Subscriber&& function)
		{
			HandlerSlot handleSlot;
			handleSlot.id = m_NextID++;
			handleSlot.subscriber = std::forward<Subscriber>(function);

			m_Subscribers[eventType].emplace_back(handleSlot);

			return handleSlot.id;
		}

		void Unsubscribe(const eEventType eventType, unsigned int id)
		{
			if (m_Subscribers.find(eventType) == m_Subscribers.end())
				return;

			auto it = m_Subscribers[eventType].begin();
			for (it; it != m_Subscribers[eventType].end();)
			{
				if (it->id == id)
				{
					it = m_Subscribers[eventType].erase(it);
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

			for (const auto& handleSlot : m_Subscribers[e.GetType()])
			{
				handleSlot.subscriber(e);
			}
		}

		void Clear()
		{
			m_Subscribers.clear();
		}

	private:
		unsigned int m_NextID = 0;

		struct HandlerSlot
		{
			unsigned int id;
			Subscriber subscriber;
		};
		std::unordered_map<eEventType, std::vector<HandlerSlot>> m_Subscribers;
	};
}