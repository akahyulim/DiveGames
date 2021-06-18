#pragma once
#include <functional>
#include <unordered_map>
#include <list>
#include <vector>

#define EVENT_HANDLE(callable)					[this](unsigned int data) {callable();}
#define EVENT_HANDLE_STATIC(callable)			[](unsigned int data) {callable();}
#define EVENT_HANDLE_DATA(callable)				[this](unsigned int data) {callable(data);}
#define EVENT_HANDLE_DATA_STATIC(callable)		[](unsigned int data) {callable(data);}

#define EVENT_SUBSCRIBE(eventID, callable)		Dive::EventManager::GetInstance().Subscribe(eventID, callable)
#define EVENT_FIRE(eventID)						Dive::EventManager::GetInstance().FireEvent(eventID)
#define EVENT_FIRE_DATA(eventID, eventData)		Dive::EventManager::GetInstance().FireEvent(eventID, eventData)

namespace Dive
{
	enum class eEventType
	{
		ChangedResolution,
	};

	class EventManager
	{
	public:
		static EventManager& GetInstance()
		{
			static EventManager instance;
			return instance;
		}
		
		// 이렇게하면 멤버 함수를 받지 못한다.
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
		std::unordered_map<eEventType, std::list<std::function<void(unsigned int)>>> m_subscribers;
	};
}