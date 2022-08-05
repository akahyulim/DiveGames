#pragma once
#include "StringHash.h"

namespace Dive
{
	class DvObject;

	class DvContext
	{
	public:
		// ������.
		DvContext();
		// �Ҹ���. �����츦 ����ϴ� ���� �ý��� ��ü���� ����.
		~DvContext();

		// ���� �ý��� ���.
		void RegisterSubsystem(std::shared_ptr<DvObject> pObject);
		// ��ϵ� ���� �ý����� ����.
		void RemoveSubsystem(StringHash type);

		// ��ϵ� ���� �ý��� ��ü�� ����.
		DvObject* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

	private:
	private:
		// ���� �ý��� ��.
		std::map<StringHash, std::shared_ptr<DvObject>> m_Subsystems;
	};

	template<class T>
	T* DvContext::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
}