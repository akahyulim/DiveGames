#pragma once
#include "Math/StringHash.h"
#include "Core/Object.h"

namespace Dive
{
	class Context
	{
	public:
		// ������.
		Context();
		// �Ҹ���. �����츦 ����ϴ� ���� �ý��� ��ü���� ����.
		~Context();

		// ���� �ý��� ���.
		void RegisterSubsystem(std::shared_ptr<Object> pObject);
		// ��ϵ� ���� �ý����� ����.
		void RemoveSubsystem(StringHash type);

		// ��ϵ� ���� �ý��� ��ü�� ����.
		Object* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

		template<class T> void RegisterFactory();
		void RegisterFactory(ObjectFactory* pFactory);

		Object* CreateObject(StringHash type);

	private:
	private:
		// ���� �ý��� ��.
		std::map<StringHash, std::shared_ptr<Object>> m_Subsystems;

		std::unordered_map<unsigned int, ObjectFactory*> m_Factories;
	};

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