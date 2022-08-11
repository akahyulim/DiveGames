#pragma once
#include "StringHash.h"

namespace Dive
{
	class Object;

	class Context
	{
	public:
		// 생성자.
		Context();
		// 소멸자. 윈도우를 사용하는 서브 시스템 객체들을 제거.
		~Context();

		// 서브 시스템 등록.
		void RegisterSubsystem(std::shared_ptr<Object> pObject);
		// 등록된 서브 시스템을 제거.
		void RemoveSubsystem(StringHash type);

		// 등록된 서브 시스템 객체를 리턴.
		Object* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

	private:
	private:
		// 서브 시스템 맵.
		std::map<StringHash, std::shared_ptr<Object>> m_Subsystems;
	};

	template<class T>
	T* Context::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
}