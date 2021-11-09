#pragma once
#include <functional>
#include <unordered_map>
#include <list>
#include <vector>

// �Ű������� ������ ���� �� �ֵ��� �� �� ������?
// �׷��ٸ� Fire���� ���޵� ������ �ؾ߰���?
// ���� ������ �ʿ��Ѱ�?
// �̺�Ʈ�� ��� �����ؾ� �ұ�?

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