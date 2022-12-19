#pragma once
#include "Math/StringHash.h"
#include "Core/Object.h"

namespace Dive
{
	// 서브시스템과 오브젝트 팩토리를 관리 및 제공하는 클래스.
	class Context
	{
	public:
		Context() = default;
		~Context();

		void RegisterSubsystem(Object* pObject);
		template<class T> T* RegisterSubsystem();
		void RemoveSubsystem(StringHash typeHash);

		Object* GetSubsystem(StringHash typeHash) const;
		template<class T> T* GetSubsystem() const;

		template<class T> void RegisterFactory();
		void RegisterFactory(ObjectFactory* pFactory);

		// 등록된 팩토리를 이용하여 타입해시로 오브젝트를 생성할 수 있습니다.
		Object* CreateObject(StringHash typeHash);

	private:
		std::unordered_map<uint32_t, Object*> m_Subsystmes;
		std::unordered_map<uint32_t, ObjectFactory*> m_Factories;
	};

	template<class T>
	T* Context::RegisterSubsystem()
	{
		auto* pSubsystem = new T(this);
		RegisterSubsystem(static_cast<Object*>(pSubsystem));

		return pSubsystem;
	}

	template<class T>
	T* Context::GetSubsystem() const
	{
		return dynamic_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}

	template<class T>
	void Context::RegisterFactory()
	{
		RegisterFactory(new ObjectFactoryImpl<T>(this));
	}
}