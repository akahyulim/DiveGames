#pragma once
#include "StringHash.h"

namespace Dive
{
	class DvObject;

	class EventReceiverGroup
	{
	public:
		EventReceiverGroup() = default;
		~EventReceiverGroup() = default;

		void Add(DvObject* pReceiver);
		void Remove(DvObject* pReceiver);

		unsigned int GetReceiverSize() const { return static_cast<unsigned int>(m_Receivers.size()); }

	public:
		std::vector<DvObject*> m_Receivers;
	};

	class DvContext
	{
	public:
		DvContext();
		~DvContext();

		void RegisterSubsystem(DvObject* pObject);
		void RemoveSubsystem(StringHash type);

		DvObject* GetSubsystem(StringHash type) const;
		template<typename T> T* GetSubsystem() const;

		// event sender, type�� key?�� receiver���� �����Ѵ�.
		// receiver�� subscribe�� �� handler�� ������ �� ������ �����Ͽ� �׷��� ������� �ϰ�,
		// sender�� send evnet�� �� �׷����κ��� receiver���� ���޹��� �� ���� invoke�� ȣ���Ѵ�.
		void AddEventReceiver(StringHash eventType, DvObject* pReceiver);
		void RemoveEventReceiver(StringHash eventType, DvObject* pReceiver);
		
		EventReceiverGroup* GetEventReceivers(StringHash eventType);

	private:
	private:
		std::map<StringHash, DvObject*> m_Subsystems;
		std::map<StringHash, EventReceiverGroup*> m_EventReceivers;
	};

	template<typename T>
	T* DvContext::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
}