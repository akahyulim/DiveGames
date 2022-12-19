#pragma once
#include "Math/StringHash.h"
#include "Core/Object.h"

namespace Dive
{
	// ����ý��۰� ������Ʈ ���丮�� ���� �� �����ϴ� Ŭ����.
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

		// ��ϵ� ���丮�� �̿��Ͽ� Ÿ���ؽ÷� ������Ʈ�� ������ �� �ֽ��ϴ�.
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