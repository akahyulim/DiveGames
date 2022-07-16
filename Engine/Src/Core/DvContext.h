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

		// event sender, type을 key?로 receiver들을 관리한다.
		// receiver는 subscribe할 때 handler를 구성한 후 정보를 전달하여 그룹을 형성토록 하고,
		// sender는 send evnet할 때 그룹으로부터 receiver들을 전달받은 후 각각 invoke를 호출한다.
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