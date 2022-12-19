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
	// ���� �ý��ۺ� �̺�Ʈ Ÿ�� ������.
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

// �̺�Ʈ Ÿ�� �Լ� ��ũ��.
#define EVENT_CLASS_TYPE(type) \
public:	\
	static eEventType GetStaticType() { return eEventType::type; }	\
	virtual eEventType GetType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

	// �̺�Ʈ Ÿ���� Ȯ���ϴ� ����� ���� �̺�Ʈ ���̽� Ŭ����.
	class Event
	{
	public:
		virtual ~Event() = default;

		virtual eEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

	// �̺�Ʈ ���� Callable.
	using Function = std::function<void(const Event&)>;

	// �̺�Ʈ ������ ���� �� ������ �����ϴ� �̺�Ʈ �ý��� Ŭ����.
	class EventSystem
	{
	public:
		static EventSystem& GetInstance()
		{
			static EventSystem instance;
			return instance;
		}

		uint32_t Subscribe(const eEventType eventType, Function&& function)
		{
			SubscriberInfo handler;
			handler.ID = m_NextID++;
			handler.Subscriber = std::forward<Function>(function);

			m_Subscribers[eventType].emplace_back(handler);

			return handler.ID;
		}

		void Unsubscribe(const eEventType eventType, uint32_t id)
		{
			if (m_Subscribers.find(eventType) == m_Subscribers.end())
				return;

			auto it = m_Subscribers[eventType].begin();
			for (it; it != m_Subscribers[eventType].end();)
			{
				if (it->ID == id)
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

			for (const auto& handler : m_Subscribers[e.GetType()])
			{
				handler.Subscriber(e);
			}
		}

		void Clear()
		{
			m_Subscribers.clear();
		}

	private:
		EventSystem() : m_NextID(0) {}

	private:
		uint32_t m_NextID;

		struct SubscriberInfo
		{
			uint32_t ID;
			Function Subscriber;
		};

		std::unordered_map<eEventType, std::vector<SubscriberInfo>> m_Subscribers;
	};
}