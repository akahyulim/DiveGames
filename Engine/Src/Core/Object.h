#pragma once

namespace Dive
{
	static unsigned int g_InstanceID = 0;

	class Object
	{
	public:
		Object()
		{
			m_id = GenerateID();
		}
		virtual ~Object() {}

		unsigned int GetID() const { return m_id; }

		static unsigned int GenerateID() { return ++g_InstanceID; }

		// ���� ������ ���ؼ� pure�Լ����� �Ѵ�.
		// �ϴ�  subsystem�� ����
		//virtual size_t GetType() const {}
		//virtual std::string GetTyepName() const {}

	protected:
		// Orho3D���� ���� object�� id�� �������� �ʴ´�.
		// ��� Scene�� Component�鿡�� Ư�� ������ ID�� �Ҵ����ִµ� �ϴ�.
		unsigned int m_id = 0;
	};
}
