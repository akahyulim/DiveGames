#pragma once
#include "StringHash.h"

namespace Dive
{
	class DvObject;

	class DvContext
	{
	public:
		DvContext();
		~DvContext();

		void RegisterSubsystem(DvObject* pObject);
		void RemoveSubsystem(StringHash type);

		DvObject* GetSubsystem(StringHash type) const;
		template<typename T> T* GetSubsystem() const;

	private:
	private:
		std::map<StringHash, DvObject*> m_Subsystems;
	};

	template<typename T>
	T* DvContext::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
}