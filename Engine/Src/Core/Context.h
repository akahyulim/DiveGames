#pragma once
#include "Math/StringHash.h"
#include "Core/Object.h"

namespace Dive
{
	class Context
	{
	public:
		Context() = default;
		~Context();

		void RegisterSubsystem(Object* pObject);
		template<class T> T* RegisterSubsystem();
		void RemoveSubsystem(StringHash type);

		Object* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

		template<class T> void RegisterFactory();
		void RegisterFactory(ObjectFactory* pFactory);

		Object* CreateObject(StringHash type);

	private:
		std::unordered_map<unsigned int, Object*> m_Subsystems;
		std::unordered_map<unsigned int, ObjectFactory*> m_Factories;
	};

	template<class T>
	T* Context::RegisterSubsystem()
	{
		return static_cast<T*>(RegisterSubsystem(new T(this)));
	}

	template<class T>
	T* Context::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}

	template<class T>
	void Context::RegisterFactory()
	{
		RegisterFactory(new ObjectFactoryImpl<T>(this));
	}
}