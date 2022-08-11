#pragma once
#include "StringHash.h"

namespace Dive
{
	class Object;

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

	private:
	private:
		// ���� �ý��� ��.
		std::map<StringHash, std::shared_ptr<Object>> m_Subsystems;
	};

	template<class T>
	T* Context::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
}