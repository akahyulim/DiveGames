#include "divepch.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<unsigned long long> s_UniformDistribution;

	// Ȥ�ó� ���� id�� ������ ���� ����� hash�� �ʿ�?

	Object::Object()
		: m_InstanceID(s_UniformDistribution(s_Engine))
	{
	}

	Object::Object(unsigned long long id)
		: m_InstanceID(id)
	{
	}

	// ���� �Ű������� �����Ѵ�.
	// �� �� �Լ��� ������ �� �ְԲ� �ϴ� ���� ���� �� ����.
	// �ƴϸ� ��� ���� ������ �ʿ��ؾ� �Ѵ�.
	//Object::Object(const std::string& name, unsigned long long id)
	//{
	//	m_Name			= name;
	//	m_InstanceID	= id == 0 ? s_UniformDistribution(s_Engine) : id;
	//}
}