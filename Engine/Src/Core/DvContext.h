#pragma once
#include "StringHash.h"

namespace Dive
{
	class DvObject;

	class DvContext
	{
	public:
		// 생성자.
		DvContext();
		// 소멸자. 윈도우를 사용하는 서브 시스템 객체들을 제거.
		~DvContext();

		// 서브 시스템 등록.
		void RegisterSubsystem(std::shared_ptr<DvObject> pObject);
		// 등록된 서브 시스템을 제거.
		void RemoveSubsystem(StringHash type);

		// 등록된 서브 시스템 객체를 리턴.
		DvObject* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

	private:
	private:
		// 서브 시스템 맵.
		std::map<StringHash, std::shared_ptr<DvObject>> m_Subsystems;
	};

	template<class T>
	T* DvContext::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
}