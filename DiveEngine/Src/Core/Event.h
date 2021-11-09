#pragma once
#include <functional>
#include <unordered_map>
#include <list>
#include <vector>

// 매개변수를 여러개 받을 수 있도록 할 수 없을까?
// 그렇다면 Fire에서 전달도 여러개 해야겠지?
// 구독 해제가 필요한가?
// 이벤트는 어떻게 구성해야 할까?

#define EVENT_HANDLE(callable)					[this](unsigned int data) {callable();}
#define EVENT_HANDLE_STATIC(callable)			[](unsigned int data) {callable();}
#define EVENT_HANDLE_DATA(callable)				[this](unsigned int data) {callable(data);}
#define EVENT_HANDLE_DATA_STATIC(callable)		[](unsigned int data) {callable(data);}
#define EVENT_SUBSCRIBE(eventID, callable)		DiveEngine::EventManager::GetInstance().Subscribe(eventID, callable)
#define EVENT_FIRE(eventID)						DiveEngine::EventManager::GetInstance().FireEvent(eventID)
#define EVENT_FIRE_DATA(eventID, eventData)		DiveEngine::EventManager::GetInstance().FireEvent(eventID, eventData)

namespace DiveEngine
{
	enum class eEventType
	{
		SceneActivate,
		SceneSave,
		SceneSaved,
		SceneLoad,
		SceneLoaded,
		SceneResolve,
		ChangedResolution,
		QuitApplication,
	};

	class EventManager
	{
	public:
		static EventManager& GetInstance()
		{
			static EventManager instance;
			return instance;
		}
		
		void Subscribe(const eEventType id, std::function<void(unsigned int)> callback)
		{
			m_subscribers[id].push_back(callback);
		}

		void FireEvent(const eEventType id, unsigned int data = 0)
		{
			auto it = m_subscribers.find(id);
			if (it != m_subscribers.end())
			{
				auto& callbacks = it->second;
				for (auto& callback : callbacks)
				{
					callback(data);
				}
			}
		}

	private:
		EventManager() = default;
		EventManager(const EventManager&) = delete;
		EventManager operator=(const EventManager&) = delete;
		
	private:
		std::unordered_map<eEventType, std::list<std::function<void(unsigned int)>>> m_subscribers;
	};
}